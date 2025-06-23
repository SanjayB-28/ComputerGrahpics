// ---------------------------------------------
// camera.h - Camera system interface
// ---------------------------------------------

#pragma once

// --- Camera modes ---
typedef enum {
    CAMERA_MODE_FIRST_PERSON,   // FPS style
    CAMERA_MODE_FREE_ORBIT      // Orbit/arcball style
} CameraMode;

// --- Camera movement directions ---
typedef enum {
    CAMERA_MOVE_FORWARD,
    CAMERA_MOVE_BACKWARD,
    CAMERA_MOVE_LEFT,
    CAMERA_MOVE_RIGHT
} CameraMoveDir;

// --- Camera state struct ---
typedef struct {
    float position[3];      // Camera position
    float lookAt[3];        // Target/look-at point
    float upVec[3];         // Up vector
    float rightVec[3];      // Right vector (not always used)
    float horizontalAngle;  // Yaw
    float verticalAngle;    // Pitch
    float orbitDistance;    // Distance for orbit mode
    CameraMode mode;        // Current camera mode
} ViewCamera;

// --- Camera API ---
ViewCamera* viewCameraCreate(void);
void viewCameraUpdateVectors(ViewCamera* cam);
void viewCameraDestroy(ViewCamera* cam);
void viewCameraMove(ViewCamera* cam, int direction, float deltaTime);
void viewCameraRotate(ViewCamera* cam, float deltaYaw, float deltaPitch);
void viewCameraZoom(ViewCamera* cam, float factor);
void viewCameraSetMode(ViewCamera* cam, CameraMode newMode);
void viewCameraUpdate(ViewCamera* cam, float deltaTime);
void viewCameraReset(ViewCamera* cam);
void viewCameraSetProjection(ViewCamera* cam, float fov, float aspect, float nearPlane, float farPlane);