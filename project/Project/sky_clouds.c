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

SkyCloudSystem* skyCloudSystemCreate(float baseHeight) {
    SkyCloudSystem* cs = (SkyCloudSystem*)malloc(sizeof(SkyCloudSystem));
    if (!cs) return NULL;
    cs->cloudCount = 120;
    cs->baseHeight = baseHeight + 30.0f;
    cs->windSpeed = 0.15f;
    cs->smoothBrightness = 1.0f;
    cs->smoothAlpha = 1.0f;
    for (int i = 0; i < cs->cloudCount; i++) {
        SkyCloud* cloud = &cs->clouds[i];
        cloud->x = ((float)rand()/RAND_MAX - 0.5f) * LANDSCAPE_SCALE * 2.0f;
        cloud->z = ((float)rand()/RAND_MAX - 0.5f) * LANDSCAPE_SCALE * 2.0f;
        cloud->y = cs->baseHeight + 60.0f + ((float)rand()/RAND_MAX) * 30.0f;
        cloud->size = 30.0f + ((float)rand()/RAND_MAX) * 20.0f;
        cloud->alpha = 0.3f + ((float)rand()/RAND_MAX) * 0.2f;
        cloud->speed = 0.05f + ((float)rand()/RAND_MAX) * 0.05f;
        cloud->variation = ((float)rand()/RAND_MAX);
    }
    return cs;
}

void getCloudColor(float dayTime, float* color) {
    float t = dayTime / 24.0f;
    float sunrise[3] = {1.0f, 0.7f, 0.5f};
    float midday[3] = {1.0f, 1.0f, 1.0f};
    float sunset[3] = {1.0f, 0.7f, 0.5f};
    float night[3] = {0.5f, 0.5f, 0.6f};
    float colorOut[3];
    if (t < 0.2f) {
        float f = t / 0.2f;
        for (int i = 0; i < 3; ++i)
            colorOut[i] = night[i] * (1.0f - f) + sunrise[i] * f;
    } else if (t < 0.3f) {
        float f = (t - 0.2f) / 0.1f;
        for (int i = 0; i < 3; ++i)
            colorOut[i] = sunrise[i] * (1.0f - f) + midday[i] * f;
    } else if (t < 0.7f) {
        for (int i = 0; i < 3; ++i)
            colorOut[i] = midday[i];
    } else if (t < 0.8f) {
        float f = (t - 0.7f) / 0.1f;
        for (int i = 0; i < 3; ++i)
            colorOut[i] = midday[i] * (1.0f - f) + sunset[i] * f;
    } else {
        float f = (t - 0.8f) / 0.2f;
        for (int i = 0; i < 3; ++i)
            colorOut[i] = sunset[i] * (1.0f - f) + night[i] * f;
    }
    color[0] = colorOut[0];
    color[1] = colorOut[1];
    color[2] = colorOut[2];
}

void skyCloudSystemUpdate(SkyCloudSystem* cs, float deltaTime, float dayTime) {
    float timeNormalized = dayTime / 24.0f;
    float sunHeight = sin((timeNormalized - 0.25f) * 2 * M_PI);
    float dayFactor = fmaxf(0.0f, sunHeight);
    float targetBrightness = 0.85f + 0.25f * dayFactor;
    float targetAlpha = 0.7f + 0.6f * dayFactor;
    float lerpSpeed = fminf(1.0f, deltaTime * 2.0f); // Smooth over ~0.5s
    cs->smoothBrightness += (targetBrightness - cs->smoothBrightness) * lerpSpeed;
    cs->smoothAlpha += (targetAlpha - cs->smoothAlpha) * lerpSpeed;
    float buffer = 40.0f;
    for (int i = 0; i < cs->cloudCount; i++) {
        SkyCloud* cloud = &cs->clouds[i];
        cloud->x += cloud->speed * cs->windSpeed * deltaTime;
        // Wrap in X with buffer
        if (cloud->x > LANDSCAPE_SCALE + buffer) {
            cloud->x = -LANDSCAPE_SCALE - buffer;
            cloud->z = ((float)rand()/RAND_MAX - 0.5f) * LANDSCAPE_SCALE * 2.0f;
            cloud->size = 30.0f + ((float)rand()/RAND_MAX) * 20.0f;
            cloud->alpha = 0.15f + ((float)rand()/RAND_MAX) * 0.15f;
        }
        if (cloud->x < -LANDSCAPE_SCALE - buffer) {
            cloud->x = LANDSCAPE_SCALE + buffer;
            cloud->z = ((float)rand()/RAND_MAX - 0.5f) * LANDSCAPE_SCALE * 2.0f;
            cloud->size = 30.0f + ((float)rand()/RAND_MAX) * 20.0f;
            cloud->alpha = 0.15f + ((float)rand()/RAND_MAX) * 0.15f;
        }
    }
}

void skyCloudSystemRender(SkyCloudSystem* cs, float dayTime) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    float brightness = cs->smoothBrightness;
    float globalAlpha = cs->smoothAlpha;
    float cloudColor[3];
    getCloudColor(dayTime, cloudColor);
    for (int i = 0; i < cs->cloudCount; i++) {
        SkyCloud* cloud = &cs->clouds[i];
        for (int layer = 0; layer < 6; layer++) {
            float t = (float)layer / 5.0f;
            float bulge = sinf(t * M_PI) * 1.5f;
            float layerSize = cloud->size * (1.0f - t * 0.25f);
            float layerAlpha = cloud->alpha * (0.6f - t * 0.1f);
            layerAlpha *= globalAlpha;
            float layerHeight = cloud->y + bulge * 12.0f;
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(cloudColor[0] * brightness, cloudColor[1] * brightness, cloudColor[2] * brightness, layerAlpha);
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

void skyCloudSystemDestroy(SkyCloudSystem* cs) {
    if (cs) {
        free(cs);
    }
}