#define GL_SILENCE_DEPRECATION
#include "CSCIx229.h"
#include "terrain.h"
#include "objects.h"
#include "particles.h"
#include "shaders.h"
#include "celestial.h"
#include "clouds.h"
#include "camera.h"
#include <stdbool.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

static CloudSystem* cloudSystem = NULL;

GLuint grassTexture = 0;
GLuint rockTexture = 0;
GLuint sandTexture = 0;
int terrainShader = 0;
int skyShader = 0;

static int th = 0;          
static int ph = 30;         
static float dim = 100;     
static int fov = 1;         
      
static float lightHeight = 250.0f;
static float dayTime = 0.0f;    
static float windStrength = 0.0f;


float waterLevel = -4.0f;
static float waterSpeed = 1.0f;
static float waterTime = 0.0f;
static int animateWater = 1;


static int wireframe = 0;
static int showAxes = 0;
static int animateLight = 1;    
static float lightSpeed = 1.0f; 


#define SNOW_PARTICLES 2000
static ParticleSystem* snowSystem = NULL;
static int weatherType = 0;  // 0=clear, 1=snow
static float weatherIntensity = 1.0f;

Terrain* terrain = NULL;
static TreeSystem* trees = NULL;

static float lastTime = 0;
static float deltaTime = 0;

float fogDensity = 0.005f;
int fogEnabled = 0;

static int animateTime = 1;
static float timeSpeed = 1.0f; 

static Camera* camera = NULL;
float asp;
static int lastX = 0, lastY = 0;
static int mouseButtons = 0;

void reshape(int width, int height);
void display();
void special(int key, int x, int y);
void keyboard(unsigned char key, int x, int y);
void idle();

#define MAX_STARS 1000
typedef struct {
    float x, y, z;
    float brightness;
} Star;
Star stars[MAX_STARS];


void reshape(int width, int height) {
    asp = (height>0) ? (double)width/height : 1;
    glViewport(0,0, RES*width,RES*height);
    
    if (camera) {
        setProjection(camera, 55.0f, asp, dim/4, dim*4);
    } else {
        Project(fov?55:0, asp, dim);
    }
}

void updateDeltaTime() {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
}

float getNormalizedDayTime(float time) {
    return time / 24.0f;
}

float smoothstep(float edge0, float edge1, float x) {
    float t = (x - edge0) / (edge1 - edge0);
    t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
    return t * t * (3.0f - 2.0f * t);
}

void getSkyColor(float time, float* color) {
    float t = time / 24.0f;  
    
    const int NUM_COLORS = 6;
    float timePoints[6] = {0.0f, 0.25f, 0.4f, 0.6f, 0.75f, 1.0f};  
    float colors[6][3] = {
        {0.02f, 0.02f, 0.1f},  
        {0.7f, 0.4f, 0.4f},    
        {0.4f, 0.7f, 1.0f},    
        {0.4f, 0.7f, 1.0f},    
        {0.7f, 0.4f, 0.4f},    
        {0.02f, 0.02f, 0.1f}   
    };
    
    
    int i;
    for(i = 0; i < NUM_COLORS-1; i++) {
        if(t >= timePoints[i] && t <= timePoints[i+1]) break;
    }
    
    float segmentPos = (t - timePoints[i]) / (timePoints[i+1] - timePoints[i]);
    float blend = smoothstep(0.0f, 1.0f, segmentPos);
    
    float sunHeight = sin(t * 2.0f * M_PI);
    float atmosphericBlend = fmax(0.0f, sunHeight * 0.2f);
    
    for(int j = 0; j < 3; j++) {
        color[j] = colors[i][j] * (1.0f - blend) + colors[i+1][j] * blend;
    }
    
    if(sunHeight > 0) {
        color[2] = fmin(1.0f, color[2] + atmosphericBlend * 0.2f);
    }
    
    if(t < 0.1f || t > 0.9f) {
        float nightBlend = (t < 0.1f) ? (t / 0.1f) : ((1.0f - t) / 0.1f);
        nightBlend = smoothstep(0.0f, 1.0f, nightBlend);
        float nightColor[3] = {0.02f, 0.02f, 0.1f};
        for(int j = 0; j < 3; j++) {
            color[j] = color[j] * nightBlend + nightColor[j] * (1.0f - nightBlend);
        }
    }
}

void updateDayCycle() {
    dayTime += deltaTime * 0.1f;  
    if (dayTime >= 24.0f) dayTime = 0.0f;
    
    float timeNormalized = dayTime / 24.0f;
    float sunHeight = sin(timeNormalized * 2 * M_PI);
    
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    float diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    if (sunHeight > 0) {
        float intensity = 0.6f + sunHeight * 0.4f; 
        ambient[0] = 0.3f + sunHeight * 0.1f;
        ambient[1] = 0.3f + sunHeight * 0.1f;
        ambient[2] = 0.3f + sunHeight * 0.1f;
        
        diffuse[0] = intensity * 1.0f;
        diffuse[1] = intensity * 0.95f;
        diffuse[2] = intensity * 0.8f;
    } else {
        float intensity = 0.3f; 
        ambient[0] = intensity * 0.2f;
        ambient[1] = intensity * 0.2f;
        ambient[2] = intensity * 0.3f;
        
        diffuse[0] = intensity * 0.6f;
        diffuse[1] = intensity * 0.6f;
        diffuse[2] = intensity * 0.8f;
    }
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
}

// Lighting system adapted from:
// - CSCI-4229/5229 course examples
// - LearnOpenGL Advanced Lighting
// Modified with Claude assistance for dynamic day/night cycle
void updateDynamicLighting(float dayTime) {
    float timeNormalized = dayTime / 24.0f;
    float sunAngle = (timeNormalized - 0.25f) * 2 * M_PI;
    float sunHeight = sin(sunAngle);
    
    // Calculate sun and moon positions
    float sunX = 500 * cos(sunAngle);
    float sunY = lightHeight * sunHeight;
    float sunZ = 0;
    
    float moonX = 500 * cos(sunAngle + M_PI);
    float moonY = lightHeight * (-sunHeight);
    float moonZ = 0;
    
    float lightPos[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
    
    if (sunHeight > 0) {
        lightPos[0] = sunX;
        lightPos[1] = sunY;
        lightPos[2] = sunZ;
        lightPos[3] = 0.0f;  
        
        float intensity = 0.5f + sunHeight * 0.5f;
        
        ambient[0] = 0.15f + sunHeight * 0.15f;
        ambient[1] = 0.15f + sunHeight * 0.15f;
        ambient[2] = 0.15f + sunHeight * 0.15f;
        ambient[3] = 1.0f;
        
        diffuse[0] = intensity * 1.0f;
        diffuse[1] = intensity * 0.95f;
        diffuse[2] = intensity * 0.85f;
        diffuse[3] = 1.0f;
        
        specular[0] = intensity * 0.7f;
        specular[1] = intensity * 0.7f;
        specular[2] = intensity * 0.6f;
        specular[3] = 1.0f;
    }
    else {
        lightPos[0] = moonX;
        lightPos[1] = moonY;
        lightPos[2] = moonZ;
        lightPos[3] = 0.0f; 
        
        float moonIntensity = 0.15f + (-sunHeight) * 0.1f;
        
        ambient[0] = 0.02f;
        ambient[1] = 0.02f;
        ambient[2] = 0.04f; 
        ambient[3] = 1.0f;
        
        diffuse[0] = moonIntensity * 0.7f;
        diffuse[1] = moonIntensity * 0.7f;
        diffuse[2] = moonIntensity * 0.9f;
        diffuse[3] = 1.0f;
        
        specular[0] = moonIntensity * 0.3f;
        specular[1] = moonIntensity * 0.3f;
        specular[2] = moonIntensity * 0.4f;
        specular[3] = 1.0f;
    }
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    
    float matSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    float matShininess = sunHeight > 0 ? 30.0f : 15.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
}

void updateTreeAnimation() {
    static float windTime = 0.0f;
    windTime += deltaTime;
    windStrength = sin(windTime * 0.5f) * 0.5f + 0.5f;
    
    for (int i = 0; i < trees->instanceCount; i++) {
        float windEffect = windStrength * (1.0f + sin(trees->instances[i].x * 0.1f + windTime));
        trees->instances[i].tiltX = sin(windTime + trees->instances[i].x) * windEffect * 5.0f;
        trees->instances[i].tiltZ = cos(windTime * 0.7f + trees->instances[i].z) * windEffect * 5.0f;
    }
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    
    float position[] = {1.0f, 2.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    float mSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0f);
    
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

// Fog system adapted from:
// - OpenGL Red Book
// - Enhanced with Claude assistance for atmospheric effects
void updateFog(float dayTime) {
    float timeNormalized = dayTime / 24.0f;
    float sunAngle = (timeNormalized - 0.25f) * 2 * M_PI;
    float sunHeight = sin(sunAngle);
    
    float baseDensity;
    float fogColor[4];
    
    if (sunHeight > 0) {
        baseDensity = 0.003f;
        fogColor[0] = 0.95f;
        fogColor[1] = 0.95f;
        fogColor[2] = 0.95f;
    } else {
        baseDensity = 0.008f;
        fogColor[0] = 0.7f;
        fogColor[1] = 0.7f;
        fogColor[2] = 0.7f;
    }
    
    fogColor[3] = 1.0f;

    if (fogEnabled) {
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE, GL_EXP2);
        glFogf(GL_FOG_DENSITY, baseDensity);
        glFogfv(GL_FOG_COLOR, fogColor);
        
        float fogStart = sunHeight > 0 ? dim * 0.1f : dim * 0.05f;
        float fogEnd = sunHeight > 0 ? dim * 0.8f : dim * 0.4f;
        
        glFogf(GL_FOG_START, fogStart);
        glFogf(GL_FOG_END, fogEnd);
        glHint(GL_FOG_HINT, GL_NICEST);
    }
}


void initGL() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glPolygonOffset(1.0f, 1.0f);
}


void display() {

    float skyColor[3];
    getSkyColor(dayTime, skyColor);
    glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camera->pos[0], camera->pos[1], camera->pos[2],
              camera->target[0], camera->target[1], camera->target[2],
              camera->up[0], camera->up[1], camera->up[2]);

    renderSunAndMoon(dayTime);

    updateFog(dayTime);
    
    updateDynamicLighting(dayTime);
    
    renderTerrain(terrain);
    useShader(0);
    if (cloudSystem) {
    updateClouds(cloudSystem, deltaTime, dayTime);
    renderClouds(cloudSystem, dayTime);
}
    
    if (trees && trees->instanceCount > 0) {
        glPushMatrix();
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        glShadeModel(GL_SMOOTH);
        renderTrees(trees, dayTime);
        glPopMatrix();
    }
    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    renderWater(waterLevel, terrain, dayTime);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    if (weatherType == 1 && snowSystem) {
        glDisable(GL_FOG);  
        renderParticles(snowSystem);
        glEnable(GL_FOG);
    }

    glDisable(GL_FOG);

    if (showAxes) {
        glDisable(GL_DEPTH_TEST);
        glColor3f(1,1,1);
        glBegin(GL_LINES);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(dim/2,0.0,0.0);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(0.0,dim/2,0.0);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(0.0,0.0,dim/2);
        glEnd();
        glEnable(GL_DEPTH_TEST);
    }

    glDisable(GL_DEPTH_TEST);
    glWindowPos2i(5,45);
    Print("Time: %02d:%02d  Weather: %s", 
          (int)dayTime, (int)((dayTime-(int)dayTime)*60), 
          weatherType ? "Snow" : "Clear");
    glWindowPos2i(5,25);
    Print("Angle=%d,%d  Dim=%.1f View=%s", 
          th, ph, dim, camera->mode == VIEW_FREE_ORBIT ? "Free Orbit" : camera->mode == VIEW_TOP_DOWN ? "Top Down" : camera->mode == VIEW_FIRST_PERSON ? "First Person" : "Third Person");
    glWindowPos2i(5,5);
    Print("Water=%.1f Light=%.1f [W]ire=%d [A]xes=%d", 
          waterLevel, lightHeight, wireframe, showAxes);
    glWindowPos2i(5,65);
    Print("Fog: %s  Density: %.3f", 
        fogEnabled ? "On" : "Off", 
        fogDensity);
    glWindowPos2i(5,85);
    Print("Time Animation: %s  Speed: %.1fx", 
        animateTime ? "On" : "Off", 
        timeSpeed);
    glEnable(GL_DEPTH_TEST);

    glutSwapBuffers();
}

void initSkyBackground() {
    skyShader = loadShader("sky.vert", "sky.frag");
}

// Mouse interaction adapted from:
// - CSCI-4229/5229 course examples
// - LearnOpenGL Camera System
void mouse(int button, int state, int x, int y) {
    lastX = x;
    lastY = y;

    if (state == GLUT_DOWN) {
        mouseButtons |= 1<<button;
    } else {
        mouseButtons &= ~(1<<button);
    }

    if (button == 3) {
        if (camera->mode == VIEW_FREE_ORBIT) {
            dim = dim * 0.9;
            camera->distance *= 0.9;
        }
    } else if (button == 4) {
        if (camera->mode == VIEW_FREE_ORBIT) {
            dim = dim * 1.1;
            camera->distance *= 1.1;
        }
    }

    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    int dx = x - lastX;
    int dy = y - lastY;

    if (camera->mode == VIEW_FREE_ORBIT) {
        if (mouseButtons & 1) {
            th += dx;
            ph += dy;
            camera->yaw += dx;
            camera->pitch += dy;

            if (ph > 89) ph = 89;
            if (ph < -89) ph = -89;
            if (camera->pitch > 89) camera->pitch = 89;
            if (camera->pitch < -89) camera->pitch = -89;
        }
        else if (mouseButtons & 4) {
            dim *= (1 + dy/100.0);
            camera->distance *= (1 + dy/100.0);
            if (dim < 1) dim = 1;
            if (camera->distance < 1) camera->distance = 1;
        }
    } else {
        if (mouseButtons & 1) {
            rotateCamera(camera, dx * 0.5f, -dy * 0.5f);
        }
    }

    lastX = x;
    lastY = y;
    glutPostRedisplay();
}



void special(int key, int x, int y) {
    float deltaTime = 0.016f;

    if (camera && camera->mode == VIEW_FREE_ORBIT) {
        switch(key) {
            case GLUT_KEY_RIGHT:
                th += 5;
                camera->yaw = th;  
                break;
            case GLUT_KEY_LEFT:
                th -= 5;
                camera->yaw = th;
                break;
            case GLUT_KEY_UP:
                if (ph < 89) {
                    ph += 5;
                    camera->pitch = ph; 
                }
                break;
            case GLUT_KEY_DOWN:
                if (ph > -89) {
                    ph -= 5;
                    camera->pitch = ph;
                }
                break;
            case GLUT_KEY_PAGE_DOWN:
                dim += 0.1;
                camera->distance = dim;
                break;
            case GLUT_KEY_PAGE_UP:
                if (dim > 1) {
                    dim -= 0.1;
                    camera->distance = dim;
                }
                break;
        }
        
        th %= 360;
        ph %= 360;
        
        updateCameraVectors(camera);  
        Project(fov?55:0, asp, dim);
    }
    else {
        switch(key) {
            case GLUT_KEY_RIGHT:
                moveCamera(camera, RIGHT, deltaTime);
                break;
            case GLUT_KEY_LEFT:
                moveCamera(camera, LEFT, deltaTime);
                break;
            case GLUT_KEY_UP:
                moveCamera(camera, FORWARD, deltaTime);
                break;
            case GLUT_KEY_DOWN:
                moveCamera(camera, BACKWARD, deltaTime);
                break;
        }
    }

    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27:  // ESC
            exit(0);
            break;
        case 'w':  // Toggle wireframe
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
            break;
        case '[':  // Lower water
            waterLevel -= 1.0f;
            break;
        case ']':  // Raise water
            waterLevel += 1.0f;
            break;
        case 'a':  // Toggle axes
            showAxes = !showAxes;
            break;
        case '+':  // Raise light
            lightHeight += 10.0f;
            break;
        case '-':  // Lower light
            lightHeight -= 10.0f;
            break;
        case 'v':  // Toggle view mode
            fov = !fov;
            if (!fov) {
                ph = 90;
                th = 0;
            }
            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            break;
        case 's':  // Toggle water animation
            animateWater = !animateWater;
            glutIdleFunc(animateWater || animateLight ? idle : NULL);
            break;
        case 'l':  // Toggle light animation
            animateLight = !animateLight;
            glutIdleFunc(animateWater || animateLight ? idle : NULL);
            break;
        case '.':  // Increase animation speeds
            waterSpeed *= 1.2f;
            lightSpeed *= 1.2f;
            break;
        case ',':  // Decrease animation speeds
            waterSpeed *= 0.8f;
            lightSpeed *= 0.8f;
            break;
        case 'r':  // Reset view
            th = 0;
            ph = 30;
            waterLevel = -4.0f;
            lightHeight = 250.0f;
            waterSpeed = 1.0f;
            lightSpeed = 1.0f;
            dim = 100.0f;
            camera->distance = dim;
            camera->yaw = th;
            camera->pitch = ph;
            updateCameraVectors(camera);
            break;
        case '0':  // Reset angles
            th = 0;
            ph = 30;
            break;
        case 'z':  // Zoom in
            dim = dim * 0.9f;
            if(dim < 1) dim = 1;
            camera->distance = dim;
            updateCameraVectors(camera);
            break;
        case 'Z':  // Zoom out
            dim = dim * 1.1f;
            camera->distance = dim;
            updateCameraVectors(camera);
            break;
        case 't':
        case 'T':
            animateTime = !animateTime;
            glutIdleFunc(animateTime ? idle : NULL);
            break;
        case '(':  // Slow down time
            timeSpeed = fmax(0.1f, timeSpeed - 0.1f);
            break;
        case ')':  // Speed up time
            timeSpeed = fmin(5.0f, timeSpeed + 0.1f);
            break;
        case 'W':  // Toggle weather
            weatherType = (weatherType + 1) % 2;
            break;
        case 'S':  // Adjust weather intensity
            weatherIntensity = fmin(1.0f, weatherIntensity + 0.1f);
            break;
        case 'D':  // Decrease weather intensity
            weatherIntensity = fmax(0.0f, weatherIntensity - 0.1f);
            break;
        case 'f':  // Toggle fog
            fogEnabled = !fogEnabled;
            break;
        case '9':  // Decrease fog density
            fogDensity = fmax(0.001f, fogDensity - 0.001f);
            break;
        case '8':  // Increase fog density
            fogDensity = fmin(0.01f, fogDensity + 0.001f);
            break;
        case '1':
            setCameraView(camera, VIEW_FIRST_PERSON);
            setProjection(camera, 55.0f, asp, dim/4, dim*4);
            break;
        case '2':
            setCameraView(camera, VIEW_TOP_DOWN);
            break;
        case '3':
            setCameraView(camera, VIEW_FREE_ORBIT);
            break;
    }
    glutPostRedisplay();
}

void idle() {
    if (animateTime) {
        dayTime += deltaTime * timeSpeed;
        if (dayTime >= 24.0f) dayTime = 0.0f;
    }
    updateDeltaTime();
    
    if (animateLight) {
        updateDayCycle();
    }

    updateCamera(camera, deltaTime);
    
    if (weatherType == 1 && snowSystem) {
        updateParticles(snowSystem, deltaTime, weatherIntensity, lastTime);
    }
    
    updateTreeAnimation();
    
    if (animateWater) {
        waterTime += deltaTime;
    }
    
    glutPostRedisplay();
}


void initWeatherSystem() {
    printf("Initializing weather system...\n");
    snowSystem = createParticleSystem(SNOW_PARTICLES, PARTICLE_SNOW);
    if (!snowSystem) {
        fprintf(stderr, "Failed to create particle system\n");
        return;
    }
    printf("Weather system initialized: Type=%d, Particles=%d\n", 
           snowSystem->type, snowSystem->maxParticles);
    snowSystem->emitterY = lightHeight;
    snowSystem->emitterRadius = TERRAIN_SCALE * 0.5f;
}


int main(int argc, char* argv[]) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
    glutInitWindowSize(800,600);
    glutCreateWindow("Advanced Terrain Generation System");

    terrain = createTerrain();
    if (!terrain) {
        fprintf(stderr, "Failed to create terrain\n");
        return 1;
    }

    camera = createCamera();
    if (!camera) {
        fprintf(stderr, "Failed to create camera\n");
        return 1;
    }

    camera->yaw = th;
    camera->pitch = ph;
    camera->distance = dim;
    camera->mode = VIEW_FREE_ORBIT;

    trees = createTreeSystem();
    if (!trees) {
        fprintf(stderr, "Failed to create tree system\n");
        return 1;
    }

    printf("Initializing weather system...\n");
    snowSystem = createParticleSystem(SNOW_PARTICLES, PARTICLE_SNOW);
    if (!snowSystem) {
        fprintf(stderr, "Failed to create particle system\n");
        return 1;
    }
    snowSystem->emitterY = lightHeight;
    snowSystem->emitterRadius = TERRAIN_SCALE * 0.5f;

    initCelestialObjects();

    cloudSystem = createCloudSystem(TERRAIN_SCALE * 0.4f);
    if (!cloudSystem) {
        fprintf(stderr, "Failed to create cloud system\n");
        return 1;
    }

    terrainShader = loadShader("terrain.vert", "terrain.frag");
    if (!terrainShader) {
        fprintf(stderr, "Failed to load terrain shaders\n");
        return 1;
    }

    skyShader = loadShader("sky.vert", "sky.frag");
    if (!skyShader) {
        fprintf(stderr, "Failed to load sky shaders\n");
        return 1;
    }

    if (!(grassTexture = LoadTexBMP("textures/grass.bmp"))) {
        fprintf(stderr, "Failed to load grass texture\n");
        return 1;
    }
    if (!(rockTexture = LoadTexBMP("textures/rock.bmp"))) {
        fprintf(stderr, "Failed to load rock texture\n");
        return 1;
    }
    if (!(sandTexture = LoadTexBMP("textures/sand.bmp"))) {
        fprintf(stderr, "Failed to load sand texture\n");
        return 1;
    }

    generateTrees(trees, terrain);

    setupLighting();

    float mSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0f);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(NULL);

    glutMainLoop();

    freeParticleSystem(snowSystem);
    freeTreeSystem(trees);
    freeTerrain(terrain);
    freeCloudSystem(cloudSystem);
    deleteShader(terrainShader);
    deleteShader(skyShader);
    freeCamera(camera);

    return 0;
}