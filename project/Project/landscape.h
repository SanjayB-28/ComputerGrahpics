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

extern float waterLevel;

typedef struct {
    float* elevationData;   
    float* vertices;        
    float* normals;         
    float* texCoords;       
    unsigned int* indices;  
    int vertexCount;        
    int indexCount;         
} Landscape;

extern GLuint grassTexture;
extern GLuint rockTexture;
extern GLuint sandTexture;
extern int terrainShader;

Landscape* landscapeCreate();                  
void landscapeGenerateHeightMap(Landscape* landscape);  
void landscapeCalculateNormals(Landscape* landscape);   
void landscapeRender(Landscape* landscape, int weatherType);  
void landscapeDestroy(Landscape* landscape);    
float landscapeGetHeight(Landscape* landscape, float x, float z);  
void landscapeRenderWater(float waterLevel, Landscape* landscape, float dayTime);  
float landscapeGetSnowBlend(float height, float slope);  
float landscapeSmoothStep(float edge0, float edge1, float x);  

#define LANDSCAPE_SIZE 128       
#define LANDSCAPE_SCALE 200.0f   
#define LANDSCAPE_HEIGHT 50.0f   