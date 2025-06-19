#include "objects.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

TreeSystem* createTreeSystem() {
    TreeSystem* trees = (TreeSystem*)malloc(sizeof(TreeSystem));
    if (!trees) return NULL;

    trees->instances = (TreeInstance*)malloc(MAX_TREES * sizeof(TreeInstance));
    
    trees->vertices = (float**)malloc(NUM_TREE_TYPES * sizeof(float*));
    trees->normals = (float**)malloc(NUM_TREE_TYPES * sizeof(float*));
    trees->colors = (float**)malloc(NUM_TREE_TYPES * sizeof(float*));
    trees->indices = (unsigned int**)malloc(NUM_TREE_TYPES * sizeof(unsigned int*));
    trees->vertexCount = (int*)malloc(NUM_TREE_TYPES * sizeof(int));
    trees->indexCount = (int*)malloc(NUM_TREE_TYPES * sizeof(int));

    if (!trees->vertices || !trees->normals || !trees->colors || 
        !trees->indices || !trees->vertexCount || !trees->indexCount) {
        freeTreeSystem(trees);
        return NULL;
    }

    for (int type = 0; type < NUM_TREE_TYPES; type++) {
        trees->vertices[type] = (float*)malloc(MAX_VERTICES * 3 * sizeof(float));
        trees->normals[type] = (float*)malloc(MAX_VERTICES * 3 * sizeof(float));
        trees->colors[type] = (float*)malloc(MAX_VERTICES * 3 * sizeof(float));
        trees->indices[type] = (unsigned int*)malloc(MAX_INDICES * sizeof(unsigned int));
        trees->vertexCount[type] = 0;
        trees->indexCount[type] = 0;

        if (!trees->vertices[type] || !trees->normals[type] || 
            !trees->colors[type] || !trees->indices[type]) {
            freeTreeSystem(trees);
            return NULL;
        }
    }

    trees->instanceCount = 0;
    return trees;
}

void generateTrunk(TreeSystem* trees, int type, int* currentVertex, int* currentIndex) {
    float trunkHeight = 1.2f;
    float trunkRadius = 0.15f;
    int trunkSegments = 8;

    for (int i = 0; i <= trunkSegments; i++) {
        float heightPercent = (float)i / trunkSegments;
        float y = heightPercent * trunkHeight;
        float radius = trunkRadius * (1.0f - heightPercent * 0.2f);

        for (int j = 0; j < BRANCH_SEGMENTS; j++) {
            float angle = (float)j / BRANCH_SEGMENTS * 2.0f * M_PI;
            float c = cosf(angle);
            float s = sinf(angle);

            trees->vertices[type][*currentVertex * 3 + 0] = c * radius;
            trees->vertices[type][*currentVertex * 3 + 1] = y;
            trees->vertices[type][*currentVertex * 3 + 2] = s * radius;

            trees->normals[type][*currentVertex * 3 + 0] = c;
            trees->normals[type][*currentVertex * 3 + 1] = 0.1f;
            trees->normals[type][*currentVertex * 3 + 2] = s;

            float colorVar = (rand() % 20 - 10) / 100.0f;
            trees->colors[type][*currentVertex * 3 + 0] = 0.25f + colorVar;
            trees->colors[type][*currentVertex * 3 + 1] = 0.15f + colorVar;
            trees->colors[type][*currentVertex * 3 + 2] = 0.05f + colorVar;

            (*currentVertex)++;
        }
    }

    for (int i = 0; i < trunkSegments; i++) {
        for (int j = 0; j < BRANCH_SEGMENTS; j++) {
            int curr = i * BRANCH_SEGMENTS + j;
            int next = i * BRANCH_SEGMENTS + ((j + 1) % BRANCH_SEGMENTS);
            int currUp = (i + 1) * BRANCH_SEGMENTS + j;
            int nextUp = (i + 1) * BRANCH_SEGMENTS + ((j + 1) % BRANCH_SEGMENTS);

            trees->indices[type][*currentIndex + 0] = curr;
            trees->indices[type][*currentIndex + 1] = next;
            trees->indices[type][*currentIndex + 2] = currUp;

            trees->indices[type][*currentIndex + 3] = next;
            trees->indices[type][*currentIndex + 4] = nextUp;
            trees->indices[type][*currentIndex + 5] = currUp;

            *currentIndex += 6;
        }
    }
}

void generatePineTree(TreeSystem* trees, int type, int* currentVertex, int* currentIndex) {
    float foliageHeight = 2.2f;
    float baseWidth = 1.1f;
    int layers = 8;
    
    float layerSpacing = foliageHeight / layers;
    float startHeight = 0.6f;
    
    for (int layer = 0; layer < layers; layer++) {
        float heightPercent = (float)layer / layers;
        float radius = baseWidth * (1.0f - powf(heightPercent, 0.85f));
        int segments = 8;

        for (int i = 0; i <= segments; i++) {
            radius *= (0.95f + ((float)rand() / RAND_MAX) * 0.1f);
            float angle = (float)i / segments * 2.0f * M_PI;
            
            trees->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * radius;
            trees->vertices[type][*currentVertex * 3 + 1] = startHeight + layer * layerSpacing;
            trees->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * radius;

            float nx = cosf(angle);
            float ny = 1.0f;
            float nz = sinf(angle);
            float nlen = sqrtf(nx*nx + ny*ny + nz*nz);
            trees->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            trees->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            trees->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            float heightShade = 0.8f + (heightPercent * 0.4f);
            float randomShade = 0.9f + ((float)rand() / RAND_MAX) * 0.2f;
            float shade = heightShade * randomShade;
            
            float colorVar = (rand() % 20 - 10) / 100.0f;
            trees->colors[type][*currentVertex * 3 + 0] = 0.45f + colorVar;
            trees->colors[type][*currentVertex * 3 + 1] = 0.25f + colorVar;
            trees->colors[type][*currentVertex * 3 + 2] = 0.15f + colorVar;

            (*currentVertex)++;

            trees->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * (radius * 0.7f);
            trees->vertices[type][*currentVertex * 3 + 1] = startHeight + (layer + 1) * layerSpacing;
            trees->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * (radius * 0.7f);

            trees->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            trees->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            trees->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            trees->colors[type][*currentVertex * 3 + 0] = 0.05f * shade;
            trees->colors[type][*currentVertex * 3 + 1] = 0.25f * shade;
            trees->colors[type][*currentVertex * 3 + 2] = 0.05f * shade; 
            (*currentVertex)++;

            if (i > 0) {
                int base = *currentVertex - 4;
                trees->indices[type][*currentIndex + 0] = base;
                trees->indices[type][*currentIndex + 1] = base + 2;
                trees->indices[type][*currentIndex + 2] = base + 1;

                trees->indices[type][*currentIndex + 3] = base + 2;
                trees->indices[type][*currentIndex + 4] = base + 3;
                trees->indices[type][*currentIndex + 5] = base + 1;

                *currentIndex += 6;
            }
        }
    }
}

// Tree mesh generation algorithms for different types
// Adapted from Real-Time Rendering 4th Edition and enhanced with Claude
void generateFirTree(TreeSystem* trees, int type, int* currentVertex, int* currentIndex) {
    float foliageHeight = 2.5f;
    float baseWidth = 0.8f;
    int layers = 10;
    
    float layerSpacing = foliageHeight / layers;
    float startHeight = 0.8f;
    
    for (int layer = 0; layer < layers; layer++) {
        float heightPercent = (float)layer / layers;
        float radius = baseWidth * (1.0f - powf(heightPercent, 0.5f));
        int segments = 6;

        for (int i = 0; i <= segments; i++) {
            float angle = (float)i / segments * 2.0f * M_PI;
            
            trees->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * radius;
            trees->vertices[type][*currentVertex * 3 + 1] = startHeight + layer * layerSpacing;
            trees->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * radius;

            float nx = cosf(angle);
            float ny = 0.5f;
            float nz = sinf(angle);
            float nlen = sqrtf(nx*nx + ny*ny + nz*nz);
            trees->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            trees->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            trees->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            float shade = 0.7f + ((float)rand() / RAND_MAX) * 0.3f;
            float colorVar = (rand() % 20 - 10) / 100.0f;
            trees->colors[type][*currentVertex * 3 + 0] = 0.30f + colorVar;
            trees->colors[type][*currentVertex * 3 + 1] = 0.20f + colorVar;
            trees->colors[type][*currentVertex * 3 + 2] = 0.10f + colorVar;

            (*currentVertex)++;

            trees->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * (radius * 0.5f);
            trees->vertices[type][*currentVertex * 3 + 1] = startHeight + (layer + 1) * layerSpacing;
            trees->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * (radius * 0.5f);

            trees->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            trees->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            trees->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            trees->colors[type][*currentVertex * 3 + 0] = 0.15f * shade;
            trees->colors[type][*currentVertex * 3 + 1] = 0.55f * shade;
            trees->colors[type][*currentVertex * 3 + 2] = 0.45f * shade;

            (*currentVertex)++;

            if (i > 0) {
                int base = *currentVertex - 4;
                trees->indices[type][*currentIndex + 0] = base;
                trees->indices[type][*currentIndex + 1] = base + 2;
                trees->indices[type][*currentIndex + 2] = base + 1;

                trees->indices[type][*currentIndex + 3] = base + 2;
                trees->indices[type][*currentIndex + 4] = base + 3;
                trees->indices[type][*currentIndex + 5] = base + 1;

                *currentIndex += 6;
            }
        }
    }
}

void generateBirchTree(TreeSystem* trees, TreeType type, int* currentVertex, int* currentIndex) {
    float trunkHeight = 2.0f;
    float foliageHeight = 2.0f;
    float baseWidth = 0.7f;
    int layers = 8;
    
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

            trees->vertices[type][*currentVertex * 3 + 0] = c * radius;
            trees->vertices[type][*currentVertex * 3 + 1] = y;
            trees->vertices[type][*currentVertex * 3 + 2] = s * radius;

            trees->normals[type][*currentVertex * 3 + 0] = c;
            trees->normals[type][*currentVertex * 3 + 1] = 0.1f;
            trees->normals[type][*currentVertex * 3 + 2] = s;

            float darkPatch = (rand() % 20) / 100.0f;
            trees->colors[type][*currentVertex * 3 + 0] = 0.85f - darkPatch;
            trees->colors[type][*currentVertex * 3 + 1] = 0.85f - darkPatch;
            trees->colors[type][*currentVertex * 3 + 2] = 0.80f - darkPatch;

            (*currentVertex)++;
        }
    }

    for (int i = 0; i < trunkSegments; i++) {
        for (int j = 0; j < BRANCH_SEGMENTS; j++) {
            int curr = i * BRANCH_SEGMENTS + j;
            int next = i * BRANCH_SEGMENTS + ((j + 1) % BRANCH_SEGMENTS);
            int currUp = (i + 1) * BRANCH_SEGMENTS + j;
            int nextUp = (i + 1) * BRANCH_SEGMENTS + ((j + 1) % BRANCH_SEGMENTS);

            trees->indices[type][*currentIndex + 0] = curr;
            trees->indices[type][*currentIndex + 1] = next;
            trees->indices[type][*currentIndex + 2] = currUp;

            trees->indices[type][*currentIndex + 3] = next;
            trees->indices[type][*currentIndex + 4] = nextUp;
            trees->indices[type][*currentIndex + 5] = currUp;

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
            
            trees->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * radius * radiusVar;
            trees->vertices[type][*currentVertex * 3 + 1] = startHeight + layer * layerSpacing;
            trees->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * radius * radiusVar;

            float nx = cosf(angle);
            float ny = 0.7f;
            float nz = sinf(angle);
            float nlen = sqrtf(nx*nx + ny*ny + nz*nz);
            trees->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            trees->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            trees->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            float shade = 0.8f + ((float)rand() / RAND_MAX) * 0.2f;
            trees->colors[type][*currentVertex * 3 + 0] = 0.9f * shade;
            trees->colors[type][*currentVertex * 3 + 1] = 0.7f * shade;
            trees->colors[type][*currentVertex * 3 + 2] = 0.3f * shade;

            (*currentVertex)++;

            float upperRadius = radius * (0.95f - heightPercent * 0.1f);
            trees->vertices[type][*currentVertex * 3 + 0] = cosf(angle) * upperRadius * radiusVar;
            trees->vertices[type][*currentVertex * 3 + 1] = startHeight + (layer + 1) * layerSpacing;
            trees->vertices[type][*currentVertex * 3 + 2] = sinf(angle) * upperRadius * radiusVar;

            trees->normals[type][*currentVertex * 3 + 0] = nx/nlen;
            trees->normals[type][*currentVertex * 3 + 1] = ny/nlen;
            trees->normals[type][*currentVertex * 3 + 2] = nz/nlen;

            trees->colors[type][*currentVertex * 3 + 0] = 0.45f * shade;
            trees->colors[type][*currentVertex * 3 + 1] = 0.75f * shade;
            trees->colors[type][*currentVertex * 3 + 2] = 0.25f * shade;

            (*currentVertex)++;

            if (i > 0) {
                int base = *currentVertex - 4;
                trees->indices[type][*currentIndex + 0] = base;
                trees->indices[type][*currentIndex + 1] = base + 2;
                trees->indices[type][*currentIndex + 2] = base + 1;

                trees->indices[type][*currentIndex + 3] = base + 2;
                trees->indices[type][*currentIndex + 4] = base + 3;
                trees->indices[type][*currentIndex + 5] = base + 1;

                *currentIndex += 6;
            }
        }
    }
}

void generateTreeMesh(TreeSystem* trees, int type) {
    int currentVertex = 0;
    int currentIndex = 0;

    generateTrunk(trees,type, &currentVertex, &currentIndex);
    
    switch(type) {
        case TREE_FIR:
            generateFirTree(trees, type, &currentVertex, &currentIndex);
            break;
        case TREE_BIRCH:
            generateBirchTree(trees, type, &currentVertex, &currentIndex);
            break;
        case TREE_PINE:
            generatePineTree(trees, type, &currentVertex, &currentIndex);
            break;
        default:
            break;
    }
    
    trees->vertexCount[type] = currentVertex;
    trees->indexCount[type] = currentIndex;
}

// Tree density calculation adapted from GPU Gems 3
// Chapter 1: Generating Complex Procedural Terrains
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

void generateTrees(TreeSystem* trees, Terrain* terrain) {
    const int maxTrees = 1000;
    const float minDist = 5.0f;
    trees->instanceCount = 0;
    
    for (int i = 0; i < maxTrees; i++) {
        float x = ((float)rand() / RAND_MAX - 1.2f) * TERRAIN_SCALE * 0.4f;
        float z = ((float)rand() / RAND_MAX - 0.5f) * TERRAIN_SCALE * 0.8f;
        float y = getTerrainHeight(terrain, x, z);
        
        float slope = 0.0f;
        if (x > -TERRAIN_SCALE/2 && x < TERRAIN_SCALE/2 &&
            z > -TERRAIN_SCALE/2 && z < TERRAIN_SCALE/2) {
            float dx = getTerrainHeight(terrain, x + 1.0f, z) - getTerrainHeight(terrain, x - 1.0f, z);
            float dz = getTerrainHeight(terrain, x, z + 1.0f) - getTerrainHeight(terrain, x, z - 1.0f);
            slope = sqrt(dx*dx + dz*dz) / 2.0f;
        }
        
        float baseDensity = getTreeDensity(y, slope);
        float edgeFactor = 1.0f;
        float distFromCenter = sqrt(x*x + z*z) / (TERRAIN_SCALE * 0.4f);
        
        if (distFromCenter > 0.8f) {
            edgeFactor = 1.0f - (distFromCenter - 0.8f) * 2.0f;
            edgeFactor = edgeFactor < 0.0f ? 0.0f : edgeFactor;
        }
        
        float randomFactor = ((float)rand() / RAND_MAX) * 0.2f + 0.8f;
        float finalDensity = baseDensity * edgeFactor * randomFactor;

        if ((float)rand() / RAND_MAX > finalDensity * 0.8f) continue;
        
        float placementDist = minDist * (1.0f + (y / 20.0f) * 0.3f);
        if (placementValid(x, z, trees->instances, trees->instanceCount, placementDist)) {
            TreeInstance* tree = &trees->instances[trees->instanceCount];
            
            float rand_val = (float)rand() / RAND_MAX;
            if (y > 15.0f) {
                tree->type = (rand_val < 0.75f) ? TREE_PINE : TREE_FIR;
            } else if (y > 8.0f) {
                if (rand_val < 0.35f) tree->type = TREE_PINE;
                else if (rand_val < 0.75f) tree->type = TREE_FIR;
                else tree->type = TREE_BIRCH;
            } else {
                if (rand_val < 0.45f) tree->type = TREE_BIRCH;
                else if (rand_val < 0.85f) tree->type = TREE_FIR;
                else tree->type = TREE_PINE;
            }
            
            tree->x = x;
            tree->y = y;
            tree->z = z;
            
            float heightInfluence = 1.0f - (y / 20.0f) * 0.2f;
            float slopeInfluence = 1.0f - slope * 0.4f;
            float randomScale = 0.9f + ((float)rand() / RAND_MAX) * 0.6f;
            float environmentalScale = heightInfluence * slopeInfluence;
            
            switch(tree->type) {
                case TREE_BIRCH:
                    tree->scale = randomScale * environmentalScale * 1.4f;
                    break;
                case TREE_PINE:
                    tree->scale = randomScale * environmentalScale * 1.2f;
                    break;
                case TREE_FIR:
                    tree->scale = randomScale * environmentalScale * 1.3f;
                    break;
                default:
                    tree->scale = randomScale * environmentalScale;
                    break;
            }
            
            tree->rotation = ((float)rand() / RAND_MAX) * 360.0f;
            tree->tiltX = ((float)(rand() % 10) - 5) * 0.5f;
            tree->tiltZ = ((float)(rand() % 10) - 5) * 0.5f;
            
            if (tree->scale > 0.4f) {
                trees->instanceCount++;
                if (trees->instanceCount >= maxTrees) break;
            }
        }
    }

    generateTreeMesh(trees, TREE_PINE);
    generateTreeMesh(trees, TREE_FIR);
    generateTreeMesh(trees, TREE_BIRCH);
}

void renderTrees(TreeSystem* trees, float dayTime) {
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

    for (int i = 0; i < trees->instanceCount; i++) {
        TreeInstance* tree = &trees->instances[i];
        int type = tree->type;

        glVertexPointer(3, GL_FLOAT, 0, trees->vertices[type]);
        glNormalPointer(GL_FLOAT, 0, trees->normals[type]);
        glColorPointer(3, GL_FLOAT, 0, trees->colors[type]);
        
        glPushMatrix();
        glTranslatef(tree->x, tree->y, tree->z);
        glRotatef(tree->rotation, 0, 1, 0);
        glRotatef(tree->tiltX, 1, 0, 0);
        glRotatef(tree->tiltZ, 0, 0, 1);
        glScalef(tree->scale, tree->scale, tree->scale);
        
        glColor3f(brightness, brightness, brightness);
        glDrawElements(GL_TRIANGLES, trees->indexCount[type], 
                      GL_UNSIGNED_INT, trees->indices[type]);
        
        glPopMatrix();
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void freeTreeSystem(TreeSystem* trees) {
    if (trees) {
        if (trees->vertices) {
            for (int type = 0; type < NUM_TREE_TYPES; type++) {
                free(trees->vertices[type]);
                free(trees->normals[type]);
                free(trees->colors[type]);
                free(trees->indices[type]);
            }
            free(trees->vertices);
            free(trees->normals);
            free(trees->colors);
            free(trees->indices);
        }
        free(trees->vertexCount);
        free(trees->indexCount);
        free(trees->instances);
        free(trees);
    }
}

int placementValid(float x, float z, TreeInstance* existing, int count, float minDist) {
    for (int i = 0; i < count; i++) {
        float dx = x - existing[i].x;
        float dz = z - existing[i].z;
        float distSq = dx*dx + dz*dz;
        if (distSq < minDist*minDist) return 0;
    }
    return 1;
}