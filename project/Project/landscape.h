// ---------------------------------------------
// landscape.h - Terrain data structures and procedural terrain API
// ---------------------------------------------

#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// Global water level for the scene
extern float waterLevel;

// Landscape mesh and heightmap data
typedef struct {
    float* elevationData;   
    float* vertices;        
    float* normals;         
    float* texCoords;       
    unsigned int* indices;  
    int vertexCount;        
    int indexCount;         
} Landscape;

// Simple 3D vector for landscape operations
typedef struct {
    float x, y, z;
} LandscapeVec3;

// Texture and shader handles for terrain rendering
extern GLuint grassTexture;
extern GLuint rockTexture;
extern GLuint sandTexture;
extern int terrainShader;

// --- Landscape API ---
Landscape* landscapeCreate();
void landscapeGenerateHeightMap(Landscape* landscape);
void landscapeCalculateNormals(Landscape* landscape);
void landscapeRender(Landscape* landscape, int weatherType);
void landscapeDestroy(Landscape* landscape);
float landscapeGetHeight(Landscape* landscape, float x, float z);
void landscapeRenderWater(float waterLevel, Landscape* landscape, float dayTime);
float landscapeGetSnowBlend(float height, float slope);
float landscapeSmoothStep(float edge0, float edge1, float x); 

// Landscape mesh parameters
#define LANDSCAPE_SIZE 128
#define LANDSCAPE_SCALE 200.0f
#define LANDSCAPE_HEIGHT 50.0f