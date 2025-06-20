#ifndef CSCIx229
#define CSCIx229

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

// GLEW MUST be included first
#ifdef USEGLEW
#include <GL/glew.h>
#endif

// Get all GL prototypes
#define GL_GLEXT_PROTOTYPES

// Select SDL, SDL2, GLFW or GLUT
#if defined(SDL2)
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#elif defined(SDL)
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#elif defined(GLFW)
#include <GLFW/glfw3.h>
#elif defined(APPLE)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Make sure GLU and GL are included
#ifdef APPLE
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

// Default resolution
// For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

// cos and sin in degrees
#define Cos(th) cos(3.14159265/180*(th))
#define Sin(th) sin(3.14159265/180*(th))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
void Print(const char* format , ...) __attribute__ ((format(printf,1,2)));
void Fatal(const char* format , ...) __attribute__ ((format(printf,1,2))) __attribute__ ((noreturn));
#else
void Print(const char* format , ...);
void Fatal(const char* format , ...);
#endif

unsigned int LoadTexBMP(const char* file);
void Project(double fov,double asp,double dim);
void ErrCheck(const char* where);
int LoadOBJ(const char* file);

#ifdef __cplusplus
}
#endif

#endif