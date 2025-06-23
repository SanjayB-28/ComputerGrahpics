// ---------------------------------------------
// sky_clouds.h - Cloud system for procedural sky rendering
// ---------------------------------------------

#ifndef CLOUDS_H
#define CLOUDS_H

#define MAX_CLOUDS 200          // Maximum cloud formations
#define CLOUD_PARTICLES 8       // Particles per cloud

// --- Cloud data structures ---
/* Based on GPU Gems 2 Ch. 15 */

/* Individual cloud properties */
typedef struct {
    float x, y, z;               // Position
    float size;                  // Diameter
    float alpha;                 // Transparency
    float speed;                 // Movement speed
    float variation;             // Shape variation
} SkyCloud;

/* Cloud system management */
typedef struct {
    SkyCloud clouds[MAX_CLOUDS]; // Cloud instances
    int cloudCount;              // Active clouds
    float baseHeight;            // Baseline altitude
    float windSpeed;             // Wind multiplier
} SkyCloudSystem;

// --- Cloud system API ---
/* Creates cloud system with random formations */
SkyCloudSystem* skyCloudSystemCreate(float baseHeight);

/* Updates cloud positions based on wind */
void skyCloudSystemUpdate(SkyCloudSystem* cs, float deltaTime, float dayTime);

/* Renders clouds as billboards */
void skyCloudSystemRender(SkyCloudSystem* cs, float dayTime);

/* Frees cloud system memory */
void skyCloudSystemDestroy(SkyCloudSystem* cs);

#endif