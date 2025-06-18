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
} Cloud;

typedef struct {
    Cloud clouds[MAX_CLOUDS];
    int cloudCount;
    float baseHeight;
    float windSpeed;
} CloudSystem;

CloudSystem* createCloudSystem(float baseHeight);
void updateClouds(CloudSystem* cs, float deltaTime, float dayTime);
void renderClouds(CloudSystem* cs, float dayTime);
void freeCloudSystem(CloudSystem* cs);

#endif