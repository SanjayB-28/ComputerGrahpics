#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "CSCIx229.h"
#include "terrain.h"
#include "particles.h"

#define PARTICLE_SNOW 0
#define PARTICLE_RAIN 1
extern Terrain* terrain;

ParticleSystem* createParticleSystem(int maxParticles, int type) {
    ParticleSystem* ps = (ParticleSystem*)malloc(sizeof(ParticleSystem));
    if (!ps) return NULL;

    ps->particles = (Particle*)malloc(maxParticles * sizeof(Particle));
    if (!ps->particles) {
        free(ps);
        return NULL;
    }

    ps->maxParticles = maxParticles;
    ps->activeParticles = 0;
    ps->type = type;
    ps->emitterRadius = 100.0f;  

    
    for (int i = 0; i < maxParticles; i++) {
        ps->particles[i].active = 0;
    }

    return ps;
}

void updateParticles(ParticleSystem* ps, float deltaTime, float intensity, float time) {
    float baseWindX = sin(time * 0.5f) * 2.0f;
    
    
    for (int i = 0; i < ps->maxParticles; i++) {
        if (ps->particles[i].active) {
            if (!ps->particles[i].accumulated) {
                float heightFactor = ps->particles[i].y / 100.0f;
                float windX = baseWindX * (1.0f + heightFactor);
                
                ps->particles[i].vy -= (2.5f + ((float)rand()/RAND_MAX)) * deltaTime;
                ps->particles[i].vx += windX * deltaTime;
                
                ps->particles[i].x += ps->particles[i].vx * deltaTime;
                ps->particles[i].y += ps->particles[i].vy * deltaTime;
                ps->particles[i].z += ps->particles[i].vz * deltaTime;

                float groundHeight = getTerrainHeight(terrain, ps->particles[i].x, ps->particles[i].z);
                
                if (ps->particles[i].y < groundHeight) {
                    ps->particles[i].y = groundHeight;
                    ps->particles[i].accumulated = 1;
                    ps->particles[i].alpha = 0.8f;
                    ps->particles[i].life = 5.0f;
                }
            } else {
                ps->particles[i].life -= deltaTime * 0.2f;
                ps->particles[i].alpha = ps->particles[i].life * 0.2f;
            }

            if (ps->particles[i].life <= 0 || 
                fabs(ps->particles[i].x) > TERRAIN_SCALE/2 || 
                fabs(ps->particles[i].z) > TERRAIN_SCALE/2) {
                ps->particles[i].active = 0;
                ps->activeParticles--;
            }
        }
    }

    int maxNewParticles = (int)(ps->maxParticles * intensity * 0.1f);
    int newParticles = 0;

    while (ps->activeParticles < ps->maxParticles * intensity && newParticles < maxNewParticles) {
        for (int i = 0; i < ps->maxParticles; i++) {
            if (!ps->particles[i].active) {
                float x = ((float)rand()/RAND_MAX - 0.5f) * TERRAIN_SCALE * 0.8f;
                float z = ((float)rand()/RAND_MAX - 0.5f) * TERRAIN_SCALE * 0.8f;
                float y = getTerrainHeight(terrain, x, z) + 
                         100.0f + ((float)rand()/RAND_MAX) * 50.0f;
                
                ps->particles[i].x = x;
                ps->particles[i].y = y;
                ps->particles[i].z = z;
                
                ps->particles[i].vx = ((float)rand()/RAND_MAX - 0.5f) * 0.8f;
                ps->particles[i].vy = -2.5f - ((float)rand()/RAND_MAX) * 1.5f;
                ps->particles[i].vz = ((float)rand()/RAND_MAX - 0.5f) * 0.8f;
                
                ps->particles[i].size = 1.5f + ((float)rand()/RAND_MAX);
                ps->particles[i].life = 8.0f + ((float)rand()/RAND_MAX) * 4.0f;
                ps->particles[i].alpha = 1.0f;
                ps->particles[i].active = 1;
                ps->particles[i].accumulated = 0;
                
                ps->activeParticles++;
                newParticles++;
                break;
            }
        }
    }
}

// Particle rendering adapted from OpenGL-Tutorial.org and modified for weather effects
// Source: http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/

void renderParticles(ParticleSystem* ps) {
    if (!ps || ps->type != PARTICLE_SNOW) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    
    glPointSize(1.8f);

    glBegin(GL_POINTS);
    for (int i = 0; i < ps->maxParticles; i++) {
        if (ps->particles[i].active) {
            float dx = ps->particles[i].x;
            float dy = ps->particles[i].y;
            float dz = ps->particles[i].z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            
            float distAlpha = 1.0f - (dist / (TERRAIN_SCALE * 1.0f));
            distAlpha = distAlpha < 0 ? 0 : (distAlpha > 1 ? 1 : distAlpha);
            
            float finalAlpha = ps->particles[i].alpha * distAlpha * 0.7f;
            
            glColor4f(1.0f, 1.0f, 1.0f, finalAlpha);
            glVertex3f(ps->particles[i].x, ps->particles[i].y, ps->particles[i].z);
        }
    }
    glEnd();

    glDisable(GL_POINT_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

void freeParticleSystem(ParticleSystem* ps) {
    if (ps) {
        if (ps->particles) free(ps->particles);
        free(ps);
    }
}