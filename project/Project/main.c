#include "CSCIx229.h"
#include "landscape.h"
#include "shaders.h"
#include "sky.h"
#include "sky_clouds.h"
#include "camera.h"
#include "fractal_tree.h"
#include "objects_render.h"
#include "particles.h"
#include "grass.h"
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

static SkyCloudSystem* cloudSystem = NULL;
static SkySystem skySystemInstance;

GLuint grassTexture = 0;
GLuint rockTexture = 0;
GLuint sandTexture = 0;
GLuint boulderTexture = 0;
GLuint barkTexture = 0;
GLuint leafTexture = 0;
int terrainShader = 0;
int skyShader = 0;

static int th = 45;
static int ph = 10;
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

Landscape* landscape = NULL;

static float lastTime = 0;
static float deltaTime = 0;

float fogDensity = 0.005f;
int fogEnabled = 0;

static int animateTime = 1;
static float timeSpeed = 1.0f;

static ViewCamera* camera = NULL;
float asp;
static int lastX = 0, lastY = 0;
static int mouseButtons = 0;

static float lastOrbitYaw = 45.0f, lastOrbitPitch = 10.0f, lastOrbitDistance = 70.0f;
static int lastOrbitTh = 45, lastOrbitPh = 10;
static float lastOrbitDim = 70.0f;
static float lastFPPos[3] = {0.0f, 2.0f, 0.0f};
static float lastFPYaw = 45.0f, lastFPPitch = 10.0f;
static const float INIT_ORBIT_YAW = 45.0f, INIT_ORBIT_PITCH = 10.0f, INIT_ORBIT_DISTANCE = 70.0f;
static const int INIT_ORBIT_TH = 45, INIT_ORBIT_PH = 10;
static const float INIT_ORBIT_DIM = 70.0f;
static const float INIT_FP_POS[3] = {0.0f, 2.0f, 0.0f};
static const float INIT_FP_YAW = 45.0f, INIT_FP_PITCH = 10.0f;

float treeSwayAngle = 0.0f;

static int snowOn = 0;

void reshape(int width, int height);
void display();
void special(int key, int x, int y);
void keyboard(unsigned char key, int x, int y);
void idle();

void reshape(int width, int height) {
    asp = (height>0) ? (double)width/height : 1;
    glViewport(0,0, RES*width,RES*height);
    if (camera) {
        viewCameraSetProjection(camera, 55.0f, asp, dim/4, dim*4);
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

void updateDynamicLighting(float dayTime) {
    float timeNormalized = dayTime / 24.0f;
    float sunAngle = (timeNormalized - 0.25f) * 2 * M_PI;
    float sunHeight = sin(sunAngle);
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
    treeSwayAngle = sin(windTime) * 8.0f;
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

void updateFog(float dayTime) {
    float timeNormalized = dayTime / 24.0f;
    float sunAngle = (timeNormalized - 0.25f) * 2 * M_PI;
    float sunHeight = sin(sunAngle);
    float baseDensity;
    float fogColor[4];
    if (sunHeight > 0) {
        baseDensity = 0.008f;
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
    } else {
        glDisable(GL_FOG);
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
    gluLookAt(camera->position[0], camera->position[1], camera->position[2],
              camera->lookAt[0], camera->lookAt[1], camera->lookAt[2],
              camera->upVec[0], camera->upVec[1], camera->upVec[2]);
    skySystemRenderSunAndMoon(&skySystemInstance, dayTime);
    updateFog(dayTime);
    updateDynamicLighting(dayTime);
    // --- Render clouds first, with depth offset and no depth writes ---
    if (cloudSystem) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 100.0f);
        glDepthMask(GL_FALSE);
        skyCloudSystemRender(cloudSystem, dayTime);
        glDepthMask(GL_TRUE);
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
    // --- Now render the rest of the scene ---
    landscapeRender(landscape, 0);
    // Render animated grass after terrain, before trees/objects
    // Compute sun direction and ambient for grass shader
    float timeNormalized = dayTime / 24.0f;
    float sunAngle = (timeNormalized - 0.25f) * 2 * M_PI;
    float sunHeight = sin(sunAngle);
    float sunX = 500 * cos(sunAngle);
    float sunY = lightHeight * sunHeight;
    float sunZ = 0;
    float sunDir[3];
    float len = sqrtf(sunX*sunX + sunY*sunY + sunZ*sunZ);
    sunDir[0] = sunX / len;
    sunDir[1] = sunY / len;
    sunDir[2] = sunZ / len;
    float ambient[3];
    if (sunHeight > 0) {
        ambient[0] = 0.15f + sunHeight * 0.15f;
        ambient[1] = 0.15f + sunHeight * 0.15f;
        ambient[2] = 0.15f + sunHeight * 0.15f;
    } else {
        ambient[0] = 0.02f;
        ambient[1] = 0.02f;
        ambient[2] = 0.04f;
    }
    grassSystemRender(dayTime, windStrength, sunDir, ambient);
    renderLandscapeObjects(landscape);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    landscapeRenderWater(waterLevel, landscape, dayTime);
    glDepthMask(GL_TRUE);
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
    glColor3f(1,1,1);
    glWindowPos2i(5, glutGet(GLUT_WINDOW_HEIGHT) - 20);
    Print("Time: %02d:%02d  Weather: Fall", 
          (int)dayTime, (int)((dayTime-(int)dayTime)*60));
    int y = 5;
    glWindowPos2i(5, y);
    Print("Angle=%d,%d  Dim=%.1f  View=%s   |   Water=%.1f   |   Wireframe=%d   |   Axes=%d   |   TimeAnim: %s  Speed: %.1fx   |   Fog: %s  Snow: %s",
        th, ph, dim, camera->mode == CAMERA_MODE_FREE_ORBIT ? "Free Orbit" : "First Person",
        waterLevel,
        wireframe,
        showAxes,
        animateTime ? "On" : "Off", timeSpeed,
        fogEnabled ? "On" : "Off",
        snowOn ? "On" : "Off");
    glEnable(GL_DEPTH_TEST);
    if (snowOn) {
        particleSystemRender();
    }
    glutSwapBuffers();
}

void initSkyBackground() {
    skyShader = loadShader("shaders/sky_shader.vert", "shaders/sky_shader.frag");
}

void mouse(int button, int state, int x, int y) {
    lastX = x;
    lastY = y;
    if (state == GLUT_DOWN) {
        mouseButtons |= 1<<button;
    } else {
        mouseButtons &= ~(1<<button);
    }
    if (button == 3) {
        if (camera->mode == CAMERA_MODE_FREE_ORBIT) {
            dim = dim * 0.9;
            camera->orbitDistance *= 0.9;
        }
    } else if (button == 4) {
        if (camera->mode == CAMERA_MODE_FREE_ORBIT) {
            dim = dim * 1.1;
            camera->orbitDistance *= 1.1;
        }
    }
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    int dx = x - lastX;
    int dy = y - lastY;
    if (camera->mode == CAMERA_MODE_FREE_ORBIT) {
        if (mouseButtons & 1) {
            th += dx;
            ph += dy;
            camera->horizontalAngle += dx;
            camera->verticalAngle += dy;
            if (ph > 89) ph = 89;
            if (ph < -89) ph = -89;
            if (camera->verticalAngle > 89) camera->verticalAngle = 89;
            if (camera->verticalAngle < -89) camera->verticalAngle = -89;
        }
        else if (mouseButtons & 4) {
            dim *= (1 + dy/100.0);
            camera->orbitDistance *= (1 + dy/100.0);
            if (dim < 1) dim = 1;
            if (camera->orbitDistance < 1) camera->orbitDistance = 1;
        }
    } else {
        if (mouseButtons & 1) {
            viewCameraRotate(camera, dx * 0.5f, -dy * 0.5f);
        }
    }
    lastX = x;
    lastY = y;
    glutPostRedisplay();
}

void special(int key, int x, int y) {
    float deltaTime = 0.016f;
    if (camera && camera->mode == CAMERA_MODE_FREE_ORBIT) {
        switch(key) {
            case GLUT_KEY_RIGHT:
                th += 5;
                camera->horizontalAngle = th;  
                break;
            case GLUT_KEY_LEFT:
                th -= 5;
                camera->horizontalAngle = th;
                break;
            case GLUT_KEY_UP:
                if (ph < 89) {
                    ph += 5;
                    if (ph < 10) ph = 10;
                    camera->verticalAngle = ph; 
                }
                break;
            case GLUT_KEY_DOWN:
                if (ph > -89) {
                    ph -= 5;
                    if (ph < 10) ph = 10;
                    camera->verticalAngle = ph;
                }
                break;
            case GLUT_KEY_PAGE_DOWN:
                dim += 0.1;
                camera->orbitDistance = dim;
                break;
            case GLUT_KEY_PAGE_UP:
                if (dim > 1) {
                    dim -= 0.1;
                    camera->orbitDistance = dim;
                }
                break;
        }
        th %= 360;
        ph %= 360;
        viewCameraUpdateVectors(camera);  
        Project(fov?55:0, asp, dim);
    }
    else {
        switch(key) {
            case GLUT_KEY_RIGHT:
                viewCameraMove(camera, CAMERA_MOVE_RIGHT, deltaTime);
                break;
            case GLUT_KEY_LEFT:
                viewCameraMove(camera, CAMERA_MOVE_LEFT, deltaTime);
                break;
            case GLUT_KEY_UP:
                viewCameraMove(camera, CAMERA_MOVE_FORWARD, deltaTime);
                break;
            case GLUT_KEY_DOWN:
                viewCameraMove(camera, CAMERA_MOVE_BACKWARD, deltaTime);
                break;
        }
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27:
            exit(0);
            break;
        case 'w':
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
            break;
        case '[':
            waterLevel -= 1.0f;
            break;
        case ']':
            waterLevel += 1.0f;
            break;
        case 'a':
            showAxes = !showAxes;
            break;
        case 'r':
            lastOrbitYaw = INIT_ORBIT_YAW;
            lastOrbitPitch = INIT_ORBIT_PITCH;
            lastOrbitDistance = INIT_ORBIT_DISTANCE;
            lastOrbitTh = INIT_ORBIT_TH;
            lastOrbitPh = INIT_ORBIT_PH < 10 ? 10 : INIT_ORBIT_PH;
            lastOrbitDim = INIT_ORBIT_DIM;
            lastFPPos[0] = INIT_FP_POS[0];
            lastFPPos[1] = INIT_FP_POS[1];
            lastFPPos[2] = INIT_FP_POS[2];
            lastFPYaw = INIT_FP_YAW;
            lastFPPitch = INIT_FP_PITCH;
            th = INIT_ORBIT_TH;
            ph = INIT_ORBIT_PH < 10 ? 10 : INIT_ORBIT_PH;
            dim = INIT_ORBIT_DIM;
            waterLevel = -4.0f;
            lightHeight = 250.0f;
            waterSpeed = 1.0f;
            lightSpeed = 1.0f;
            fov = 1;
            camera->horizontalAngle = lastOrbitYaw;
            camera->verticalAngle = lastOrbitPitch;
            camera->orbitDistance = lastOrbitDistance;
            viewCameraSetMode(camera, CAMERA_MODE_FREE_ORBIT);
            viewCameraUpdateVectors(camera);
            break;
        case '1': {
            lastOrbitYaw = camera->horizontalAngle;
            lastOrbitPitch = camera->verticalAngle;
            lastOrbitDistance = camera->orbitDistance;
            lastOrbitTh = th;
            lastOrbitPh = ph < 10 ? 10 : ph;
            lastOrbitDim = dim;
            camera->horizontalAngle = lastFPYaw;
            camera->verticalAngle = lastFPPitch < 5.0f ? 10.0f : lastFPPitch;
            camera->position[0] = lastFPPos[0];
            camera->position[2] = lastFPPos[2];
            float groundHeight = landscapeGetHeight(landscape, camera->position[0], camera->position[2]);
            camera->position[1] = groundHeight + 2.0f;
            viewCameraSetMode(camera, CAMERA_MODE_FIRST_PERSON);
            viewCameraUpdateVectors(camera);
            viewCameraSetProjection(camera, 55.0f, asp, dim/4, dim*4);
            break;
        }
        case '2': {
            lastFPPos[0] = camera->position[0];
            lastFPPos[1] = camera->position[1];
            lastFPPos[2] = camera->position[2];
            lastFPYaw = camera->horizontalAngle;
            lastFPPitch = camera->verticalAngle;
            camera->horizontalAngle = lastOrbitYaw;
            camera->verticalAngle = lastOrbitPitch;
            camera->orbitDistance = lastOrbitDistance;
            th = lastOrbitTh;
            ph = lastOrbitPh < 10 ? 10 : lastOrbitPh;
            dim = lastOrbitDim;
            viewCameraSetMode(camera, CAMERA_MODE_FREE_ORBIT);
            viewCameraUpdateVectors(camera);
            break;
        }
        case 't':
            animateTime = !animateTime;
            glutIdleFunc(animateTime ? idle : NULL);
            break;
        case 'k':
            timeSpeed = fmax(0.1f, timeSpeed - 0.1f);
            break;
        case 'l':
            timeSpeed = fmin(5.0f, timeSpeed + 0.1f);
            break;
        case 'b':
            fogEnabled = !fogEnabled;
            break;
        case 'z':
            if (camera->mode == CAMERA_MODE_FREE_ORBIT) {
                dim -= 5.0f;
                if (dim < 1.0f) dim = 1.0f;
                camera->orbitDistance -= 5.0f;
                if (camera->orbitDistance < 1.0f) camera->orbitDistance = 1.0f;
                viewCameraUpdateVectors(camera);
                Project(fov?55:0, asp, dim);
            }
            break;
        case 'Z':
            if (camera->mode == CAMERA_MODE_FREE_ORBIT) {
                dim += 5.0f;
                camera->orbitDistance += 5.0f;
                viewCameraUpdateVectors(camera);
                Project(fov?55:0, asp, dim);
            }
            break;
        case 'n':
            snowOn = !snowOn;
            particleSystemSetEnabled(snowOn);
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
    viewCameraUpdate(camera, deltaTime);
    updateTreeAnimation();
    if (animateWater) {
        waterTime += deltaTime;
    }
    if (cloudSystem) {
        skyCloudSystemUpdate(cloudSystem, deltaTime, dayTime);
    }
    if (snowOn) {
        particleSystemUpdate(deltaTime);
    }
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Project: Sanjay Baskaran");
    
    landscape = landscapeCreate();
    if (!landscape) {
        fprintf(stderr, "Failed to create landscape\n");
        return 1;
    }
    // Initialize animated grass system
    grassSystemInit(landscape, LANDSCAPE_SCALE, 500000);
    particleSystemUploadHeightmap(landscape->elevationData);
    initLandscapeObjects(landscape);
    initBoulders(landscape);
    camera = viewCameraCreate();
    if (!camera) {
        fprintf(stderr, "Failed to create camera\n");
        return 1;
    }
    
    lastOrbitYaw = INIT_ORBIT_YAW;
    lastOrbitPitch = INIT_ORBIT_PITCH;
    lastOrbitDistance = INIT_ORBIT_DISTANCE;
    lastOrbitTh = INIT_ORBIT_TH;
    lastOrbitPh = INIT_ORBIT_PH < 10 ? 10 : INIT_ORBIT_PH;
    lastOrbitDim = INIT_ORBIT_DIM;
    lastFPPos[0] = INIT_FP_POS[0];
    lastFPPos[2] = INIT_FP_POS[2];
    float groundHeight = landscapeGetHeight(landscape, lastFPPos[0], lastFPPos[2]);
    lastFPPos[1] = groundHeight + 2.0f;
    lastFPYaw = INIT_FP_YAW;
    lastFPPitch = INIT_FP_PITCH;
    camera->horizontalAngle = lastOrbitYaw;
    camera->verticalAngle = lastOrbitPitch;
    camera->orbitDistance = lastOrbitDistance;
    dim = lastOrbitDim;
    viewCameraSetMode(camera, CAMERA_MODE_FREE_ORBIT);
    viewCameraUpdateVectors(camera);
    
    skySystemInit(&skySystemInstance);
    cloudSystem = skyCloudSystemCreate(LANDSCAPE_SCALE * 0.4f);
    if (!cloudSystem) {
        fprintf(stderr, "Failed to create cloud system\n");
        return 1;
    }

    terrainShader = loadShader("shaders/terrain_shader.vert", "shaders/terrain_shader.frag");
    if (!terrainShader) {
        fprintf(stderr, "Failed to load terrain shaders\n");
        return 1;
    }
    skyShader = loadShader("shaders/sky_shader.vert", "shaders/sky_shader.frag");
    if (!skyShader) {
        fprintf(stderr, "Failed to load sky shaders\n");
        return 1;
    }
    if (!(grassTexture = LoadTexBMP("tex/grassland.bmp"))) {
        fprintf(stderr, "Failed to load grass texture\n");
        return 1;
    }
    if (!(rockTexture = LoadTexBMP("tex/rocky.bmp"))) {
        fprintf(stderr, "Failed to load rock texture\n");
        return 1;
    }
    if (!(sandTexture = LoadTexBMP("tex/sandy.bmp"))) {
        fprintf(stderr, "Failed to load sand texture\n");
        return 1;
    }
    if (!(boulderTexture = LoadTexBMP("tex/boulder.bmp"))) {
        fprintf(stderr, "Failed to load boulder texture\n");
        return 1;
    }
    if (!(barkTexture = LoadTexBMP("tex/bark.bmp"))) {
        fprintf(stderr, "Failed to load bark texture\n");
        return 1;
    }
    if (!(leafTexture = LoadTexBMP("tex/leaf.bmp"))) {
        fprintf(stderr, "Failed to load leaf texture\n");
        return 1;
    }
    
    // Initialize fractal tree shaders
    fractalTreeInit();
    boulderShaderInit();
    
    setupLighting();
    float mSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0f);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    
    particleSystemInit(2000.0f, 20000.0f);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(NULL);
    
    glutMainLoop();
    
    landscapeDestroy(landscape);
    freeBoulders();
    freeLandscapeObjects();
    skyCloudSystemDestroy(cloudSystem);
    deleteShader(terrainShader);
    deleteShader(skyShader);
    viewCameraDestroy(camera);
    particleSystemCleanup();
    // Cleanup grass system
    grassSystemCleanup();
    return 0;
}