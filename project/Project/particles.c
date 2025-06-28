#ifndef __APPLE__
#include <GL/glew.h>
#endif
#include "CSCIx229.h"
#include "particles.h"
#include "shaders.h"
#include "landscape.h"

static GLuint particleVBOs[2] = {0, 0};
static GLuint particleVAOs[2] = {0, 0};
static GLuint updateShader = 0;
static GLuint renderShader = 0;
static int curSrc = 0;
static GLuint heightmapTex = 0;
#define NUM_PARTICLES 20000

static int numParticles = NUM_PARTICLES;
static float cloudHeight = 128.0f;
static float terrainMinX = -LANDSCAPE_SCALE * 0.5f;
static float terrainMaxX = LANDSCAPE_SCALE * 0.5f;
static float terrainMinZ = -LANDSCAPE_SCALE * 0.5f;
static float terrainMaxZ = LANDSCAPE_SCALE * 0.5f;

void particleSystemInit(float terrainScale, float terrainHeight) {
    updateShader = loadShader("shaders/particle_update.vert", NULL);
    glBindAttribLocation(updateShader, 0, "pos");
    glBindAttribLocation(updateShader, 1, "vel");
    glBindAttribLocation(updateShader, 2, "restTime");
    glBindAttribLocation(updateShader, 3, "state");
    const char* varyings[] = { "outPos", "outVel", "outRestTime", "outState" };
#ifdef __APPLE__
    glTransformFeedbackVaryingsEXT(updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS_EXT);
    glLinkProgram(updateShader);
#else
    glTransformFeedbackVaryings(updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS);
    glLinkProgram(updateShader);
#endif
    renderShader = loadShader("shaders/particle_render.vert", "shaders/particle_render.frag");
#ifdef __APPLE__
    glGenVertexArraysAPPLE(2, particleVAOs);
#else
    glGenVertexArrays(2, particleVAOs);
#endif
    glGenBuffers(2, particleVBOs);
    float* initData = (float*)malloc(numParticles * 8 * sizeof(float));
    for (int i = 0; i < numParticles; ++i) {
        float x = terrainMinX + ((float)rand() / RAND_MAX) * (terrainMaxX - terrainMinX);
        float z = terrainMinZ + ((float)rand() / RAND_MAX) * (terrainMaxZ - terrainMinZ);
        float y = cloudHeight;
        float vy = -5.0f - ((float)rand() / RAND_MAX) * 10.0f;
        initData[i*8 + 0] = x;
        initData[i*8 + 1] = y;
        initData[i*8 + 2] = z;
        initData[i*8 + 3] = 0.0f;
        initData[i*8 + 4] = vy;
        initData[i*8 + 5] = 0.0f;
        initData[i*8 + 6] = 0.0f;
        initData[i*8 + 7] = 0.0f;
    }
    for (int b = 0; b < 2; ++b) {
#ifdef __APPLE__
        glBindVertexArrayAPPLE(particleVAOs[b]);
#else
        glBindVertexArray(particleVAOs[b]);
#endif
        glBindBuffer(GL_ARRAY_BUFFER, particleVBOs[b]);
        glBufferData(GL_ARRAY_BUFFER, numParticles * 8 * sizeof(float), initData, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(7 * sizeof(float)));
    }
#ifdef __APPLE__
    glBindVertexArrayAPPLE(0);
#else
    glBindVertexArray(0);
#endif
    free(initData);
}

void particleSystemUpdate(float dt) {
    int src = curSrc;
    int dst = 1 - curSrc;
    glUseProgram(updateShader);
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    GLint timeLoc = glGetUniformLocation(updateShader, "time");
    glUniform1f(timeLoc, time);
    GLint dtLoc = glGetUniformLocation(updateShader, "dt");
    glUniform1f(dtLoc, dt);
    GLint cloudHeightLoc = glGetUniformLocation(updateShader, "cloudHeight");
    glUniform1f(cloudHeightLoc, cloudHeight);
    GLint restThresholdLoc = glGetUniformLocation(updateShader, "restThreshold");
    glUniform1f(restThresholdLoc, 5.0f);
    GLint landscapeScaleLoc = glGetUniformLocation(updateShader, "landscapeScale");
    glUniform1f(landscapeScaleLoc, LANDSCAPE_SCALE);
    GLint landscapeSizeLoc = glGetUniformLocation(updateShader, "landscapeSize");
    glUniform1f(landscapeSizeLoc, LANDSCAPE_SIZE);
    GLint terrainMinXLoc = glGetUniformLocation(updateShader, "terrainMinX");
    glUniform1f(terrainMinXLoc, terrainMinX);
    GLint terrainMaxXLoc = glGetUniformLocation(updateShader, "terrainMaxX");
    glUniform1f(terrainMaxXLoc, terrainMaxX);
    GLint terrainMinZLoc = glGetUniformLocation(updateShader, "terrainMinZ");
    glUniform1f(terrainMinZLoc, terrainMinZ);
    GLint terrainMaxZLoc = glGetUniformLocation(updateShader, "terrainMaxZ");
    glUniform1f(terrainMaxZLoc, terrainMaxZ);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmapTex);
    GLint heightmapLoc = glGetUniformLocation(updateShader, "heightmap");
    glUniform1i(heightmapLoc, 0);
    float windX = 8.0f * sinf(time * 0.3f);
    float windZ = 3.0f * cosf(time * 0.2f);
    GLint windLoc = glGetUniformLocation(updateShader, "wind");
    glUniform2f(windLoc, windX, windZ);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) printf("OpenGL error after uniforms: %d\n", err);
#ifdef __APPLE__
    glBindVertexArrayAPPLE(particleVAOs[src]);
    err = glGetError();
    if (err != GL_NO_ERROR) printf("OpenGL error after glBindVertexArrayAPPLE: %d\n", err);
    glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0, particleVBOs[dst]);
    err = glGetError();
    if (err != GL_NO_ERROR) printf("OpenGL error after glBindBufferBaseEXT: %d\n", err);
    glEnable(GL_RASTERIZER_DISCARD_EXT);
    glBeginTransformFeedbackEXT(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, numParticles);
    glEndTransformFeedbackEXT();
    glDisable(GL_RASTERIZER_DISCARD_EXT);
    glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0, 0);
    glBindVertexArrayAPPLE(0);
#else
    glBindVertexArray(particleVAOs[src]);
    err = glGetError();
    if (err != GL_NO_ERROR) printf("OpenGL error after glBindVertexArray: %d\n", err);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleVBOs[dst]);
    err = glGetError();
    if (err != GL_NO_ERROR) printf("OpenGL error after glBindBufferBase: %d\n", err);
    glEnable(GL_RASTERIZER_DISCARD);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, numParticles);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
    glBindVertexArray(0);
#endif
    glUseProgram(0);
    err = glGetError();
    if (err != GL_NO_ERROR) printf("OpenGL error after transform feedback: %d\n", err);
    curSrc = dst;
}

void particleSystemRender() {
    glUseProgram(renderShader);
#ifdef __APPLE__
    glEnable(GL_POINT_SPRITE);
#endif
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef __APPLE__
    glBindVertexArrayAPPLE(particleVAOs[curSrc]);
#else
    glBindVertexArray(particleVAOs[curSrc]);
#endif
    glPointSize(30.0f);
    glDrawArrays(GL_POINTS, 0, numParticles);
#ifdef __APPLE__
    glBindVertexArrayAPPLE(0);
    glDisable(GL_POINT_SPRITE);
#else
    glBindVertexArray(0);
#endif
    glDisable(GL_BLEND);
    glUseProgram(0);
}

void particleSystemCleanup() {
#ifdef __APPLE__
    glDeleteVertexArraysAPPLE(2, particleVAOs);
#else
    glDeleteVertexArrays(2, particleVAOs);
#endif
    glDeleteBuffers(2, particleVBOs);
}

void particleSystemSetEnabled(int enabled) {
}

void particleSystemUploadHeightmap(float* elevationData) {
    if (!heightmapTex) {
        glGenTextures(1, &heightmapTex);
    }
    glBindTexture(GL_TEXTURE_2D, heightmapTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 128, 128, 0, GL_RED, GL_FLOAT, elevationData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
} 