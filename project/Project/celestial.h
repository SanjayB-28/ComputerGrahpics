#ifndef CELESTIAL_H
#define CELESTIAL_H

#include "terrain.h"

// Celestial object system inspired by:
// - Real-Time Rendering 4th Edition, Chapter 10: Local Illumination
// - Enhanced with ChatGPT assistance for day/night cycle
typedef struct {
    float x, y, z;
    float size;
    float brightness;
    float color[4];
} CelestialObject;

void initCelestialObjects(void);
void renderSunAndMoon(float dayTime);
void updateCelestialObjects(float dayTime);
void updateLighting(CelestialObject* sun, CelestialObject* moon);
void cleanupCelestialObjects(void);

#endif