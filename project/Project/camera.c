#include "CSCIx229.h"
#include "camera.h"
#include "landscape.h"

extern Landscape* landscape;

#define PI 3.14159265359f
#define DEG2RAD (PI/180.0f)
#define MIN_DISTANCE 1.0f
#define EYE_HEIGHT 2.0f
#define CAMERA_SPEED 20.0f

static void constrainToTerrain(ViewCamera* cam);

// Create and initialize camera with default settings
ViewCamera* viewCameraCreate(void) {
    ViewCamera* cam = (ViewCamera*)malloc(sizeof(ViewCamera));
    if (!cam) return NULL;
    cam->mode = CAMERA_MODE_FREE_ORBIT;
    cam->fpPosition[0] = 0.0f;
    cam->fpPosition[1] = 2.0f;
    cam->fpPosition[2] = 0.0f;
    cam->fpYaw = 45.0f;
    cam->fpPitch = 10.0f;
    cam->orbitYaw = 45.0f;
    cam->orbitPitch = 10.0f;
    cam->orbitDistance = LANDSCAPE_SCALE * 0.8f;
    cam->lookAt[0] = 0.0f;
    cam->lookAt[1] = 0.0f;
    cam->lookAt[2] = 0.0f;
    cam->upVec[0] = 0.0f;
    cam->upVec[1] = 1.0f;
    cam->upVec[2] = 0.0f;
    viewCameraUpdateVectors(cam);
    return cam;
}

// Update camera view vectors based on current mode
void viewCameraUpdateVectors(ViewCamera* cam) {
    switch(cam->mode) {
        case CAMERA_MODE_FIRST_PERSON: {
            float yawRad = cam->fpYaw * DEG2RAD;
            float pitchRad = cam->fpPitch * DEG2RAD;
            float frontX = -sin(yawRad) * cos(pitchRad);
            float frontY = sin(pitchRad);
            float frontZ = -cos(yawRad) * cos(pitchRad);
            cam->lookAt[0] = cam->fpPosition[0] + frontX;
            cam->lookAt[1] = cam->fpPosition[1] + frontY;
            cam->lookAt[2] = cam->fpPosition[2] + frontZ;
            cam->upVec[0] = 0.0f;
            cam->upVec[1] = 1.0f;
            cam->upVec[2] = 0.0f;
            break;
        }
        case CAMERA_MODE_FREE_ORBIT: {
            float yawRad = cam->orbitYaw * DEG2RAD;
            float pitchRad = cam->orbitPitch * DEG2RAD;
            float posX = -2 * cam->orbitDistance * sin(yawRad) * cos(pitchRad);
            float posY = 2 * cam->orbitDistance * sin(pitchRad);
            float posZ = 2 * cam->orbitDistance * cos(yawRad) * cos(pitchRad);
            cam->lookAt[0] = 0.0f;
            cam->lookAt[1] = 0.0f;
            cam->lookAt[2] = 0.0f;
            cam->upVec[0] = 0.0f;
            cam->upVec[1] = 1.0f;
            cam->upVec[2] = 0.0f;
            cam->fpPosition[0] = posX;
            cam->fpPosition[1] = posY;
            cam->fpPosition[2] = posZ;
            break;
        }
    }
}

// Move camera based on input direction (had a little help from copilot)
void viewCameraMove(ViewCamera* cam, int direction, float deltaTime) {
    if (!cam) return;
    float speed = CAMERA_SPEED * deltaTime;
    float halfScale = LANDSCAPE_SCALE * 0.5f;
    if (cam->mode == CAMERA_MODE_FIRST_PERSON) {
        float yawRad = cam->fpYaw * DEG2RAD;
        float forward[3] = {
            -sin(yawRad),
            0.0f,
            -cos(yawRad)
        };
        float right[3] = {
            cos(yawRad),
            0.0f,
            -sin(yawRad)
        };
        float moveX = 0.0f;
        float moveZ = 0.0f;
        switch(direction) {
            case CAMERA_MOVE_FORWARD:
                moveX = forward[0] * speed;
                moveZ = forward[2] * speed;
                break;
            case CAMERA_MOVE_BACKWARD:
                moveX = -forward[0] * speed;
                moveZ = -forward[2] * speed;
                break;
            case CAMERA_MOVE_LEFT:
                moveX = -right[0] * speed;
                moveZ = -right[2] * speed;
                break;
            case CAMERA_MOVE_RIGHT:
                moveX = right[0] * speed;
                moveZ = right[2] * speed;
                break;
        }
        float newX = cam->fpPosition[0] + moveX;
        float newZ = cam->fpPosition[2] + moveZ;
        if (newX >= -halfScale && newX <= halfScale && 
            newZ >= -halfScale && newZ <= halfScale) {
            float groundHeight = landscapeGetHeight(landscape, newX, newZ);
            cam->fpPosition[0] = newX;
            cam->fpPosition[2] = newZ;
            cam->fpPosition[1] = groundHeight + EYE_HEIGHT;
        }
    } else if (cam->mode == CAMERA_MODE_FREE_ORBIT) {
        switch(direction) {
            case CAMERA_MOVE_FORWARD:
                cam->orbitDistance = fmax(10.0f, cam->orbitDistance - speed * 10);
                break;
            case CAMERA_MOVE_BACKWARD:
                cam->orbitDistance = fmin(500.0f, cam->orbitDistance + speed * 10);
                break;
            case CAMERA_MOVE_LEFT:
                cam->orbitYaw -= speed * 100;
                break;
            case CAMERA_MOVE_RIGHT:
                cam->orbitYaw += speed * 100;
                break;
        }
    }
    viewCameraUpdateVectors(cam);
}

// Rotate camera based on mouse input
void viewCameraRotate(ViewCamera* cam, float deltaX, float deltaY) {
    if (!cam) return;
    if (cam->mode == CAMERA_MODE_FREE_ORBIT) {
        cam->orbitYaw += deltaX * 0.4f;
        cam->orbitPitch += deltaY * 0.4f;
    } else {
        cam->fpYaw += deltaX * 0.2f;
        cam->fpPitch += deltaY * 0.2f;
    }
    viewCameraUpdateVectors(cam);
}

// Switch between camera modes (orbit vs first-person)
void viewCameraSetMode(ViewCamera* cam, CameraMode newMode) {
    if (!cam) return;
    if (newMode == cam->mode) return;
    cam->mode = newMode;
    viewCameraUpdateVectors(cam);
}

// Update camera position and constraints
void viewCameraUpdate(ViewCamera* cam, float deltaTime) {
    if (!cam) return;
    if (cam->mode == CAMERA_MODE_FIRST_PERSON) {
        float groundHeight = landscapeGetHeight(landscape, cam->fpPosition[0], cam->fpPosition[2]);
        cam->fpPosition[1] = groundHeight + EYE_HEIGHT;
    }
    constrainToTerrain(cam);
}

// Constrain camera to terrain boundaries
static void constrainToTerrain(ViewCamera* cam) {
    float halfScale = LANDSCAPE_SCALE * 0.5f;
    if (cam->mode == CAMERA_MODE_FIRST_PERSON) {
        cam->fpPosition[0] = fmax(-halfScale, fmin(halfScale, cam->fpPosition[0]));
        cam->fpPosition[2] = fmax(-halfScale, fmin(halfScale, cam->fpPosition[2]));
    }
}

void viewCameraDestroy(ViewCamera* cam) {
    free(cam);
}

// Set camera projection matrix
void viewCameraSetProjection(ViewCamera* cam, float fov, float aspect, float nearPlane, float farPlane) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (cam->mode == CAMERA_MODE_FIRST_PERSON) {
        gluPerspective(fov, aspect, 0.1f, farPlane);
    } else {
        gluPerspective(fov, aspect, nearPlane, farPlane);
    }
    glMatrixMode(GL_MODELVIEW);
}