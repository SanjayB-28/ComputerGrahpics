#include "CSCIx229.h"
#include "sky.h"
#include "landscape.h"

static GLUquadric* quadric = NULL;

// Initialize sky system with sun and moon properties
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

// Render individual sky object with emission lighting
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

// Update sun and moon positions based on time of day
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

// Update global lighting based on sun/moon blend
void skySystemUpdateLighting(SkySystem* sky) {
    float sunB = sky->sun.brightness;
    float moonB = sky->moon.brightness;
    float sunBlend = sunB / (sunB + moonB + 1e-5f);

    float sunPos[4] = { sky->sun.position[0], sky->sun.position[1], sky->sun.position[2], 0.0f };
    float sunAmbient[4]  = { 0.5f, 0.5f, 0.45f, 1.0f };
    float sunDiffuse[4]  = { 1.0f, 0.95f, 0.85f, 1.0f };
    float sunSpecular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };

    float moonPos[4] = { sky->moon.position[0], sky->moon.position[1], sky->moon.position[2], 0.0f };
    float moonAmbient[4]  = { 0.07f, 0.07f, 0.12f, 1.0f };
    float moonDiffuse[4]  = { 0.2f, 0.2f, 0.3f, 1.0f };
    float moonSpecular[4] = { 0.1f, 0.1f, 0.15f, 1.0f };

    float lightPos[4], ambient[4], diffuse[4], specular[4];
    for (int i = 0; i < 4; ++i) {
        lightPos[i] = sunBlend * sunPos[i] + (1.0f - sunBlend) * moonPos[i];
        ambient[i]  = sunBlend * sunAmbient[i]  + (1.0f - sunBlend) * moonAmbient[i];
        diffuse[i]  = sunBlend * sunDiffuse[i]  + (1.0f - sunBlend) * moonDiffuse[i];
        specular[i] = sunBlend * sunSpecular[i] + (1.0f - sunBlend) * moonSpecular[i];
    }

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

// Render sun and moon with updated positions and lighting
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