#ifndef OBJECTS_H
#define OBJECTS_H

#include "terrain.h"


#define TREE_SEGMENTS 8
#define BRANCH_SEGMENTS 8
#define MAX_TREES 200
#define MAX_VERTICES 50000
#define MAX_INDICES 100000
#define TREE_TYPES 3
#define MIN_TREE_DIST 6.0f
#define TRUNK_SEGMENTS 8
#define FOLIAGE_SEGMENTS 12


extern GLuint barkTexture;
extern GLuint leafTexture;
extern int treeShader;


typedef enum {
    TREE_PINE,
    TREE_FIR,
    TREE_BIRCH,
} TreeType;

#define NUM_TREE_TYPES 3

typedef struct {
    float x, y, z;
    float scale;
    float rotation;
    float tiltX, tiltZ;
    TreeType type;  
} TreeInstance;

typedef struct {
    TreeInstance* instances;
    float** vertices;    
    float** normals;    
    float** colors;      
    unsigned int** indices;  
    int* vertexCount;    
    int* indexCount;    
    int instanceCount;
} TreeSystem;

TreeSystem* createTreeSystem();
void generateTreeMesh(TreeSystem* trees, int type);
void generatePineTree(TreeSystem* trees, int type,int* currentVertex, int* currentIndex);
void generateRoundTree(TreeSystem* trees, int* currentVertex, int* currentIndex);
void generateBushyTree(TreeSystem* trees, int* currentVertex, int* currentIndex);
void generateTrunk(TreeSystem* trees, int type, int* currentVertex, int* currentIndex);
void generateTrees(TreeSystem* trees, Terrain* terrain);
void renderTrees(TreeSystem* trees, float dayTime);
void freeTreeSystem(TreeSystem* trees);
int placementValid(float x, float z, TreeInstance* existing, int count, float minDist);
float getTerrainHeight(Terrain* terrain, float x, float z);
void initTreeResources(void);
void generateFirTree(TreeSystem* trees, int type,int* currentVertex, int* currentIndex);


#endif