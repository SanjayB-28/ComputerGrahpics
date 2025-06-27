#include "fractal_tree.h"
#include "shaders.h"
#include "landscape.h"
#include "objects_render.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdlib.h>

static int branchShader = 0;
static int leafShader = 0;
extern GLuint barkTexture;
extern GLuint leafTexture;

// Deterministic pseudo-random function for stable branch angles, now with per-tree seed
static float branchRandom(int depth, int branch, unsigned int treeSeed)
{
    unsigned int seed = (depth * 73856093u) ^ (branch * 19349663u) ^ treeSeed;
    seed = (seed ^ (seed >> 13)) * 1274126177u;
    return ((seed & 0xFFFF) / 65535.0f) - 0.5f; // [-0.5, 0.5]
}

// Helper to draw a solid cylinder (branch) along the y-axis using immediate mode
static void drawCylinderY(double length, double baseRadius, double topRadius)
{
    const int segments = 8;
    double angleStep = 2.0 * M_PI / segments;
    if (barkTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, barkTexture);
        GLint texLoc = glGetUniformLocation(branchShader, "barkTex");
        glUniform1i(texLoc, 0);
        glEnable(GL_TEXTURE_2D);
    }
    // Draw side
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        double angle = i * angleStep;
        double x = cos(angle);
        double z = sin(angle);
        // Normal for lighting
        glNormal3d(x, 0, z);
        // Texture coordinates: s = angle/2pi, t = 0 (bottom) or 1 (top)
        glTexCoord2f(i/(double)segments, 0.0);
        glVertex3d(baseRadius * x, 0, baseRadius * z);
        glTexCoord2f(i/(double)segments, 1.0);
        glVertex3d(topRadius * x, length, topRadius * z);
    }
    glEnd();
    if (barkTexture) glDisable(GL_TEXTURE_2D);
    // Draw bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(0, -1, 0);
    glVertex3d(0, 0, 0);
    for (int i = 0; i <= segments; ++i) {
        double angle = i * angleStep;
        double x = cos(angle);
        double z = sin(angle);
        glVertex3d(baseRadius * x, 0, baseRadius * z);
    }
    glEnd();
    // Draw top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(0, 1, 0);
    glVertex3d(0, length, 0);
    for (int i = 0; i <= segments; ++i) {
        double angle = -i * angleStep; // Reverse winding for top
        double x = cos(angle);
        double z = sin(angle);
        glVertex3d(topRadius * x, length, topRadius * z);
    }
    glEnd();
}

// Draw a volumetric leaf cluster using layered rings (triangle strips)
static void drawLeafCluster(float height, float baseRadius, int layers, int segments, unsigned int seed, int leafColorIndex) {
    float layerSpacing = height / layers;
    float startHeight = 0.0f;
    srand(seed); // For reproducibility per cluster

    if (leafTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, leafTexture);
        GLint texLoc = glGetUniformLocation(leafShader, "leafTex");
        glUniform1i(texLoc, 0);
        glEnable(GL_TEXTURE_2D);
        GLint colorIndexLoc = glGetUniformLocation(leafShader, "leafColorIndex");
        glUniform1i(colorIndexLoc, leafColorIndex);
    }

    for (int layer = 0; layer < layers; layer++) {
        float heightPercent = (float)layer / layers;
        float radius = baseRadius * (1.0f - powf(heightPercent - 0.3f, 2.0f)) * 1.8f;
        float y = startHeight + layer * layerSpacing;

        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= segments; i++) {
            float angle = (float)i / segments * 2.0f * M_PI;
            float radiusVar = 1.0f + ((float)rand() / RAND_MAX) * 0.2f - 0.1f;
            float x = cosf(angle) * radius * radiusVar;
            float z = sinf(angle) * radius * radiusVar;

            float nx = cosf(angle);
            float ny = 0.7f;
            float nz = sinf(angle);
            float nlen = sqrtf(nx*nx + ny*ny + nz*nz);

            float shade = 0.8f + ((float)rand() / RAND_MAX) * 0.2f;
            glColor3f(0.45f * shade, 0.75f * shade, 0.25f * shade);
            glNormal3f(nx/nlen, ny/nlen, nz/nlen);
            glTexCoord2f(i/(float)segments, 0.0f);
            glVertex3f(x, y, z);

            // Upper ring
            float upperRadius = radius * (0.95f - heightPercent * 0.1f);
            float x2 = cosf(angle) * upperRadius * radiusVar;
            float z2 = sinf(angle) * upperRadius * radiusVar;
            float y2 = y + layerSpacing;

            glColor3f(0.9f * shade, 0.7f * shade, 0.3f * shade);
            glNormal3f(nx/nlen, ny/nlen, nz/nlen);
            glTexCoord2f(i/(float)segments, 1.0f);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    if (leafTexture) glDisable(GL_TEXTURE_2D);
}

// Recursive function to draw the tree, now with per-tree seed
static void drawFractalTree(int depth, double length, double baseRadius, double topRadius, int drawLeaves, unsigned int treeSeed, int leafColorIndex)
{
    if (depth == 0)
    {
        if (drawLeaves) {
            // Draw a volumetric leaf cluster at the end node
            drawLeafCluster(length * 0.8f, 0.5f, 5, 8, treeSeed, leafColorIndex);
        }
        return;
    }
    glColor3f(0.55, 0.27, 0.07); // Brown
    drawCylinderY(length, baseRadius, topRadius);
    glPushMatrix();
    glTranslated(0, length, 0); // Move up along y for next branches
    // Randomize azimuthal angles for the two branches
    int numBranches = 2;
    double baseAzimuth = branchRandom(depth, 0, treeSeed) * 360.0; // [-180, 180]
    double offset = 90.0 + fabs(branchRandom(depth, 1, treeSeed)) * 90.0; // [90, 180]
    double azimuths[2] = {baseAzimuth, baseAzimuth + offset};
    double elevations[2] = {30.0, -30.0}; // Still allow up/down split
    for (int i = 0; i < numBranches; ++i)
    {
        glPushMatrix();
        glRotated(azimuths[i], 0, 1, 0); // Y axis (azimuth)
        glRotated(elevations[i], 1, 0, 0); // X axis (elevation)
        drawFractalTree(depth-1, length*0.7, baseRadius*0.7, topRadius*0.7, drawLeaves, treeSeed, leafColorIndex);
        glPopMatrix();
    }
    glPopMatrix();
}

void fractalTreeInit() {
    branchShader = loadShader("shaders/tree_branch.vert", "shaders/tree_branch.frag");
    leafShader = loadShader("shaders/tree_leaf.vert", "shaders/tree_leaf.frag");
}
void fractalTreeCleanup() {
    deleteShader(branchShader);
    deleteShader(leafShader);
}

// Now takes a treeSeed for unique branching, and comments out leaves for performance
void fractalTreeDraw(double x, double y, double z, double scale, int depth, unsigned int treeSeed, int leafColorIndex)
{
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(scale, scale, scale);
    // Set up lighting uniforms for the branch shader
    useShader(branchShader);
    // Get OpenGL light position and color (GL_LIGHT0)
    float lightPos[4];
    float diffuse[4];
    glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glGetLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    float len = sqrtf(lightPos[0]*lightPos[0] + lightPos[1]*lightPos[1] + lightPos[2]*lightPos[2]);
    float lightDir[3] = {lightPos[0]/len, lightPos[1]/len, lightPos[2]/len};
    GLint lightDirLoc = glGetUniformLocation(branchShader, "lightDir");
    GLint lightColorLoc = glGetUniformLocation(branchShader, "lightColor");
    glUniform3fv(lightDirLoc, 1, lightDir);
    glUniform3fv(lightColorLoc, 1, diffuse);
    GLint lightPosLoc = glGetUniformLocation(branchShader, "lightPos");
    glUniform3fv(lightPosLoc, 1, lightPos);
    // Draw all branches with branch shader
    drawFractalTree(depth , 1.0, 0.12, 0.08, 0, treeSeed, leafColorIndex);
    useShader(0);
    useShader(leafShader);
    // Set up lighting uniforms for the leaf shader
    GLint leafLightDirLoc = glGetUniformLocation(leafShader, "lightDir");
    GLint leafLightColorLoc = glGetUniformLocation(leafShader, "lightColor");
    glUniform3fv(leafLightDirLoc, 1, lightDir);
    glUniform3fv(leafLightColorLoc, 1, diffuse);
    GLint leafLightPosLoc = glGetUniformLocation(leafShader, "lightPos");
    glUniform3fv(leafLightPosLoc, 1, lightPos);
    drawFractalTree(depth, 1.0, 0.12, 0.08, 1, treeSeed, leafColorIndex);
    useShader(0);
    glPopMatrix();
} 