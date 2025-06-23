// ---------------------------------------------
// objects.h - Forest, rock, shrub, and log object systems for procedural landscape scenes
// ---------------------------------------------

#ifndef OBJECTS_H
#define OBJECTS_H

#include "landscape.h"

#define TREE_SEGMENTS 8
#define BRANCH_SEGMENTS 8
#define MAX_TREES 2000
#define MAX_VERTICES 50000
#define MAX_INDICES 100000
#define TREE_TYPES 3
#define MIN_TREE_DIST 6.0f
#define TRUNK_SEGMENTS 8
#define FOLIAGE_SEGMENTS 12
#define MAX_BUSH_SUBSPHERES 14
#define ROCK_VERTICES 12
#define ROCK_TRIANGLES 20

extern GLuint barkTexture;
extern GLuint leafTexture;
extern int treeShader;

// Forest system types and API

typedef enum {
    FOREST_TREE_PINE,
    FOREST_TREE_FIR,
    FOREST_TREE_BIRCH,
} ForestTreeType;

#define NUM_FOREST_TREE_TYPES 3

typedef struct {
    float x, y, z;
    float scale;
    float rotation;
    float tiltX, tiltZ;
    ForestTreeType type;
} ForestTree;

typedef struct {
    ForestTree* instances;
    float** vertices;
    float** normals;
    float** colors;
    unsigned int** indices;
    int* vertexCount;
    int* indexCount;
    int instanceCount;
} ForestSystem;

ForestSystem* forestSystemCreate();
void forestSystemGenerateMesh(ForestSystem* forest, int type);
void forestSystemGeneratePine(ForestSystem* forest, int type, int* currentVertex, int* currentIndex);
void forestSystemGenerateRound(ForestSystem* forest, int* currentVertex, int* currentIndex);
void forestSystemGenerateBushy(ForestSystem* forest, int* currentVertex, int* currentIndex);
void forestSystemGenerateTrunk(ForestSystem* forest, int type, int* currentVertex, int* currentIndex);
void forestSystemGenerate(ForestSystem* forest, Landscape* landscape);
void forestSystemRender(ForestSystem* forest, float dayTime);
void forestSystemDestroy(ForestSystem* forest);
int forestPlacementValid(float x, float z, ForestTree* existing, int count, float minDist);
void forestSystemInitResources(void);
void forestSystemGenerateFir(ForestSystem* forest, int type, int* currentVertex, int* currentIndex);

// Rock system types and API

static const int rockIcosahedronIndices[ROCK_TRIANGLES][3] = {
    {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
    {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
};

typedef struct {
    float x, y, z;
    float scale;
    float xScale, zScale;
    float rotation;
    float color[3];
    float vertices[ROCK_VERTICES][3];
} RockObject;

typedef struct {
    RockObject* instances;
    int instanceCount;
    int maxRocks;
} RockField;

RockField* rockFieldCreate(int maxRocks);
void rockFieldGenerate(RockField* field, Landscape* landscape);
void rockFieldRender(RockField* field);
void rockFieldDestroy(RockField* field);

// Shrub system types and API

typedef struct {
    float x, y, z;
    float scale;
    float rotation;
    float color[3];
    float subOffsets[MAX_BUSH_SUBSPHERES][3];
    float subRadii[MAX_BUSH_SUBSPHERES];
    float subColors[MAX_BUSH_SUBSPHERES][3];
    int subCount;
} ShrubObject;

typedef struct {
    ShrubObject* instances;
    int instanceCount;
    int maxBushes;
} ShrubField;

ShrubField* shrubFieldCreate(int maxBushes);
void shrubFieldGenerate(ShrubField* field, Landscape* landscape);
void shrubFieldRender(ShrubField* field);
void shrubFieldDestroy(ShrubField* field);

// Log system types and API

typedef struct {
    float x, y, z;
    float length;
    float radius;
    float rotation;
    float tilt;
    float color[3];
} LogObject;

typedef struct {
    LogObject* instances;
    int instanceCount;
    int maxLogs;
} LogField;

LogField* logFieldCreate(int maxLogs);
void logFieldGenerate(LogField* field, Landscape* landscape);
void logFieldRender(LogField* field);
void logFieldDestroy(LogField* field);

#endif