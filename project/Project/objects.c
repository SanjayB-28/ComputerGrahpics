// ---------------------------------------------
// objects.c - Forest, rock, shrub, and log systems for procedural landscape objects
// ---------------------------------------------

#include "objects.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// Macros for color and size variety in procedural generation
#define EXTRA_TREE_VARIETY 1
#define COLOR_VARIETY 0.15f
#define SIZE_VARIETY 0.25f
#define TRUNK_VARIETY 0.25f
#define FOLIAGE_VARIETY 0.25f

// Forest system creation and initialization
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

// Generate trunk geometry for a tree type
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
            if (type == FOREST_TREE_BIRCH) {
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

// Generate pine tree foliage geometry
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
            forest->colors[type][*currentVertex * 3 + 0] = 0.35f + colorVar * 1.2f;
            forest->colors[type][*currentVertex * 3 + 1] = 0.45f + colorVar * 1.0f;
            forest->colors[type][*currentVertex * 3 + 2] = 0.15f + colorVar * 0.7f;
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

// Tree mesh generation algorithms for different types
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
            forest->colors[type][*currentVertex * 3 + 0] = 0.20f + colorVar * 1.2f;
            forest->colors[type][*currentVertex * 3 + 1] = 0.55f + colorVar * 1.0f;
            forest->colors[type][*currentVertex * 3 + 2] = 0.10f + colorVar * 0.7f;

            (*currentVertex)++;

            forest->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * (radius * 0.5f);
            forest->vertices[type][*currentVertex * 3 + 1] = startHeight + (layer + 1) * layerSpacing;
            forest->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * (radius * 0.5f);

            forest->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            forest->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            forest->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            forest->colors[type][*currentVertex * 3 + 0] = 0.15f * shade;
            forest->colors[type][*currentVertex * 3 + 1] = 0.55f * shade;
            forest->colors[type][*currentVertex * 3 + 2] = 0.45f * shade;

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

void generateBirchTree(ForestSystem* forest, ForestTreeType type, int* currentVertex, int* currentIndex) {
    float trunkHeight = 2.0f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * SIZE_VARIETY);
    float foliageHeight = 2.0f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * SIZE_VARIETY);
    float baseWidth = 0.7f * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * FOLIAGE_VARIETY);
    int layers = 8 + (EXTRA_TREE_VARIETY ? rand() % 2 : 0);
    
    float trunkRadius = 0.12f;
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
            forest->colors[type][*currentVertex * 3 + 0] = 0.85f - darkPatch;
            forest->colors[type][*currentVertex * 3 + 1] = 0.85f - darkPatch;
            forest->colors[type][*currentVertex * 3 + 2] = 0.80f - darkPatch;

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
        float radius = baseWidth * (1.0f - powf(heightPercent - 0.3f, 2.0f)) * 1.2f;
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
            forest->colors[type][*currentVertex * 3 + 0] = 0.9f * shade;
            forest->colors[type][*currentVertex * 3 + 1] = 0.7f * shade;
            forest->colors[type][*currentVertex * 3 + 2] = 0.3f * shade;

            (*currentVertex)++;

            float upperRadius = radius * (0.95f - heightPercent * 0.1f);
            forest->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * upperRadius * radiusVar;
            forest->vertices[type][*currentVertex * 3 + 1] = startHeight + (layer + 1) * layerSpacing;
            forest->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * upperRadius * radiusVar;

            forest->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            forest->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            forest->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            forest->colors[type][*currentVertex * 3 + 0] = 0.45f * shade;
            forest->colors[type][*currentVertex * 3 + 1] = 0.75f * shade;
            forest->colors[type][*currentVertex * 3 + 2] = 0.25f * shade;

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

void generateTreeMesh(ForestSystem* forest, int type) {
    int currentVertex = 0;
    int currentIndex = 0;

    generateTrunk(forest,type, &currentVertex, &currentIndex);
    
    switch(type) {
        case FOREST_TREE_FIR:
            generateFirTree(forest, type, &currentVertex, &currentIndex);
            break;
        case FOREST_TREE_BIRCH:
            generateBirchTree(forest, type, &currentVertex, &currentIndex);
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

// Tree density calculation
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
                else tree->type = FOREST_TREE_BIRCH;
            } else {
                if (rand_val < 0.45f) tree->type = FOREST_TREE_BIRCH;
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
                case FOREST_TREE_BIRCH:
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
    generateTreeMesh(forest, FOREST_TREE_BIRCH);
}

void forestSystemRender(ForestSystem* forest, float dayTime) {
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

        glVertexPointer(3, GL_FLOAT, 0, forest->vertices[type]);
        glNormalPointer(GL_FLOAT, 0, forest->normals[type]);
        glColorPointer(3, GL_FLOAT, 0, forest->colors[type]);
        
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

int forestPlacementValid(float x, float z, ForestTree* existing, int count, float minDist) {
    for (int i = 0; i < count; i++) {
        float dx = x - existing[i].x;
        float dz = z - existing[i].z;
        float distSq = dx*dx + dz*dz;
        if (distSq < minDist*minDist) return 0;
    }
    return 1;
}

// Helper: Check if a point is in water
static bool is_in_water(float y, float waterLevel) {
    return y <= waterLevel + 0.5f;
}

// Helper: Random float in [a, b]
static float randf(float a, float b) {
    return a + ((float)rand() / RAND_MAX) * (b - a);
}

// --- Rock System Implementation ---
// Handles procedural generation and rendering of rocks
RockField* rockFieldCreate(int maxRocks) {
    RockField* field = (RockField*)malloc(sizeof(RockField));
    if (!field) return NULL;
    field->instances = (RockObject*)malloc(maxRocks * sizeof(RockObject));
    field->instanceCount = 0;
    field->maxRocks = maxRocks;
    return field;
}

// Icosahedron base vertices
static const float rockIcosahedronBase[ROCK_VERTICES][3] = {
    {-0.525731, 0.850651, 0.0}, {0.525731, 0.850651, 0.0}, {-0.525731, -0.850651, 0.0}, {0.525731, -0.850651, 0.0},
    {0.0, -0.525731, 0.850651}, {0.0, 0.525731, 0.850651}, {0.0, -0.525731, -0.850651}, {0.0, 0.525731, -0.850651},
    {0.850651, 0.0, -0.525731}, {0.850651, 0.0, 0.525731}, {-0.850651, 0.0, -0.525731}, {-0.850651, 0.0, 0.525731}
};

void rockFieldGenerate(RockField* field, Landscape* landscape) {
    extern float waterLevel;
    field->instanceCount = 0;
    int maxRocks = field->maxRocks;
    int attempts = 0;
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
        float gray = randf(0.25f, 0.6f);
        rock->color[0] = gray + randf(-0.08f, 0.08f);
        rock->color[1] = gray + randf(-0.08f, 0.08f);
        rock->color[2] = gray + randf(-0.08f, 0.08f);
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

void rockFieldRender(RockField* field) {
    extern const int rockIcosahedronIndices[ROCK_TRIANGLES][3];
    for (int i = 0; i < field->instanceCount; i++) {
        RockObject* rock = &field->instances[i];
        glPushMatrix();
        glTranslatef(rock->x, rock->y, rock->z);
        glRotatef(rock->rotation, 0, 1, 0);
        glScalef(rock->scale * rock->xScale, rock->scale, rock->scale * rock->zScale);
        glColor3fv(rock->color);
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

void rockFieldDestroy(RockField* field) {
    if (field) {
        if (field->instances) free(field->instances);
        free(field);
    }
}

// --- Shrub System Implementation ---
// Handles procedural generation and rendering of shrubs
ShrubField* shrubFieldCreate(int maxBushes) {
    ShrubField* field = (ShrubField*)malloc(sizeof(ShrubField));
    if (!field) return NULL;
    field->instances = (ShrubObject*)malloc(maxBushes * sizeof(ShrubObject));
    field->instanceCount = 0;
    field->maxBushes = maxBushes;
    return field;
}

void shrubFieldGenerate(ShrubField* field, Landscape* landscape) {
    extern float waterLevel;
    field->instanceCount = 0;
    int maxBushes = field->maxBushes;
    int attempts = 0;
    while (field->instanceCount < maxBushes && attempts < maxBushes * 10) {
        float x = randf(-LANDSCAPE_SCALE * 0.48f, LANDSCAPE_SCALE * 0.48f);
        float z = randf(-LANDSCAPE_SCALE * 0.48f, LANDSCAPE_SCALE * 0.48f);
        float y = landscapeGetHeight(landscape, x, z);
        if (is_in_water(y, waterLevel)) { attempts++; continue; }
        ShrubObject* bush = &field->instances[field->instanceCount];
        bush->x = x;
        bush->y = y;
        bush->z = z;
        bush->scale = randf(0.85f, 2.0f);
        bush->rotation = randf(0, 360);
        float g = randf(0.3f, 0.7f);
        bush->color[0] = randf(0.05f, 0.18f);
        bush->color[1] = g + randf(-0.08f, 0.08f);
        bush->color[2] = randf(0.05f, 0.18f);
        bush->subCount = 8 + (rand() % 5);
        for (int s = 0; s < bush->subCount; s++) {
            float angle = randf(0, 2*M_PI);
            float dist = randf(0.0f, 0.38f);
            bush->subOffsets[s][0] = cosf(angle) * dist;
            bush->subOffsets[s][1] = randf(-0.08f, 0.12f);
            bush->subOffsets[s][2] = sinf(angle) * dist;
            bush->subRadii[s] = randf(0.28f, 0.55f);
            float baseG = randf(0.3f, 0.7f);
            bush->subColors[s][0] = randf(0.05f, 0.18f);
            bush->subColors[s][1] = baseG + randf(-0.15f, 0.15f);
            bush->subColors[s][2] = randf(0.05f, 0.18f);
        }
        int flowerCount = rand() % 3;
        for (int f = 0; f < flowerCount && bush->subCount < 12; f++) {
            int s = bush->subCount++;
            float angle = randf(0, 2*M_PI);
            float dist = randf(0.1f, 0.32f);
            bush->subOffsets[s][0] = cosf(angle) * dist;
            bush->subOffsets[s][1] = randf(0.05f, 0.18f);
            bush->subOffsets[s][2] = sinf(angle) * dist;
            bush->subRadii[s] = randf(0.07f, 0.13f);
            float hue = randf(0, 1);
            float r = fabsf(hue * 6 - 3) - 1;
            float g = 2 - fabsf(hue * 6 - 2);
            float b = 2 - fabsf(hue * 6 - 4);
            bush->subColors[s][0] = fmaxf(0, fminf(1, r));
            bush->subColors[s][1] = fmaxf(0, fminf(1, g));
            bush->subColors[s][2] = fmaxf(0, fminf(1, b));
        }
        field->instanceCount++;
        attempts++;
    }
}

void shrubFieldRender(ShrubField* field) {
    for (int i = 0; i < field->instanceCount; i++) {
        ShrubObject* bush = &field->instances[i];
        glPushMatrix();
        glTranslatef(bush->x, bush->y, bush->z);
        glRotatef(bush->rotation, 0, 1, 0);
        glScalef(bush->scale, bush->scale, bush->scale);
        for (int s = 0; s < bush->subCount; s++) {
            glPushMatrix();
            glTranslatef(bush->subOffsets[s][0], bush->subOffsets[s][1], bush->subOffsets[s][2]);
            glColor3fv(bush->subColors[s]);
            int slices = 14, stacks = 10;
            for (int k = 0; k < stacks; k++) {
                float phi1 = M_PI * k / (2 * stacks);
                float phi2 = M_PI * (k + 1) / (2 * stacks);
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

void shrubFieldDestroy(ShrubField* field) {
    if (field) {
        if (field->instances) free(field->instances);
        free(field);
    }
}

// --- Log System Implementation ---
// Handles procedural generation and rendering of logs
LogField* logFieldCreate(int maxLogs) {
    LogField* field = (LogField*)malloc(sizeof(LogField));
    if (!field) return NULL;
    field->instances = (LogObject*)malloc(maxLogs * sizeof(LogObject));
    field->instanceCount = 0;
    field->maxLogs = maxLogs;
    return field;
}

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
        field->instanceCount++;
        attempts++;
    }
}

void logFieldRender(LogField* field) {
    for (int i = 0; i < field->instanceCount; i++) {
        LogObject* log = &field->instances[i];
        glPushMatrix();
        glTranslatef(log->x, log->y, log->z);
        glRotatef(log->rotation, 0, 1, 0);
        glRotatef(log->tilt, 0, 0, 1);
        glColor3fv(log->color);
        int slices = 12;
        float len = log->length, r = log->radius;
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float theta = 2 * M_PI * j / slices;
            float x = cosf(theta), z = sinf(theta);
            glNormal3f(x, 0, z);
            glVertex3f(x * r, 0, z * r);
            glVertex3f(x * r, len, z * r);
        }
        glEnd();
        for (int end = 0; end < 2; end++) {
            float y = (end == 0) ? 0 : len;
            float sign = (end == 0) ? -1.0f : 1.0f;
            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0, sign, 0);
            glVertex3f(0, y, 0);
            for (int j = 0; j <= slices; j++) {
                float theta = 2 * M_PI * j / slices;
                float x = cosf(theta), z = sinf(theta);
                glVertex3f(x * r, y, z * r);
            }
            glEnd();
        }
        glPopMatrix();
    }
}

void logFieldDestroy(LogField* field) {
    if (field) {
        if (field->instances) free(field->instances);
        free(field);
    }
}