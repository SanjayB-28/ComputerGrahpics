#include "celestial.h"
#include <math.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

static CelestialObject sun;
static CelestialObject moon;
static GLUquadric* quadric = NULL;

void initCelestialObjects(void) {
    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    
    sun.size = TERRAIN_SCALE * 0.08f;
    sun.color[0] = 1.0f;
    sun.color[1] = 0.92f;
    sun.color[2] = 0.55f;
    sun.color[3] = 0.95f;
    
    moon.size = TERRAIN_SCALE * 0.06f;
    moon.color[0] = 0.95f;
    moon.color[1] = 0.95f;
    moon.color[2] = 1.0f;
    moon.color[3] = 0.9f;
}

void renderCelestialObject(CelestialObject* obj) {
    if (obj->brightness <= 0.0f) return;
    
    glPushMatrix();
    glTranslatef(obj->x, obj->y, obj->z);
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    float glowSize = obj->size * 1.8f;
    float glowIntensity = (obj == &sun) ? obj->brightness * 0.8f : obj->brightness * 0.5f;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (int layer = 0; layer < 3; layer++) {
        float layerSize = glowSize * (1.0f - layer * 0.2f);
        float layerAlpha = glowIntensity * (1.0f - layer * 0.3f);
        
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(obj->color[0], obj->color[1], obj->color[2], 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        
        glColor4f(obj->color[0], obj->color[1], obj->color[2], layerAlpha);
        for (int i = 0; i <= 32; i++) {
            float angle = i * (2 * M_PI / 32);
            glVertex3f(cos(angle) * layerSize, sin(angle) * layerSize, 0.0f);
        }
        glEnd();
    }
    
    glColor4f(obj->color[0], obj->color[1], obj->color[2], 
              obj->brightness * (obj == &sun ? 1.0f : 0.95f));
              
    glEnable(GL_LIGHTING);
    float emission[] = {obj->color[0] * obj->brightness, 
                       obj->color[1] * obj->brightness,
                       obj->color[2] * obj->brightness, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    
    gluSphere(quadric, obj->size, 32, 32);
    
    float noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}

void updateCelestialObjects(float dayTime) {
    float timeNormalized = dayTime / 24.0f;
    float angle = (timeNormalized - 0.25f) * 2 * M_PI;
    float height = TERRAIN_SCALE * 2.0;
    float radius = TERRAIN_SCALE * 2.8;
    
    sun.x = radius * cos(angle);
    sun.y = height * sin(angle);
    sun.z = 0;
    
    float sunHeight = sin(angle);
    sun.brightness = fmax(0.0f, sunHeight * 1.2f);
    
    moon.x = radius * cos(angle + M_PI);
    moon.y = height * sin(angle + M_PI);
    moon.z = 0;
    
    moon.brightness = fmax(0.0f, -sunHeight) * 0.9f;
}

void updateLighting(CelestialObject* sun, CelestialObject* moon) {
    float lightPos[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
    
    float sunHeight = sun->y / (TERRAIN_SCALE * 2.0);
    
    if (sun->brightness > moon->brightness) {
        lightPos[0] = sun->x;
        lightPos[1] = sun->y;
        lightPos[2] = sun->z;
        lightPos[3] = 0.0f;
        
        float ambientStrength = 0.2f + 0.3f * sunHeight;
        ambient[0] = ambientStrength;
        ambient[1] = ambientStrength;
        ambient[2] = ambientStrength * 0.9f;
        ambient[3] = 1.0f;
        
        float intensity = 0.6f + 0.4f * sunHeight;
        diffuse[0] = intensity * 1.0f;
        diffuse[1] = intensity * 0.95f;
        diffuse[2] = intensity * 0.85f;
        diffuse[3] = 1.0f;
        
        specular[0] = intensity * 0.7f;
        specular[1] = intensity * 0.7f;
        specular[2] = intensity * 0.7f;
        specular[3] = 1.0f;
    } else {
        lightPos[0] = moon->x;
        lightPos[1] = moon->y;
        lightPos[2] = moon->z;
        lightPos[3] = 0.0f;
        
        float ambientStrength = 0.1f + 0.1f * moon->brightness;
        ambient[0] = ambientStrength * 0.7f;
        ambient[1] = ambientStrength * 0.7f;
        ambient[2] = ambientStrength * 0.9f;
        ambient[3] = 1.0f;
        
        float intensity = 0.3f * moon->brightness;
        diffuse[0] = intensity * 0.8f;
        diffuse[1] = intensity * 0.8f;
        diffuse[2] = intensity * 1.0f;
        diffuse[3] = 1.0f;
        
        specular[0] = intensity * 0.4f;
        specular[1] = intensity * 0.4f;
        specular[2] = intensity * 0.5f;
        specular[3] = 1.0f;
    }
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

void renderSunAndMoon(float dayTime) {
    updateCelestialObjects(dayTime);
    updateLighting(&sun, &moon);
    renderCelestialObject(&sun);
    renderCelestialObject(&moon);
}

void cleanupCelestialObjects(void) {
    if (quadric) {
        gluDeleteQuadric(quadric);
        quadric = NULL;
    }
}