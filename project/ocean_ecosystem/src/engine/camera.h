#ifndef CAMERA_H
#define CAMERA_H

#include "math_utils.h"

typedef struct {
    Vector3 position;
    float yaw;          // Horizontal angle
    float pitch;        // Vertical angle

    // Mouse look state
    float target_yaw;
    float target_pitch;

    // Sway effect state
    float sway_timer;
} Camera;

void camera_init(Camera* camera);
void camera_update(Camera* camera, float delta_time);
void camera_handle_input(Camera* camera, int mouse_dx, int mouse_dy);
void camera_get_view_matrix(Camera* camera, Matrix4x4* view_matrix);
void camera_get_projection_matrix(float aspect_ratio, Matrix4x4* proj_matrix);

#endif // CAMERA_H