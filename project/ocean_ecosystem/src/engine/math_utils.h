#ifndef MATH_UTILS_H
#define MATH_UTILS_H

// --- Structures ---

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    float m[16]; // Column-major order
} Matrix4x4;

// --- Vector Operations ---
Vector3 vec3_add(Vector3 a, Vector3 b);
Vector3 vec3_subtract(Vector3 a, Vector3 b);
Vector3 vec3_scale(Vector3 v, float s);
float vec3_magnitude(Vector3 v);
Vector3 vec3_normalize(Vector3 v);
float vec3_dot(Vector3 a, Vector3 b);
Vector3 vec3_cross(Vector3 a, Vector3 b);

// --- Matrix Operations ---
Matrix4x4 mat4_identity();
Matrix4x4 mat4_multiply(Matrix4x4 a, Matrix4x4 b);
Matrix4x4 mat4_perspective(float fov_degrees, float aspect_ratio, float near_plane, float far_plane);
Matrix4x4 mat4_lookat(Vector3 position, Vector3 target, Vector3 up);

// --- Utility Functions ---
float lerp(float a, float b, float t);
float to_radians(float degrees);

#endif // MATH_UTILS_H