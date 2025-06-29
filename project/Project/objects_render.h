#pragma once
#include "landscape.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float minSlope;
    float maxSlope;
    float minHeight;
    float maxHeight;
    float minDistanceFromWater;
    int   density;
} ObjectPlacementParams;

typedef struct {
    float x, y, z;
    float scale;
    int depth;
    float rotation;
    unsigned int branchBias;
    int leafColorIndex;
} TreeInstance;

void renderLandscapeObjects(Landscape* landscape);
void initLandscapeObjects(Landscape* landscape);
void freeLandscapeObjects();

extern TreeInstance* treeInstances;
extern int numTrees;

#ifdef __cplusplus
}
#endif 