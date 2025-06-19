#include "camera.h"
#include <math.h>
#include <string.h> // For memcpy

// This references the global key state array that will be in main.c
extern unsigned char g_key_state[256];

// Camera settings
#define MOUSE_SENSITIVITY 0.1f
#define MOVEMENT_SPEED 5.0f
#define CAMERA_DAMPENING 10.0f
#define SWAY_AMOUNT 0.02f
#define SWAY_SPEED 0.5f
#define WATER_SURFACE_Y 0.0f

void camera_init(Camera* camera) {
    camera->position = (Vector3){0.0f, 5.0f, 15.0f}; // Start underwater
    camera->yaw = -90.0f;
    camera->pitch = -10.0f;
    camera->target_yaw = camera->yaw;
    camera->target_pitch = camera->pitch;
    camera->sway_timer = 0.0f;
}

void camera_handle_input(Camera* camera, int mouse_dx, int mouse_dy) {
    // Update target orientation from mouse input
    camera->target_yaw += mouse_dx * MOUSE_SENSITIVITY;
    camera->target_pitch -= mouse_dy * MOUSE_SENSITIVITY;

    // Clamp pitch to avoid flipping
    if (camera->target_pitch > 89.0f) camera->target_pitch = 89.0f;
    if (camera->target_pitch < -89.0f) camera->target_pitch = -89.0f;
}

void camera_update(Camera* camera, float delta_time) {
    // Smoothly interpolate current orientation to target orientation
    camera->yaw = lerp(camera->yaw, camera->target_yaw, delta_time * CAMERA_DAMPENING);
    camera->pitch = lerp(camera->pitch, camera->target_pitch, delta_time * CAMERA_DAMPENING);

    // Calculate forward and right vectors
    Vector3 front;
    front.x = cosf(to_radians(camera->yaw)) * cosf(to_radians(camera->pitch));
    front.y = sinf(to_radians(camera->pitch));
    front.z = sinf(to_radians(camera->yaw)) * cosf(to_radians(camera->pitch));
    front = vec3_normalize(front);
    Vector3 right = vec3_normalize(vec3_cross(front, (Vector3){0.0f, 1.0f, 0.0f}));

    // Handle keyboard movement
    Vector3 velocity = {0};
    if (g_key_state['w'] || g_key_state['W']) velocity = vec3_add(velocity, front);
    if (g_key_state['s'] || g_key_state['S']) velocity = vec3_subtract(velocity, front);
    if (g_key_state['a'] || g_key_state['A']) velocity = vec3_subtract(velocity, right);
    if (g_key_state['d'] || g_key_state['D']) velocity = vec3_add(velocity, right);
    if (g_key_state[' ']) velocity.y += 1.0f; // Up
    if (g_key_state['c'] || g_key_state['C']) velocity.y -= 1.0f; // Down

    camera->position = vec3_add(camera->position, vec3_scale(velocity, MOVEMENT_SPEED * delta_time));

    // Add underwater sway
    camera->sway_timer += delta_time * SWAY_SPEED;
    float sway_offset_x = sinf(camera->sway_timer) * SWAY_AMOUNT;
    float sway_offset_y = cosf(camera->sway_timer * 2.0f) * SWAY_AMOUNT;
    camera->position.x += sway_offset_x;
    camera->position.y += sway_offset_y;

    // Constrain camera to stay below water surface
    if (camera->position.y > WATER_SURFACE_Y - 0.1f) {
        camera->position.y = WATER_SURFACE_Y - 0.1f;
    }
}

void camera_get_view_matrix(Camera* camera, Matrix4x4* view_matrix) {
    Vector3 front;
    front.x = cosf(to_radians(camera->yaw)) * cosf(to_radians(camera->pitch));
    front.y = sinf(to_radians(camera->pitch));
    front.z = sinf(to_radians(camera->yaw)) * cosf(to_radians(camera->pitch));
    front = vec3_normalize(front);

    Vector3 target = vec3_add(camera->position, front);
    Vector3 up = {0.0f, 1.0f, 0.0f};

    *view_matrix = mat4_lookat(camera->position, target, up);
}

void camera_get_projection_matrix(float aspect_ratio, Matrix4x4* proj_matrix) {
    *proj_matrix = mat4_perspective(60.0f, aspect_ratio, 0.1f, 500.0f);
}