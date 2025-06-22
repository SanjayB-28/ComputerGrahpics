#include "objects.h"

// ... (Move all object definition functions from hw3.c here)
// Do not redefine global texture variables, use extern as in objects.h 

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