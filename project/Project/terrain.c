#include "terrain.h"
#include "CSCIx229.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "shaders.h"

Terrain* createTerrain() {
    Terrain* terrain = (Terrain*)malloc(sizeof(Terrain));
    if (!terrain) return NULL;

    int gridSize = TERRAIN_SIZE - 1;
    terrain->vertexCount = TERRAIN_SIZE * TERRAIN_SIZE;
    terrain->indexCount = gridSize * gridSize * 6;

    terrain->heightData = (float*)malloc(terrain->vertexCount * sizeof(float));
    terrain->vertices = (float*)malloc(terrain->vertexCount * 3 * sizeof(float));
    terrain->normals = (float*)malloc(terrain->vertexCount * 3 * sizeof(float));
    terrain->texCoords = (float*)malloc(terrain->vertexCount * 2 * sizeof(float));
    terrain->indices = (unsigned int*)malloc(terrain->indexCount * sizeof(unsigned int));

    if (!terrain->heightData || !terrain->vertices || !terrain->normals || 
        !terrain->texCoords || !terrain->indices) {
        freeTerrain(terrain);
        return NULL;
    }

    generateHeightMap(terrain);
    
    for (int z = 0; z < TERRAIN_SIZE; z++) {
        for (int x = 0; x < TERRAIN_SIZE; x++) {
            int index = z * TERRAIN_SIZE + x;
            
            terrain->vertices[index*3 + 0] = ((float)x/TERRAIN_SIZE - 0.5f) * TERRAIN_SCALE;
            terrain->vertices[index*3 + 1] = terrain->heightData[index];
            terrain->vertices[index*3 + 2] = ((float)z/TERRAIN_SIZE - 0.5f) * TERRAIN_SCALE;

            terrain->texCoords[index*2 + 0] = (float)x/TERRAIN_SIZE;
            terrain->texCoords[index*2 + 1] = (float)z/TERRAIN_SIZE;
        }
    }

    int index = 0;
    for (int z = 0; z < gridSize; z++) {
        for (int x = 0; x < gridSize; x++) {
            int topLeft = z * TERRAIN_SIZE + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * TERRAIN_SIZE + x;
            int bottomRight = bottomLeft + 1;

            terrain->indices[index++] = topLeft;
            terrain->indices[index++] = bottomLeft;
            terrain->indices[index++] = topRight;

            terrain->indices[index++] = topRight;
            terrain->indices[index++] = bottomLeft;
            terrain->indices[index++] = bottomRight;
        }
    }

    calculateNormals(terrain);

    return terrain;
}

float noise2D(int x, int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float smoothNoise2D(float x, float y) {
    float corners = (noise2D(x-1, y-1) + noise2D(x+1, y-1) + noise2D(x-1, y+1) + noise2D(x+1, y+1)) / 16.0f;
    float sides = (noise2D(x-1, y) + noise2D(x+1, y) + noise2D(x, y-1) + noise2D(x, y+1)) / 8.0f;
    float center = noise2D(x, y) / 4.0f;
    return corners + sides + center;
}

// Interpolation functions adapted from Ken Perlin's improved noise
// Source: http://mrl.nyu.edu/~perlin/noise/
float interpolate(float a, float b, float x) {
    float ft = x * 3.1415927f;
    float f = (1.0f - cosf(ft)) * 0.5f;
    return a*(1.0f-f) + b*f;
}

float interpolatedNoise2D(float x, float y) {
    int intX = (int)x;
    float fracX = x - intX;
    
    int intY = (int)y;
    float fracY = y - intY;
    
    float v1 = smoothNoise2D(intX, intY);
    float v2 = smoothNoise2D(intX + 1, intY);
    float v3 = smoothNoise2D(intX, intY + 1);
    float v4 = smoothNoise2D(intX + 1, intY + 1);
    
    float i1 = interpolate(v1, v2, fracX);
    float i2 = interpolate(v3, v4, fracX);
    
    return interpolate(i1, i2, fracY);
}

float min_f(float a, float b) {
    return a < b ? a : b;
}

void generateHeightMap(Terrain* terrain) {
    float persistence = 0.5f;
    int octaves = 6;
    
    for (int z = 0; z < TERRAIN_SIZE; z++) {
        for (int x = 0; x < TERRAIN_SIZE; x++) {
            float total = 0;
            float frequency = 1.0f;
            float amplitude = 1.0f;
            float maxValue = 0;
            
            for(int i = 0; i < octaves; i++) {
                float xf = (float)x * frequency / TERRAIN_SIZE * 8.0f;
                float zf = (float)z * frequency / TERRAIN_SIZE * 8.0f;
                
                total += interpolatedNoise2D(xf, zf) * amplitude;
                maxValue += amplitude;
                amplitude *= persistence;
                frequency *= 2.0f;
            }
            
            total = total / maxValue;
            
            float xPos = ((float)x / TERRAIN_SIZE - 0.5f) * 2.0f;
            
            if(xPos > 0) {
                float heightMultiplier = 1.0f + xPos * 1.5f;
                total *= heightMultiplier;
            } else {
                total *= 0.5f;
            }
            
            terrain->heightData[z * TERRAIN_SIZE + x] = total * TERRAIN_HEIGHT;
        }
    }
}

// Normal calculation adapted from Real-Time Rendering 4th Edition
// Chapter 7: Advanced Shading
void calculateNormals(Terrain* terrain) {
    memset(terrain->normals, 0, terrain->vertexCount * 3 * sizeof(float));

    for (int i = 0; i < terrain->indexCount; i += 3) {
        unsigned int i1 = terrain->indices[i];
        unsigned int i2 = terrain->indices[i+1];
        unsigned int i3 = terrain->indices[i+2];

        float* v1 = &terrain->vertices[i1*3];
        float* v2 = &terrain->vertices[i2*3];
        float* v3 = &terrain->vertices[i3*3];

        float ux = v2[0] - v1[0];
        float uy = v2[1] - v1[1];
        float uz = v2[2] - v1[2];

        float vx = v3[0] - v1[0];
        float vy = v3[1] - v1[1];
        float vz = v3[2] - v1[2];

        float nx = uy*vz - uz*vy;
        float ny = uz*vx - ux*vz;
        float nz = ux*vy - uy*vx;

        terrain->normals[i1*3 + 0] += nx;
        terrain->normals[i1*3 + 1] += ny;
        terrain->normals[i1*3 + 2] += nz;

        terrain->normals[i2*3 + 0] += nx;
        terrain->normals[i2*3 + 1] += ny;
        terrain->normals[i2*3 + 2] += nz;

        terrain->normals[i3*3 + 0] += nx;
        terrain->normals[i3*3 + 1] += ny;
        terrain->normals[i3*3 + 2] += nz;
    }

    for (int i = 0; i < terrain->vertexCount; i++) {
        float* n = &terrain->normals[i*3];
        float len = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
        if (len > 0) {
            n[0] /= len;
            n[1] /= len;
            n[2] /= len;
        }
    }
}

void freeTerrain(Terrain* terrain) {
    if (terrain) {
        if (terrain->heightData) free(terrain->heightData);
        if (terrain->vertices) free(terrain->vertices);
        if (terrain->normals) free(terrain->normals);
        if (terrain->texCoords) free(terrain->texCoords);
        if (terrain->indices) free(terrain->indices);
        free(terrain);
    }
}

float mix(float a, float b, float t) {
    return a * (1.0f - t) + b * t;
}

float getSnowBlendFactor(float height, float slope) {
    float snowStartHeight = 12.0f;
    float snowEndHeight = 25.0f;
    float maxSnowSlope = 0.65f;
    
    float noise = sin(height * 0.3f + slope * 2.0f) * 0.15f;
    
    float heightFactor = (height - snowStartHeight) / (snowEndHeight - snowStartHeight);
    heightFactor = heightFactor < 0 ? 0 : (heightFactor > 1 ? 1 : heightFactor);
    
    float slopeFactor = (maxSnowSlope - slope) / maxSnowSlope + noise;
    slopeFactor = slopeFactor < 0 ? 0 : (slopeFactor > 1 ? 1 : slopeFactor);
    
    return heightFactor * slopeFactor;
}

void renderTerrain(Terrain* terrain) {
    if (!terrain) return;
    
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < terrain->indexCount; i++) {
        int index = terrain->indices[i];
        float x = terrain->vertices[index * 3 + 0];
        float height = terrain->vertices[index * 3 + 1];
        float z = terrain->vertices[index * 3 + 2];
        float normal_y = terrain->normals[index * 3 + 1];
        
        float grassColor[] = {0.3f, 0.65f, 0.2f};
        float lightRock[] = {0.7f, 0.68f, 0.62f};
        float darkRock[] = {0.55f, 0.52f, 0.47f};
        float sandColor[] = {0.72f, 0.68f, 0.48f};
        float snowColor[] = {0.95f, 0.95f, 0.95f};
        float skyColor[] = {0.7f, 0.8f, 1.0f};
        
        float slope = 1.0f - normal_y;
        float terrainDiv = x / (TERRAIN_SCALE/2);
        float transition = (terrainDiv + 1.0f) * 0.5f;
        
        float beachZone = 2.0f;
        float heightAboveWater = height - waterLevel;
        
        float baseColor[3];
        if(transition < 0.5f) {
            baseColor[0] = grassColor[0] * 0.8f;
            baseColor[1] = grassColor[1] * 0.8f;
            baseColor[2] = grassColor[2] * 0.8f;
        } else {
            float heightFactor = (height + 10.0f) / 20.0f;
            heightFactor = heightFactor < 0 ? 0 : (heightFactor > 1 ? 1 : heightFactor);
            float rockFactor = (slope * 0.6f + heightFactor * 0.4f);
            for(int c = 0; c < 3; c++) {
                baseColor[c] = lightRock[c] * (1-rockFactor) + darkRock[c] * rockFactor * 0.8f;
            }
        }

        float finalColor[3];
        if(heightAboveWater < beachZone && heightAboveWater > -1.0f) {
            float beachFactor = heightAboveWater / beachZone;
            beachFactor = beachFactor < 0 ? 0 : (beachFactor > 1 ? 1 : beachFactor);
            for(int c = 0; c < 3; c++) {
                finalColor[c] = sandColor[c] * (1-beachFactor) + baseColor[c] * beachFactor;
            }
        } else {
            for(int c = 0; c < 3; c++) {
                finalColor[c] = baseColor[c];
            }
        }

        if(height > 12.0f) {
            float snowFactor = (height - 12.0f) / 15.0f;
            snowFactor = snowFactor > 1 ? 1 : snowFactor;
            snowFactor *= (1.0f - slope * 1.2f);
            
            for(int c = 0; c < 3; c++) {
                finalColor[c] = finalColor[c] * (1-snowFactor) + snowColor[c] * snowFactor;
            }
        }

        float dist = sqrt(x*x + height*height + z*z);
        
        float fogStart = TERRAIN_SCALE * 0.3f;
        float fogEnd = TERRAIN_SCALE * 0.7f;
        float fogFactor = (fogEnd - dist) / (fogEnd - fogStart);
        fogFactor = fogFactor < 0 ? 0 : (fogFactor > 1 ? 1 : fogFactor);
        
        for(int c = 0; c < 3; c++) {
            finalColor[c] = finalColor[c] * fogFactor + skyColor[c] * (1-fogFactor);
        }
        
        glColor3fv(finalColor);
        glNormal3fv(&terrain->normals[index * 3]);
        glVertex3fv(&terrain->vertices[index * 3]);
    }
    glEnd();
}

float smooth_step(float edge0, float edge1, float x) {
    float t = (x - edge0) / (edge1 - edge0);
    t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
    return t * t * (3.0f - 2.0f * t);
}

void renderWater(float waterLevel, Terrain* terrain, float dayTime) {
    float waterSize = TERRAIN_SCALE * 4.0f;
    int segments = 64;
    float segSize = waterSize / segments;
    float specColor[] = {1.0f, 1.0f, 1.0f, 0.3f};
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float t = dayTime / 24.0f;
    
    const int NUM_COLORS = 6;
    float timePoints[6] = {0.0f, 0.25f, 0.4f, 0.6f, 0.75f, 1.0f};
    float colors[6][4] = {
        {0.02f, 0.02f, 0.1f, 0.7f},
        {0.3f, 0.2f, 0.3f, 0.7f},
        {0.2f, 0.3f, 0.5f, 0.7f},
        {0.2f, 0.3f, 0.5f, 0.7f},
        {0.3f, 0.2f, 0.3f, 0.7f},
        {0.02f, 0.02f, 0.1f, 0.7f}
    };
    
    int i;
    for(i = 0; i < NUM_COLORS-1; i++) {
        if(t >= timePoints[i] && t <= timePoints[i+1]) break;
    }
    
    float segmentPos = (t - timePoints[i]) / (timePoints[i+1] - timePoints[i]);
    float blend = smooth_step(0.0f, 1.0f, segmentPos);
    
    float waterColor[4];
    for(int j = 0; j < 4; j++) {
        waterColor[j] = colors[i][j] * (1.0f - blend) + colors[i+1][j] * blend;
    }
    
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    
    glPushMatrix();
    glTranslatef(0, waterLevel, 0);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);
    
    glBegin(GL_QUADS);
    for(int i = -segments/2; i < segments/2; i++) {
        for(int j = -segments/2; j < segments/2; j++) {
            float x1 = i * segSize;
            float x2 = x1 + segSize;
            float z1 = j * segSize;
            float z2 = z1 + segSize;
            
            float waveFreq = 0.02f;
            float waveAmp = 0.05f;
            float y1 = sin(x1*waveFreq + z1*waveFreq + currentTime) * waveAmp;
            float y2 = sin(x2*waveFreq + z1*waveFreq + currentTime) * waveAmp;
            float y3 = sin(x2*waveFreq + z2*waveFreq + currentTime) * waveAmp;
            float y4 = sin(x1*waveFreq + z2*waveFreq + currentTime) * waveAmp;
            
            float colorVar = (y1 + 0.05f) * 0.05f;
            float finalColor[4];
            for(int c = 0; c < 4; c++) {
                finalColor[c] = waterColor[c] + (c < 3 ? colorVar : 0);
            }
            
            glColor4fv(finalColor);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z1);
            glVertex3f(x2, y3, z2);
            glVertex3f(x1, y4, z2);
        }
    }
    glEnd();
    
    glPopMatrix();
    glDisable(GL_BLEND);
}

float getTerrainHeight(Terrain* terrain, float x, float z) {
    float nx = (x / TERRAIN_SCALE + 0.5f) * (TERRAIN_SIZE - 1);
    float nz = (z / TERRAIN_SCALE + 0.5f) * (TERRAIN_SIZE - 1);
    
    int x0 = (int)nx;
    int z0 = (int)nz;
    
    if (x0 < 0) x0 = 0;
    if (z0 < 0) z0 = 0;
    if (x0 >= TERRAIN_SIZE-1) x0 = TERRAIN_SIZE-2;
    if (z0 >= TERRAIN_SIZE-1) z0 = TERRAIN_SIZE-2;
    
    float fx = nx - x0;
    float fz = nz - z0;
    
    float h00 = terrain->heightData[z0 * TERRAIN_SIZE + x0];
    float h10 = terrain->heightData[z0 * TERRAIN_SIZE + (x0+1)];
    float h01 = terrain->heightData[(z0+1) * TERRAIN_SIZE + x0];
    float h11 = terrain->heightData[(z0+1) * TERRAIN_SIZE + (x0+1)];
    
    float h0 = h00 * (1-fx) + h10 * fx;
    float h1 = h01 * (1-fx) + h11 * fx;
    return h0 * (1-fz) + h1 * fz;
}