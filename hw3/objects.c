#include "objects.h"
#include <math.h>

static void normal(float* a, float* b, float* c) {
    float u[3] = {b[0]-a[0], b[1]-a[1], b[2]-a[2]};
    float v[3] = {c[0]-a[0], c[1]-a[1], c[2]-a[2]};
    float n[3] = {
        u[1]*v[2] - u[2]*v[1],
        u[2]*v[0] - u[0]*v[2],
        u[0]*v[1] - u[1]*v[0]
    };
    float len = sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
    glNormal3f(n[0]/len, n[1]/len, n[2]/len);
}

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
  float emit[] = {0.0, 0.0, 0.0, 1.0};
  glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT,GL_SPECULAR, color);
  glMaterialfv(GL_FRONT,GL_EMISSION, emit);

  for (int ph=-90; ph<90; ph+=d) {
    glBegin(GL_QUAD_STRIP);
    for (int th=0; th<=360; th+=d) {
      double x_1 = Sin(th)*Cos(ph);
      double y_1 = Sin(ph);
      double z_1 = Cos(th)*Cos(ph);
      glNormal3d(x_1, y_1, z_1);
      glTexCoord2f(th/360.0, (ph+90)/180.0);
      glVertex3d(x_1, y_1, z_1);

      double x_2 = Sin(th)*Cos(ph+d);
      double y_2 = Sin(ph+d);
      double z_2 = Cos(th)*Cos(ph+d);
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

  // Base - individual triangles instead of triangle fan
  for (int th=0; th<360; th+=d) {
    glBegin(GL_TRIANGLES);
    glColor3f(1, 1, 1);
    float shininess[] = {0};
    float color[] = {1, 1, 1, 1.0};
    float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
    glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT,GL_SPECULAR, color);
    glMaterialfv(GL_FRONT,GL_EMISSION, emit);
    glNormal3d(0, -1, 0); // Base normal
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, 0, 0);
    glTexCoord2f(Sin(th)+0.5, Cos(th)+0.5);
    glVertex3f(Sin(th), 0, Cos(th));
    glTexCoord2f(Sin(th+d)+0.5, Cos(th+d)+0.5);
    glVertex3f(Sin(th+d), 0, Cos(th+d));
    glEnd();
  }

  // Side surface with correct normals
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

    glNormal3d(Sin(th), 0, Cos(th));
    glTexCoord2f(tc, 0);
    glVertex3f(Sin(th), 0, Cos(th));

    glNormal3d(Sin(th+d), 0, Cos(th+d));
    glTexCoord2f(tcd, 0);
    glVertex3f(Sin(th+d), 0, Cos(th+d));

    glNormal3d(Sin(th+d), 0, Cos(th+d));
    glTexCoord2f(tcd, 1);
    glVertex3f(Sin(th+d), 1, Cos(th+d));

    glNormal3d(Sin(th), 0, Cos(th));
    glTexCoord2f(tc, 1);
    glVertex3f(Sin(th), 1, Cos(th));
    glEnd();
  }

  // Top - individual triangles instead of triangle fan
  for (int th=0; th<360; th+=d) {
    glBegin(GL_TRIANGLES);
    glColor3f(1, 1, 1);
    float shininess[] = {0};
    float color[] = {1, 1, 1, 1.0};
    float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
    glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT,GL_SPECULAR, color);
    glMaterialfv(GL_FRONT,GL_EMISSION, emit);
    glNormal3d(0, 1, 0); // Top normal
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, 1, 0);
    glTexCoord2f(Cos(th)+0.5, Sin(th)+0.5);
    glVertex3f(Sin(th), 1, Cos(th));
    glTexCoord2f(Cos(th+d)+0.5, Sin(th+d)+0.5);
    glVertex3f(Sin(th+d), 1, Cos(th+d));
    glEnd();
  }
  glPopMatrix();
}

void Cone (double x, double y, double z, double radius, double height) {
  const int d=5;
  glPushMatrix();
  glTranslatef(x, y, z);
  glScalef(radius, height, radius);

  // Base - individual triangles instead of triangle fan
  for (int th=0; th<360; th+=d) {
    glBegin(GL_TRIANGLES);
    glColor3f(1, 1, 1);
    float shininess[] = {0};
    float color[] = {1, 1, 1, 1.0};
    float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
    glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT,GL_SPECULAR, color);
    glMaterialfv(GL_FRONT,GL_EMISSION, emit);
    glNormal3d(0, -1, 0);
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, 0, 0);
    glTexCoord2f(Sin(th)+0.5, Cos(th)+0.5);
    glVertex3f(Sin(th), 0, Cos(th));
    glTexCoord2f(Sin(th+d)+0.5, Cos(th+d)+0.5);
    glVertex3f(Sin(th+d), 0, Cos(th+d));
    glEnd();
  }

  // Side - individual triangles instead of triangle fan
  for (int th=0; th<360; th+=d) {
    glBegin(GL_TRIANGLES);
    glColor3f(1, 1, 1);
    float shininess[] = {0};
    float color[] = {1, 1, 1, 1.0};
    float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
    glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT,GL_SPECULAR, color);
    glMaterialfv(GL_FRONT,GL_EMISSION, emit);
    // Correct normal for cone side
    double nx = Sin(th);
    double ny = radius / height;
    double nz = Cos(th);
    double len = sqrt(nx*nx + ny*ny + nz*nz);
    nx /= len; ny /= len; nz /= len;
    glNormal3d(nx, ny, nz);
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, 1, 0);
    glTexCoord2f(Sin(th)+0.5, Cos(th)+0.5);
    glVertex3f(Sin(th), 0, Cos(th));
    glTexCoord2f(Sin(th+d)+0.5, Cos(th+d)+0.5);
    glVertex3f(Sin(th+d), 0, Cos(th+d));
    glEnd();
  }
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

  float shininess[] = {0};
  float color[] = {1, 1, 1, 1.0};
  float emit[]  = {0.0, 0.0, 0.01*emission, 1.0};
  glMaterialfv(GL_FRONT,GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT,GL_SPECULAR, color);
  glMaterialfv(GL_FRONT,GL_EMISSION, emit);

  // Front triangle
  float v0[3] = {-1, 0, +1};
  float v1[3] = {+1, 0, +1};
  float v2[3] = {0, +1, 0};
  glBegin(GL_TRIANGLES);
  normal(v0, v1, v2);
  glTexCoord2f(0, 0); glVertex3fv(v0);
  glTexCoord2f(1, 0); glVertex3fv(v1);
  glTexCoord2f(0.5, 1); glVertex3fv(v2);
  glEnd();

  // Back triangle
  float v3[3] = {-1, 0, -1};
  float v4[3] = {+1, 0, -1};
  float v5[3] = {0, +1, 0};
  glBegin(GL_TRIANGLES);
  normal(v3, v4, v5);
  glTexCoord2f(0, 0); glVertex3fv(v3);
  glTexCoord2f(1, 0); glVertex3fv(v4);
  glTexCoord2f(0.5, 1); glVertex3fv(v5);
  glEnd();

  // Right side - converted to triangles
  float v6[3] = {+1, 0, +1};
  float v7[3] = {+1, 0, -1};
  float v8[3] = {0, +1, 0};
  glBegin(GL_TRIANGLES);
  normal(v6, v7, v8);
  glTexCoord2f(0, 0); glVertex3fv(v6);
  glTexCoord2f(1, 0); glVertex3fv(v7);
  glTexCoord2f(0.5, 1); glVertex3fv(v8);
  glEnd();

  // Left side - converted to triangles
  float v9[3] = {-1, 0, +1};
  float v10[3] = {-1, 0, -1};
  float v11[3] = {0, +1, 0};
  glBegin(GL_TRIANGLES);
  normal(v9, v10, v11);
  glTexCoord2f(0, 0); glVertex3fv(v9);
  glTexCoord2f(1, 0); glVertex3fv(v10);
  glTexCoord2f(0.5, 1); glVertex3fv(v11);
  glEnd();

  glPopMatrix();
}

void Vertex(double th, double ph) {
    double x = Sin(th) * Cos(ph);
    double y = Sin(ph);
    double z = Cos(th) * Cos(ph);
    glNormal3d(-x, -y, -z);
    glVertex3d(x, y, z);
}

// Helper function for sphere cap normals
void CapVertex(double th, double ph) {
    double x = Sin(th) * Cos(ph);
    double y = Sin(ph);
    double z = Cos(th) * Cos(ph);
    glNormal3d(-x, -y, -z);
    glVertex3d(x, y, z);
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

    // Top cap - individual triangles with proper spherical normals
    for (int th = 0; th < 360; th += d) {
        glBegin(GL_TRIANGLES);
        if (textureID) glTexCoord2f(0.5, 0.0);
        CapVertex(0, -90);
        if (textureID) glTexCoord2f(th/360.0, 0.05);
        CapVertex(th, d - 90);
        if (textureID) glTexCoord2f((th+d)/360.0, 0.05);
        CapVertex(th + d, d - 90);
        glEnd();
    }

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

    // Bottom cap - individual triangles with proper spherical normals
    for (int th = 0; th < 360; th += d) {
        glBegin(GL_TRIANGLES);
        if (textureID) glTexCoord2f(0.5, 1.0);
        CapVertex(0, 90);
        if (textureID) glTexCoord2f(th/360.0, 0.95);
        CapVertex(th, 90 - d);
        if (textureID) glTexCoord2f((th+d)/360.0, 0.95);
        CapVertex(th + d, 90 - d);
        glEnd();
    }
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

// Utility function to compute and set a normalized normal from three points
static void setNormal(const float* a, const float* b, const float* c) {
    float ux = b[0] - a[0];
    float uy = b[1] - a[1];
    float uz = b[2] - a[2];
    float vx = c[0] - a[0];
    float vy = c[1] - a[1];
    float vz = c[2] - a[2];
    float nx = uy * vz - uz * vy;
    float ny = uz * vx - ux * vz;
    float nz = ux * vy - uy * vx;
    float len = sqrt(nx * nx + ny * ny + nz * nz);
    if (len > 0.0f)
        glNormal3f(nx / len, ny / len, nz / len);
}

// Procedural skyscraper (non-simple primitive, custom geometry)
void Skyscraper(double x, double y, double z, double width, double depth, double height, int levels, int sides) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Apply metal texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, metalTexture);

    float shininess[] = {64};
    float specular[] = {0.5, 0.5, 0.6, 1.0};
    float diffuse[] = {0.25, 0.3, 0.35, 1.0};
    float ambient[] = {0.1, 0.1, 0.1, 1.0};

    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

    float halfW = width / 2;
    float halfD = depth / 2;
    float levelHeight = height / levels;

    // Draw the flat base (before sides)
    float baseCenter[3] = {0, 0, 0};
    glColor3f(0.2f, 0.3f, 0.7f); // Blueish base
    glBegin(GL_POLYGON);
    for (int j = 0; j < sides; ++j) {
        float t = 2 * M_PI * j / sides;
        float x_ = halfW * cos(t);
        float z_ = halfD * sin(t);
        glNormal3f(0, -1, 0);
        glTexCoord2f(0.5f + 0.5f * cos(t), 0.5f + 0.5f * sin(t));
        glVertex3f(x_, 0.0f, z_);
        baseCenter[0] += x_;
        baseCenter[2] += z_;
    }
    glEnd();
    baseCenter[0] /= sides; baseCenter[1] = 0; baseCenter[2] /= sides;

    // Draw the sides with a color gradient
    for (int i = 0; i < levels; ++i) {
        float y1 = i * levelHeight;
        float y2 = (i + 1) * levelHeight;
        float bump = 0.02 * sin(i * 1.5); // slight extrusion for realism
        // Color gradient: blue at base, silver at top
        float tcol = (float)i / (levels-1);
        float r = 0.2f + 0.6f * tcol;
        float g = 0.3f + 0.6f * tcol;
        float b = 0.7f + 0.2f * tcol;
        glColor3f(r, g, b);

        glBegin(GL_QUADS);
        for (int j = 0; j < sides; ++j) {
            float t1 = 2 * M_PI * j / sides;
            float t2 = 2 * M_PI * (j + 1) / sides;

            float x1a = (halfW + bump) * cos(t1);
            float z1a = (halfD + bump) * sin(t1);
            float x2a = (halfW + bump) * cos(t2);
            float z2a = (halfD + bump) * sin(t2);

            float p1[] = {x1a, y1, z1a};
            float p2[] = {x2a, y1, z2a};
            float p3[] = {x2a, y2, z2a};
            float p4[] = {x1a, y2, z1a};

            setNormal(p1, p2, p3);
            glTexCoord2f((float)j/sides, (float)i/levels); glVertex3fv(p1);
            glTexCoord2f((float)(j+1)/sides, (float)i/levels); glVertex3fv(p2);
            glTexCoord2f((float)(j+1)/sides, (float)(i+1)/levels); glVertex3fv(p3);
            glTexCoord2f((float)j/sides, (float)(i+1)/levels); glVertex3fv(p4);
        }
        glEnd();
    }

    // Draw the roof (after sides)
    float roofCenter[3] = {0, height, 0};
    float roofBump = 0.02f * sin((levels - 1) * 1.5f);  // match last level bump
    glColor3f(0.8f, 0.8f, 0.9f); // Silver roof
    glBegin(GL_POLYGON);
    for (int j = sides-1; j >= 0; --j) {
        float t = 2 * M_PI * j / sides;
        float x_ = (halfW + roofBump) * cos(t);
        float z_ = (halfD + roofBump) * sin(t);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0.5f + 0.5f * cos(t), 0.5f + 0.5f * sin(t));
        glVertex3f(x_, height, z_);
        roofCenter[0] += x_;
        roofCenter[2] += z_;
    }
    glEnd();
    roofCenter[0] /= sides; roofCenter[2] /= sides;

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}