#include "CSCIx229.h"

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

void Sun (double x, double y, double z, double radius) {
  const int d=5;
  glPushMatrix();
  glTranslatef(x, y, z);
  glScalef(radius, radius, radius);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, sunTexture);

  glColor3f(1, 1, 1);
  float shininess[] = {0};
  float color[] = {1, 1, 1, 1.0};
  float emit[] = {0.0, 0.0, 0.01*emission, 1.0};
  glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT,GL_SPECULAR, color);
  glMaterialfv(GL_FRONT,GL_EMISSION, emit);

  for (int ph=-90; ph<90; ph+=d) {
    glBegin(GL_QUAD_STRIP);
    for (int th=0; th<=360; th+=d) {
      double x_1 = Sin(th)*Cos(ph);
      double y_1 = Cos(th)*Cos(ph);
      double z_1 = Sin(ph);
      glNormal3d(x_1, y_1, z_1);
      glTexCoord2f(th/360.0, (ph+90)/180.0);
      glVertex3d(x_1, y_1, z_1);

      double x_2 = Sin(th)*Cos(ph+d);
      double y_2 = Cos(th)*Cos(ph+d);
      double z_2 = Sin(ph+d);
      glNormal3d(x_2, y_2, z_2);
      glTexCoord2f(th/360.0, (ph+d+90)/180.0);
      glVertex3d(x_2, y_2, z_2);
    }
    glEnd();
  }
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
}

void Cylinder (double x, double y, double z, double radius, double height) {
  const int d=5;
  glPushMatrix();
  glTranslatef(x, y, z);
  glScalef(radius, height, radius);

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0, 0, 0);
  glTexCoord2f(0.5, 0.5);
  for (int th=0; th<=360; th+=d) {
    glColor3f(1, 1, 1);
    glNormal3d(Sin(th), 0, Cos(th));
    glTexCoord2f(Sin(th)+0.5, Cos(th)+0.5);
    glVertex3f(Sin(th), 0, Cos(th));
  }
  glEnd();

  for (int th=0; th<=360; th+=d) {
    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);

    float shininess[] = {0};
    float color[] = {1, 1, 1, 1.0};
    float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
    glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT,GL_SPECULAR, color);
    glMaterialfv(GL_FRONT,GL_EMISSION, emit);

    float tc = th/(float) 360;
    float tcd = (th+d)/(float) 360;

    glTexCoord2f(tc, 0);
    glVertex3f(Sin(th), 0, Cos(th));

    glTexCoord2f(tc, 1);
    glVertex3f(Sin(th), 1, Cos(th));

    glTexCoord2f(tcd, 1);
    glVertex3f(Sin(th+d), 1, Cos(th+d));

    glTexCoord2f(tcd, 0);
    glVertex3f(Sin(th+d), 0, Cos(th+d));
    glEnd();
  }

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0, 1, 0);
  for (int th=0; th<=360; th+=d) {
    glColor3f(1, 1, 1);
    glTexCoord2f(Cos(th)+0.5, Sin(th)+0.5);
    glVertex3f(Sin(th), 1, Cos(th));
  }
  glEnd();
  glPopMatrix();
}

void Cone (double x, double y, double z, double radius, double height) {
  const int d=5;
  glPushMatrix();
  glTranslatef(x, y, z);
  glScalef(radius, height, radius);

  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.5, 0.5);
  glVertex3f(0, 0, 0);

  for (int th=0; th<=360; th+=d) {
    glColor3f(1, 1, 1);

    float shininess[] = {0};
    float color[] = {1, 1, 1, 1.0};
    float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
    glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT,GL_SPECULAR, color);
    glMaterialfv(GL_FRONT,GL_EMISSION, emit);

    glNormal3d(Sin(th), 0, Cos(th));
    glTexCoord2f(Sin(th)+0.5, Cos(th)+0.5);
    glVertex3f(Sin(th), 0, Cos(th));
  }
  glEnd();

  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.5, 0.5);
  glVertex3f(0, 1, 0);
  for (int th=0; th<=360; th+=d) {
    glColor3f(1, 1, 1);

    float shininess[] = {0};
    float color[] = {1, 1, 1, 1.0};
    float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
    glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT,GL_SPECULAR, color);
    glMaterialfv(GL_FRONT,GL_EMISSION, emit);

    glNormal3d(Sin(th), 0, Cos(th));
    glTexCoord2f(Sin(th)+0.5, Cos(th)+0.5);
    glVertex3f(Sin(th), 0, Cos(th));
  }
  glEnd();

  glPopMatrix();
}

void Cube(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az) {
  glPushMatrix();
  glTranslatef(x, y, z);
  glRotatef(angle, ax, ay, az);
  glScalef(width, height, depth);
  glColor3f(1, 1, 1);

  float shininess[] = {0};
  float color[] = {1, 1, 1, 1.0};
  float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
  glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT,GL_SPECULAR, color);
  glMaterialfv(GL_FRONT,GL_EMISSION, emit);

  glBegin(GL_QUADS);
  glNormal3f( 0, 0,+1);
  glTexCoord2f(0, 0); glVertex3f(-1, 0,+1);
  glTexCoord2f(1, 0); glVertex3f(+1, 0,+1);
  glTexCoord2f(1, 1); glVertex3f(+1,+1,+1);
  glTexCoord2f(0, 1); glVertex3f(-1,+1,+1);
  glNormal3f( 0, 0,-1);
  glTexCoord2f(0, 0); glVertex3f(+1, 0,-1);
  glTexCoord2f(1, 0); glVertex3f(-1, 0,-1);
  glTexCoord2f(1, 1); glVertex3f(-1,+1,-1);
  glTexCoord2f(0, 1); glVertex3f(+1,+1,-1);
  glNormal3f(+1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(+1, 0,+1);
  glTexCoord2f(1, 0); glVertex3f(+1, 0,-1);
  glTexCoord2f(1, 1); glVertex3f(+1,+1,-1);
  glTexCoord2f(0, 1); glVertex3f(+1,+1,+1);
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-1, 0,-1);
  glTexCoord2f(1, 0); glVertex3f(-1, 0,+1);
  glTexCoord2f(1, 1); glVertex3f(-1,+1,+1);
  glTexCoord2f(0, 1); glVertex3f(-1,+1,-1);
  glNormal3f( 0,+1, 0);
  glTexCoord2f(0, 0); glVertex3f(-1,+1,+1);
  glTexCoord2f(1, 0); glVertex3f(+1,+1,+1);
  glTexCoord2f(1, 1); glVertex3f(+1,+1,-1);
  glTexCoord2f(0, 1); glVertex3f(-1,+1,-1);
  glNormal3f( 0,-1, 0);
  glTexCoord2f(0, 0); glVertex3f(-1, 0,-1);
  glTexCoord2f(1, 0); glVertex3f(+1, 0,-1);
  glTexCoord2f(1, 1); glVertex3f(+1, 0,+1);
  glTexCoord2f(0, 1); glVertex3f(-1, 0,+1);
  glEnd();
  glPopMatrix();
}

void Roof(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az) {
  glPushMatrix();
  glTranslatef(x, y, z);
  glRotatef(angle, ax, ay, az);
  glScalef(width, height, depth);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLES);
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-1, 0, +1);
  glTexCoord2f(1, 0); glVertex3f(+1, 0, +1);
  glTexCoord2f(0.5, 1); glVertex3f(0, +1, 0);
  glEnd();

  glBegin(GL_TRIANGLES);
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-1, 0, -1);
  glTexCoord2f(1, 0); glVertex3f(+1, 0, -1);
  glTexCoord2f(0.5, 1); glVertex3f(0, +1, 0);
  glEnd();

  glBegin(GL_QUADS);
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(+1, 0, +1);
  glTexCoord2f(1, 0); glVertex3f(+1, 0, -1);
  glTexCoord2f(1, 1); glVertex3f(0, +1, 0);
  glTexCoord2f(0, 1); glVertex3f(0, +1, 0);
  glEnd();

  glBegin(GL_QUADS);
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(-1, 0, +1);
  glTexCoord2f(1, 0); glVertex3f(-1, 0, -1);
  glTexCoord2f(1, 1); glVertex3f(0, +1, 0);
  glTexCoord2f(0, 1); glVertex3f(0, +1, 0);
  glEnd();

  glPopMatrix();
}
void Vertex(double th, double ph) {
    glVertex3d(Sin(th) * Cos(ph), Sin(ph), Cos(th) * Cos(ph));
}

void Sphere(double x, double y, double z, double radius, unsigned int textureID) {
    const int d = 5;
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, radius, radius);

    if (textureID) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    glBegin(GL_TRIANGLE_FAN);
    if (textureID) glTexCoord2f(0.5, 0.0);
    Vertex(0, -90);
    for (int th = 0; th <= 360; th += d) {
        if (textureID) glTexCoord2f(th/360.0, 0.05);
        Vertex(th, d - 90);
    }
    glEnd();

    for (int ph = d - 90; ph <= 90 - 2 * d; ph += d) {
        glBegin(GL_QUAD_STRIP);
        for (int th = 0; th <= 360; th += d) {
            if (textureID) glTexCoord2f(th/360.0, (ph+90)/180.0);
            Vertex(th, ph);
            if (textureID) glTexCoord2f(th/360.0, (ph+d+90)/180.0);
            Vertex(th, ph + d);
        }
        glEnd();
    }

    glBegin(GL_TRIANGLE_FAN);
    if (textureID) glTexCoord2f(0.5, 1.0);
    Vertex(0, 90);
    for (int th = 0; th <= 360; th += d) {
        if (textureID) glTexCoord2f(th/360.0, 0.95);
        Vertex(th, 90 - d);
    }
    glEnd();
    glPopMatrix();
    if (textureID) glDisable(GL_TEXTURE_2D);
}

void House(double x, double y, double z, double dx, double dy, double dz) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, brickTexture);
    Cube(x, y, z, dx, dy, dz, 0, 0, 0, 0);
    glBindTexture(GL_TEXTURE_2D, roofTexture);
    Roof(x, y + dy, z,  dx, dy, dz, 0, 0, 0, 0); 
  glDisable(GL_TEXTURE_2D);
}

void Tree(double x, double y, double z, double size) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    Cylinder(x, y, z, 0.1 * size, 0.5 * size);
    glBindTexture(GL_TEXTURE_2D, leafTexture);
    Cone(x, y + 0.5 * size, z, 0.3 * size, 0.3 * size);
    Cone(x, y + 0.8 * size, z, 0.25 * size, 0.25 * size);
    Cone(x, y + 1.05 * size, z, 0.2 * size, 0.2 * size);
    glDisable(GL_TEXTURE_2D);
}

void Cloud(double x, double y, double z, double size) {
    Sphere(x, y, z, size, cloudTexture);
    Sphere(x - 0.6 * size, y, z, 0.7 * size, cloudTexture);
    Sphere(x + 0.6 * size, y, z, 0.7 * size, cloudTexture);
}

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
  else if (ch == 'm' || ch == 'M') {
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
else if (ch == 'j' || ch == 'J') {
    light_distance++;
}
else if (ch == 'k' || ch == 'K') {
    if (light_distance > 0)
        light_distance--;
}
else if (ch == 'u' || ch == 'U') {
    light_height++;
}
else if (ch == 'g' || ch == 'G') {
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
