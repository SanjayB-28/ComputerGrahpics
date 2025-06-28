#include "landscape.h"
#include "CSCIx229.h"
#include "shaders.h"

#define NOISE_OFFSET_X 37.0f
#define NOISE_OFFSET_Z 91.0f

Landscape* landscapeCreate() {
    Landscape* landscape = (Landscape*)malloc(sizeof(Landscape));
    if (!landscape) return NULL;
    int gridSize = LANDSCAPE_SIZE - 1;
    landscape->vertexCount = LANDSCAPE_SIZE * LANDSCAPE_SIZE;
    landscape->indexCount = gridSize * gridSize * 6;
    landscape->elevationData = (float*)malloc(landscape->vertexCount * sizeof(float));
    landscape->vertices = (float*)malloc(landscape->vertexCount * 3 * sizeof(float));
    landscape->normals = (float*)malloc(landscape->vertexCount * 3 * sizeof(float));
    landscape->texCoords = (float*)malloc(landscape->vertexCount * 2 * sizeof(float));
    landscape->indices = (unsigned int*)malloc(landscape->indexCount * sizeof(unsigned int));
    if (!landscape->elevationData || !landscape->vertices || !landscape->normals || 
        !landscape->texCoords || !landscape->indices) {
        landscapeDestroy(landscape);
        return NULL;
    }
    landscapeGenerateHeightMap(landscape);
    for (int z = 0; z < LANDSCAPE_SIZE; z++) {
        for (int x = 0; x < LANDSCAPE_SIZE; x++) {
            int index = z * LANDSCAPE_SIZE + x;
            landscape->vertices[index*3 + 0] = ((float)x/LANDSCAPE_SIZE - 0.5f) * LANDSCAPE_SCALE;
            landscape->vertices[index*3 + 1] = landscape->elevationData[index];
            landscape->vertices[index*3 + 2] = ((float)z/LANDSCAPE_SIZE - 0.5f) * LANDSCAPE_SCALE;
            landscape->texCoords[index*2 + 0] = (float)x/LANDSCAPE_SIZE;
            landscape->texCoords[index*2 + 1] = (float)z/LANDSCAPE_SIZE;
        }
    }
    int index = 0;
    for (int z = 0; z < gridSize; z++) {
        for (int x = 0; x < gridSize; x++) {
            int topLeft = z * LANDSCAPE_SIZE + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * LANDSCAPE_SIZE + x;
            int bottomRight = bottomLeft + 1;
            landscape->indices[index++] = topLeft;
            landscape->indices[index++] = bottomLeft;
            landscape->indices[index++] = topRight;
            landscape->indices[index++] = topRight;
            landscape->indices[index++] = bottomLeft;
            landscape->indices[index++] = bottomRight;
        }
    }
    landscapeCalculateNormals(landscape);
    return landscape;
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

void landscapeGenerateHeightMap(Landscape* landscape) {
    float persistence = 0.5f;
    int octaves = 5;
    
    for (int z = 0; z < LANDSCAPE_SIZE; z++) {
        for (int x = 0; x < LANDSCAPE_SIZE; x++) {
            float total = 0;
            float frequency = 1.0f;
            float amplitude = 1.0f;
            float maxValue = 0;
            
            for(int i = 0; i < octaves; i++) {
                float xf = ((float)x + NOISE_OFFSET_X) * frequency / LANDSCAPE_SIZE * 8.0f;
                float zf = ((float)z + NOISE_OFFSET_Z) * frequency / LANDSCAPE_SIZE * 8.0f;
                
                total += interpolatedNoise2D(xf, zf) * amplitude;
                maxValue += amplitude;
                amplitude *= persistence;
                frequency *= 2.0f;
            }
            
            total = total / maxValue;
            
            float xPos = ((float)x / LANDSCAPE_SIZE - 0.5f) * 2.0f;
            
            if(xPos > 0) {
                float heightMultiplier = 1.0f + xPos * 1.5f;
                total *= heightMultiplier;
            } else {
                total *= 0.5f;
            }
            
            landscape->elevationData[z * LANDSCAPE_SIZE + x] = total * LANDSCAPE_HEIGHT * 1.25f;
        }
    }
}

void landscapeCalculateNormals(Landscape* landscape) {
    memset(landscape->normals, 0, landscape->vertexCount * 3 * sizeof(float));

    for (int i = 0; i < landscape->indexCount; i += 3) {
        unsigned int i1 = landscape->indices[i];
        unsigned int i2 = landscape->indices[i+1];
        unsigned int i3 = landscape->indices[i+2];

        float* v1 = &landscape->vertices[i1*3];
        float* v2 = &landscape->vertices[i2*3];
        float* v3 = &landscape->vertices[i3*3];

        float ux = v2[0] - v1[0];
        float uy = v2[1] - v1[1];
        float uz = v2[2] - v1[2];

        float vx = v3[0] - v1[0];
        float vy = v3[1] - v1[1];
        float vz = v3[2] - v1[2];

        float nx = uy*vz - uz*vy;
        float ny = uz*vx - ux*vz;
        float nz = ux*vy - uy*vx;

        landscape->normals[i1*3 + 0] += nx;
        landscape->normals[i1*3 + 1] += ny;
        landscape->normals[i1*3 + 2] += nz;

        landscape->normals[i2*3 + 0] += nx;
        landscape->normals[i2*3 + 1] += ny;
        landscape->normals[i2*3 + 2] += nz;

        landscape->normals[i3*3 + 0] += nx;
        landscape->normals[i3*3 + 1] += ny;
        landscape->normals[i3*3 + 2] += nz;
    }

    for (int i = 0; i < landscape->vertexCount; i++) {
        float* n = &landscape->normals[i*3];
        float len = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
        if (len > 0) {
            n[0] /= len;
            n[1] /= len;
            n[2] /= len;
        }
    }
}

void landscapeDestroy(Landscape* landscape) {
    if (landscape) {
        if (landscape->elevationData) free(landscape->elevationData);
        if (landscape->vertices) free(landscape->vertices);
        if (landscape->normals) free(landscape->normals);
        if (landscape->texCoords) free(landscape->texCoords);
        if (landscape->indices) free(landscape->indices);
        free(landscape);
    }
}

float landscapeMix(float a, float b, float t) {
    return a * (1.0f - t) + b * t;
}

float landscapeGetSnowBlend(float height, float slope) {
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

void landscapeRender(Landscape* landscape, int weatherType) {
    if (!landscape) return;
    float noSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, noSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0f);
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < landscape->indexCount; i++) {
        int index = landscape->indices[i];
        float height = landscape->vertices[index * 3 + 1];
        float normal_y = landscape->normals[index * 3 + 1];
        
        float grassColor[] = {0.13f, 0.45f, 0.13f};
        float lightRock[] = {0.5f, 0.48f, 0.42f};
        float darkRock[] = {0.25f, 0.23f, 0.21f};
        float sandColor[] = {0.72f, 0.68f, 0.48f};
        float snowColor[] = {0.95f, 0.95f, 0.95f};
        
        float finalColor[3];

        if (weatherType == 1) {
            float slope = 1.0f - normal_y;
            float rockFactor = (slope - 0.2f) / 0.4f;
            rockFactor = fmax(0.0f, fmin(1.0f, rockFactor));
            for(int c = 0; c < 3; c++) {
                finalColor[c] = snowColor[c] * (1.0f - rockFactor) + darkRock[c] * rockFactor;
            }
        } else {
            float slope = 1.0f - normal_y;
            float heightAboveWater = height - waterLevel;
            float darkRockFactor = (slope - 0.3f) / 0.3f;
            darkRockFactor = fmax(0.0f, fmin(1.0f, darkRockFactor));
            float rockColor[3];
            for(int c=0; c<3; c++) {
                rockColor[c] = lightRock[c] * (1.0f - darkRockFactor) + darkRock[c] * darkRockFactor;
            }
            float grassFactor = (slope - 0.12f) / 0.25f;
            grassFactor = fmax(0.0f, fmin(1.0f, grassFactor));
            float baseColor[3];
            for(int c=0; c<3; c++) {
                baseColor[c] = grassColor[c] * (1.0f - grassFactor) + rockColor[c] * grassFactor;
            }
            float beachZone = 2.0f;
            if(heightAboveWater < beachZone && heightAboveWater > -1.0f) {
                float beachFactor = heightAboveWater / beachZone;
                beachFactor = fmax(0.0f, fmin(1.0f, beachFactor));
                for(int c = 0; c < 3; c++) {
                    finalColor[c] = sandColor[c] * (1-beachFactor) + baseColor[c] * beachFactor;
                }
            } else {
                for(int c = 0; c < 3; c++) {
                    finalColor[c] = baseColor[c];
                }
            }
        }
        
        glColor3fv(finalColor);
        glNormal3fv(&landscape->normals[index * 3]);
        glVertex3fv(&landscape->vertices[index * 3]);
    }
    glEnd();
}

float landscapeSmoothStep(float edge0, float edge1, float x) {
    float t = (x - edge0) / (edge1 - edge0);
    t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
    return t * t * (3.0f - 2.0f * t);
}

void landscapeRenderWater(float waterLevel, Landscape* landscape, float dayTime) {
    float waterSize = LANDSCAPE_SCALE * 1.0f;
    int segments = 64;
    float segSize = waterSize / segments;
    float specColor[] = {1.0f, 1.0f, 1.0f, 0.3f};
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float t = dayTime / 24.0f;
    
    const int NUM_COLORS = 6;
    float timePoints[6] = {0.0f, 0.25f, 0.4f, 0.6f, 0.75f, 1.0f};
    float colors[6][4] = {
        {0.02f, 0.02f, 0.1f, 0.9f},
        {0.3f, 0.2f, 0.3f, 0.9f},
        {0.2f, 0.3f, 0.5f, 0.9f},
        {0.2f, 0.3f, 0.5f, 0.9f},
        {0.3f, 0.2f, 0.3f, 0.9f},
        {0.02f, 0.02f, 0.1f, 0.9f}
    };
    
    int i;
    for(i = 0; i < NUM_COLORS-1; i++) {
        if(t >= timePoints[i] && t <= timePoints[i+1]) break;
    }
    
    float segmentPos = (t - timePoints[i]) / (timePoints[i+1] - timePoints[i]);
    float blend = landscapeSmoothStep(0.0f, 1.0f, segmentPos);
    
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

float landscapeGetHeight(Landscape* landscape, float x, float z) {
    float nx = (x / LANDSCAPE_SCALE + 0.5f) * (LANDSCAPE_SIZE - 1);
    float nz = (z / LANDSCAPE_SCALE + 0.5f) * (LANDSCAPE_SIZE - 1);
    
    int x0 = (int)nx;
    int z0 = (int)nz;
    
    if (x0 < 0) x0 = 0;
    if (z0 < 0) z0 = 0;
    if (x0 >= LANDSCAPE_SIZE-1) x0 = LANDSCAPE_SIZE-2;
    if (z0 >= LANDSCAPE_SIZE-1) z0 = LANDSCAPE_SIZE-2;
    
    float fx = nx - x0;
    float fz = nz - z0;
    
    float h00 = landscape->elevationData[z0 * LANDSCAPE_SIZE + x0];
    float h10 = landscape->elevationData[z0 * LANDSCAPE_SIZE + (x0+1)];
    float h01 = landscape->elevationData[(z0+1) * LANDSCAPE_SIZE + x0];
    float h11 = landscape->elevationData[(z0+1) * LANDSCAPE_SIZE + (x0+1)];
    
    float h0 = h00 * (1-fx) + h10 * fx;
    float h1 = h01 * (1-fx) + h11 * fx;
    return h0 * (1-fz) + h1 * fz;
}