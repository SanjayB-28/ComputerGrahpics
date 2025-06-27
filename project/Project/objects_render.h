#pragma once
#include "landscape.h"

#ifdef __cplusplus
extern "C" {
#endif

// Parameters for object placement (can be extended for more object types)
typedef struct {
    float minSlope;
    float maxSlope;
    float minHeight;
    float maxHeight;
    float minDistanceFromWater;
    int   density; // objects per terrain area unit
} ObjectPlacementParams;

typedef struct {
    float x, y, z;
    float scale;
    int depth;
    float rotation; // Y-axis rotation in degrees
    unsigned int branchBias; // Extra randomizer for branching
    int leafColorIndex; // Per-tree leaf color
} TreeInstance;

// --- Boulder System ---
typedef struct {
    float x, y, z;
    float scale;
    float rotation;
    unsigned int shapeSeed;
    int colorIndex; // Per-boulder color
} BoulderInstance;

// Render all procedural objects (trees, rocks, etc.) on the landscape
void renderLandscapeObjects(Landscape* landscape);

void initLandscapeObjects(Landscape* landscape);
void freeLandscapeObjects();

void renderBoulders(Landscape* landscape);
void initBoulders(Landscape* landscape);
void freeBoulders();

void boulderShaderInit();

void boulderDraw(float x, float y, float z, float scale, float rotation, unsigned int shapeSeed, int colorIndex);

// Expose treeInstances and numTrees for collision checks in boulder.c
extern TreeInstance* treeInstances;
extern int numTrees;

#ifdef __cplusplus
}
#endif 