#ifndef __APPLE__
#include <GL/glew.h>
#endif
#include "boulder.h"
#include "objects_render.h"
#include "landscape.h"
#include "shaders.h"
#include "CSCIx229.h"

#define NUM_BOULDERS 40

static BoulderInstance* boulders = NULL;
static int numBoulders = 0;
static int boulderShader = 0;

extern TreeInstance* treeInstances;
extern int numTrees;
extern GLuint boulderTexture;
extern float waterLevel;

static float randf() { return rand() / (float)RAND_MAX; }

void freeBoulders() {
    if (boulders) {
        free(boulders);
        boulders = NULL;
        numBoulders = 0;
    }
}

void initBoulders(Landscape* landscape) {
    freeBoulders();
    if (!landscape) return;
    boulders = (BoulderInstance*)malloc(sizeof(BoulderInstance) * NUM_BOULDERS);
    numBoulders = 0;
    float halfScale = LANDSCAPE_SCALE * 0.5f * 0.95f;
    int attempts = 0;
    while (numBoulders < NUM_BOULDERS && attempts < NUM_BOULDERS * 10) {
        attempts++;
        float x = -halfScale + randf() * LANDSCAPE_SCALE * 0.95f;
        float z = -halfScale + randf() * LANDSCAPE_SCALE * 0.95f;
        float y = landscapeGetHeight(landscape, x, z);
        float slope = 0.0f;
        float nx = (x / LANDSCAPE_SCALE + 0.5f) * (LANDSCAPE_SIZE - 1);
        float nz = (z / LANDSCAPE_SCALE + 0.5f) * (LANDSCAPE_SIZE - 1);
        int ix = (int)nx;
        int iz = (int)nz;
        if (ix < 0) ix = 0;
        if (iz < 0) iz = 0;
        if (ix >= LANDSCAPE_SIZE-1) ix = LANDSCAPE_SIZE-2;
        if (iz >= LANDSCAPE_SIZE-1) iz = LANDSCAPE_SIZE-2;
        int idx = iz * LANDSCAPE_SIZE + ix;
        float* n = &landscape->normals[idx*3];
        slope = acosf(fminf(fmaxf(n[1], -1.0f), 1.0f)) / (float)M_PI;
        if (slope > 0.25f) continue;
        if (y < waterLevel + 0.5f) continue;
        int collides = 0;
        for (int t = 0; t < numTrees; ++t) {
            float dx = x - treeInstances[t].x;
            float dz = z - treeInstances[t].z;
            float dist2 = dx*dx + dz*dz;
            float minDist = 4.0f + treeInstances[t].scale * 0.5f;
            if (dist2 < minDist * minDist) {
                collides = 1;
                break;
            }
        }
        if (collides) continue;
        float scale = 1.2f + randf() * 2.8f + randf() * randf() * 1.2f;
        float rotation = randf() * 360.0f;
        unsigned int shapeSeed = rand();
        int colorIndex = rand() % 8;
        boulders[numBoulders++] = (BoulderInstance){x, y, z, scale, rotation, shapeSeed, colorIndex};
    }
}

void boulderDraw(float x, float y, float z, float scale, float rotation, unsigned int shapeSeed, int colorIndex) {
    float baseVerts[28][3] = {
        {0.0f, 1.0f, 0.0f}, {0.8f, 0.6f, 0.2f}, {0.5f, 0.5f, -0.9f}, {-0.7f, 0.7f, -0.6f},
        {-1.0f, 0.5f, 0.4f}, {0.0f, -0.1f, 1.1f}, {1.1f, -0.2f, -0.3f}, {0.4f, -0.8f, -1.0f},
        {-0.8f, -0.6f, -0.8f}, {-1.0f, -0.7f, 0.6f}, {0.6f, 0.2f, 0.8f}, {-0.5f, 0.1f, 1.0f},
        {1.0f, 0.1f, 0.5f}, {1.2f, -0.5f, 0.2f}, {0.7f, -0.7f, 0.7f}, {-0.2f, -1.0f, 0.2f},
        {-0.9f, -0.9f, -0.2f}, {-0.3f, -0.8f, 0.9f}, {0.3f, 0.7f, 0.7f}, {0.9f, -0.3f, 0.9f},
        {-0.6f, 0.3f, 1.0f}, {1.1f, 0.3f, -0.7f}, {-1.1f, 0.2f, -0.5f}, {0.2f, -0.9f, -0.7f},
        {-0.7f, -0.8f, 0.3f}, {0.8f, -0.7f, -0.6f}, {-0.3f, 0.9f, 0.2f}, {0.5f, -0.5f, 1.0f}
    };
    int faces[52][3] = {
        {0,1,2},{0,2,3},{0,3,4},{0,4,1},{1,10,12},{1,12,2},{2,12,7},{2,7,3},{3,7,8},{3,8,4},{4,8,9},{4,9,1},
        {1,9,11},{1,11,10},{5,10,11},{5,11,9},{5,9,8},{5,8,7},{5,7,13},{5,13,14},{5,14,10},{10,14,12},{12,14,13},{12,13,7},
        {6,12,13},{6,13,7},{6,7,2},{6,2,12},{6,12,10},{6,10,15},{6,15,16},{6,16,7},{17,18,19},{17,19,20},{17,20,21},{17,21,18},
        {18,22,23},{18,23,19},{19,23,24},{19,24,20},{20,24,25},{20,25,21},{21,25,26},{21,26,18},{18,26,22},{22,26,25},{22,25,23},{23,25,24}
    };
    float verts[28][3];
    for (int i = 0; i < 28; ++i) {
        for (int j = 0; j < 3; ++j) {
            float noise = (sinf(shapeSeed * 0.13f + i * 1.7f + j * 2.3f) + cosf(shapeSeed * 0.21f + i * 2.1f + j * 1.3f)) * 0.18f * randf();
            verts[i][j] = baseVerts[i][j] + noise;
        }
    }
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotation, 0, 1, 0);
    glScalef(scale, scale, scale);
    if (boulderShader) {
        useShader(boulderShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, boulderTexture);
        GLint texLoc = glGetUniformLocation(boulderShader, "boulderTex");
        glUniform1i(texLoc, 0);
        glEnable(GL_TEXTURE_2D);
        float lightPos[4], diffuse[4];
        glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glGetLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        GLint lightColorLoc = glGetUniformLocation(boulderShader, "lightColor");
        glUniform3fv(lightColorLoc, 1, diffuse);
        GLint colorIndexLoc = glGetUniformLocation(boulderShader, "boulderColorIndex");
        glUniform1i(colorIndexLoc, colorIndex);
        GLint lightPosLoc = glGetUniformLocation(boulderShader, "lightPos");
        glUniform3fv(lightPosLoc, 1, lightPos);
    }
    glBegin(GL_TRIANGLES);
    for (int f = 0; f < 52; ++f) {
        float* v0 = verts[faces[f][0]];
        float* v1 = verts[faces[f][1]];
        float* v2 = verts[faces[f][2]];
        float ux = v1[0] - v0[0], uy = v1[1] - v0[1], uz = v1[2] - v0[2];
        float vx = v2[0] - v0[0], vy = v2[1] - v0[1], vz = v2[2] - v0[2];
        float nx = uy * vz - uz * vy;
        float ny = uz * vx - ux * vz;
        float nz = ux * vy - uy * vx;
        float len = sqrtf(nx*nx + ny*ny + nz*nz);
        if (len > 0.0001f) { nx /= len; ny /= len; nz /= len; }
        glNormal3f(nx, ny, nz);
        glTexCoord2f(v0[0]*0.5f+0.5f, v0[2]*0.5f+0.5f);
        glVertex3fv(v0);
        glTexCoord2f(v1[0]*0.5f+0.5f, v1[2]*0.5f+0.5f);
        glVertex3fv(v1);
        glTexCoord2f(v2[0]*0.5f+0.5f, v2[2]*0.5f+0.5f);
        glVertex3fv(v2);
    }
    glEnd();
    if (boulderShader) useShader(0);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void renderBoulders() {
    for (int i = 0; i < numBoulders; ++i) {
        BoulderInstance* b = &boulders[i];
        boulderDraw(b->x, b->y, b->z, b->scale, b->rotation, b->shapeSeed, b->colorIndex);
    }
}

void boulderShaderInit() {
    boulderShader = loadShader("shaders/boulder_shader.vert", "shaders/boulder_shader.frag");
} 