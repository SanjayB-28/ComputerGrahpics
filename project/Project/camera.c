#include "camera.h"
#include "terrain.h"
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

extern Terrain* terrain;

#define PI 3.14159265359f
#define DEG2RAD (PI/180.0f)
#define MIN_DISTANCE 1.0f
#define MAX_DISTANCE 300.0f
#define EYE_HEIGHT 2.0f
#define CAMERA_SPEED 20.0f
#define FIRST_PERSON_HEIGHT 1.7f   
#define THIRD_PERSON_HEIGHT 1.0f   
#define CAMERA_SMOOTHING 0.1f
#define MIN_ZOOM 5.0f          
#define MAX_ZOOM 1000.0f        
#define DEFAULT_DISTANCE 500.0f 
#define ZOOM_SPEED 0.1f

void updateCameraVectors(Camera* cam);
static void constrainToTerrain(Camera* cam);

Camera* createCamera(void) {
    Camera* cam = (Camera*)malloc(sizeof(Camera));
    if (!cam) return NULL;
    
    cam->mode = VIEW_FREE_ORBIT;
    
    cam->target[0] = 0.0f;
    cam->target[1] = 0.0f;
    cam->target[2] = 0.0f;
    
   
    cam->yaw = -30.0f;  
    cam->pitch = 30.0f;
    cam->distance = 100.0f;
    
    
    float yawRad = cam->yaw * DEG2RAD;
    float pitchRad = cam->pitch * DEG2RAD;
    
    cam->pos[0] = -2 * cam->distance * sin(yawRad) * cos(pitchRad);
    cam->pos[1] = 2 * cam->distance * sin(pitchRad);
    cam->pos[2] = 2 * cam->distance * cos(yawRad) * cos(pitchRad);
    
    
    cam->up[0] = 0.0f;
    cam->up[1] = 1.0f;
    cam->up[2] = 0.0f;
    
    updateCameraVectors(cam);
    return cam;
}



void updateCameraVectors(Camera* cam) {
    float yawRad = cam->yaw * DEG2RAD;
    float pitchRad = cam->pitch * DEG2RAD;
    
    switch(cam->mode) {
        case VIEW_FIRST_PERSON: {
            float frontX = -sin(yawRad) * cos(pitchRad);
            float frontY = sin(pitchRad);
            float frontZ = -cos(yawRad) * cos(pitchRad);
            
            cam->target[0] = cam->pos[0] + frontX;
            cam->target[1] = cam->pos[1] + frontY;
            cam->target[2] = cam->pos[2] + frontZ;
            
            cam->up[0] = 0.0f;
            cam->up[1] = 1.0f;
            cam->up[2] = 0.0f;
            break;
        }
        
        case VIEW_THIRD_PERSON: {
            
            float horizontalDistance = cam->distance * cos(pitchRad);
            float verticalDistance = cam->distance * sin(pitchRad);
            
            float targetGroundHeight = getTerrainHeight(terrain, cam->target[0], cam->target[2]);
            cam->target[1] = targetGroundHeight + THIRD_PERSON_HEIGHT;
            
            float desiredX = cam->target[0] + horizontalDistance * sin(yawRad);
            float desiredZ = cam->target[2] + horizontalDistance * cos(yawRad);
            float desiredY = cam->target[1] + verticalDistance;
            
            float cameraGroundHeight = getTerrainHeight(terrain, desiredX, desiredZ);
            if (desiredY < cameraGroundHeight + 1.0f) {
                desiredY = cameraGroundHeight + 1.0f;
            }
            
            cam->pos[0] += (desiredX - cam->pos[0]) * CAMERA_SMOOTHING;
            cam->pos[1] += (desiredY - cam->pos[1]) * CAMERA_SMOOTHING;
            cam->pos[2] += (desiredZ - cam->pos[2]) * CAMERA_SMOOTHING;
            break;
        }
        
        case VIEW_TOP_DOWN: {
            
            float groundHeight = getTerrainHeight(terrain, cam->pos[0], cam->pos[2]);
            cam->pos[1] = groundHeight + cam->distance;
            cam->target[0] = cam->pos[0];
            cam->target[1] = groundHeight;
            cam->target[2] = cam->pos[2];
            
            cam->up[0] = 0.0f;
            cam->up[1] = 0.0f;
            cam->up[2] = -1.0f;
            break;
        }
        
        case VIEW_FREE_ORBIT: {
            
            cam->pos[0] = -2 * cam->distance * sin(yawRad) * cos(pitchRad);
            cam->pos[1] = 2 * cam->distance * sin(pitchRad);
            cam->pos[2] = 2 * cam->distance * cos(yawRad) * cos(pitchRad);
            
           
            cam->target[0] = 0.0f;
            cam->target[1] = 0.0f;
            cam->target[2] = 0.0f;
            
            cam->up[0] = 0.0f;
            cam->up[1] = 1.0f;
            cam->up[2] = 0.0f;
            break;
        }
    }
}


void moveCamera(Camera* cam, int direction, float deltaTime) {
    if (!cam) return;
    
    float baseSpeed = 20.0f;
    if (cam->mode == VIEW_TOP_DOWN) {
        baseSpeed = 50.0f;
    }
    
    float speed = baseSpeed * deltaTime;
    float yawRad = cam->yaw * DEG2RAD;
    
    
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
        case FORWARD:
            moveX = forward[0] * speed;
            moveZ = forward[2] * speed;
            break;
        case BACKWARD:
            moveX = -forward[0] * speed;
            moveZ = -forward[2] * speed;
            break;
        case LEFT:
            moveX = -right[0] * speed;
            moveZ = -right[2] * speed;
            break;
        case RIGHT:
            moveX = right[0] * speed;
            moveZ = right[2] * speed;
            break;
    }
    
    float halfScale = TERRAIN_SCALE * 0.5f;
    
    if (cam->mode == VIEW_FIRST_PERSON) {
        float newX = cam->pos[0] + moveX;
        float newZ = cam->pos[2] + moveZ;
        
        if (newX >= -halfScale && newX <= halfScale && 
            newZ >= -halfScale && newZ <= halfScale) {

            float groundHeight = getTerrainHeight(terrain, newX, newZ);
            
            
            float currentHeight = getTerrainHeight(terrain, cam->pos[0], cam->pos[2]);
            float slope = fabs(groundHeight - currentHeight) / speed;
            
            if (slope < 2.0f) {  
                cam->pos[0] = newX;
                cam->pos[2] = newZ;
                cam->pos[1] = groundHeight + FIRST_PERSON_HEIGHT;
            }
        }
    }
    else if (cam->mode == VIEW_THIRD_PERSON) {
        float newX = cam->target[0] + moveX;
        float newZ = cam->target[2] + moveZ;
        
        if (newX >= -halfScale && newX <= halfScale && 
            newZ >= -halfScale && newZ <= halfScale) {
            float groundHeight = getTerrainHeight(terrain, newX, newZ);
            float currentHeight = getTerrainHeight(terrain, cam->target[0], cam->target[2]);
            float slope = fabs(groundHeight - currentHeight) / speed;
            
            if (slope < 2.0f) {
                cam->target[0] = newX;
                cam->target[2] = newZ;
                float targetHeight = groundHeight + THIRD_PERSON_HEIGHT;
                cam->target[1] += (targetHeight - cam->target[1]) * CAMERA_SMOOTHING;
            }
        }
    }
    else if (cam->mode == VIEW_TOP_DOWN) {
        switch(direction) {
            case FORWARD:
                moveZ = -speed;
                break;
            case BACKWARD:
                moveZ = speed;
                break;
            case LEFT:
                moveX = -speed;
                break;
            case RIGHT:
                moveX = speed;
                break;
        }
        
        float newX = cam->pos[0] + moveX;
        float newZ = cam->pos[2] + moveZ;
        
        if (newX >= -halfScale && newX <= halfScale && 
            newZ >= -halfScale && newZ <= halfScale) {
            cam->pos[0] = newX;
            cam->pos[2] = newZ;
            cam->target[0] = cam->pos[0];
            cam->target[2] = cam->pos[2];
        }
    }
    else if (cam->mode == VIEW_FREE_ORBIT) {
        switch(direction) {
            case FORWARD:
                cam->distance = fmax(10.0f, cam->distance - speed * 10);
                break;
            case BACKWARD:
                cam->distance = fmin(500.0f, cam->distance + speed * 10);
                break;
            case LEFT:
                cam->yaw -= speed * 100;
                break;
            case RIGHT:
                cam->yaw += speed * 100;
                break;
        }
    }
    
    updateCameraVectors(cam);
}

void rotateCamera(Camera* cam, float deltaX, float deltaY) {
    if (!cam) return;
    
    float sensitivity = 0.1f;
    if (cam->mode == VIEW_FREE_ORBIT) {
        sensitivity = 0.3f;
    }
    else if (cam->mode == VIEW_TOP_DOWN) {
        return;
    }
    
    cam->yaw += deltaX * sensitivity;
    cam->pitch += deltaY * sensitivity;
    
    if (cam->mode == VIEW_FREE_ORBIT) {
        if (cam->pitch > 89.0f) cam->pitch = 89.0f;
        if (cam->pitch < -89.0f) cam->pitch = -89.0f;
    } else {
        if (cam->pitch > 89.0f) cam->pitch = 89.0f;
        if (cam->pitch < -89.0f) cam->pitch = -89.0f;
    }
    
    updateCameraVectors(cam);
}

void zoomCamera(Camera* cam, float factor) {
    if (!cam || cam->mode != VIEW_FREE_ORBIT) return;
    
    float zoomAmount = (factor - 1.0f) * cam->distance * ZOOM_SPEED;
    cam->distance += zoomAmount;
    
    if (cam->distance < MIN_ZOOM) cam->distance = MIN_ZOOM;
    if (cam->distance > MAX_ZOOM) cam->distance = MAX_ZOOM;
    
    updateCameraVectors(cam);
}

void setCameraView(Camera* cam, ViewMode newMode) {
    if (!cam) return;
    
    cam->mode = newMode;
    
    switch(newMode) {
        case VIEW_FIRST_PERSON: {
            cam->distance = 0.0f;
            cam->pitch = 0.0f;
            cam->pos[0] = TERRAIN_SCALE * 0.005f;
            cam->pos[2] = TERRAIN_SCALE * 0.005f;
            float groundHeight = getTerrainHeight(terrain, cam->pos[0], cam->pos[2]);
            cam->pos[1] = groundHeight + FIRST_PERSON_HEIGHT;
            break;
        }
        
        case VIEW_THIRD_PERSON: {
            cam->distance = 15.0f;
            cam->pitch = 25.0f;
            cam->target[0] = TERRAIN_SCALE * 0.25f;
            cam->target[2] = TERRAIN_SCALE * 0.25f;
            float groundHeight = getTerrainHeight(terrain, cam->target[0], cam->target[2]);
            cam->target[1] = groundHeight + THIRD_PERSON_HEIGHT;
            break;
        }
        
        case VIEW_TOP_DOWN: {
            cam->distance = TERRAIN_SCALE * 0.5f;
            cam->pitch = -89.0f;
            cam->yaw = 0.0f;
            cam->pos[0] = 0.0f;
            cam->pos[2] = 0.0f;
            float groundHeight = getTerrainHeight(terrain, 0, 0);
            cam->pos[1] = groundHeight + cam->distance;
            break;
        }
        
        case VIEW_FREE_ORBIT: {
            cam->distance = TERRAIN_SCALE * 0.75f;
            cam->pitch = 35.0f;
            cam->yaw = -45.0f;
            cam->target[0] = 0.0f;
            cam->target[1] = 0.0f;
            cam->target[2] = 0.0f;
            break;
        }
    }
    
    updateCameraVectors(cam);
}

void updateCamera(Camera* cam, float deltaTime) {
    if (!cam) return;
    
    float groundHeight = getTerrainHeight(terrain, cam->pos[0], cam->pos[2]);
    
    switch(cam->mode) {
        case VIEW_FIRST_PERSON:
            cam->pos[1] = groundHeight + EYE_HEIGHT;
            break;
            
        case VIEW_THIRD_PERSON:
            cam->target[1] = getTerrainHeight(terrain, cam->target[0], cam->target[2]) + EYE_HEIGHT/2;
            updateCameraVectors(cam);
            break;
            
        case VIEW_TOP_DOWN:
            cam->pos[1] = groundHeight + cam->distance;
            cam->target[1] = groundHeight;
            break;
            
        case VIEW_FREE_ORBIT:
            break;
    }
    
    constrainToTerrain(cam);
}

static void constrainToTerrain(Camera* cam) {

    float halfScale = TERRAIN_SCALE * 0.5f;
    
    if (cam->mode != VIEW_FREE_ORBIT) {
        cam->pos[0] = fmax(-halfScale, fmin(halfScale, cam->pos[0]));
        cam->pos[2] = fmax(-halfScale, fmin(halfScale, cam->pos[2]));
        
        if (cam->mode == VIEW_THIRD_PERSON) {
            cam->target[0] = fmax(-halfScale, fmin(halfScale, cam->target[0]));
            cam->target[2] = fmax(-halfScale, fmin(halfScale, cam->target[2]));
        }
    }
}

void resetCamera(Camera* cam) {
    if (!cam) return;
    
    cam->yaw = -90.0f;
    cam->pitch = 0.0f;
    cam->pos[0] = 0.0f;
    cam->pos[2] = 0.0f;
    cam->distance = 15.0f;
    
    setCameraView(cam, VIEW_THIRD_PERSON);
}

void freeCamera(Camera* cam) {
    free(cam);
}

void setProjection(Camera* cam, float fov, float aspect, float nearPlane, float farPlane) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    if (cam->mode == VIEW_FIRST_PERSON) {
        gluPerspective(fov, aspect, 0.1f, farPlane);
    } else {
        gluPerspective(fov, aspect, nearPlane, farPlane);
    }
    
    glMatrixMode(GL_MODELVIEW);
}