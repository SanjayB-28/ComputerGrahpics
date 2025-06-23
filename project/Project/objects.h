// ---------------------------------------------
// objects.h - Forest, rock, shrub, and log systems for landscape scenes
// ---------------------------------------------

#ifndef OBJECTS_H
#define OBJECTS_H

#include "landscape.h"

/* Geometry parameters */
#define TREE_SEGMENTS 8          // Branch segments
#define BRANCH_SEGMENTS 8        // Trunk circumference
#define MAX_TREES 2000           // Forest capacity
#define MAX_VERTICES 50000       // Per tree type
#define MAX_INDICES 100000       // Per tree type
#define TREE_TYPES 3             // Tree variations
#define MIN_TREE_DIST 6.0f       // Tree spacing
#define TRUNK_SEGMENTS 8         // Trunk height segments
#define FOLIAGE_SEGMENTS 12      // Foliage segments
#define MAX_BUSH_SUBSPHERES 14   // Shrub parts
#define ROCK_VERTICES 12         // Rock vertices
#define ROCK_TRIANGLES 20        // Rock faces

/* External resources */
extern GLuint barkTexture;
extern GLuint leafTexture;
extern int treeShader;

// --- Forest system ---
/* Tree types */
typedef enum {
    FOREST_TREE_PINE,            // Layered
    FOREST_TREE_FIR,             // Slender
    FOREST_TREE_BIRCH,           // Round foliage
} ForestTreeType;

#define NUM_FOREST_TREE_TYPES 3

/* Tree instance */
typedef struct {
    float x, y, z;               // Position
    float scale;                 // Size
    float rotation;              // Y-rotation
    float tiltX, tiltZ;          // Trunk tilt
    ForestTreeType type;         // Species
} ForestTree;

/* Forest system */
typedef struct {
    ForestTree* instances;       // Trees
    float** vertices;            // Positions
    float** normals;             // Normals
    float** colors;              // Colors
    unsigned int** indices;      // Triangles
    int* vertexCount;            // Vertex counts
    int* indexCount;             // Index counts
    int instanceCount;           // Active trees
} ForestSystem;

/* Forest API */
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

// --- Rock system ---
/* Rock mesh indices */
static const int rockIcosahedronIndices[ROCK_TRIANGLES][3] = {
    {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
    {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
};

/* Rock instance */
typedef struct {
    float x, y, z;               // Position
    float scale;                 // Size
    float xScale, zScale;        // Axis scaling
    float rotation;              // Y-rotation
    float color[3];              // RGB color
    float vertices[ROCK_VERTICES][3]; // Vertex positions
} RockObject;

/* Rock field */
typedef struct {
    RockObject* instances;       // Rocks
    int instanceCount;           // Active rocks
    int maxRocks;                // Field capacity
} RockField;

/* Rock API */
RockField* rockFieldCreate(int maxRocks);
void rockFieldGenerate(RockField* field, Landscape* landscape);
void rockFieldRender(RockField* field);
void rockFieldDestroy(RockField* field);

// --- Shrub system ---
/* Shrub instance */
typedef struct {
    float x, y, z;               // Position
    float scale;                 // Size
    float rotation;              // Y-rotation
    float color[3];              // RGB color
    float subOffsets[MAX_BUSH_SUBSPHERES][3]; // Sub-positions
    float subRadii[MAX_BUSH_SUBSPHERES];      // Sub-sizes
    float subColors[MAX_BUSH_SUBSPHERES][3];  // Sub-colors
    int subCount;                // Active subs
} ShrubObject;

/* Shrub field */
typedef struct {
    ShrubObject* instances;      // Shrubs
    int instanceCount;           // Active shrubs
    int maxBushes;               // Field capacity
} ShrubField;

/* Shrub API */
ShrubField* shrubFieldCreate(int maxBushes);
void shrubFieldGenerate(ShrubField* field, Landscape* landscape);
void shrubFieldRender(ShrubField* field);
void shrubFieldDestroy(ShrubField* field);

// --- Log system ---
/* Log instance */
typedef struct {
    float x, y, z;               // Position
    float length;                // Length
    float radius;                // Radius
    float rotation;              // Y-rotation
    float tilt;                  // Z-tilt
    float color[3];              // RGB color
} LogObject;

/* Log field */
typedef struct {
    LogObject* instances;        // Logs
    int instanceCount;           // Active logs
    int maxLogs;                 // Field capacity
} LogField;

/* Log API */
LogField* logFieldCreate(int maxLogs);
void logFieldGenerate(LogField* field, Landscape* landscape);
void logFieldRender(LogField* field);
void logFieldDestroy(LogField* field);

#endif