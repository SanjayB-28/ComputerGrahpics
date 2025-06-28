#include "sky.h"
#include "landscape.h"
#include "CSCIx229.h"

static GLUquadric* quadric = NULL;

void skySystemInit(SkySystem* sky) {
    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    sky->sun.size = LANDSCAPE_SCALE * 0.18f;
    sky->sun.color[0] = 1.0f;
    sky->sun.color[1] = 0.92f;
    sky->sun.color[2] = 0.55f;
    sky->sun.color[3] = 0.95f;
    sky->moon.size = LANDSCAPE_SCALE * 0.13f;
    sky->moon.color[0] = 0.95f;
    sky->moon.color[1] = 0.95f;
    sky->moon.color[2] = 1.0f;
    sky->moon.color[3] = 0.9f;
}

static void renderSkyObject(SkyObject* obj) {
    if (obj->brightness <= 0.0f) return;
    glPushMatrix();
    glTranslatef(obj->position[0], obj->position[1], obj->position[2]);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glColor4f(obj->color[0], obj->color[1], obj->color[2], obj->brightness);
    glEnable(GL_LIGHTING);
    float emission[] = {obj->color[0] * obj->brightness, 
                       obj->color[1] * obj->brightness,
                       obj->color[2] * obj->brightness, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    gluSphere(quadric, obj->size, 32, 32);
    float noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}

void skySystemUpdate(SkySystem* sky, float dayTime) {
    float timeNormalized = dayTime / 24.0f;
    float angle = (timeNormalized - 0.25f) * 2 * M_PI;
    float height = LANDSCAPE_SCALE * 1.2f;
    float radius = LANDSCAPE_SCALE * 1.4f;
    sky->sun.position[0] = radius * cos(angle);
    sky->sun.position[1] = height * sin(angle);
    sky->sun.position[2] = 0;
    float sunHeight = sin(angle);
    sky->sun.brightness = fmax(0.0f, sunHeight * 1.2f);
    sky->moon.position[0] = radius * cos(angle + M_PI);
    sky->moon.position[1] = height * sin(angle + M_PI);
    sky->moon.position[2] = 0;
    sky->moon.brightness = fmax(0.0f, -sunHeight) * 0.9f;
}

void skySystemUpdateLighting(SkySystem* sky) {
    float lightPos[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float sunHeight = sky->sun.position[1] / (LANDSCAPE_SCALE * 2.0);
    if (sky->sun.brightness > sky->moon.brightness) {
        lightPos[0] = sky->sun.position[0];
        lightPos[1] = sky->sun.position[1];
        lightPos[2] = sky->sun.position[2];
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
        lightPos[0] = sky->moon.position[0];
        lightPos[1] = sky->moon.position[1];
        lightPos[2] = sky->moon.position[2];
        lightPos[3] = 0.0f;
        float ambientStrength = 0.1f + 0.1f * sky->moon.brightness;
        ambient[0] = ambientStrength * 0.7f;
        ambient[1] = ambientStrength * 0.7f;
        ambient[2] = ambientStrength * 0.9f;
        ambient[3] = 1.0f;
        float intensity = 0.3f * sky->moon.brightness;
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

void skySystemRenderSunAndMoon(SkySystem* sky, float dayTime) {
    skySystemUpdate(sky, dayTime);
    skySystemUpdateLighting(sky);
    renderSkyObject(&sky->sun);
    renderSkyObject(&sky->moon);
}

void skySystemDestroy(SkySystem* sky) {
    if (quadric) {
        gluDeleteQuadric(quadric);
        quadric = NULL;
    }
}