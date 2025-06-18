#include "CSCIx229.h"

int axes = 0; // Off by default
int th = 30;
// Elevation of view angle
int ph = 30;
// First person view angles
int fp_th = 0;
int fp_ph = 0;
// Color mode
int mode = 0;
// Dimension of orthogonal box
double dim = 5.0;
// Length of axes
double len = 2;
// Maximum number
int n = 50000;
// Field of view (for perspective)
int fov = 55;
// Aspect ratio
double asp=1;
// View mode for print
char* views[] = {"Orthogonal", "Perspective", "First Person"};
// X-coordinate of camera position
double EX = 0;
// Y-coordinate of camera position
double EY = 0;
// Z-coordinate of camera position
double EZ = 10;
// X-coordinate of where the camera is looking
double AX = 0;
// Y-coordinate of where the camera is looking
double AY = 0;
// Z-coordinate of where the camera is looking
double AZ = 0;

// Lighting
int light = 1;
// Ambient light
int ambient = 30;
// Diffuse light
int diffuse = 100;
// Emission light
int emission = 0;
// Specular light
int specular = 0;
// Light distance
int light_distance = 5;
// Light angle
int light_angle = 90;
// Light height
int light_height = 3;
// Light move
int light_move = 1;


unsigned int leafTexture;
unsigned int woodTexture;
unsigned int brickTexture;
unsigned int roofTexture;
unsigned int sunTexture;
unsigned int cloudTexture;


/*
 *  Draw a sphere at (x,y,z) radius (r)
 */
void Sun (double x, double y, double z, double radius) {
  const int d=5;
  //  Save transformation
  glPushMatrix();
  //  Offset and scale
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

  //  Undo transformations
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
}



void Cylinder (double x, double y, double z, double radius, double height) {
  const int d=5;
  //  Save transformation
  glPushMatrix();
  //  Offset and scale
  glTranslatef(x, y, z);
  glScalef(radius, height, radius);

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0, 0, 0);
  glTexCoord2f(0.5, 0.5);
  for (int th=0; th<=360; th+=d) {
    // glColor3f(0.4*Cos(th), 0.4 , 0.4*Sin(th));
    glColor3f(1, 1, 1);
    glNormal3d(Sin(th), 0, Cos(th));
    glTexCoord2f(Sin(th)+0.5, Cos(th)+0.5);
    glVertex3f(Sin(th), 0, Cos(th));
  }
  glEnd();

  // Latitude bands
  for (int th=0; th<=360; th+=d) {
    glBegin(GL_QUADS);
    // glColor3f(0.4*Cos(th), 0.4 , 0.4*Sin(th));
    glColor3f(1, 1, 1);

    float shininess[] = {0};
    // float color[] = {0.4*Cos(th), 0.4, 0.4*Sin(th), 1.0};
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
    // glColor3f(0.4*Cos(th), 0.4 , 0.4*Sin(th));
    glColor3f(1, 1, 1);
    glTexCoord2f(Cos(th)+0.5, Sin(th)+0.5);
    glVertex3f(Sin(th), 1, Cos(th));
  }
  glEnd();
  //  Undo transformations
  glPopMatrix();
}

void Cone (double x, double y, double z, double radius, double height) {
  const int d=5;
  //  Save transformation
  glPushMatrix();
  //  Offset and scale
  glTranslatef(x, y, z);
  glScalef(radius, height, radius);

  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.5, 0.5);
  glVertex3f(0, 0, 0);

  for (int th=0; th<=360; th+=d) {
    // glColor3f(0.5*Cos(th), 1 , 0.5*Sin(th));
    glColor3f(1, 1, 1);

    float shininess[] = {0};
    // float color[] = {0.5*Cos(th), 1, 0.5*Sin(th), 1.0};
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
    // glColor3f(0.5*Cos(th), 1 , 0.5*Sin(th));
    glColor3f(1, 1, 1);

    float shininess[] = {0};
    // float color[] = {0.5*Cos(th), 1, 0.5*Sin(th), 1.0};
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

  //  Undo transformations
  glPopMatrix();
}


/*
 *  Draw a cube at (x,y,z)
 *  with height (h), length (l), width (w)
 *  at angle (angle) on x (ax), y (ay), or z (az)
 *
 */
void Cube(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az) {
  //  Save transformation
  glPushMatrix();
  //  Offset, scale
  glTranslatef(x, y, z);
  glRotatef(angle, ax, ay, az);
  glScalef(width, height, depth);
  // glColor3f(0, 0, 1);
  glColor3f(1, 1, 1);

  float shininess[] = {0};
  // float color[] = {0, 0, 1, 1.0};
  float color[] = {1, 1, 1, 1.0};
  float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
  glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT,GL_SPECULAR, color);
  glMaterialfv(GL_FRONT,GL_EMISSION, emit);


  //  Cube
  glBegin(GL_QUADS);
  glNormal3f( 0, 0,+1);
  glTexCoord2f(0, 0); glVertex3f(-1, 0,+1);
  glTexCoord2f(1, 0); glVertex3f(+1, 0,+1);
  glTexCoord2f(1, 1); glVertex3f(+1,+1,+1);
  glTexCoord2f(0, 1); glVertex3f(-1,+1,+1);
  //  Back
  glNormal3f( 0, 0,-1);
  glTexCoord2f(0, 0); glVertex3f(+1, 0,-1);
  glTexCoord2f(1, 0); glVertex3f(-1, 0,-1);
  glTexCoord2f(1, 1); glVertex3f(-1,+1,-1);
  glTexCoord2f(0, 1); glVertex3f(+1,+1,-1);
  //  Right
  glNormal3f(+1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(+1, 0,+1);
  glTexCoord2f(1, 0); glVertex3f(+1, 0,-1);
  glTexCoord2f(1, 1); glVertex3f(+1,+1,-1);
  glTexCoord2f(0, 1); glVertex3f(+1,+1,+1);
  //  Left
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-1, 0,-1);
  glTexCoord2f(1, 0); glVertex3f(-1, 0,+1);
  glTexCoord2f(1, 1); glVertex3f(-1,+1,+1);
  glTexCoord2f(0, 1); glVertex3f(-1,+1,-1);
  //  Top
  glNormal3f( 0,+1, 0);
  glTexCoord2f(0, 0); glVertex3f(-1,+1,+1);
  glTexCoord2f(1, 0); glVertex3f(+1,+1,+1);
  glTexCoord2f(1, 1); glVertex3f(+1,+1,-1);
  glTexCoord2f(0, 1); glVertex3f(-1,+1,-1);
  //  Bottom
  glNormal3f( 0,-1, 0);
  glTexCoord2f(0, 0); glVertex3f(-1, 0,-1);
  glTexCoord2f(1, 0); glVertex3f(+1, 0,-1);
  glTexCoord2f(1, 1); glVertex3f(+1, 0,+1);
  glTexCoord2f(0, 1); glVertex3f(-1, 0,+1);
  //  End
  glEnd();
  //  Undo transformations
  glPopMatrix();
}

void Roof(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az) {
  //  Save transformation
  glPushMatrix();
  //  Offset, scale
  glTranslatef(x, y, z);
  glRotatef(angle, ax, ay, az);
  glScalef(width, height, depth);

  // Set color or texture (if using textures, bind them here)
  glColor3f(1, 1, 1);

  // Front face (triangular)
  glBegin(GL_TRIANGLES);
  glNormal3f(0, 1, 0);  // Normal pointing up
  glTexCoord2f(0, 0); glVertex3f(-1, 0, +1);  // Left bottom of the roof
  glTexCoord2f(1, 0); glVertex3f(+1, 0, +1);  // Right bottom of the roof
  glTexCoord2f(0.5, 1); glVertex3f(0, +1, 0);  // Peak of the roof
  glEnd();

  // Back face (triangular)
  glBegin(GL_TRIANGLES);
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-1, 0, -1);  // Left bottom of the roof (back)
  glTexCoord2f(1, 0); glVertex3f(+1, 0, -1);  // Right bottom of the roof (back)
  glTexCoord2f(0.5, 1); glVertex3f(0, +1, 0);  // Peak of the roof
  glEnd();

  // Right side (rectangular)
  glBegin(GL_QUADS);
  glNormal3f(0, 0, 1);  // Normal pointing forward
  glTexCoord2f(0, 0); glVertex3f(+1, 0, +1);  // Front right bottom
  glTexCoord2f(1, 0); glVertex3f(+1, 0, -1);  // Back right bottom
  glTexCoord2f(1, 1); glVertex3f(0, +1, 0);  // Peak of the roof
  glTexCoord2f(0, 1); glVertex3f(0, +1, 0);  // Peak of the roof (redundant to close)
  glEnd();

  // Left side (rectangular)
  glBegin(GL_QUADS);
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(-1, 0, +1);  // Front left bottom
  glTexCoord2f(1, 0); glVertex3f(-1, 0, -1);  // Back left bottom
  glTexCoord2f(1, 1); glVertex3f(0, +1, 0);  // Peak of the roof
  glTexCoord2f(0, 1); glVertex3f(0, +1, 0);  // Peak of the roof (redundant to close)
  glEnd();

  //  Undo transformations
  glPopMatrix();
}
void Vertex(double th, double ph) {
    //glColor3f(Cos(th) * Cos(th), Sin(ph) * Sin(ph), Sin(th) * Sin(th));
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
    //Cylinder(x, y, z, radius/5, height);
    Cylinder(x, y, z, 0.1 * size, 0.5 * size);
    glBindTexture(GL_TEXTURE_2D, leafTexture);
    Cone(x, y + 0.5 * size, z, 0.3 * size, 0.3 * size); // Base cone
    Cone(x, y + 0.8 * size, z, 0.25 * size, 0.25 * size); // Middle cone
    Cone(x, y + 1.05 * size, z, 0.2 * size, 0.2 * size); // Top cone
    glDisable(GL_TEXTURE_2D);
}

void Cloud(double x, double y, double z, double size) {
    // Center sphere
    Sphere(x, y, z, size, cloudTexture);
    // Left sphere
    Sphere(x - 0.6 * size, y, z, 0.7 * size, cloudTexture);
    // Right sphere
    Sphere(x + 0.6 * size, y, z, 0.7 * size, cloudTexture);
}

void projection() {
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}
/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
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


/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y) {
  // Right arrow key - increase angle by 5 degrees
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

/*
 *  GLUT calls this routine when a key is pressed
 */
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
        } else {
            // No movement in other modes
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
// Increase light distance (orbiting radius)
else if (ch == 'j' || ch == 'J') {
    light_distance++;
}
// Decrease light distance (orbiting radius)
else if (ch == 'k' || ch == 'K') {
    if (light_distance > 0)
        light_distance--;
}
// Increase light height
else if (ch == 'u' || ch == 'U') {
    light_height++;
}
// Decrease light height
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
  //  Ratio of the width to the height of the window
  asp = (height>0) ? (double)width/height : 1;
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  //  Undo previous transformations
  glLoadIdentity();
  //  Switch to manipulating the model matrix
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Set projection
  projection();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main (int argc, char *argv[]) {

  // Initialize GLUT and process user parameters
  glutInit(&argc,argv);
  // Request double buffered, true color window with Z buffering
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  // Request 800 x 800 pixel window
  glutInitWindowSize(800, 800);
  // Create the window
  glutCreateWindow("Homework 3: Sanjay Baskaran");
  // Set background color to RGB(11,29,81)
  glClearColor(0.043f, 0.114f, 0.318f, 1.0f);
  // Tell GLUT to call "display" when the scene should be drawn
  glutDisplayFunc(display);
  // Tell GLUT to call "reshape" when the window is resized
  glutReshapeFunc(reshape);
  // Tell GLUT to call "special" when an arrow key is pressed
  glutSpecialFunc(special);
  // Tell GLUT to call "key" when a key is pressed
  glutKeyboardFunc(key);
  glutIdleFunc(idle);
  

  leafTexture = LoadTexBMP("textures/leaf.bmp");
  woodTexture = LoadTexBMP("textures/wood.bmp");
  brickTexture = LoadTexBMP("textures/brick.bmp");
  roofTexture = LoadTexBMP("textures/roof.bmp");
  sunTexture = LoadTexBMP("textures/sun.bmp");
  cloudTexture = LoadTexBMP("textures/cloud.bmp");

  glutMainLoop();
  // Return code

  return 0;
}



