#include "math_utils.h"
#include <math.h>

// --- Constants ---
#define PI 3.14159265359

// --- Vector Implementations ---

Vector3 vec3_add(Vector3 a, Vector3 b) { return (Vector3){a.x + b.x, a.y + b.y, a.z + b.z}; }
Vector3 vec3_subtract(Vector3 a, Vector3 b) { return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z}; }
Vector3 vec3_scale(Vector3 v, float s) { return (Vector3){v.x * s, v.y * s, v.z * s}; }
float vec3_magnitude(Vector3 v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }

Vector3 vec3_normalize(Vector3 v) {
    float mag = vec3_magnitude(v);
    if (mag > 0.0f) {
        return vec3_scale(v, 1.0f / mag);
    }
    return v;
}

float vec3_dot(Vector3 a, Vector3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vector3 vec3_cross(Vector3 a, Vector3 b) {
    return (Vector3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// --- Matrix Implementations ---

Matrix4x4 mat4_identity() {
    Matrix4x4 result = {0};
    result.m[0] = 1.0f;
    result.m[5] = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;
    return result;
}

Matrix4x4 mat4_multiply(Matrix4x4 a, Matrix4x4 b) {
    Matrix4x4 result = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.m[i * 4 + j] += a.m[k * 4 + j] * b.m[i * 4 + k];
            }
        }
    }
    return result;
}

Matrix4x4 mat4_perspective(float fov_degrees, float aspect_ratio, float near_plane, float far_plane) {
    Matrix4x4 result = {0};
    float tan_half_fov = tanf(to_radians(fov_degrees) / 2.0f);
    result.m[0] = 1.0f / (aspect_ratio * tan_half_fov);
    result.m[5] = 1.0f / (tan_half_fov);
    result.m[10] = -(far_plane + near_plane) / (far_plane - near_plane);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    return result;
}

Matrix4x4 mat4_lookat(Vector3 position, Vector3 target, Vector3 up) {
    Vector3 f = vec3_normalize(vec3_subtract(target, position));
    Vector3 s = vec3_normalize(vec3_cross(f, up));
    Vector3 u = vec3_cross(s, f);

    Matrix4x4 result = mat4_identity();
    result.m[0] = s.x; result.m[4] = s.y; result.m[8] = s.z;
    result.m[1] = u.x; result.m[5] = u.y; result.m[9] = u.z;
    result.m[2] = -f.x; result.m[6] = -f.y; result.m[10] = -f.z;
    result.m[12] = -vec3_dot(s, position);
    result.m[13] = -vec3_dot(u, position);
    result.m[14] = vec3_dot(f, position);
    return result;
}


// --- Utility Implementations ---
float lerp(float a, float b, float t) { return a + t * (b - a); }
float to_radians(float degrees) { return degrees * (PI / 180.0f); }