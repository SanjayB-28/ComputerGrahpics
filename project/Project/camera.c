// ---------------------------------------------
// camera.c - Camera system for orbit and first-person views
// ---------------------------------------------

#include "camera.h"
#include "landscape.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

extern Landscape* landscape;

#define PI 3.14159265359f
#define DEG2RAD (PI/180.0f)
#define MIN_DISTANCE 1.0f
#define MAX_DISTANCE 300.0f
#define EYE_HEIGHT 2.0f
#define CAMERA_SPEED 20.0f
#define MIN_ZOOM 5.0f
#define MAX_ZOOM 1000.0f
#define ZOOM_SPEED 0.1f

static void constrainToTerrain(ViewCamera* cam);

// --- Camera creation and initialization ---
ViewCamera* viewCameraCreate(void) {
    ViewCamera* cam = (ViewCamera*)malloc(sizeof(ViewCamera));
    if (!cam) return NULL;
    cam->mode = CAMERA_MODE_FREE_ORBIT;
    cam->lookAt[0] = 0.0f;
    cam->lookAt[1] = 0.0f;
    cam->lookAt[2] = 0.0f;
    cam->horizontalAngle = 45.0f;
    cam->verticalAngle = 10.0f;
    cam->orbitDistance = LANDSCAPE_SCALE * 0.8f;
    float yawRad = cam->horizontalAngle * DEG2RAD;
    float pitchRad = cam->verticalAngle * DEG2RAD;
    cam->position[0] = -2 * cam->orbitDistance * sin(yawRad) * cos(pitchRad);
    cam->position[1] = 2 * cam->orbitDistance * sin(pitchRad);
    cam->position[2] = 2 * cam->orbitDistance * cos(yawRad) * cos(pitchRad);
    cam->upVec[0] = 0.0f;
    cam->upVec[1] = 1.0f;
    cam->upVec[2] = 0.0f;
    viewCameraUpdateVectors(cam);
    return cam;
}

// --- Update camera vectors based on mode and angles ---
void viewCameraUpdateVectors(ViewCamera* cam) {
    float yawRad = cam->horizontalAngle * DEG2RAD;
    float pitchRad = cam->verticalAngle * DEG2RAD;
    switch(cam->mode) {
        case CAMERA_MODE_FIRST_PERSON: {
            float frontX = -sin(yawRad) * cos(pitchRad);
            float frontY = sin(pitchRad);
            float frontZ = -cos(yawRad) * cos(pitchRad);
            cam->lookAt[0] = cam->position[0] + frontX;
            cam->lookAt[1] = cam->position[1] + frontY;
            cam->lookAt[2] = cam->position[2] + frontZ;
            cam->upVec[0] = 0.0f;
            cam->upVec[1] = 1.0f;
            cam->upVec[2] = 0.0f;
            break;
        }
        case CAMERA_MODE_FREE_ORBIT: {
            cam->position[0] = -2 * cam->orbitDistance * sin(yawRad) * cos(pitchRad);
            cam->position[1] = 2 * cam->orbitDistance * sin(pitchRad);
            cam->position[2] = 2 * cam->orbitDistance * cos(yawRad) * cos(pitchRad);
            cam->lookAt[0] = 0.0f;
            cam->lookAt[1] = 0.0f;
            cam->lookAt[2] = 0.0f;
            cam->upVec[0] = 0.0f;
            cam->upVec[1] = 1.0f;
            cam->upVec[2] = 0.0f;
            break;
        }
    }
}

void viewCameraMove(ViewCamera* cam, int direction, float deltaTime) {
    if (!cam) return;
    float speed = CAMERA_SPEED * deltaTime;
    float yawRad = cam->horizontalAngle * DEG2RAD;
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
    float halfScale = LANDSCAPE_SCALE * 0.5f;
    if (cam->mode == CAMERA_MODE_FIRST_PERSON) {
        float newX = cam->position[0] + moveX;
        float newZ = cam->position[2] + moveZ;
        if (newX >= -halfScale && newX <= halfScale && 
            newZ >= -halfScale && newZ <= halfScale) {
            float groundHeight = landscapeGetHeight(landscape, newX, newZ);
            cam->position[0] = newX;
            cam->position[2] = newZ;
            cam->position[1] = groundHeight + EYE_HEIGHT;
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
                cam->horizontalAngle -= speed * 100;
                break;
            case CAMERA_MOVE_RIGHT:
                cam->horizontalAngle += speed * 100;
                break;
        }
    }
    viewCameraUpdateVectors(cam);
}

void viewCameraRotate(ViewCamera* cam, float deltaX, float deltaY) {
    if (!cam) return;
    float sensitivity = (cam->mode == CAMERA_MODE_FREE_ORBIT) ? 0.4f : 0.2f;
    cam->horizontalAngle += deltaX * sensitivity;
    cam->verticalAngle += deltaY * sensitivity;
    if (cam->verticalAngle > 89.0f) cam->verticalAngle = 89.0f;
    if (cam->verticalAngle < -89.0f) cam->verticalAngle = -89.0f;
    if (cam->horizontalAngle >= 360.0f) cam->horizontalAngle -= 360.0f;
    if (cam->horizontalAngle < 0.0f) cam->horizontalAngle += 360.0f;
    viewCameraUpdateVectors(cam);
}

void viewCameraZoom(ViewCamera* cam, float factor) {
    if (!cam || cam->mode != CAMERA_MODE_FREE_ORBIT) return;
    float zoomAmount = (factor - 1.0f) * cam->orbitDistance * ZOOM_SPEED;
    cam->orbitDistance += zoomAmount;
    if (cam->orbitDistance < MIN_ZOOM) cam->orbitDistance = MIN_ZOOM;
    if (cam->orbitDistance > LANDSCAPE_SCALE * 2.0f) cam->orbitDistance = LANDSCAPE_SCALE * 2.0f;
    viewCameraUpdateVectors(cam);
}

void viewCameraSetMode(ViewCamera* cam, CameraMode newMode) {
    if (!cam) return;
    if (newMode == cam->mode) return;
    cam->mode = newMode;
    viewCameraUpdateVectors(cam);
}

void viewCameraUpdate(ViewCamera* cam, float deltaTime) {
    if (!cam) return;
    float groundHeight = landscapeGetHeight(landscape, cam->position[0], cam->position[2]);
    switch(cam->mode) {
        case CAMERA_MODE_FIRST_PERSON:
            cam->position[1] = groundHeight + EYE_HEIGHT;
            break;
        case CAMERA_MODE_FREE_ORBIT:
            break;
    }
    constrainToTerrain(cam);
}

static void constrainToTerrain(ViewCamera* cam) {
    float halfScale = LANDSCAPE_SCALE * 0.5f;
    if (cam->mode != CAMERA_MODE_FREE_ORBIT) {
        cam->position[0] = fmax(-halfScale, fmin(halfScale, cam->position[0]));
        cam->position[2] = fmax(-halfScale, fmin(halfScale, cam->position[2]));
    }
}

void viewCameraReset(ViewCamera* cam) {
    if (!cam) return;
    cam->horizontalAngle = -60.0f;
    cam->verticalAngle = 30.0f;
    cam->orbitDistance = LANDSCAPE_SCALE * 0.8f;
    viewCameraSetMode(cam, CAMERA_MODE_FREE_ORBIT);
}

void viewCameraDestroy(ViewCamera* cam) {
    free(cam);
}

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