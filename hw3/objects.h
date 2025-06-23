#ifndef OBJECTS_H
#define OBJECTS_H

#include "CSCIx229.h"

void Sun(double x, double y, double z, double radius);
void Cylinder(double x, double y, double z, double radius, double height);
void Cone(double x, double y, double z, double radius, double height);
void Cube(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az);
void Roof(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az);
void Vertex(double th, double ph);
void Sphere(double x, double y, double z, double radius, unsigned int textureID);
void House(double x, double y, double z, double dx, double dy, double dz);
void Tree(double x, double y, double z, double size);
void Cloud(double x, double y, double z, double size);

extern unsigned int leafTexture;
extern unsigned int woodTexture;
extern unsigned int brickTexture;
extern unsigned int roofTexture;
extern unsigned int sunTexture;
extern unsigned int cloudTexture;

#endif