#ifndef PARTICLES_H
#define PARTICLES_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "CSCIx229.h"
#include "terrain.h"

#define PARTICLE_SNOW 0
#define PARTICLE_RAIN 1

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float ax, ay, az;
    float life;
    float size;
    float alpha;
    int active;
    int accumulated;
} Particle;

typedef struct {
    Particle* particles;
    int maxParticles;
    int activeParticles;
    float emitterX, emitterY, emitterZ;
    float emitterRadius;
    int type;
} ParticleSystem;

ParticleSystem* createParticleSystem(int maxParticles, int type);
void updateParticles(ParticleSystem* ps, float deltaTime, float intensity, float time);
void renderParticles(ParticleSystem* ps);
void freeParticleSystem(ParticleSystem* ps);

#endif