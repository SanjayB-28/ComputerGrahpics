#include "CSCIx229.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#define TEXTURE_SIZE 256

int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world

// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light

unsigned int texture;  // Texture name

typedef struct {float x,y,z;} Point;
typedef struct {float x,y,z;} Normal;
#define MAX_VERTICES 50000
Point vertices[MAX_VERTICES];
Normal normals[MAX_VERTICES];
float texcoords[MAX_VERTICES][2];
int vertexCount = 0;

void drawSphere(double x, double y, double z, double r);

static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

GLuint createSimpleTexture()
{
    GLuint textureID;
    unsigned char* data = (unsigned char*)malloc(TEXTURE_SIZE * TEXTURE_SIZE * 3);

    for(int y = 0; y < TEXTURE_SIZE; y++)
    {
        for(int x = 0; x < TEXTURE_SIZE; x++)
        {
            float value = sin((float)(x + y) / TEXTURE_SIZE * 20.0) * 0.5 + 0.5;
            unsigned char color = (unsigned char)(255 * value);
            int index = (y * TEXTURE_SIZE + x) * 3;
            data[index] = data[index+1] = data[index+2] = color;
        }
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    free(data);
    return textureID;
}

void createSpiralTube()
{
    int segments = 200;
    int sides = 20;
    float radius = 1.0;
    float tubeRadius = 0.3;
    float height = 3.0;
    float coils = 3.0;

    vertexCount = 0;

    // Create the main body of the tube
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        float theta = 2.0 * M_PI * coils * t;
        float y = height * (t - 0.5);
        float centerX = radius * cos(theta);
        float centerZ = radius * sin(theta);

        for (int j = 0; j <= sides; j++) {
            float phi = 2.0 * M_PI * j / sides;
            float x = centerX + tubeRadius * cos(phi) * cos(theta);
            float z = centerZ + tubeRadius * cos(phi) * sin(theta);
            float y_offset = tubeRadius * sin(phi);

            vertices[vertexCount].x = x;
            vertices[vertexCount].y = y + y_offset;
            vertices[vertexCount].z = z;

            float nx = x - centerX;
            float ny = y_offset;
            float nz = z - centerZ;
            float len = sqrt(nx*nx + ny*ny + nz*nz);
            normals[vertexCount].x = nx / len;
            normals[vertexCount].y = ny / len;
            normals[vertexCount].z = nz / len;

            texcoords[vertexCount][0] = (float)i / segments * coils;
            texcoords[vertexCount][1] = (float)j / sides;

            vertexCount++;
        }
    }

    // Create end caps
    for (int end = 0; end < 2; end++) {
        float t = end == 0 ? 0 : 1;
        float theta = 2.0 * M_PI * coils * t;
        float y = height * (t - 0.5);
        float centerX = radius * cos(theta);
        float centerZ = radius * sin(theta);

        float normalY = end == 0 ? -1 : 1;

        // Center vertex of the cap
        vertices[vertexCount].x = centerX;
        vertices[vertexCount].y = y;
        vertices[vertexCount].z = centerZ;
        normals[vertexCount].x = 0;
        normals[vertexCount].y = normalY;
        normals[vertexCount].z = 0;
        texcoords[vertexCount][0] = t * coils;
        texcoords[vertexCount][1] = 0.5;
        vertexCount++;

        // Create vertices for the cap edge
        for (int j = 0; j <= sides; j++) {
            float phi = 2.0 * M_PI * j / sides;
            float cosPhiCosTheta = cos(phi) * cos(theta);
            float cosPhiSinTheta = cos(phi) * sin(theta);
            float sinPhi = sin(phi);

            float x = centerX + tubeRadius * cosPhiCosTheta;
            float z = centerZ + tubeRadius * cosPhiSinTheta;
            float y_offset = tubeRadius * sinPhi;

            vertices[vertexCount].x = x;
            vertices[vertexCount].y = y + y_offset;
            vertices[vertexCount].z = z;

            // Calculate normals that blend the cap normal with the tube body normal
            float blendFactor = 0.7;
            normals[vertexCount].x = (1 - blendFactor) * cosPhiCosTheta;
            normals[vertexCount].y = blendFactor * normalY + (1 - blendFactor) * sinPhi;
            normals[vertexCount].z = (1 - blendFactor) * cosPhiSinTheta;

            // Normalize the normal vector
            float len = sqrt(normals[vertexCount].x * normals[vertexCount].x +
                             normals[vertexCount].y * normals[vertexCount].y +
                             normals[vertexCount].z * normals[vertexCount].z);
            normals[vertexCount].x /= len;
            normals[vertexCount].y /= len;
            normals[vertexCount].z /= len;

            texcoords[vertexCount][0] = t * coils;
            texcoords[vertexCount][1] = (float)j / sides;
            vertexCount++;
        }
    }
}

void drawSphere(double x, double y, double z, double r)
{
   const int d=15;
   glPushMatrix();
   glTranslated(x,y,z);
   glScaled(r,r,r);

   for (int ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   glPopMatrix();
}

void display()
{
   const double len=2.0;
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);
   glLoadIdentity();
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   if (light)
   {
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      glColor3f(1,1,1);
      drawSphere(Position[0],Position[1],Position[2] , 0.1);
      glEnable(GL_NORMALIZE);
      glEnable(GL_LIGHTING);
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ambient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Diffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Specular);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   }
   else
      glDisable(GL_LIGHTING);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texture);
   glColor3f(1,1,1);
    
   int sides = 20;
   int segments = 200;

   glBegin(GL_TRIANGLES);
   for (int i = 0; i < segments; i++) {
       for (int j = 0; j < sides; j++) {
           int idx1 = i * (sides + 1) + j;
           int idx2 = i * (sides + 1) + (j + 1);
           int idx3 = (i + 1) * (sides + 1) + j;
           int idx4 = (i + 1) * (sides + 1) + (j + 1);

           // First triangle
           glNormal3f(normals[idx1].x, normals[idx1].y, normals[idx1].z);
           glTexCoord2fv(texcoords[idx1]);
           glVertex3f(vertices[idx1].x, vertices[idx1].y, vertices[idx1].z);

           glNormal3f(normals[idx2].x, normals[idx2].y, normals[idx2].z);
           glTexCoord2fv(texcoords[idx2]);
           glVertex3f(vertices[idx2].x, vertices[idx2].y, vertices[idx2].z);

           glNormal3f(normals[idx3].x, normals[idx3].y, normals[idx3].z);
           glTexCoord2fv(texcoords[idx3]);
           glVertex3f(vertices[idx3].x, vertices[idx3].y, vertices[idx3].z);

           // Second triangle
           glNormal3f(normals[idx2].x, normals[idx2].y, normals[idx2].z);
           glTexCoord2fv(texcoords[idx2]);
           glVertex3f(vertices[idx2].x, vertices[idx2].y, vertices[idx2].z);

           glNormal3f(normals[idx4].x, normals[idx4].y, normals[idx4].z);
           glTexCoord2fv(texcoords[idx4]);
           glVertex3f(vertices[idx4].x, vertices[idx4].y, vertices[idx4].z);

           glNormal3f(normals[idx3].x, normals[idx3].y, normals[idx3].z);
           glTexCoord2fv(texcoords[idx3]);
           glVertex3f(vertices[idx3].x, vertices[idx3].y, vertices[idx3].z);
       }
   }

   // Render the end caps
    int baseIndex = segments * (sides + 1);
    for (int end = 0; end < 2; end++) {
        int centerIdx = baseIndex + end * (sides + 2);
        for (int j = 0; j < sides; j++) {
            int idx1 = centerIdx;
            int idx2 = centerIdx + j + 1;
            int idx3 = centerIdx + j + 2;

            glNormal3f(normals[idx1].x, normals[idx1].y, normals[idx1].z);
            glTexCoord2fv(texcoords[idx1]);
            glVertex3f(vertices[idx1].x, vertices[idx1].y, vertices[idx1].z);

            glNormal3f(normals[idx2].x, normals[idx2].y, normals[idx2].z);
            glTexCoord2fv(texcoords[idx2]);
            glVertex3f(vertices[idx2].x, vertices[idx2].y, vertices[idx2].z);

            glNormal3f(normals[idx3].x, normals[idx3].y, normals[idx3].z);
            glTexCoord2fv(texcoords[idx3]);
            glVertex3f(vertices[idx3].x, vertices[idx3].y, vertices[idx3].z);
        }
    }
    
   glEnd();

   glDisable(GL_TEXTURE_2D);
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
     th,ph,dim,fov,mode?"Perspective":"Orthogonal",light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",
        smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",
        ambient,diffuse,specular,emission,shiny);
   }

   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   glutPostRedisplay();
}

void special(int key,int x,int y)
{
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   else if (key == GLUT_KEY_UP)
      ph += 5;
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   th %= 360;
   ph %= 360;
   Project(mode?fov:0,asp,dim);
   glutPostRedisplay();
}

void key(unsigned char ch,int x,int y)
{
   if (ch == 27)
      exit(0);
   else if (ch == '0')
      th = ph = 0;
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
   else if (ch == 'm' || ch == 'M')
   {
      move = 1-move;
      glutIdleFunc(move ? idle : NULL);
   }
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   else if (ch == '-' && fov>1)
      fov--;
   else if (ch == '+' && fov<179)
      fov++;
   else if (ch == '[')
      ylight -= 0.1;
   else if (ch == ']')
      ylight += 0.1;
   else if (ch == 'a' && ambient>0)
      ambient -= 5;
   else if (ch == 'A' && ambient<100)
      ambient += 5;
   else if (ch == 'd' && diffuse>0)
      diffuse -= 5;
   else if (ch == 'D' && diffuse<100)
      diffuse += 5;
   else if (ch == 's' && specular>0)
      specular -= 5;
   else if (ch == 'S' && specular<100)
      specular += 5;
   else if (ch == 'e' && emission>0)
      emission -= 5;
   else if (ch == 'E' && emission<100)
      emission += 5;
   else if (ch == 'n' && shininess>-1)
      shininess -= 1;
   else if (ch == 'N' && shininess<7)
      shininess += 1;
   else if (ch == 'z')
      dim += 0.1;
   else if (ch == 'Z')
      dim = dim>0.1 ? dim-0.1 : 0.1;
   else if (ch == 'r' || ch == 'R')
   {
      th = ph = 0;
      zh = 90;
      fov = 55;
      emission = 0;
      ambient = 30;
      diffuse = 100;
      specular = 0;
      shininess = 0;
      shiny = 1;
      ylight = 0;
      axes = 1;
      light = 1;
      mode = 1;
      move = 1;
   }
   shiny = shininess<0 ? 0 : pow(2.0, shininess);
   Project(mode?fov:0, asp, dim);
   glutPostRedisplay();
}

void reshape(int width,int height)
{
   asp = (height>0) ? (double)width/height : 1;
   glViewport(0,0, width,height);
   Project(mode?fov:0,asp,dim);
}

int main(int argc,char* argv[])
{
   glutInit(&argc,argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Manojdeep Homework 6");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   texture = createSimpleTexture();
   createSpiralTube();
   ErrCheck("init");
   glutMainLoop();
   return 0;
}