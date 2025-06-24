// ---------------------------------------------
// objects.c - Forest, rock, shrub, and log systems for procedural landscape objects
// ---------------------------------------------

#include "objects.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

/* Tuning parameters for procedural generation variety */
#define EXTRA_TREE_VARIETY 1
#define COLOR_VARIETY 0.15f
#define SIZE_VARIETY 0.25f
#define TRUNK_VARIETY 0.25f
#define FOLIAGE_VARIETY 0.25f

// --- Forest system implementation ---
/* Allocates and initializes the forest data structure with memory for trees */
ForestSystem* forestSystemCreate() {
    ForestSystem* forest = (ForestSystem*)malloc(sizeof(ForestSystem));
    if (!forest) return NULL;
    forest->instances = (ForestTree*)malloc(MAX_TREES * sizeof(ForestTree));
    forest->vertices = (float**)malloc(NUM_FOREST_TREE_TYPES * sizeof(float*));
    forest->normals = (float**)malloc(NUM_FOREST_TREE_TYPES * sizeof(float*));
    forest->colors = (float**)malloc(NUM_FOREST_TREE_TYPES * sizeof(float*));
    forest->indices = (unsigned int**)malloc(NUM_FOREST_TREE_TYPES * sizeof(unsigned int*));
    forest->vertexCount = (int*)malloc(NUM_FOREST_TREE_TYPES * sizeof(int));
    forest->indexCount = (int*)malloc(NUM_FOREST_TREE_TYPES * sizeof(int));
    if (!forest->vertices || !forest->normals || !forest->colors || 
        !forest->indices || !forest->vertexCount || !forest->indexCount) {
        forestSystemDestroy(forest);
        return NULL;
    }
    for (int type = 0; type < NUM_FOREST_TREE_TYPES; type++) {
        forest->vertices[type] = (float*)malloc(MAX_VERTICES * 3 * sizeof(float));
        forest->normals[type] = (float*)malloc(MAX_VERTICES * 3 * sizeof(float));
        forest->colors[type] = (float*)malloc(MAX_VERTICES * 3 * sizeof(float));
        forest->indices[type] = (unsigned int*)malloc(MAX_INDICES * sizeof(unsigned int));
        forest->vertexCount[type] = 0;
        forest->indexCount[type] = 0;
        if (!forest->vertices[type] || !forest->normals[type] || 
            !forest->colors[type] || !forest->indices[type]) {
            forestSystemDestroy(forest);
            return NULL;
        }
    }
    forest->instanceCount = 0;
    return forest;
}

/* Creates cylindrical trunk geometry with random variations */
void generateTrunk(ForestSystem* forest, int type, int* currentVertex, int* currentIndex) {
    float trunkHeight = 1.2f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * SIZE_VARIETY);
    float trunkRadius = 0.15f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * TRUNK_VARIETY);
    int trunkSegments = 8;
    for (int i = 0; i <= trunkSegments; i++) {
        float heightPercent = (float)i / trunkSegments;
        float y = heightPercent * trunkHeight;
        float radius = trunkRadius * (1.0f - heightPercent * 0.2f);
        for (int j = 0; j < BRANCH_SEGMENTS; j++) {
            float angle = (float)j / BRANCH_SEGMENTS * 2.0f * M_PI;
            float c = cosf(angle);
            float s = sinf(angle);
            forest->vertices[type][*currentVertex * 3 + 0] = c * radius;
            forest->vertices[type][*currentVertex * 3 + 1] = y;
            forest->vertices[type][*currentVertex * 3 + 2] = s * radius;
            forest->normals[type][*currentVertex * 3 + 0] = c;
            forest->normals[type][*currentVertex * 3 + 1] = 0.1f;
            forest->normals[type][*currentVertex * 3 + 2] = s;
            float colorVar = (rand() % 20 - 10) / 100.0f + ((float)rand() / RAND_MAX - 0.5f) * COLOR_VARIETY;
            if (type == FOREST_TREE_MAPLE) {
                forest->colors[type][*currentVertex * 3 + 0] = 0.95f - fabs(colorVar);
                forest->colors[type][*currentVertex * 3 + 1] = 0.95f - fabs(colorVar);
                forest->colors[type][*currentVertex * 3 + 2] = 0.85f - fabs(colorVar) * 2.0f;
            } else {
                forest->colors[type][*currentVertex * 3 + 0] = 0.25f + colorVar;
                forest->colors[type][*currentVertex * 3 + 1] = 0.15f + colorVar * 0.7f;
                forest->colors[type][*currentVertex * 3 + 2] = 0.05f + colorVar * 0.5f;
            }
            (*currentVertex)++;
        }
    }
    for (int i = 0; i < trunkSegments; i++) {
        for (int j = 0; j < BRANCH_SEGMENTS; j++) {
            int curr = i * BRANCH_SEGMENTS + j;
            int next = i * BRANCH_SEGMENTS + ((j + 1) % BRANCH_SEGMENTS);
            int currUp = (i + 1) * BRANCH_SEGMENTS + j;
            int nextUp = (i + 1) * BRANCH_SEGMENTS + ((j + 1) % BRANCH_SEGMENTS);
            forest->indices[type][*currentIndex + 0] = curr;
            forest->indices[type][*currentIndex + 1] = next;
            forest->indices[type][*currentIndex + 2] = currUp;
            forest->indices[type][*currentIndex + 3] = next;
            forest->indices[type][*currentIndex + 4] = nextUp;
            forest->indices[type][*currentIndex + 5] = currUp;
            *currentIndex += 6;
        }
    }
}

/* Generates pine tree foliage as layered conical shapes */
void generatePineTree(ForestSystem* forest, int type, int* currentVertex, int* currentIndex) {
    float foliageHeight = 2.2f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * SIZE_VARIETY);
    float baseWidth = 1.1f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * FOLIAGE_VARIETY);
    int layers = 8 + (EXTRA_TREE_VARIETY ? rand() % 2 : 0);
    float layerSpacing = foliageHeight / layers;
    float startHeight = 0.6f;
    for (int layer = 0; layer < layers; layer++) {
        float heightPercent = (float)layer / layers;
        float radius = baseWidth * (1.0f - powf(heightPercent, 0.85f));
        int segments = 8;
        for (int i = 0; i <= segments; i++) {
            radius *= (0.95f + ((float)rand() / RAND_MAX) * 0.1f);
            float angle = (float)i / segments * 2.0f * M_PI;
            forest->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * radius;
            forest->vertices[type][*currentVertex * 3 + 1] = startHeight + layer * layerSpacing;
            forest->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * radius;
            float nx = cosf(angle);
            float ny = 1.0f;
            float nz = sinf(angle);
            float nlen = sqrtf(nx*nx + ny*ny + nz*nz);
            forest->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            forest->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            forest->normals[type][*currentVertex * 3 + 2] = nz/nlen;
            float heightShade = 0.8f + (heightPercent * 0.4f);
            float randomShade = 0.9f + ((float)rand() / RAND_MAX) * 0.2f;
            float shade = heightShade * randomShade;
            float colorVar = (rand() % 20 - 10) / 100.0f + ((float)rand() / RAND_MAX - 0.5f) * COLOR_VARIETY;
            forest->colors[type][*currentVertex * 3 + 0] = 0.40f + colorVar * 0.2f;
            forest->colors[type][*currentVertex * 3 + 1] = 0.40f + colorVar * 0.2f;
            forest->colors[type][*currentVertex * 3 + 2] = 0.20f + colorVar * 0.1f;
            (*currentVertex)++;
            forest->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * (radius * 0.7f);
            forest->vertices[type][*currentVertex * 3 + 1] = startHeight + (layer + 1) * layerSpacing;
            forest->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * (radius * 0.7f);
            forest->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            forest->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            forest->normals[type][*currentVertex * 3 + 2] = nz/nlen;
            forest->colors[type][*currentVertex * 3 + 0] = 0.05f * shade;
            forest->colors[type][*currentVertex * 3 + 1] = 0.25f * shade;
            forest->colors[type][*currentVertex * 3 + 2] = 0.05f * shade; 
            (*currentVertex)++;
            if (i > 0) {
                int base = *currentVertex - 4;
                forest->indices[type][*currentIndex + 0] = base;
                forest->indices[type][*currentIndex + 1] = base + 2;
                forest->indices[type][*currentIndex + 2] = base + 1;
                forest->indices[type][*currentIndex + 3] = base + 2;
                forest->indices[type][*currentIndex + 4] = base + 3;
                forest->indices[type][*currentIndex + 5] = base + 1;
                *currentIndex += 6;
            }
        }
    }
}

/* Generates fir tree with more slender shape than pine */
void generateFirTree(ForestSystem* forest, int type, int* currentVertex, int* currentIndex) {
    float foliageHeight = 2.5f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * SIZE_VARIETY);
    float baseWidth = 0.8f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * FOLIAGE_VARIETY);
    int layers = 10 + (EXTRA_TREE_VARIETY ? rand() % 2 : 0);
    
    float layerSpacing = foliageHeight / layers;
    float startHeight = 0.8f;
    
    for (int layer = 0; layer < layers; layer++) {
        float heightPercent = (float)layer / layers;
        float radius = baseWidth * (1.0f - powf(heightPercent, 0.5f));
        int segments = 6;

        for (int i = 0; i <= segments; i++) {
            float angle = (float)i / segments * 2.0f * M_PI;
            
            forest->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * radius;
            forest->vertices[type][*currentVertex * 3 + 1] = startHeight + layer * layerSpacing;
            forest->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * radius;

            float nx = cosf(angle);
            float ny = 0.5f;
            float nz = sinf(angle);
            float nlen = sqrtf(nx*nx + ny*ny + nz*nz);
            forest->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            forest->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            forest->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            float shade = 0.7f + ((float)rand() / RAND_MAX) * 0.3f;
            float colorVar = (rand() % 20 - 10) / 100.0f + ((float)rand() / RAND_MAX - 0.5f) * COLOR_VARIETY;
            forest->colors[type][*currentVertex * 3 + 0] = 0.25f + colorVar * 0.1f;
            forest->colors[type][*currentVertex * 3 + 1] = 0.45f + colorVar * 0.1f;
            forest->colors[type][*currentVertex * 3 + 2] = 0.15f + colorVar * 0.05f;

            (*currentVertex)++;

            forest->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * (radius * 0.5f);
            forest->vertices[type][*currentVertex * 3 + 1] = startHeight + (layer + 1) * layerSpacing;
            forest->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * (radius * 0.5f);

            forest->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            forest->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            forest->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            forest->colors[type][*currentVertex * 3 + 0] = 0.05f * shade;
            forest->colors[type][*currentVertex * 3 + 1] = 0.85f * shade;
            forest->colors[type][*currentVertex * 3 + 2] = 0.35f * shade;

            (*currentVertex)++;

            if (i > 0) {
                int base = *currentVertex - 4;
                forest->indices[type][*currentIndex + 0] = base;
                forest->indices[type][*currentIndex + 1] = base + 2;
                forest->indices[type][*currentIndex + 2] = base + 1;

                forest->indices[type][*currentIndex + 3] = base + 2;
                forest->indices[type][*currentIndex + 4] = base + 3;
                forest->indices[type][*currentIndex + 5] = base + 1;

                *currentIndex += 6;
            }
        }
    }
}

/* Generates maple tree with distinctive white trunk and round foliage */
void generateMapleTree(ForestSystem* forest, ForestTreeType type, int* currentVertex, int* currentIndex) {
    float trunkHeight = 2.0f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * SIZE_VARIETY);
    float foliageHeight = 2.7f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * SIZE_VARIETY);
    float baseWidth = 0.8f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * FOLIAGE_VARIETY);
    int layers = 10 + (EXTRA_TREE_VARIETY ? rand() % 2 : 0);
    
    float trunkRadius = 0.13f;
    int trunkSegments = TRUNK_SEGMENTS;

    for (int i = 0; i <= trunkSegments; i++) {
        float heightPercent = (float)i / trunkSegments;
        float y = heightPercent * trunkHeight;
        float radius = trunkRadius * (1.0f - heightPercent * 0.1f);

        for (int j = 0; j < BRANCH_SEGMENTS; j++) {
            float angle = (float)j / BRANCH_SEGMENTS * 2.0f * M_PI;
            float c = cosf(angle);
            float s = sinf(angle);

            forest->vertices[type][*currentVertex * 3 + 0] = c * radius;
            forest->vertices[type][*currentVertex * 3 + 1] = y;
            forest->vertices[type][*currentVertex * 3 + 2] = s * radius;

            forest->normals[type][*currentVertex * 3 + 0] = c;
            forest->normals[type][*currentVertex * 3 + 1] = 0.1f;
            forest->normals[type][*currentVertex * 3 + 2] = s;

            float darkPatch = (rand() % 20) / 100.0f;
            forest->colors[type][*currentVertex * 3 + 0] = 0.65f - darkPatch;
            forest->colors[type][*currentVertex * 3 + 1] = 0.45f - darkPatch * 0.5f;
            forest->colors[type][*currentVertex * 3 + 2] = 0.25f - darkPatch * 0.5f;

            (*currentVertex)++;
        }
    }

    for (int i = 0; i < trunkSegments; i++) {
        for (int j = 0; j < BRANCH_SEGMENTS; j++) {
            int curr = i * BRANCH_SEGMENTS + j;
            int next = i * BRANCH_SEGMENTS + ((j + 1) % BRANCH_SEGMENTS);
            int currUp = (i + 1) * BRANCH_SEGMENTS + j;
            int nextUp = (i + 1) * BRANCH_SEGMENTS + ((j + 1) % BRANCH_SEGMENTS);

            forest->indices[type][*currentIndex + 0] = curr;
            forest->indices[type][*currentIndex + 1] = next;
            forest->indices[type][*currentIndex + 2] = currUp;

            forest->indices[type][*currentIndex + 3] = next;
            forest->indices[type][*currentIndex + 4] = nextUp;
            forest->indices[type][*currentIndex + 5] = currUp;

            *currentIndex += 6;
        }
    }

    float layerSpacing = foliageHeight / layers;
    float startHeight = trunkHeight * 0.6f;
    
    for (int layer = 0; layer < layers; layer++) {
        float heightPercent = (float)layer / layers;
        float radius = baseWidth * (0.8f + 0.6f * heightPercent) * (1.0f - powf(heightPercent - 0.15f, 2.0f)) * 1.1f;
        int segments = FOLIAGE_SEGMENTS;

        for (int i = 0; i <= segments; i++) {
            float angle = (float)i / segments * 2.0f * M_PI;
            float radiusVar = 1.0f + ((float)rand() / RAND_MAX) * 0.2f - 0.1f;
            
            forest->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * radius * radiusVar;
            forest->vertices[type][*currentVertex * 3 + 1] = startHeight + layer * layerSpacing;
            forest->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * radius * radiusVar;

            float nx = cosf(angle);
            float ny = 0.7f;
            float nz = sinf(angle);
            float nlen = sqrtf(nx*nx + ny*ny + nz*nz);
            forest->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            forest->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            forest->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            float shade = 0.8f + ((float)rand() / RAND_MAX) * 0.2f + ((float)rand() / RAND_MAX - 0.5f) * COLOR_VARIETY;
            float orange = 0.7f + ((float)rand() / RAND_MAX) * 0.3f;
            float yellow = 0.5f + ((float)rand() / RAND_MAX) * 0.4f;
            float red = 0.3f + ((float)rand() / RAND_MAX) * 0.5f;
            float colorMix = (float)layer / layers;
            forest->colors[type][*currentVertex * 3 + 0] = (orange * (1.0f - colorMix) + red * colorMix) * shade;
            forest->colors[type][*currentVertex * 3 + 1] = yellow * shade;
            forest->colors[type][*currentVertex * 3 + 2] = (0.15f + 0.2f * (1.0f - colorMix)) * shade;

            (*currentVertex)++;

            float upperRadius = radius * (0.95f - heightPercent * 0.08f);
            forest->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * upperRadius * radiusVar;
            forest->vertices[type][*currentVertex * 3 + 1] = startHeight + (layer + 1) * layerSpacing;
            forest->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * upperRadius * radiusVar;

            forest->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            forest->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            forest->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            forest->colors[type][*currentVertex * 3 + 0] = (orange * (1.0f - colorMix) + red * colorMix) * shade * 0.8f;
            forest->colors[type][*currentVertex * 3 + 1] = yellow * shade * 0.8f;
            forest->colors[type][*currentVertex * 3 + 2] = (0.15f + 0.2f * (1.0f - colorMix)) * shade * 0.8f;

            (*currentVertex)++;

            if (i > 0) {
                int base = *currentVertex - 4;
                forest->indices[type][*currentIndex + 0] = base;
                forest->indices[type][*currentIndex + 1] = base + 2;
                forest->indices[type][*currentIndex + 2] = base + 1;

                forest->indices[type][*currentIndex + 3] = base + 2;
                forest->indices[type][*currentIndex + 4] = base + 3;
                forest->indices[type][*currentIndex + 5] = base + 1;

                *currentIndex += 6;
            }
        }
    }
}

/* Creates mesh data for different tree types */
void generateTreeMesh(ForestSystem* forest, int type) {
    int currentVertex = 0;
    int currentIndex = 0;

    generateTrunk(forest,type, &currentVertex, &currentIndex);
    
    switch(type) {
        case FOREST_TREE_FIR:
            generateFirTree(forest, type, &currentVertex, &currentIndex);
            break;
        case FOREST_TREE_MAPLE:
            generateMapleTree(forest, type, &currentVertex, &currentIndex);
            break;
        case FOREST_TREE_PINE:
            generatePineTree(forest, type, &currentVertex, &currentIndex);
            break;
        default:
            break;
    }
    
    forest->vertexCount[type] = currentVertex;
    forest->indexCount[type] = currentIndex;
}

/* Determines tree density based on terrain height and slope */
float getTreeDensity(float height, float slope) {
    float snowStartHeight = 12.0f;
    float treelineHeight = 20.0f;
    float maxSlope = 0.7f;
    
    float heightFactor = 1.0f - (height - snowStartHeight) / (treelineHeight - snowStartHeight);
    heightFactor = heightFactor < 0 ? 0 : (heightFactor > 1 ? 1 : heightFactor);
    
    float slopeFactor = 1.0f - slope / maxSlope;
    slopeFactor = slopeFactor < 0 ? 0 : (slopeFactor > 1 ? 1 : slopeFactor);
    
    return heightFactor * slopeFactor;
}

/* Populates the landscape with trees based on terrain characteristics */
void forestSystemGenerate(ForestSystem* forest, Landscape* landscape) {
    extern float waterLevel;
    const int maxTrees = 2000;
    forest->instanceCount = 0;
    
    for (int i = 0; i < maxTrees; i++) {
        float x = ((float)rand() / RAND_MAX - 0.5f) * LANDSCAPE_SCALE * 0.95f;
        float z = ((float)rand() / RAND_MAX - 0.5f) * LANDSCAPE_SCALE * 0.95f;
        float y = landscapeGetHeight(landscape, x, z);
        
        if (y <= waterLevel + 1.0f) continue;
        
        float slope = 0.0f;
        if (x > -LANDSCAPE_SCALE/2 && x < LANDSCAPE_SCALE/2 &&
            z > -LANDSCAPE_SCALE/2 && z < LANDSCAPE_SCALE/2) {
            float dx = landscapeGetHeight(landscape, x + 1.0f, z) - landscapeGetHeight(landscape, x - 1.0f, z);
            float dz = landscapeGetHeight(landscape, x, z + 1.0f) - landscapeGetHeight(landscape, x, z - 1.0f);
            slope = sqrt(dx*dx + dz*dz) / 2.0f;
        }
        
        float baseDensity = getTreeDensity(y, slope);
        float edgeFactor = 1.0f;
        
        float randomFactor = ((float)rand() / RAND_MAX) * 0.2f + 0.8f;
        float finalDensity = baseDensity * edgeFactor * randomFactor;
        if (y > 15.0f) finalDensity *= 2.0f;

        if ((float)rand() / RAND_MAX > finalDensity) continue;
        
        float placementDist = 3.0f * (1.0f + (y / 20.0f) * 0.3f);
        if (forestPlacementValid(x, z, forest->instances, forest->instanceCount, placementDist)) {
            ForestTree* tree = &forest->instances[forest->instanceCount];
            
            float rand_val = (float)rand() / RAND_MAX;
            if (y > 15.0f) {
                tree->type = (rand_val < 0.75f) ? FOREST_TREE_PINE : FOREST_TREE_FIR;
            } else if (y > 8.0f) {
                if (rand_val < 0.35f) tree->type = FOREST_TREE_PINE;
                else if (rand_val < 0.75f) tree->type = FOREST_TREE_FIR;
                else tree->type = FOREST_TREE_MAPLE;
            } else {
                if (rand_val < 0.45f) tree->type = FOREST_TREE_MAPLE;
                else if (rand_val < 0.85f) tree->type = FOREST_TREE_FIR;
                else tree->type = FOREST_TREE_PINE;
            }
            
            tree->x = x;
            tree->y = y;
            tree->z = z;
            
            float heightInfluence = 1.0f - (y / 20.0f) * 0.2f;
            float slopeInfluence = 1.0f - slope * 0.4f;
            float randomScale = 0.9f + ((float)rand() / RAND_MAX) * 0.6f;
            float environmentalScale = heightInfluence * slopeInfluence;
            
            switch(tree->type) {
                case FOREST_TREE_MAPLE:
                    tree->scale = randomScale * environmentalScale * 1.4f;
                    break;
                case FOREST_TREE_PINE:
                    tree->scale = randomScale * environmentalScale * 1.2f;
                    break;
                case FOREST_TREE_FIR:
                    tree->scale = randomScale * environmentalScale * 1.3f;
                    break;
                default:
                    tree->scale = randomScale * environmentalScale;
                    break;
            }
            
            tree->rotation = ((float)rand() / RAND_MAX) * 360.0f + ((float)rand() / RAND_MAX - 0.5f) * 30.0f;
            tree->tiltX = ((float)(rand() % 10) - 5) * 0.5f + ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
            tree->tiltZ = ((float)(rand() % 10) - 5) * 0.5f + ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
            
            if (tree->scale > 0.4f) {
                forest->instanceCount++;
                if (forest->instanceCount >= maxTrees) break;
            }
        }
    }

    generateTreeMesh(forest, FOREST_TREE_PINE);
    generateTreeMesh(forest, FOREST_TREE_FIR);
    generateTreeMesh(forest, FOREST_TREE_MAPLE);
}

/* Renders all trees in the forest with day/night lighting */
void forestSystemRender(ForestSystem* forest, float dayTime, int snowActive) {
    float matAmbient[] = {0.4f, 0.4f, 0.4f, 1.0f};
    float matDiffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
    float matSpecular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    float timeNormalized = dayTime / 24.0f;
    float sunHeight = sin((timeNormalized - 0.25f) * 2 * M_PI);
    float minBrightness = 0.3f;
    float brightness = minBrightness + (1.0f - minBrightness) * (sunHeight + 1.0f) * 0.5f;

    for (int i = 0; i < forest->instanceCount; i++) {
        ForestTree* tree = &forest->instances[i];
        int type = tree->type;
        float* vcolors = forest->colors[type];
        int vcount = forest->vertexCount[type];
        float blendedColors[MAX_VERTICES * 3];
        for (int v = 0; v < vcount * 3; v += 3) {
            float r = vcolors[v];
            float g = vcolors[v+1];
            float b = vcolors[v+2];
            if (snowActive) {
                r = r * 0.6f + 0.4f;
                g = g * 0.6f + 0.4f;
                b = b * 0.6f + 0.4f;
            }
            blendedColors[v] = r;
            blendedColors[v+1] = g;
            blendedColors[v+2] = b;
        }
        glVertexPointer(3, GL_FLOAT, 0, forest->vertices[type]);
        glNormalPointer(GL_FLOAT, 0, forest->normals[type]);
        glColorPointer(3, GL_FLOAT, 0, snowActive ? blendedColors : forest->colors[type]);
        
        glPushMatrix();
        glTranslatef(tree->x, tree->y, tree->z);
        glRotatef(tree->rotation, 0, 1, 0);
        glRotatef(tree->tiltX, 1, 0, 0);
        glRotatef(tree->tiltZ, 0, 0, 1);
        glScalef(tree->scale, tree->scale, tree->scale);
        
        glColor3f(brightness, brightness, brightness);
        glDrawElements(GL_TRIANGLES, forest->indexCount[type], 
                      GL_UNSIGNED_INT, forest->indices[type]);
        
        glPopMatrix();
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

/* Frees all memory associated with the forest system */
void forestSystemDestroy(ForestSystem* forest) {
    if (forest) {
        if (forest->vertices) {
            for (int type = 0; type < NUM_FOREST_TREE_TYPES; type++) {
                free(forest->vertices[type]);
                free(forest->normals[type]);
                free(forest->colors[type]);
                free(forest->indices[type]);
            }
            free(forest->vertices);
            free(forest->normals);
            free(forest->colors);
            free(forest->indices);
        }
        free(forest->vertexCount);
        free(forest->indexCount);
        free(forest->instances);
        free(forest);
    }
}

/* Checks if a proposed tree location has sufficient distance from other trees */
int forestPlacementValid(float x, float z, ForestTree* existing, int count, float minDist) {
    for (int i = 0; i < count; i++) {
        float dx = x - existing[i].x;
        float dz = z - existing[i].z;
        float distSq = dx*dx + dz*dz;
        if (distSq < minDist*minDist) return 0;
    }
    return 1;
}

// --- Utility functions ---
/* Check if a point is below water level */
static bool is_in_water(float y, float waterLevel) {
    return y <= waterLevel + 0.5f;
}

/* Generate random float in specified range */
static float randf(float a, float b) {
    return a + ((float)rand() / RAND_MAX) * (b - a);
}

// --- Rock system implementation ---
/* Allocates memory for rock data structures */
RockField* rockFieldCreate(int maxRocks) {
    RockField* field = (RockField*)malloc(sizeof(RockField));
    if (!field) return NULL;
    field->instances = (RockObject*)malloc(maxRocks * sizeof(RockObject));
    field->instanceCount = 0;
    field->maxRocks = maxRocks;
    return field;
}

/* Icosahedron vertices for basic rock shape */
static const float rockIcosahedronBase[ROCK_VERTICES][3] = {
    {-0.525731, 0.850651, 0.0}, {0.525731, 0.850651, 0.0}, {-0.525731, -0.850651, 0.0}, {0.525731, -0.850651, 0.0},
    {0.0, -0.525731, 0.850651}, {0.0, 0.525731, 0.850651}, {0.0, -0.525731, -0.850651}, {0.0, 0.525731, -0.850651},
    {0.850651, 0.0, -0.525731}, {0.850651, 0.0, 0.525731}, {-0.850651, 0.0, -0.525731}, {-0.850651, 0.0, 0.525731}
};

/* Populates the landscape with rock formations */
void rockFieldGenerate(RockField* field, Landscape* landscape) {
    extern float waterLevel;
    field->instanceCount = 0;
    int maxRocks = field->maxRocks;
    int attempts = 0;
    // Define a palette of earthy/foliage tones (RGB)
    const float rockPalette[][3] = {
        {0.35f, 0.25f, 0.12f}, // brown
        {0.18f, 0.32f, 0.13f}, // mossy green
        {0.45f, 0.38f, 0.22f}, // tan
        {0.25f, 0.25f, 0.25f}, // dark gray
        {0.55f, 0.48f, 0.38f}, // light brown/gray
        {0.60f, 0.50f, 0.20f}, // ochre
        {0.30f, 0.28f, 0.18f}, // muted olive
        {0.40f, 0.30f, 0.18f}, // reddish brown
        {0.20f, 0.20f, 0.20f}, // near black
        {0.60f, 0.55f, 0.45f}  // light tan
    };
    int rockPaletteSize = sizeof(rockPalette) / sizeof(rockPalette[0]);
    while (field->instanceCount < maxRocks && attempts < maxRocks * 10) {
        float x = randf(-LANDSCAPE_SCALE * 0.48f, LANDSCAPE_SCALE * 0.48f);
        float z = randf(-LANDSCAPE_SCALE * 0.48f, LANDSCAPE_SCALE * 0.48f);
        float y = landscapeGetHeight(landscape, x, z);
        if (is_in_water(y, waterLevel)) { attempts++; continue; }
        RockObject* rock = &field->instances[field->instanceCount];
        rock->x = x;
        rock->y = y;
        rock->z = z;
        rock->scale = randf(0.85f, 2.0f);
        rock->xScale = randf(0.7f, 1.3f);
        rock->zScale = randf(0.7f, 1.3f);
        rock->rotation = randf(0, 360);
        // Pick a base color from the palette
        int paletteIdx = rand() % rockPaletteSize;
        float baseR = rockPalette[paletteIdx][0];
        float baseG = rockPalette[paletteIdx][1];
        float baseB = rockPalette[paletteIdx][2];
        // Add per-rock color variety
        float colorVar = ((float)rand() / RAND_MAX - 0.5f) * 0.18f;
        rock->color[0] = baseR + colorVar + ((float)rand() / RAND_MAX - 0.5f) * 0.08f;
        rock->color[1] = baseG + colorVar + ((float)rand() / RAND_MAX - 0.5f) * 0.08f;
        rock->color[2] = baseB + colorVar + ((float)rand() / RAND_MAX - 0.5f) * 0.08f;
        for (int v = 0; v < ROCK_VERTICES; v++) {
            float scale = randf(0.85f, 1.15f);
            rock->vertices[v][0] = rockIcosahedronBase[v][0] * scale;
            rock->vertices[v][1] = rockIcosahedronBase[v][1] * scale;
            rock->vertices[v][2] = rockIcosahedronBase[v][2] * scale;
        }
        field->instanceCount++;
        attempts++;
    }
}

/* Renders all rocks in the scene */
void rockFieldRender(RockField* field, int snowActive) {
    extern const int rockIcosahedronIndices[ROCK_TRIANGLES][3];
    for (int i = 0; i < field->instanceCount; i++) {
        RockObject* rock = &field->instances[i];
        float color[3];
        for (int c = 0; c < 3; c++) {
            color[c] = snowActive ? (rock->color[c] * 0.6f + 0.4f) : rock->color[c];
        }
        glPushMatrix();
        glTranslatef(rock->x, rock->y, rock->z);
        glRotatef(rock->rotation, 0, 1, 0);
        glScalef(rock->scale * rock->xScale, rock->scale, rock->scale * rock->zScale);
        glColor3fv(color);
        for (int t = 0; t < ROCK_TRIANGLES; t++) {
            int i0 = rockIcosahedronIndices[t][0];
            int i1 = rockIcosahedronIndices[t][1];
            int i2 = rockIcosahedronIndices[t][2];
            float* v0 = rock->vertices[i0];
            float* v1 = rock->vertices[i1];
            float* v2 = rock->vertices[i2];
            float ux = v1[0] - v0[0], uy = v1[1] - v0[1], uz = v1[2] - v0[2];
            float vx = v2[0] - v0[0], vy = v2[1] - v0[1], vz = v2[2] - v0[2];
            float nx = uy * vz - uz * vy;
            float ny = uz * vx - ux * vz;
            float nz = ux * vy - uy * vx;
            float nlen = sqrtf(nx*nx + ny*ny + nz*nz);
            if (nlen > 0.0001f) { nx /= nlen; ny /= nlen; nz /= nlen; }
            glBegin(GL_TRIANGLES);
            glNormal3f(nx, ny, nz);
            glVertex3fv(v0);
            glVertex3fv(v1);
            glVertex3fv(v2);
            glEnd();
        }
        glPopMatrix();
    }
}

/* Frees memory used by rock system */
void rockFieldDestroy(RockField* field) {
    if (field) {
        if (field->instances) free(field->instances);
        free(field);
    }
}

// --- Shrub system implementation ---
/* Allocates memory for shrub data structures */
ShrubField* shrubFieldCreate(int maxBushes) {
    ShrubField* field = (ShrubField*)malloc(sizeof(ShrubField));
    if (!field) return NULL;
    field->instances = (ShrubObject*)malloc(maxBushes * sizeof(ShrubObject));
    field->instanceCount = 0;
    field->maxBushes = maxBushes;
    return field;
}

/* Populates the landscape with shrubs and small plants */
void shrubFieldGenerate(ShrubField* field, Landscape* landscape) {
    extern float waterLevel;
    field->instanceCount = 0;
    int maxBushes = field->maxBushes;
    int attempts = 0;
    // Define a palette of foliage tones (RGB)
    const float bushPalette[][3] = {
        {0.18f, 0.32f, 0.13f}, // deep green
        {0.40f, 0.30f, 0.18f}, // brownish
        {0.55f, 0.48f, 0.22f}, // yellow-green
        {0.65f, 0.40f, 0.15f}, // orange-brown
        {0.35f, 0.25f, 0.12f}, // brown
        {0.60f, 0.50f, 0.20f}, // ochre
        {0.45f, 0.38f, 0.22f}, // tan
        {0.25f, 0.25f, 0.25f}, // dark gray
        {0.55f, 0.48f, 0.38f}, // light brown/gray
        {0.30f, 0.28f, 0.18f}  // muted olive
    };
    int bushPaletteSize = sizeof(bushPalette) / sizeof(bushPalette[0]);
    while (field->instanceCount < maxBushes && attempts < maxBushes * 10) {
        float x = randf(-LANDSCAPE_SCALE * 0.48f, LANDSCAPE_SCALE * 0.48f);
        float z = randf(-LANDSCAPE_SCALE * 0.48f, LANDSCAPE_SCALE * 0.48f);
        float y = landscapeGetHeight(landscape, x, z);
        if (is_in_water(y, waterLevel)) { attempts++; continue; }
        // Compute slope at (x, z)
        float dx = landscapeGetHeight(landscape, x + 1.0f, z) - landscapeGetHeight(landscape, x - 1.0f, z);
        float dz = landscapeGetHeight(landscape, x, z + 1.0f) - landscapeGetHeight(landscape, x, z - 1.0f);
        float slope = sqrtf(dx*dx + dz*dz) / 2.0f;
        if (slope > 0.5f) { attempts++; continue; }
        ShrubObject* bush = &field->instances[field->instanceCount];
        bush->x = x;
        bush->y = y;
        bush->z = z;
        bush->scale = randf(0.85f, 2.0f);
        bush->rotation = randf(0, 360);
        // Pick a base color from the palette
        int paletteIdx = rand() % bushPaletteSize;
        float baseR = bushPalette[paletteIdx][0];
        float baseG = bushPalette[paletteIdx][1];
        float baseB = bushPalette[paletteIdx][2];
        float colorVar = ((float)rand() / RAND_MAX - 0.5f) * 0.18f;
        bush->color[0] = baseR + colorVar + ((float)rand() / RAND_MAX - 0.5f) * 0.08f;
        bush->color[1] = baseG + colorVar + ((float)rand() / RAND_MAX - 0.5f) * 0.08f;
        bush->color[2] = baseB + colorVar + ((float)rand() / RAND_MAX - 0.5f) * 0.08f;
        bush->subCount = 8 + (rand() % 5);
        for (int s = 0; s < bush->subCount; s++) {
            float angle = randf(0, 2*M_PI);
            float dist = randf(0.0f, 0.38f);
            bush->subOffsets[s][0] = cosf(angle) * dist;
            bush->subOffsets[s][1] = randf(-0.08f, 0.12f);
            bush->subOffsets[s][2] = sinf(angle) * dist;
            bush->subRadii[s] = randf(0.28f, 0.55f);
            // Pick a base color for the sub-sphere
            int subPaletteIdx = rand() % bushPaletteSize;
            float subBaseR = bushPalette[subPaletteIdx][0];
            float subBaseG = bushPalette[subPaletteIdx][1];
            float subBaseB = bushPalette[subPaletteIdx][2];
            float subColorVar = ((float)rand() / RAND_MAX - 0.5f) * 0.18f;
            bush->subColors[s][0] = subBaseR + subColorVar + ((float)rand() / RAND_MAX - 0.5f) * 0.08f;
            bush->subColors[s][1] = subBaseG + subColorVar + ((float)rand() / RAND_MAX - 0.5f) * 0.08f;
            bush->subColors[s][2] = subBaseB + subColorVar + ((float)rand() / RAND_MAX - 0.5f) * 0.08f;
        }
        bush->mossCoverage = randf(0.0f, 0.4f);
        field->instanceCount++;
        attempts++;
    }
}

/* Renders all shrubs in the scene */
void shrubFieldRender(ShrubField* field, int snowActive) {
    for (int i = 0; i < field->instanceCount; i++) {
        ShrubObject* bush = &field->instances[i];
        float color[3];
        for (int c = 0; c < 3; c++) {
            color[c] = snowActive ? (bush->color[c] * 0.6f + 0.4f) : bush->color[c];
        }
        glPushMatrix();
        glTranslatef(bush->x, bush->y, bush->z);
        glRotatef(bush->rotation, 0, 1, 0);
        glScalef(bush->scale, bush->scale, bush->scale);
        float shrubColor[3] = {color[0], color[1], color[2]};
        if (bush->mossCoverage > 0.01f) {
            shrubColor[0] = shrubColor[0]*(1.0f-bush->mossCoverage) + 0.2f*bush->mossCoverage;
            shrubColor[1] = shrubColor[1]*(1.0f-bush->mossCoverage) + 0.5f*bush->mossCoverage;
            shrubColor[2] = shrubColor[2]*(1.0f-bush->mossCoverage) + 0.2f*bush->mossCoverage;
        }
        for (int s = 0; s < bush->subCount; s++) {
            glPushMatrix();
            glTranslatef(bush->subOffsets[s][0], bush->subOffsets[s][1], bush->subOffsets[s][2]);
            float subColor[3];
            for (int c = 0; c < 3; c++) {
                subColor[c] = snowActive ? (bush->subColors[s][c] * 0.6f + 0.4f) : bush->subColors[s][c];
            }
            glColor3fv(subColor);
            // Render as full sphere
            int slices = 14, stacks = 14;
            for (int k = 0; k < stacks; k++) {
                float phi1 = M_PI * k / stacks;
                float phi2 = M_PI * (k + 1) / stacks;
                glBegin(GL_TRIANGLE_STRIP);
                for (int l = 0; l <= slices; l++) {
                    float theta = 2 * M_PI * l / slices;
                    for (int m = 0; m < 2; m++) {
                        float phi = (m == 0) ? phi1 : phi2;
                        float x = bush->subRadii[s] * sinf(phi) * cosf(theta);
                        float y = bush->subRadii[s] * cosf(phi);
                        float z = bush->subRadii[s] * sinf(phi) * sinf(theta);
                        glNormal3f(x, y, z);
                        glVertex3f(x, y, z);
                    }
                }
                glEnd();
            }
            glPopMatrix();
        }
        glPopMatrix();
    }
}

/* Frees memory used by shrub system */
void shrubFieldDestroy(ShrubField* field) {
    if (field) {
        if (field->instances) free(field->instances);
        free(field);
    }
}

// --- Log system implementation ---
/* Allocates memory for fallen log data structures */
LogField* logFieldCreate(int maxLogs) {
    LogField* field = (LogField*)malloc(sizeof(LogField));
    if (!field) return NULL;
    field->instances = (LogObject*)malloc(maxLogs * sizeof(LogObject));
    field->instanceCount = 0;
    field->maxLogs = maxLogs;
    return field;
}

/* Populates the landscape with fallen logs */
void logFieldGenerate(LogField* field, Landscape* landscape) {
    extern float waterLevel;
    field->instanceCount = 0;
    int maxLogs = field->maxLogs;
    int attempts = 0;
    while (field->instanceCount < maxLogs && attempts < maxLogs * 10) {
        float x = randf(-LANDSCAPE_SCALE * 0.48f, LANDSCAPE_SCALE * 0.48f);
        float z = randf(-LANDSCAPE_SCALE * 0.48f, LANDSCAPE_SCALE * 0.48f);
        float y = landscapeGetHeight(landscape, x, z);
        if (is_in_water(y, waterLevel)) { attempts++; continue; }
        LogObject* log = &field->instances[field->instanceCount];
        log->x = x;
        log->y = y + randf(0.05f, 0.15f);
        log->z = z;
        log->length = randf(2.5f, 7.0f) * 0.8f;
        log->radius = randf(0.12f, 0.32f) * 1.2f;
        log->rotation = randf(0, 360);
        log->tilt = randf(-25, 25);
        float bark = randf(0.22f, 0.48f);
        log->color[0] = bark + randf(-0.07f, 0.07f);
        log->color[1] = bark * 0.8f + randf(-0.07f, 0.07f);
        log->color[2] = bark * 0.5f + randf(-0.07f, 0.07f);
        log->endColor[0] = 0.7f + randf(-0.1f, 0.1f);
        log->endColor[1] = 0.5f + randf(-0.1f, 0.1f);
        log->endColor[2] = 0.3f + randf(-0.1f, 0.1f);
        log->bend = randf(-0.15f, 0.15f);
        log->barkPattern = rand() % 3;
        log->numKnots = 1 + rand() % 4;
        for (int k = 0; k < log->numKnots; ++k) {
            log->knotPos[k][0] = randf(0.1f, 0.9f);
            log->knotPos[k][1] = randf(0, 2*M_PI);
            log->knotSize[k] = randf(0.04f, 0.09f);
        }
        log->mossCoverage = randf(0.0f, 0.5f);
        log->age = randf(0.0f, 1.0f);
        field->instanceCount++;
        attempts++;
    }
}

/* Renders all fallen logs in the scene */
void logFieldRender(LogField* field, int snowActive) {
    for (int i = 0; i < field->instanceCount; i++) {
        LogObject* log = &field->instances[i];
        float color[3], endColor[3];
        for (int c = 0; c < 3; c++) {
            color[c] = snowActive ? (log->color[c] * 0.6f + 0.4f) : log->color[c];
            endColor[c] = snowActive ? (log->endColor[c] * 0.6f + 0.4f) : log->endColor[c];
        }
        glPushMatrix();
        glTranslatef(log->x, log->y, log->z);
        glRotatef(log->rotation, 0, 1, 0);
        glRotatef(log->tilt, 0, 0, 1);
        float ageTint = 1.0f - 0.5f * log->age;
        float barkColor[3] = {color[0]*ageTint, color[1]*ageTint, color[2]*ageTint};
        if (log->mossCoverage > 0.01f) {
            barkColor[0] = barkColor[0]*(1.0f-log->mossCoverage) + 0.2f*log->mossCoverage;
            barkColor[1] = barkColor[1]*(1.0f-log->mossCoverage) + 0.5f*log->mossCoverage;
            barkColor[2] = barkColor[2]*(1.0f-log->mossCoverage) + 0.2f*log->mossCoverage;
        }
        int slices = 12;
        float len = log->length, r = log->radius;
        int segments = 8;
        float segLen = len / segments;
        float angleStep = log->bend / segments;
        float y = 0;
        float bendAngle = 0;
        for (int s = 0; s < segments; ++s) {
            glPushMatrix();
            glTranslatef(0, y, 0);
            glRotatef(bendAngle * 180.0f / M_PI, 0, 0, 1);
            glColor3fv(barkColor);
            glBegin(GL_QUAD_STRIP);
            for (int j = 0; j <= slices; j++) {
                float theta = 2 * M_PI * j / slices;
                float x = cosf(theta), z = sinf(theta);
                glNormal3f(x, 0, z);
                glVertex3f(x * r, 0, z * r);
                glVertex3f(x * r, segLen, z * r);
            }
            glEnd();
            glPopMatrix();
            y += segLen;
            bendAngle += angleStep;
        }
        for (int end = 0; end < 2; end++) {
            float ycap = (end == 0) ? 0 : len;
            float sign = (end == 0) ? -1.0f : 1.0f;
            glColor3fv(endColor);
            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0, sign, 0);
            glVertex3f(0, ycap, 0);
            for (int j = 0; j <= slices; j++) {
                float theta = 2 * M_PI * j / slices;
                float x = cosf(theta), z = sinf(theta);
                glVertex3f(x * r, ycap, z * r);
            }
            glEnd();
        }
        for (int k = 0; k < log->numKnots; ++k) {
            float along = log->knotPos[k][0] * len;
            float around = log->knotPos[k][1];
            float kr = log->knotSize[k];
            float kx = cosf(around) * (r + kr * 0.5f);
            float kz = sinf(around) * (r + kr * 0.5f);
            float ky = along;
            glPushMatrix();
            glTranslatef(kx, ky, kz);
            glColor3f(barkColor[0]*0.8f, barkColor[1]*0.8f, barkColor[2]*0.8f);
            int stacks = 6, kslices = 8;
            for (int m = 0; m < stacks; m++) {
                float phi1 = M_PI * m / stacks;
                float phi2 = M_PI * (m + 1) / stacks;
                glBegin(GL_TRIANGLE_STRIP);
                for (int n = 0; n <= kslices; n++) {
                    float theta = 2 * M_PI * n / kslices;
                    for (int p = 0; p < 2; p++) {
                        float phi = (p == 0) ? phi1 : phi2;
                        float sx = kr * sinf(phi) * cosf(theta);
                        float sy = kr * cosf(phi);
                        float sz = kr * sinf(phi) * sinf(theta);
                        glNormal3f(sx, sy, sz);
                        glVertex3f(sx, sy, sz);
                    }
                }
                glEnd();
            }
            glPopMatrix();
        }
        glPopMatrix();
    }
}

/* Frees memory used by log system */
void logFieldDestroy(LogField* field) {
    if (field) {
        if (field->instances) free(field->instances);
        free(field);
    }
}

// --- Animated Gull Flock System ---

static void setGullColor(int pattern) {
    switch (pattern) {
        case 0: glColor3f(0.98f, 0.98f, 0.98f); break; // white
        case 1: glColor3f(0.1f, 0.1f, 0.1f); break;    // black
        case 2: glColor3f(0.8f, 0.2f, 0.2f); break;    // red
        case 3: glColor3f(0.7f, 0.7f, 0.7f); break;    // gray
        case 4: glColor3f(0.5f, 0.3f, 0.1f); break;    // brown
        case 5: glColor3f(0.95f, 0.95f, 0.2f); break;  // yellow
        default: glColor3f(0.98f, 0.98f, 0.98f); break;
    }
}

static void drawProceduralGull(float wingPhase, int colorPattern) {
    glPushMatrix();
    glScalef(1.5f, 0.5f, 0.5f);
    setGullColor(colorPattern);
    glutSolidSphere(0.22, 16, 10);
    glPushMatrix();
    glTranslatef(0.0f, -0.09f, 0.0f);
    glColor3f(0.95f, 0.95f, 0.95f);
    glutSolidSphere(0.18, 14, 8);
    glPopMatrix();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.38f, 0.09f, 0.0f);
    setGullColor(colorPattern);
    glutSolidSphere(0.10, 12, 10);
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.06f, 0.03f, 0.04f);
    glutSolidSphere(0.018, 6, 6);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.06f, 0.03f, -0.04f);
    glutSolidSphere(0.018, 6, 6);
    glPopMatrix();
    glPopMatrix();
    glColor3f(1.0f, 0.8f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.52f, 0.11f, 0.0f);
    glVertex3f(0.42f, 0.13f, 0.03f);
    glVertex3f(0.42f, 0.13f, -0.03f);
    glVertex3f(0.52f, 0.09f, 0.0f);
    glVertex3f(0.42f, 0.07f, 0.03f);
    glVertex3f(0.42f, 0.07f, -0.03f);
    glEnd();
    glPushMatrix();
    glTranslatef(-0.08f, -0.02f, 0.0f);
    float wingFlap = sinf(wingPhase) * 0.5f + 0.2f;
    glBegin(GL_TRIANGLE_STRIP);
    setGullColor(colorPattern);
    glVertex3f(0, 0, 0);
    glVertex3f(-0.7f, wingFlap, 0.3f);
    glColor3f(0.1f, 0.1f, 0.1f);
    glVertex3f(-1.2f, wingFlap * 1.2f, 0.5f);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    setGullColor(colorPattern);
    glVertex3f(0, 0, 0);
    glVertex3f(0.7f, wingFlap, 0.3f);
    glColor3f(0.1f, 0.1f, 0.1f);
    glVertex3f(1.2f, wingFlap * 1.2f, 0.5f);
    glEnd();
    glPopMatrix();
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(-0.35f, 0, 0);
    for (int i = 0; i <= 4; ++i) {
        float angle = -M_PI / 4 + (M_PI / 2) * (i / 4.0f);
        glVertex3f(-0.5f, 0, 0.12f * sinf(angle));
    }
    glEnd();
    glPopMatrix();
}

GullFlock* gullFlockCreate(int numFlocks, int gullsPerFlock, float landscapeScale) {
    int total = numFlocks * gullsPerFlock;
    GullFlock* flock = (GullFlock*)malloc(sizeof(GullFlock));
    flock->gulls = (Gull*)malloc(sizeof(Gull) * total);
    flock->count = total;
    int idx = 0;
    float baseRadius = landscapeScale * 0.25f;
    for (int f = 0; f < numFlocks; ++f) {
        float cx = ((float)rand() / RAND_MAX - 0.5f) * landscapeScale * 0.95f;
        float cz = ((float)rand() / RAND_MAX - 0.5f) * landscapeScale * 0.95f;
        float cy = 18.0f + ((float)rand() / RAND_MAX) * 18.0f;
        float patchRadius = baseRadius * (0.7f + 0.6f * ((float)rand()/RAND_MAX));
        float baseAngle = ((float)rand()/RAND_MAX) * 2.0f * M_PI;
        for (int g = 0; g < gullsPerFlock; ++g, ++idx) {
            Gull* gull = &flock->gulls[idx];
            gull->centerX = cx + (rand()%8 - 4);
            gull->centerY = cy + (rand()%4 - 2);
            gull->centerZ = cz + (rand()%8 - 4);
            gull->radius = patchRadius * (0.8f + 0.4f * ((float)rand()/RAND_MAX));
            gull->angle = baseAngle + g * (2.0f * M_PI / gullsPerFlock);
            gull->speed = 0.15f + 0.3f * ((float)rand()/RAND_MAX);
            gull->wingPhase = ((float)rand()/RAND_MAX) * 2.0f * M_PI;
            gull->heightOffset = (rand()%3 - 1);
            gull->colorPattern = rand() % 6;
            gull->dirX = 1.0f; gull->dirY = 0.0f; gull->dirZ = 0.0f;
            gull->undulationPhase = ((float)rand()/RAND_MAX) * 2.0f * M_PI;
        }
    }
    return flock;
}

void gullFlockUpdate(GullFlock* flock, Landscape* landscape, float deltaTime) {
    for (int i = 0; i < flock->count; ++i) {
        Gull* g = &flock->gulls[i];
        g->angle += g->speed * deltaTime;
        if (g->angle > 2.0f * M_PI) g->angle -= 2.0f * M_PI;
        g->x = g->centerX + g->radius * cosf(g->angle);
        g->z = g->centerZ + g->radius * sinf(g->angle);
        g->y = g->centerY + 0.2f * sinf(g->angle + g->undulationPhase) + g->heightOffset;
        // Direction vector: tangent to path
        g->dirX = cosf(g->angle);
        g->dirY = 0.0f;
        g->dirZ = sinf(g->angle);
        // Clamp gull above terrain
        if (landscape) {
            float terrainHeight = landscapeGetHeight(landscape, g->x, g->z);
            float minAltitude = 7.0f;
            if (g->y < terrainHeight + minAltitude) {
                g->y = terrainHeight + minAltitude;
            }
        }
        g->wingPhase += deltaTime * 5.0f;
        if (g->wingPhase > 2.0f * M_PI) g->wingPhase -= 2.0f * M_PI;
    }
}

void gullFlockRender(const GullFlock* flock, int unused) {
    for (int i = 0; i < flock->count; ++i) {
        const Gull* g = &flock->gulls[i];
        glPushMatrix();
        glTranslatef(g->x, g->y, g->z);
        float angleY = atan2f(g->dirZ, g->dirX) * 180.0f / M_PI;
        glRotatef(angleY, 0, 1, 0);
        glScalef(1.0f, 1.0f, 1.0f);
        drawProceduralGull(g->wingPhase, g->colorPattern);
        glPopMatrix();
    }
}

void gullFlockDestroy(GullFlock* flock) {
    if (flock) {
        free(flock->gulls);
        free(flock);
    }
}