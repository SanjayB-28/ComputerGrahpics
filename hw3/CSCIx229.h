#ifndef CSCIx229
#define CSCIx229

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))

#ifdef __cplusplus
extern "C" {
#endif

void Print(const char* format , ...);
void Fatal(const char* format , ...);
unsigned int LoadTexBMP(const char* file);
void Project(double fov,double asp,double dim);
void ErrCheck(const char* where);
int  LoadOBJ(const char* file);
void special(int key,int x,int y);
void key(unsigned char ch,int x,int y);
void idle();
void reshape(int width,int height);

// Global Variables
extern int axes;         // Display axes
extern int th;           // Azimuth of view angle
extern int ph;           // Elevation of view angle
extern int mode;         // Color mode
extern double dim;       // Dimension of orthogonal box
extern double len;       // Length of axes
extern int n;            // Maximum number
extern int fov;          // Field of view (for perspective)
extern double asp;       // Aspect ratio
extern char* views[];    // View mode for print
extern double EX, EY, EZ; // Camera position
extern double AX, AY, AZ; // Camera look-at position

// Lighting
extern int light;
extern int ambient;
extern int diffuse;
extern int emission;
extern int specular;
extern int light_distance;
extern int light_angle;
extern int light_height;
extern int light_move;

#ifdef __cplusplus
}
#endif

#endif
