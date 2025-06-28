#pragma once
#include "landscape.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, z;
    float scale;
    float rotation;
    unsigned int shapeSeed;
    int colorIndex;
} BoulderInstance;

void initBoulders(Landscape* landscape);
void freeBoulders();
void renderBoulders();
void boulderShaderInit();
void boulderDraw(float x, float y, float z, float scale, float rotation, unsigned int shapeSeed, int colorIndex);

#ifdef __cplusplus
}
#endif 