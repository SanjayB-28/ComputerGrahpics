// weather_particles.h - Weather effects system (snow, rain)

#ifndef PARTICLES_H
#define PARTICLES_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "CSCIx229.h"
#include "landscape.h"

// --- Particle types ---
#define WEATHER_PARTICLE_SNOW 0  // Snow
#define WEATHER_PARTICLE_RAIN 1  // Rain

// --- Data structures ---
/* Individual weather particle */
typedef struct {
    float x, y, z;               // Position
    float vx, vy, vz;            // Velocity
    float ax, ay, az;            // Acceleration
    float life;                  // Remaining lifetime
    float size;                  // Size
    float alpha;                 // Transparency
    int active;                  // Active flag
    int accumulated;             // Ground settled flag
} WeatherParticle;

/* Complete particle system */
typedef struct {
    WeatherParticle* particles;  // Particle array
    int maxParticles;            // Maximum particles
    int activeParticles;         // Current count
    float emitterX, emitterY, emitterZ; // Emitter position
    float emitterRadius;         // Emission radius
    int type;                    // Particle type (snow/rain)
} WeatherParticleSystem;

// --- Particle system API ---
/* Creates a new particle system */
WeatherParticleSystem* weatherParticleSystemCreate(int maxParticles, int type);

/* Updates particle positions and lifecycle */
void weatherParticleSystemUpdate(WeatherParticleSystem* ps, Landscape* landscape, float deltaTime, float intensity, float time);

/* Renders particles */
void weatherParticleSystemRender(WeatherParticleSystem* ps);

/* Frees system resources */
void weatherParticleSystemDestroy(WeatherParticleSystem* ps);

#endif