#include "objects_render.h"
#include "fractal_tree.h"
#include "boulder.h"
#include "CSCIx229.h"

static float getSlopeAt(Landscape* landscape, float x, float z);

TreeInstance* treeInstances = NULL;
int numTrees = 0;

extern float treeSwayAngle;
extern float waterLevel;

void freeLandscapeObjects() {
    if (treeInstances) {
        free(treeInstances);
        treeInstances = NULL;
        numTrees = 0;
    }
}

void initLandscapeObjects(Landscape* landscape) {
    freeLandscapeObjects();
    if (!landscape) return;
    ObjectPlacementParams treeParams = {
        .minSlope = 0.0f,
        .maxSlope = 0.35f,
        .minHeight = waterLevel + 1.5f,
        .maxHeight = LANDSCAPE_HEIGHT * 1.2f,
        .minDistanceFromWater = 1.0f,
        .density = 15
    };
    int grid = treeParams.density;
    int maxTrees = grid * grid;
    treeInstances = (TreeInstance*)malloc(sizeof(TreeInstance) * maxTrees);
    numTrees = 0;
    float halfScale = LANDSCAPE_SCALE * 0.5f * 0.95f;
    float step = (LANDSCAPE_SCALE * 0.95f) / (float)grid;
    for (int i = 0; i < grid; ++i) {
        for (int j = 0; j < grid; ++j) {
            float x = -halfScale + i * step + (rand()/(float)RAND_MAX - 0.5f) * step * 0.5f;
            float z = -halfScale + j * step + (rand()/(float)RAND_MAX - 0.5f) * step * 0.5f;
            float y = landscapeGetHeight(landscape, x, z);
            float slope = getSlopeAt(landscape, x, z);
            if (y < treeParams.minHeight) continue;
            if (y > treeParams.maxHeight) continue;
            if (slope < treeParams.minSlope || slope > treeParams.maxSlope) continue;
            if (y - waterLevel < treeParams.minDistanceFromWater) continue;
            float scale = 1.8f + (rand()/(float)RAND_MAX) * 2.2f;
            int depth = 4 + rand() % 2;
            float rotation = (rand()/(float)RAND_MAX) * 360.0f;
            unsigned int branchBias = rand();
            int leafColorIndex = rand() % 8;
            treeInstances[numTrees++] = (TreeInstance){x, y, z, scale, depth, rotation, branchBias, leafColorIndex};
        }
    }
}

static float getSlopeAt(Landscape* landscape, float x, float z) {
    float nx = (x / LANDSCAPE_SCALE + 0.5f) * (LANDSCAPE_SIZE - 1);
    float nz = (z / LANDSCAPE_SCALE + 0.5f) * (LANDSCAPE_SIZE - 1);
    int ix = (int)nx;
    int iz = (int)nz;
    if (ix < 0) ix = 0;
    if (iz < 0) iz = 0;
    if (ix >= LANDSCAPE_SIZE-1) ix = LANDSCAPE_SIZE-2;
    if (iz >= LANDSCAPE_SIZE-1) iz = LANDSCAPE_SIZE-2;
    int idx = iz * LANDSCAPE_SIZE + ix;
    float* n = &landscape->normals[idx*3];
    float slope = acosf(fminf(fmaxf(n[1], -1.0f), 1.0f));
    return slope / (float)M_PI;
}

void renderLandscapeObjects(Landscape* landscape) {
    if (!landscape || !treeInstances) return;
    for (int i = 0; i < numTrees; ++i) {
        TreeInstance* t = &treeInstances[i];
        glPushMatrix();
        glTranslatef(t->x, t->y, t->z);
        glRotatef(t->rotation, 0, 1, 0);
        float sway = treeSwayAngle + (t->branchBias % 360) * 0.01f;
        glRotatef(sway, 0, 0, 1);
        fractalTreeDraw(0, 0, 0, t->scale, t->depth, t->branchBias, t->leafColorIndex);
        glPopMatrix();
    }

    renderBoulders();
} 