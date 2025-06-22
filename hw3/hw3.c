#include "CSCIx229.h"
#include "objects.h"

int axes = 0;
int th = 30;
int ph = 30;
int fp_th = 0;
int fp_ph = 0;
int mode = 0;
double dim = 5.0;
double len = 2;
int n = 50000;
int fov = 55;
double asp=1;
char* views[] = {"Orthogonal", "Perspective", "First Person"};
double EX = 0, EY = 0, EZ = 10;
double AX = 0, AY = 0, AZ = 0;

int light = 1;
int ambient = 30;
int diffuse = 100;
int emission = 0;
int specular = 0;
int light_distance = 5;
int light_angle = 90;
int light_height = 3;
int light_move = 1;

unsigned int leafTexture;
unsigned int woodTexture;
unsigned int brickTexture;
unsigned int roofTexture;
unsigned int sunTexture;
unsigned int cloudTexture;

void projection() {
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void display () {
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glLoadIdentity();
  if (mode == 1){
    double Ex = -2*dim*Sin(th)*Cos(ph);
    double Ey = +2*dim        *Sin(ph);
    double Ez = +2*dim*Cos(th)*Cos(ph);
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
  }
  else if(mode == 0){
    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);
  }
    else if (mode == 2) {
        gluLookAt(EX, EY, EZ, AX, AY, AZ, 0, 1, 0);
    }

  if (light) {
float Ambient[] = {0.01 * ambient, 0.01 * ambient, 0.01 * ambient, 1.0};
float Diffuse[] = {0.01 * diffuse, 0.01 * diffuse, 0.01 * diffuse, 1.0};
float Specular[] = {0.01 * specular, 0.01 * specular, 0.01 * specular, 1.0};
float Position[] = {light_distance * Cos(light_angle), light_height, light_distance * Sin(light_angle), 1.0};

glColor3f(1, 1, 1);
Sun(Position[0], Position[1], Position[2], 0.5);

glEnable(GL_NORMALIZE);
glEnable(GL_LIGHTING);
glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
glEnable(GL_COLOR_MATERIAL);
glEnable(GL_LIGHT0);

glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
glLightfv(GL_LIGHT0, GL_POSITION, Position);
  }

  glPushMatrix();
  glColor3f(0, 0, 1);
  House(-1, 0, 0, 0.3, 0.2, 0.3);
    House(0, 0, 0, 0.36, 0.24, 0.36);
    House(1, 0, 0, 0.3, 0.2, 0.3);
    House(2, 0, 0, 0.35, 0.22, 0.35);
    House(-1, 0, 1, 0.3, 0.2, 0.3);
    House(1, 0, 1, 0.3, 0.2, 0.3);
    House(0, 0, 2, 0.3, 0.2, 0.3);
    House(2, 0, 2, 0.36, 0.24, 0.36);
    House(-2, 0, 1, 0.3, 0.2, 0.3);
    House(1.5, 0, -1, 0.34, 0.22, 0.34);

    Tree(-0.5, 0, -0.5, 0.5);
    Tree(0.5, 0, -0.5, 0.6);
    Tree(-1.5, 0, 1.5, 0.7);
    Tree(1.5, 0, 1.5, 0.5);
    Tree(0, 0, -1.5, 0.6);
    Tree(2, 0, -0.5, 0.5);
    Tree(-2, 0, 0, 0.6);
    Tree(2.5, 0, 1.5, 0.5);
    Tree(-1, 0, 2, 0.55);
    Tree(1, 0, 2.5, 0.5);
    Tree(1, 0, 1.8, 0.6);

    Cloud(-2.0, 2.5, -1.0, 0.3);  
    Cloud(1.5, 2.7, 0.5, 0.25);
    Cloud(-0.5, 2.9, -0.5, 0.2);
    Cloud(0.8, 2.8, 1.5, 0.28);
    Cloud(-1.2, 2.6, 1.0, 0.22);
    Cloud(2.2, 2.4, -0.8, 0.25);
    Cloud(-2.5, 2.8, 1.8, 0.3);

  glPopMatrix();
  glDisable(GL_LIGHTING);
  glColor3f(1, 1, 1);
  if (axes) {
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(len, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, len, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, len);
    glEnd();
    glRasterPos3d(len, 0.0, 0.0);
    Print("X");
    glRasterPos3d(0.0, len, 0.0);
    Print("Y");
    glRasterPos3d(0.0, 0.0, len);
    Print("Z");
  }

  glWindowPos2i(5,5);
  if (mode == 2) {
    Print("Axes=%s  Dim=%.1f  Projection=%s  LightDist=%d  LightHeight=%d  LightMove=%s", axes ? "On" : "Off", dim, views[mode], light_distance, light_height, light_move ? "Move" : "Stop");
    glWindowPos2i(5,25);
    Print("FP Angle=%d,%d", fp_th, fp_ph);
  } else {
    Print("Axes=%s  Dim=%.1f  Projection=%s  LightDist=%d  LightHeight=%d  LightMove=%s", axes ? "On" : "Off", dim, views[mode], light_distance, light_height, light_move ? "Move" : "Stop");
    glWindowPos2i(5,25);
    Print("Angle=%d,%d", th, ph);
  }
  glFlush();
  glutSwapBuffers();
}

void idle () {
  double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
  light_angle = fmod(90*t,360);
  glutPostRedisplay();
}

void special(int key, int x, int y) {
  if (mode == 2) {
    if (key == GLUT_KEY_RIGHT)
      fp_th += 5;
    else if (key == GLUT_KEY_LEFT)
      fp_th -= 5;
    else if (key == GLUT_KEY_UP)
      fp_ph += 5;
    else if (key == GLUT_KEY_DOWN)
      fp_ph -= 5;
    fp_th %= 360;
    fp_ph %= 360;
    AX = EX + Sin(fp_th) * Cos(fp_ph);
    AY = EY + Sin(fp_ph);
    AZ = EZ - Cos(fp_th) * Cos(fp_ph);
  } else {
    if (key == GLUT_KEY_RIGHT)
      th += 5;
    else if (key == GLUT_KEY_LEFT)
      th -= 5;
    else if (key == GLUT_KEY_UP)
      ph += 5;
    else if (key == GLUT_KEY_DOWN)
      ph -= 5;
    th %= 360;
    ph %= 360;
  }
  projection();
  glutPostRedisplay();
}

void key(unsigned char ch, int x, int y) {
  double step = 0.1;  
  if (ch == 27) {
    exit(0);
  }
  else if (ch == '0') {
    th = ph = 30;
    fp_th = fp_ph = 0;
    axes = 0;
    fov = 55;
    dim = 5;
    light_move = 1;
    light_distance = 5;
    light_height = 3;
    EX = 0;
    EY = 0;
    EZ = 10;
    AX = 0;
    AY = 0;
    AZ = 0;
  }
  else if (ch == 'p' || ch == 'P') {
    mode = (mode + 1) % 3;
  }
 else if (ch == 'w' || ch == 'W') {  
        if (mode == 2) {
            EX += step * Sin(fp_th);
            EZ -= step * Cos(fp_th);
            AX = EX + Sin(fp_th);  
            AZ = EZ - Cos(fp_th);
        }
    }
    else if (ch == 's' || ch == 'S') {  
        if (mode == 2) {
            EX -= step * Sin(fp_th);
            EZ += step * Cos(fp_th);
            AX = EX + Sin(fp_th);  
            AZ = EZ - Cos(fp_th);
        }
    }
    else if (ch == 'a' || ch == 'A') {  
        if (mode == 2) {
            EX -= step * Cos(fp_th);
            EZ -= step * Sin(fp_th);
            AX = EX + Sin(fp_th);  
            AZ = EZ - Cos(fp_th);
        }
    }
    else if (ch == 'd' || ch == 'D') {  
        if (mode == 2) {
            EX += step * Cos(fp_th);
            EZ += step * Sin(fp_th);
            AX = EX + Sin(fp_th); 
            AZ = EZ - Cos(fp_th);
        }
    }
  else if (ch == 'r' || ch == 'R') {
    light_move = 1-light_move;
  }
else if (ch == 'm' || ch == 'M') {
    light_distance++;
}
else if (ch == 'n' || ch == 'n') {
    if (light_distance > 0)
        light_distance--;
}
else if (ch == 'k' || ch == 'K') {
    light_height++;
}
else if (ch == 'j' || ch == 'J') {
    light_height--;
}
  if (ch == 'x' || ch == 'X') {
    axes = !axes;
  }
  projection();
  if (light_move) {
    glutIdleFunc(idle);
  } else {
    glutIdleFunc(NULL);
  }
  glutPostRedisplay();
}
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

void reshape(int width,int height) {
  asp = (height>0) ? (double)width/height : 1;
  glViewport(0,0, width,height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0,0, width,height);
  projection();
}

int main (int argc, char *argv[]) {
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Homework 3: Sanjay Baskaran");
  glClearColor(0.043f, 0.114f, 0.318f, 1.0f);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);

  leafTexture = LoadTexBMP("textures/leaf.bmp");
  woodTexture = LoadTexBMP("textures/wood.bmp");
  brickTexture = LoadTexBMP("textures/brick.bmp");
  roofTexture = LoadTexBMP("textures/roof.bmp");
  sunTexture = LoadTexBMP("textures/sun.bmp");
  cloudTexture = LoadTexBMP("textures/cloud.bmp");

  glutMainLoop();
  return 0;
}
