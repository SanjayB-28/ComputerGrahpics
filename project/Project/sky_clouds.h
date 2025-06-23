// sky_clouds.h - Cloud system data structures and API for procedural sky rendering
#ifndef CLOUDS_H
#define CLOUDS_H

#define MAX_CLOUDS 200
#define CLOUD_PARTICLES 8

// Cloud system data structures adapted from:
// - GPU Gems 2 Chapter 15: Realistic Cloud Rendering
// - Modified with Claude assistance for real-time animation
typedef struct {
    float x, y, z;
    float size;
    float alpha;
    float speed;
    float variation;
} SkyCloud;

typedef struct {
    SkyCloud clouds[MAX_CLOUDS];
    int cloudCount;
    float baseHeight;
    float windSpeed;
} SkyCloudSystem;

SkyCloudSystem* skyCloudSystemCreate(float baseHeight);
void skyCloudSystemUpdate(SkyCloudSystem* cs, float deltaTime, float dayTime);
void skyCloudSystemRender(SkyCloudSystem* cs, float dayTime);
void skyCloudSystemDestroy(SkyCloudSystem* cs);

#endif