// ---------------------------------------------
// sky_clouds.c - Procedural cloud system
// ---------------------------------------------

#include "sky_clouds.h"
#include <stdlib.h>
#include <math.h>
#include "landscape.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

// --- Cloud system creation ---
/* Creates a cloud system with random formations */
SkyCloudSystem* skyCloudSystemCreate(float baseHeight) {
    SkyCloudSystem* cs = (SkyCloudSystem*)malloc(sizeof(SkyCloudSystem));
    if (!cs) return NULL;
    cs->cloudCount = 120;
    cs->baseHeight = baseHeight;
    cs->windSpeed = 0.15f;
    for (int i = 0; i < cs->cloudCount; i++) {
        SkyCloud* cloud = &cs->clouds[i];
        cloud->x = ((float)rand()/RAND_MAX - 0.5f) * LANDSCAPE_SCALE * 1.2f;
        cloud->z = ((float)rand()/RAND_MAX - 0.5f) * LANDSCAPE_SCALE * 1.2f;
        cloud->y = baseHeight + 60.0f + ((float)rand()/RAND_MAX) * 10.0f;
        cloud->size = 20.0f + ((float)rand()/RAND_MAX) * 15.0f;
        cloud->alpha = 0.3f + ((float)rand()/RAND_MAX) * 0.2f;
        cloud->speed = 0.05f + ((float)rand()/RAND_MAX) * 0.05f;
        cloud->variation = ((float)rand()/RAND_MAX);
    }
    return cs;
}

// --- Cloud update logic ---
/* Updates cloud positions and recycles clouds outside bounds */
void skyCloudSystemUpdate(SkyCloudSystem* cs, float deltaTime, float dayTime) {
    for (int i = 0; i < cs->cloudCount; i++) {
        SkyCloud* cloud = &cs->clouds[i];
        cloud->x += cloud->speed * cs->windSpeed * deltaTime;
        if (cloud->x > LANDSCAPE_SCALE) {
            cloud->x = -LANDSCAPE_SCALE;
            cloud->z = ((float)rand()/RAND_MAX - 0.5f) * LANDSCAPE_SCALE;
            cloud->size = 30.0f + ((float)rand()/RAND_MAX) * 20.0f;
            cloud->alpha = 0.15f + ((float)rand()/RAND_MAX) * 0.15f;
        }
    }
}

// --- Cloud rendering ---
/* Renders clouds as multi-layered billboards */
void skyCloudSystemRender(SkyCloudSystem* cs, float dayTime) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    float timeNormalized = dayTime / 24.0f;
    float sunHeight = sin((timeNormalized - 0.25f) * 2 * M_PI);
    float brightness = 0.95f + 0.05f * fmaxf(sunHeight, 0.0f);
    for (int i = 0; i < cs->cloudCount; i++) {
        SkyCloud* cloud = &cs->clouds[i];
        for (int layer = 0; layer < 6; layer++) {
            float t = (float)layer / 5.0f;
            float bulge = sinf(t * M_PI) * 1.5f;
            float layerSize = cloud->size * (1.0f - t * 0.25f);
            float layerAlpha = cloud->alpha * (0.6f - t * 0.1f);
            float layerHeight = cloud->y + bulge * 12.0f;
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(brightness, brightness, brightness, layerAlpha);
            glVertex3f(cloud->x, layerHeight + bulge * 3.0f, cloud->z);
            int segments = 16;
            for (int j = 0; j <= segments; j++) {
                float angle = (float)j / segments * 2 * M_PI;
                float radialDist = 0.85f + 0.15f * sinf(angle * 2 + cloud->x * 0.1f);
                float variation = 1.0f + 0.2f * cosf(angle * 3 + cloud->z * 0.1f);
                float px = cloud->x + cosf(angle) * layerSize * variation * radialDist;
                float pz = cloud->z + sinf(angle) * layerSize * variation * radialDist;
                float py = layerHeight + bulge * (1.0f - radialDist) * 6.0f + sinf(angle * 2) * bulge * 3.0f;
                glVertex3f(px, py, pz);
            }
            glEnd();
        }
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

// --- Cloud system cleanup ---
/* Frees cloud system memory */
void skyCloudSystemDestroy(SkyCloudSystem* cs) {
    if (cs) {
        free(cs);
    }
}