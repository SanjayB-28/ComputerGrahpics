// weather_particles.h - Particle system for weather effects (snow, rain)
#ifndef PARTICLES_H
#define PARTICLES_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "CSCIx229.h"
#include "landscape.h"

#define WEATHER_PARTICLE_SNOW 0
#define WEATHER_PARTICLE_RAIN 1

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float ax, ay, az;
    float life;
    float size;
    float alpha;
    int active;
    int accumulated;
} WeatherParticle;

typedef struct {
    WeatherParticle* particles;
    int maxParticles;
    int activeParticles;
    float emitterX, emitterY, emitterZ;
    float emitterRadius;
    int type;
} WeatherParticleSystem;

WeatherParticleSystem* weatherParticleSystemCreate(int maxParticles, int type);
void weatherParticleSystemUpdate(WeatherParticleSystem* ps, Landscape* landscape, float deltaTime, float intensity, float time);
void weatherParticleSystemRender(WeatherParticleSystem* ps);
void weatherParticleSystemDestroy(WeatherParticleSystem* ps);

#endif