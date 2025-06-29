// Particle update vertex shader - handles GPU-based particle physics and collision (had a little help from claude)

#version 120

attribute vec3 pos;
attribute vec3 vel;
attribute float restTime;
attribute float state;

uniform float dt;
uniform float time;
uniform float cloudHeight;
uniform sampler2D heightmap;
uniform float landscapeScale;
uniform float landscapeSize;
uniform vec2 wind;
uniform float terrainMinX;
uniform float terrainMaxX;
uniform float terrainMinZ;
uniform float terrainMaxZ;

varying vec3 outPos;
varying vec3 outVel;
varying float outRestTime;
varying float outState;

float getTerrainHeight(float x, float z) {
    float nx = (x / landscapeScale + 0.5) * (landscapeSize - 1.0);
    float nz = (z / landscapeScale + 0.5) * (landscapeSize - 1.0);
    vec2 uv = vec2(nx, nz) / (landscapeSize - 1.0);
    return texture2D(heightmap, uv).r;
}

float hash(float n) { return fract(sin(n) * 43758.5453); }

float hash2(float n, float m) { return fract(sin(n * 12.9898 + m * 78.233) * 43758.5453); }

void main() {
    vec3 newPos = pos;
    vec3 newVel = vel;
    float newRestTime = restTime;
    float newState = state;

    float terrainY = getTerrainHeight(pos.x, pos.z);

    float swaySeed = pos.x + pos.z + restTime + state * 17.0 + time * 0.1;
    float swayX = sin(pos.x * 0.2 + time * 1.2 + pos.z * 0.15 + hash2(swaySeed, 1.0) * 6.28) * (3.0 + 3.0 * hash2(swaySeed, 2.0));
    float swayZ = cos(pos.z * 0.18 + time * 1.1 + pos.x * 0.13 + hash2(swaySeed, 3.0) * 6.28) * (2.0 + 2.0 * hash2(swaySeed, 4.0));
    vec3 windVel = vec3(wind.x + swayX, 0.0, wind.y + swayZ);

    if (state < 0.5) {
        newVel = windVel + vec3(0.0, vel.y, 0.0);
        newPos = pos + newVel * dt;
        float margin = 1.0;
        newPos.x = clamp(newPos.x, terrainMinX + margin, terrainMaxX - margin);
        newPos.z = clamp(newPos.z, terrainMinZ + margin, terrainMaxZ - margin);
        if (newPos.y <= terrainY) {
            newPos.y = terrainY;
            newVel = vec3(0.0);
            newRestTime = 0.0;
            newState = 1.0;
        }
    } else {
        newRestTime = restTime + dt;
        newVel = vec3(0.0);
        float margin = 1.0;
        if (newRestTime > 5.0) {
            float seed = time + pos.x + pos.z + restTime + state * 31.0;
            float rx = clamp(mix(terrainMinX + margin, terrainMaxX - margin, hash2(seed, 1.0)), terrainMinX, terrainMaxX);
            float rz = clamp(mix(terrainMinZ + margin, terrainMaxZ - margin, hash2(seed, 2.0)), terrainMinZ, terrainMaxZ);
            float rv = mix(-15.0, -5.0, hash2(seed, 3.0));
            newPos = vec3(rx, cloudHeight, rz);
            newVel = vec3(0.0, rv, 0.0);
            newRestTime = 0.0;
            newState = 0.0;
        }
    }

    outPos = newPos;
    outVel = newVel;
    outRestTime = newRestTime;
    outState = newState;
    gl_Position = vec4(newPos, 1.0);
}