#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include "CSCIx229.h"
#include "grass.h"
#include "shaders.h"

typedef struct {
    float x, y, z;      
    float swaySeed;     
    float offsetX;      
    float offsetY;      
    float bladeHeight;  
    float bladeWidth;   
    float colorVar;     
    float rotation;     
} GrassVertex;

static GLuint grassVBO = 0, grassVAO = 0;
static GLuint grassShader = 0;
static GLuint grassTex = 0;
static int grassCount = 0;

extern float waterLevel;

static float randf(float a, float b) {
    return a + ((float)rand() / RAND_MAX) * (b - a);
}

void grassSystemInit(Landscape* landscape, float areaSize, int numBlades) {
    grassCount = numBlades;
    float clampFactor = 0.98f;
    float halfScale = areaSize * 0.5f * clampFactor;
    GrassVertex* data = (GrassVertex*)malloc(sizeof(GrassVertex) * 6 * numBlades);
    for (int i = 0; i < numBlades; ++i) {
        float x = randf(-halfScale, halfScale);
        float z = randf(-halfScale, halfScale);
        float y = landscapeGetHeight(landscape, x, z);
        float nx = (x / LANDSCAPE_SCALE + 0.5f) * (LANDSCAPE_SIZE - 1);
        float nz = (z / LANDSCAPE_SCALE + 0.5f) * (LANDSCAPE_SIZE - 1);
        int ix = (int)nx;
        int iz = (int)nz;
        if (ix < 0) ix = 0;
        if (iz < 0) iz = 0;
        if (ix >= LANDSCAPE_SIZE-1) ix = LANDSCAPE_SIZE-2;
        if (iz >= LANDSCAPE_SIZE-1) iz = LANDSCAPE_SIZE-2;
        int normalIndex = iz * LANDSCAPE_SIZE + ix;
        float* normal = &landscape->normals[normalIndex * 3];
        float slope = acosf(fminf(fmaxf(normal[1], -1.0f), 1.0f)); 
        float slopeDeg = slope * (180.0f / M_PI);
        if (y < waterLevel + 0.2f) continue;
        if (slopeDeg > 32.0f) continue;
        float swaySeed = randf(0.0f, 1.0f);
        float bladeHeight = randf(0.7f, 1.5f);
        float bladeWidth = randf(0.05f, 0.13f);
        float colorVar = randf(-0.08f, 0.08f);
        float rotation = randf(0.0f, 2.0f * (float)M_PI);
        int colorIndex = rand() % 4; 
        for (int v = 0; v < 6; ++v) {
            GrassVertex vert = {x, y, z, swaySeed, 0, 0, bladeHeight, bladeWidth, colorVar, rotation};
            switch (v) {
                case 0: vert.offsetX = -bladeWidth/2; vert.offsetY = 0; break; 
                case 1: vert.offsetX = -bladeWidth/2; vert.offsetY = bladeHeight; break; 
                case 2: vert.offsetX = bladeWidth/2; vert.offsetY = bladeHeight; break; 
                case 3: vert.offsetX = -bladeWidth/2; vert.offsetY = 0; break; 
                case 4: vert.offsetX = bladeWidth/2; vert.offsetY = bladeHeight; break; 
                case 5: vert.offsetX = bladeWidth/2; vert.offsetY = 0; break; 
            }
            vert.colorVar += colorIndex * 0.25f;
            data[i*6+v] = vert;
        }
    }
#ifdef __APPLE__
    glGenVertexArraysAPPLE(1, &grassVAO);
    glBindVertexArrayAPPLE(grassVAO);
#else
    glGenVertexArrays(1, &grassVAO);
    glBindVertexArray(grassVAO);
#endif
    glGenBuffers(1, &grassVBO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrassVertex) * 6 * numBlades, data, GL_STATIC_DRAW);
    free(data);
    grassShader = loadShader("shaders/grass.vert", "shaders/grass.frag");
    grassTex = LoadTexBMP("tex/leaf.bmp");
}

void grassSystemRender(float time, float windStrength, const float sunDir[3], const float ambient[3]) {
    if (!grassShader || !grassVBO || !grassVAO) return;
    glUseProgram(grassShader);
    GLint timeLoc = glGetUniformLocation(grassShader, "time");
    GLint windLoc = glGetUniformLocation(grassShader, "windStrength");
    glUniform1f(timeLoc, time);
    glUniform1f(windLoc, windStrength);
    GLint sunDirLoc = glGetUniformLocation(grassShader, "sunDir");
    GLint ambientLoc = glGetUniformLocation(grassShader, "ambient");
    glUniform3fv(sunDirLoc, 1, sunDir);
    glUniform3fv(ambientLoc, 1, ambient);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTex);
    GLint texLoc = glGetUniformLocation(grassShader, "grassTex");
    glUniform1i(texLoc, 0);
#ifdef __APPLE__
    glBindVertexArrayAPPLE(grassVAO);
#else
    glBindVertexArray(grassVAO);
#endif
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    GLint posLoc = glGetAttribLocation(grassShader, "position");
    GLint swayLoc = glGetAttribLocation(grassShader, "swaySeed");
    GLint offsetXLoc = glGetAttribLocation(grassShader, "offsetX");
    GLint offsetYLoc = glGetAttribLocation(grassShader, "offsetY");
    GLint bladeHeightLoc = glGetAttribLocation(grassShader, "bladeHeight");
    GLint bladeWidthLoc = glGetAttribLocation(grassShader, "bladeWidth");
    GLint colorVarLoc = glGetAttribLocation(grassShader, "colorVar");
    GLint rotationLoc = glGetAttribLocation(grassShader, "rotation");
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)0);
    glEnableVertexAttribArray(swayLoc);
    glVertexAttribPointer(swayLoc, 1, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(offsetXLoc);
    glVertexAttribPointer(offsetXLoc, 1, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)(sizeof(float)*4));
    glEnableVertexAttribArray(offsetYLoc);
    glVertexAttribPointer(offsetYLoc, 1, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)(sizeof(float)*5));
    glEnableVertexAttribArray(bladeHeightLoc);
    glVertexAttribPointer(bladeHeightLoc, 1, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)(sizeof(float)*6));
    glEnableVertexAttribArray(bladeWidthLoc);
    glVertexAttribPointer(bladeWidthLoc, 1, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)(sizeof(float)*7));
    glEnableVertexAttribArray(colorVarLoc);
    glVertexAttribPointer(colorVarLoc, 1, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)(sizeof(float)*8));
    glEnableVertexAttribArray(rotationLoc);
    glVertexAttribPointer(rotationLoc, 1, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)(sizeof(float)*9));
    glDrawArrays(GL_TRIANGLES, 0, grassCount * 6);
    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(swayLoc);
    glDisableVertexAttribArray(offsetXLoc);
    glDisableVertexAttribArray(offsetYLoc);
    glDisableVertexAttribArray(bladeHeightLoc);
    glDisableVertexAttribArray(bladeWidthLoc);
    glDisableVertexAttribArray(colorVarLoc);
    glDisableVertexAttribArray(rotationLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
#ifdef __APPLE__
    glBindVertexArrayAPPLE(0);
#else
    glBindVertexArray(0);
#endif
    glUseProgram(0);
}

void grassSystemCleanup() {
    if (grassVBO) glDeleteBuffers(1, &grassVBO);
    if (grassVAO) {
#ifdef __APPLE__
        glDeleteVertexArraysAPPLE(1, &grassVAO);
#else
        glDeleteVertexArrays(1, &grassVAO);
#endif
    }
    grassVBO = 0;
    grassVAO = 0;
    grassShader = 0;
    if (grassTex) glDeleteTextures(1, &grassTex);
    grassTex = 0;
} 