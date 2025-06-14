#include "CSCIx229.h"

// Draw a solid cube centered at (x,y,z) with half-extents (dx,dy,dz)
static void Cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);

    glBegin(GL_QUADS);
      glVertex3f(-1,-1,+1); glVertex3f(+1,-1,+1);
      glVertex3f(+1,+1,+1); glVertex3f(-1,+1,+1);
      glVertex3f(+1,-1,-1); glVertex3f(-1,-1,-1);
      glVertex3f(-1,+1,-1); glVertex3f(+1,+1,-1);
      glVertex3f(+1,-1,+1); glVertex3f(+1,-1,-1);
      glVertex3f(+1,+1,-1); glVertex3f(+1,+1,+1);
      glVertex3f(-1,-1,-1); glVertex3f(-1,-1,+1);
      glVertex3f(-1,+1,+1); glVertex3f(-1,+1,-1);
      glVertex3f(-1,+1,+1); glVertex3f(+1,+1,+1);
      glVertex3f(+1,+1,-1); glVertex3f(-1,+1,-1);
      glVertex3f(-1,-1,-1); glVertex3f(+1,-1,-1);
      glVertex3f(+1,-1,+1); glVertex3f(-1,-1,+1);
    glEnd();
    glPopMatrix();
}

// Draw a black wireframe outline for a cube
static void CubeOutline(double x,double y,double z,
                        double dx,double dy,double dz,
                        double th)
{
    glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.f,-1.f);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glColor3f(0,0,0);

    Cube(x,y,z,dx,dy,dz,th);

    glPopAttrib();
}

void drawDesk(double x,double y,double z,
              double dx,double dy,double dz,double th)
{
    glColor3f(0.30,0.20,0.10);
    Cube(x,y,z, dx,dy,dz, th);
    CubeOutline(x,y,z, dx,dy,dz, th);

    const double H=1.5, R=0.1;
    struct {double ox,oz;} leg[4]={{ dx*0.9, dz*0.9},{-dx*0.9, dz*0.9},
                                   { dx*0.9,-dz*0.9},{-dx*0.9,-dz*0.9}};
    glPushMatrix();
    glTranslated(x,y-dy,z);
    for(int i=0;i<4;i++)
    {
        glColor3f(0.20,0.20,0.20);
        Cube(leg[i].ox, -H/2, leg[i].oz, R,H/2,R,0);
        CubeOutline(leg[i].ox, -H/2, leg[i].oz, R,H/2,R,0);
    }
    glPopMatrix();
}

void drawMonitor(double x,double y,double z,double s,double th)
{
    glPushMatrix();
    glTranslated(x,y,z); glRotated(th,0,1,0); glScaled(s,s,s);

    glColor3f(0.1,0.1,0.1);
    Cube(0,1.0,0, 1.2,0.7,0.05,0);
    CubeOutline(0,1.0,0, 1.2,0.7,0.05,0);

    glColor3f(0.5,0.8,1.0);
    Cube(0,1.0,0.06, 1.1,0.6,0.01,0);

    glColor3f(0.3,0.3,0.3);
    Cube(0,0.3,-0.1, 0.1,0.3,0.05,0);
    CubeOutline(0,0.3,-0.1, 0.1,0.3,0.05,0);

    Cube(0,0,0, 0.5,0.05,0.3,0);
    CubeOutline(0,0,0, 0.5,0.05,0.3,0);

    glPopMatrix();
}

void drawMacBook(double x,double y,double z,
                 double s,double th,double angle)
{
    const float BW=1.20f, BD=0.80f, BH=0.05f;
    const float LW=BW,    LH=0.70f, LT=0.04f;

    glPushMatrix();
    glTranslated(x,y,z); glRotated(th,0,1,0); glScaled(s,s,s);

    glColor3f(0.20,0.20,0.20);
    Cube(0,BH*0.5,0, BW*0.5,BH*0.5,BD*0.5,0);
    CubeOutline(0,BH*0.5,0, BW*0.5,BH*0.5,BD*0.5,0);

    glColor3f(0.10,0.10,0.10);
    Cube(0,BH+0.005,0, BW*0.45,0.005,BD*0.40,0);

    glPushMatrix();
      glTranslatef(0,BH,-BD*0.5);
      glRotatef(-angle,1,0,0);
      glTranslatef(0,LH*0.5,-LT*0.5);

      glColor3f(0.20,0.20,0.20);
      Cube(0,0,0, LW*0.5,LH*0.5,LT*0.5,0);
      CubeOutline(0,0,0, LW*0.5,LH*0.5,LT*0.5,0);

      glColor3f(0.50,0.80,1.0);
      Cube(0,0, LT*0.5+0.001, LW*0.45,LH*0.45,0.002,0);
    glPopMatrix();
    glPopMatrix();
}

void drawKeyboard(double x,double y,double z,double scale,double th)
{
    glPushMatrix();
    glTranslated(x,y,z); glRotated(th,0,1,0); glScaled(scale,scale,scale);

    glColor3f(0.2,0.2,0.25);
    Cube(0,0,0, 1.5,0.1,0.5,0);
    CubeOutline(0,0,0, 1.5,0.1,0.5,0);

    glColor3f(0.4,0.4,0.45);
    Cube(0,0.11,0, 1.4,0.05,0.4,0);

    glColor3f(0.2,0.2,0.25); glLineWidth(1.0);
    glPushMatrix();
      glTranslatef(0,0.17,0); glScalef(1.4,1,0.4);
      glBegin(GL_LINES);
        for(int i=1;i<10;i++){ float kx=-1.0+i*0.2; glVertex3f(kx,0,1); glVertex3f(kx,0,-1);}
        for(int i=1;i<4;i++){  float kz=-1.0+i*0.5; glVertex3f(-1,0,kz); glVertex3f(1,0,kz);}
      glEnd();
    glPopMatrix();
    glPopMatrix();
}

// Draw mouse (no outline)
void drawMouse(double x,double y,double z,double scale,double th)
{
    glPushMatrix();
    glTranslated(x,y,z); glRotated(th,0,1,0); glScaled(scale,scale,scale);

    glColor3f(0.96,0.96,0.86);
    glBegin(GL_QUADS);
      glVertex3f(-0.2,0.0, 0.3); glVertex3f( 0.2,0.0, 0.3);
      glVertex3f( 0.2,0.0,-0.3); glVertex3f(-0.2,0.0,-0.3);
      glVertex3f(-0.2,0.0, 0.3); glVertex3f( 0.2,0.0, 0.3);
      glVertex3f( 0.2,0.2, 0.3); glVertex3f(-0.2,0.2, 0.3);
      glVertex3f(-0.2,0.0,-0.3); glVertex3f( 0.2,0.0,-0.3);
      glVertex3f( 0.2,0.1,-0.3); glVertex3f(-0.2,0.1,-0.3);
      glVertex3f(-0.2,0.0,-0.3); glVertex3f(-0.2,0.0, 0.3);
      glVertex3f(-0.2,0.2, 0.3); glVertex3f(-0.2,0.1,-0.3);
      glVertex3f( 0.2,0.0,-0.3); glVertex3f( 0.2,0.1,-0.3);
      glVertex3f( 0.2,0.2, 0.3); glVertex3f( 0.2,0.0, 0.3);
    glEnd();
    glBegin(GL_POLYGON);
      glVertex3f(-0.2,0.2, 0.3); glVertex3f( 0.2,0.2, 0.3);
      glVertex3f( 0.2,0.1,-0.3); glVertex3f(-0.2,0.1,-0.3);
    glEnd();
    glPopMatrix();
}

void drawMousePad(double x,double y,double z,double dx,double dz,double th)
{
    glColor3f(0.15,0.15,0.2);
    Cube(x,y,z, dx,0.02,dz, th);
    CubeOutline(x,y,z, dx,0.02,dz, th);
}

void drawNotebook(double x,double y,double z,
                  double dx,double dy,double dz,double th,
                  float r,float g,float b)
{
    glPushMatrix();
    glTranslated(x,y,z); glRotated(th,0,1,0);

    glColor3f(r,g,b);
    Cube(0,dy/2,0, dx/2,dy/2,dz/2,0);
    CubeOutline(0,dy/2,0, dx/2,dy/2,dz/2,0);

    glColor3f(0.9,0.9,0.9);
    Cube(0.05,dy/2,0, dx/2-0.05,dy/2-0.02,dz/2-0.02,0);
    glPopMatrix();
}

void drawFloor(double y,double sizeX,double sizeZ,
               double thick,
               float r,float g,float b)
{
    glColor3f(r,g,b);
    Cube(0, y - thick*0.5, 0, sizeX,thick*0.5,sizeZ,0);
    CubeOutline(0, y - thick*0.5, 0, sizeX,thick*0.5,sizeZ,0);
}
