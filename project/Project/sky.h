// sky.h - Celestial object system for sun, moon, and sky rendering
#ifndef CELESTIAL_H
#define CELESTIAL_H

// Celestial object system inspired by:
// - Real-Time Rendering 4th Edition, Chapter 10: Local Illumination
// - Enhanced with ChatGPT assistance for day/night cycle
typedef struct {
    float position[3];
    float size;
    float brightness;
    float color[4];
} SkyObject;

typedef struct {
    SkyObject sun;
    SkyObject moon;
    // Add more fields if needed for stars, planets, etc.
} SkySystem;

void skySystemInit(SkySystem* sky);
void skySystemUpdate(SkySystem* sky, float dayTime);
void skySystemRenderSunAndMoon(SkySystem* sky, float dayTime);
void skySystemUpdateLighting(SkySystem* sky);
void skySystemDestroy(SkySystem* sky);

#endif