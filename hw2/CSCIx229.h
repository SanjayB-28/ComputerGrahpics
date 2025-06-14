#ifndef CSCIx229
#define CSCIx229

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))

#ifdef __cplusplus
extern "C" {
#endif

// Prototypes from utils.c
#ifdef __GNUC__
void Print(const char* format , ...) __attribute__ ((format(printf,1,2)));
void Fatal(const char* format , ...) __attribute__ ((format(printf,1,2))) __attribute__ ((noreturn));
void ErrCheck(const char* where);
#else
void Print(const char* format , ...);
void Fatal(const char* format , ...);
void ErrCheck(const char* where);
#endif

// Prototypes from objects.c
void drawDesk(double x, double y, double z, double dx, double dy, double dz, double th);
void drawMonitor(double x, double y, double z, double scale, double th);
void drawMacBook(double x, double y, double z, double scale, double th, double angle);
void drawKeyboard(double x, double y, double z, double scale, double th);
void drawMouse(double x, double y, double z, double scale, double th);
void drawMousePad(double x, double y, double z, double dx, double dz, double th);
void drawNotebook(double x, double y, double z, double dx, double dy, double dz, double th, float r, float g, float b);
void drawFloor(double y, double sizeX, double sizeZ, double thick, float r, float g, float b);

// Prototype from projection.c
void Project(int projMode, double asp, double dim, int fov);

#ifdef __cplusplus
}
#endif

#endif
