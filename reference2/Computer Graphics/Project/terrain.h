#ifndef TERRAIN_H
#define TERRAIN_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

extern float waterLevel;

// Terrain data structures and constants adapted from:
// - CSCI-4229/5229 course examples
// - GPU Gems 3 Chapter 1: Generating Complex Procedural Terrains
typedef struct {
    float* heightData;
    float* vertices;
    float* normals;
    float* texCoords;
    unsigned int* indices;
    int vertexCount;
    int indexCount;
} Terrain;

typedef struct {
    float x, y, z;
} Vec3;

extern GLuint grassTexture;
extern GLuint rockTexture;
extern GLuint sandTexture;
extern int terrainShader;

Terrain* createTerrain();
void generateHeightMap(Terrain* terrain);
void calculateNormals(Terrain* terrain);
void renderTerrain(Terrain* terrain);
void freeTerrain(Terrain* terrain);
float getTerrainHeight(Terrain* terrain, float x, float z);
void renderWater(float waterLevel, Terrain* terrain, float dayTime);

#define TERRAIN_SIZE 128
#define TERRAIN_SCALE 200.0f
#define TERRAIN_HEIGHT 50.0f

#endif