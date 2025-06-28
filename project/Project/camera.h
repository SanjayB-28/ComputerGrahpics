#pragma once

typedef enum {
    CAMERA_MODE_FIRST_PERSON,
    CAMERA_MODE_FREE_ORBIT
} CameraMode;

typedef enum {
    CAMERA_MOVE_FORWARD,
    CAMERA_MOVE_BACKWARD,
    CAMERA_MOVE_LEFT,
    CAMERA_MOVE_RIGHT
} CameraMoveDir;

typedef struct {
    float fpPosition[3];
    float fpYaw;
    float fpPitch;
    float orbitYaw;
    float orbitPitch;
    float orbitDistance;
    float lookAt[3];
    float upVec[3];
    CameraMode mode;
} ViewCamera;

ViewCamera* viewCameraCreate(void);
void viewCameraUpdateVectors(ViewCamera* cam);
void viewCameraDestroy(ViewCamera* cam);
void viewCameraMove(ViewCamera* cam, int direction, float deltaTime);
void viewCameraRotate(ViewCamera* cam, float deltaYaw, float deltaPitch);
void viewCameraSetMode(ViewCamera* cam, CameraMode newMode);
void viewCameraUpdate(ViewCamera* cam, float deltaTime);
void viewCameraSetProjection(ViewCamera* cam, float fov, float aspect, float nearPlane, float farPlane);