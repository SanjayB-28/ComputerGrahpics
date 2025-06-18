#include "CSCIx229.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void createCustomObject();
void createVase();

int axes=1; 
int mode=1;       
int th=0;         
int ph=0;         
int fov=55;       
double asp=1;     
double dim=5.0;   
int lightMove = 1;

int light     =   1;  
int distance  =   5;  
int smooth    =   1;  
int local     =   0;  
int emission  =   0;  
int ambient   =  30;  
int diffuse   = 100;  
int specular  =  30;  
int shininess =   0;  
float shiny   =   1;  
int zh        =  90;  
float ylight  =   0;  

typedef struct {float x,y,z;} VertexPosition;
typedef struct {float nx,ny,nz;} Normal;
#define MAX_VERTICES 20000
#define MAX_FACES 40000
VertexPosition vertices[MAX_VERTICES];
Normal normals[MAX_VERTICES];
int faces[MAX_FACES][3];
int vertexCount = 0;
int faceCount = 0;

void createCustomObject() {
    int majorSegments = 40;
    int minorSegments = 20;
    float majorRadius = 1.0;
    float minorRadius = 0.3;
    float twistFactor = 0.5;

    int startVertex = vertexCount;

    for (int i = 0; i <= majorSegments; i++) {
        float theta = i * 2.0 * M_PI / majorSegments;
        for (int j = 0; j <= minorSegments; j++) {
            float phi = j * 2.0 * M_PI / minorSegments;
            float twist = twistFactor * theta;

            float x = (majorRadius + minorRadius * cos(phi)) * cos(theta);
            float y = (majorRadius + minorRadius * cos(phi)) * sin(theta);
            float z = minorRadius * sin(phi) + twist;

            vertices[vertexCount].x = x;
            vertices[vertexCount].y = y;
            vertices[vertexCount].z = z;

            float nx = cos(theta) * cos(phi);
            float ny = sin(theta) * cos(phi);
            float nz = sin(phi);
            float length = sqrt(nx*nx + ny*ny + nz*nz);

            normals[vertexCount].nx = nx / length;
            normals[vertexCount].ny = ny / length;
            normals[vertexCount].nz = nz / length;

            vertexCount++;

            if (i < majorSegments && j < minorSegments) {
                int current = startVertex + i * (minorSegments + 1) + j;
                int next = current + minorSegments + 1;

                faces[faceCount][0] = current;
                faces[faceCount][1] = next;
                faces[faceCount][2] = current + 1;
                faceCount++;

                faces[faceCount][0] = next;
                faces[faceCount][1] = next + 1;
                faces[faceCount][2] = current + 1;
                faceCount++;
            }
        }
    }
}

void createVase()
{
    int slices = 30;
    int stacks = 30;
    float height = 2.0f;
    float maxRadius = 0.5f;

    int startVertex = vertexCount;

    for (int i = 0; i <= stacks; ++i) {
        float y = height * ((float)i / stacks - 0.5f);
        float stackRadius = maxRadius * (1.0f - 0.5f * sin(M_PI * i / stacks));

        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * M_PI * (float)j / slices;
            float x = stackRadius * cos(theta);
            float z = stackRadius * sin(theta);

            vertices[vertexCount].x = x;
            vertices[vertexCount].y = y;
            vertices[vertexCount].z = z;

            float nx = x / stackRadius;
            float ny = (0.5f * M_PI * cos(M_PI * i / stacks)) / height;
            float nz = z / stackRadius;
            float length = sqrt(nx*nx + ny*ny + nz*nz);

            normals[vertexCount].nx = nx / length;
            normals[vertexCount].ny = ny / length;
            normals[vertexCount].nz = nz / length;

            vertexCount++;

            if (i < stacks && j < slices) {
                int current = startVertex + i * (slices + 1) + j;
                int next = current + slices + 1;

                faces[faceCount][0] = current;
                faces[faceCount][1] = next;
                faces[faceCount][2] = current + 1;
                faceCount++;

                faces[faceCount][0] = next;
                faces[faceCount][1] = next + 1;
                faces[faceCount][2] = current + 1;
                faceCount++;
            }
        }
    }
}

void drawCustomObject() {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < faceCount / 2; i++) {
        for (int j = 0; j < 3; j++) {
            int index = faces[i][j];
            glNormal3f(normals[index].nx, normals[index].ny, normals[index].nz);
            glVertex3f(vertices[index].x, vertices[index].y, vertices[index].z);
        }
    }
    glEnd();
}

void drawVase() {
    glBegin(GL_TRIANGLES);
    for (int i = faceCount / 2; i < faceCount; i++) {
        for (int j = 0; j < 3; j++) {
            int index = faces[i][j];
            glNormal3f(normals[index].nx, normals[index].ny, normals[index].nz);
            glVertex3f(vertices[index].x, vertices[index].y, vertices[index].z);
        }
    }
    glEnd();
}

void display()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
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
      float Ambient[]  = {0.01f*ambient, 0.01f*ambient, 0.01f*ambient, 1.0f};
      float Diffuse[]  = {0.01f*diffuse, 0.01f*diffuse, 0.01f*diffuse, 1.0f};
      float Specular[] = {0.01f*specular, 0.01f*specular, 0.01f*specular, 1.0f};
      float Position[] = {distance*Cos(zh), ylight, distance*Sin(zh), 1.0};

      glEnable(GL_NORMALIZE);
      glEnable(GL_LIGHTING);

      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);

      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
      glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
      glLightfv(GL_LIGHT0, GL_POSITION, Position);

      glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
      glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
      glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);

      float Emission[] = {0.01f*emission, 0.01f*emission, 0.01f*emission, 1.0f};
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
   }
   else
      glDisable(GL_LIGHTING);

   // Drawing light position as ball (no lighting here)
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   glPushMatrix();
   glTranslated(distance*Cos(zh),ylight,distance*Sin(zh));
   glutSolidSphere(0.05, 10, 10);
   glPopMatrix();
   if (light) glEnable(GL_LIGHTING);

   // Drawing custom object (twisted tube)
   glPushMatrix();
   glTranslatef(-1.0, 0, 0);
   glRotatef(90, 1, 0, 0);
   glScalef(0.3, 0.3, 0.3);
   glColor3f(0.5, 0.5, 1.0);
   drawCustomObject();
   glPopMatrix();

   // Drawing vase
   glPushMatrix();
   glTranslatef(1.0, 0, 0);
   glScalef(0.5, 0.5, 0.5);
   glColor3f(1.0, 0.5, 0.5);
   drawVase();
   glPopMatrix();

   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
      const double len=2.0;
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
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%d",
        ambient,diffuse,specular,emission, shininess);
   }

   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

void idle()
{
   if (lightMove)
   {
      double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
      zh = fmod(90*t, 360.0);
   }
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
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   th %= 360;
   ph %= 360;
   Project(mode?fov:0,asp,dim);
   glutPostRedisplay();
}

void key(unsigned char ch, int x, int y)
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
      lightMove = 1 - lightMove;
      glutIdleFunc(lightMove ? idle : NULL);
   }
   else if (ch == '>')
      zh -= 1;
   else if (ch == '<')
      zh += 1;
   else if (ch == '-')
      fov = fov > 1 ? fov-1 : 1;
   else if (ch == '+')
      fov = fov < 179 ? fov+1 : 179;
   else if (ch == '[')
      ylight -= 0.1;
   else if (ch == ']')
      ylight += 0.1;
   else if (ch == 'a' && ambient > 0)
      ambient -= 5;
   else if (ch == 'A' && ambient < 100)
      ambient += 5;
   else if (ch == 'd' && diffuse > 0)
      diffuse -= 5;
   else if (ch == 'D' && diffuse < 100)
      diffuse += 5;
   else if (ch == 's' && specular > 0)
      specular -= 5;
   else if (ch == 'S' && specular < 100)
      specular += 5;
   else if (ch == 'e' && emission > 0)
      emission -= 5;
   else if (ch == 'E' && emission < 100)
      emission += 5;
   else if (ch == 'n' && shininess > 0)
      shininess -= 1;
   else if (ch == 'N' && shininess < 128)
      shininess += 1;
   else if (ch == 'z')
      dim += 0.1;
   else if (ch == 'Z')
      dim = dim > 0.1 ? dim-0.1 : 0.1;
   else if (ch == 'r' || ch == 'R')
   {
      th = ph = 0;
      dim = 5.0;
      fov = 55;
      light = 1;
      distance = 5;
      ambient = 30;
      diffuse = 100;
      specular = 30;
      emission = 0;
      shininess = 0;
      ylight = 0;
      smooth = 1;
      local = 0;
      zh = 90;
      lightMove = 1;
      glutIdleFunc(idle);
   }

   Project(mode ? fov : 0, asp, dim);
   glutPostRedisplay();
}

void reshape(int width, int height)
{
    asp = (height>0) ? (double)width/height : 1;
    glViewport(0,0, RES*width,RES*height);
    Project(55, asp, 3.5);
}


int main(int argc,char* argv[])
{
   glutInit(&argc,argv);
   glutInitWindowSize(400,400);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutCreateWindow("Manojdeep Homwework 5");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);

   // Initializing light position
   zh = 90;
   ylight = 0.0;
   
   createCustomObject();
   createVase();
   
   ErrCheck("init");
   glutMainLoop();
   return 0;
}