#ifndef CAMERA_H
#define CAMERA_H

// Camera enumeration and structures adapted from:
// - LearnOpenGL Camera System (https://learnopengl.com/Getting-started/Camera)
// - Modified with assistance from ChatGPT for multiple view modes
typedef enum {
    VIEW_FIRST_PERSON,
    VIEW_THIRD_PERSON,
    VIEW_TOP_DOWN,
    VIEW_FREE_ORBIT
} ViewMode;

typedef enum {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    float pos[3];
    float target[3];
    float up[3];
    float right[3];
    float yaw;
    float pitch;
    float distance;
    ViewMode mode;
} Camera;

Camera* createCamera(void);
void updateCameraVectors(Camera* cam);
void freeCamera(Camera* cam);
void moveCamera(Camera* cam, int direction, float deltaTime);
void rotateCamera(Camera* cam, float deltaYaw, float deltaPitch);
void zoomCamera(Camera* cam, float factor);
void setCameraView(Camera* cam, ViewMode newMode);
void updateCamera(Camera* cam, float deltaTime);
void resetCamera(Camera* cam);
void setProjection(Camera* cam, float fov, float aspect, float nearPlane, float farPlane);

#endif