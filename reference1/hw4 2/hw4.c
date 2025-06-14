
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes = 1;
int th = 0;
int ph = 5;
int mode = 0;
double dim = 3;
double len = 2;
int n = 50000;
int current = 0;
double percent = 0;
double points[50000][3];
double colors[50000][3];
int fov = 60;
double asp = 1;
// Camera coordinates for first-person view
double EX = 0, EY = 1.5, EZ = 5;   
double AX = 0, AY = 1.5, AZ = 0;   

char* views[] = {"Orthogonal", "Perspective", "First Person"};


#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

#define LEN 8192  
void Print(const char* format, ...) {
    char buf[LEN];
    char* ch = buf;
    va_list args;
    va_start(args, format);
    vsnprintf(buf, LEN, format, args);
    va_end(args);
    while (*ch)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}


void Vertex(double th, double ph) {
    glColor3f(Cos(th) * Cos(th), Sin(ph) * Sin(ph), Sin(th) * Sin(th));
    glVertex3d(Sin(th) * Cos(ph), Sin(ph), Cos(th) * Cos(ph));
}


void Sphere(double x, double y, double z, double radius) {
    const int d = 5;
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, radius, radius);

    glBegin(GL_TRIANGLE_FAN);
    Vertex(0, -90);
    for (int th = 0; th <= 360; th += d) {
        Vertex(th, d - 90);
    }
    glEnd();

    for (int ph = d - 90; ph <= 90 - 2 * d; ph += d) {
        glBegin(GL_QUAD_STRIP);
        for (int th = 0; th <= 360; th += d) {
            Vertex(th, ph);
            Vertex(th, ph + d);
        }
        glEnd();
    }

    glBegin(GL_TRIANGLE_FAN);
    Vertex(0, 90);
    for (int th = 0; th <= 360; th += d) {
        Vertex(th, 90 - d);
    }
    glEnd();
    glPopMatrix();
}


void Cylinder(double x, double y, double z, double radius, double height) {
    const int d = 5;
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, height, radius);

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    for (int th = 0; th <= 360; th += d) {
        glColor3f(0.4 * Cos(th), 0.4, 0.4 * Sin(th));
        glVertex3f(Sin(th), 0, Cos(th));
    }
    glEnd();

    for (int th = 0; th <= 360; th += d) {
        glBegin(GL_QUADS);
        glColor3f(0.4 * Cos(th), 0.4, 0.4 * Sin(th));
        glVertex3f(Sin(th), 0, Cos(th));
        glVertex3f(Sin(th), 1, Cos(th));
        glVertex3f(Sin(th + d), 1, Cos(th + d));
        glVertex3f(Sin(th + d), 0, Cos(th + d));
        glEnd();
    }

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 1, 0);
    for (int th = 0; th <= 360; th += d) {
        glColor3f(0.4 * Cos(th), 0.4, 0.4 * Sin(th));
        glVertex3f(Sin(th), 1, Cos(th));
    }
    glEnd();
    glPopMatrix();
}


void Cone(double x, double y, double z, double radius, double height) {
    const int d = 5;
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, height, radius);

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    for (int th = 0; th <= 360; th += d) {
        glColor3f(0.5 * Cos(th), 1, 0.5 * Sin(th));
        glVertex3f(Sin(th), 0, Cos(th));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 1, 0);
    for (int th = 0; th <= 360; th += d) {
        glColor3f(0.5 * Cos(th), 1, 0.5 * Sin(th));
        glVertex3f(Sin(th), 0, Cos(th));
    }
    glEnd();
    glPopMatrix();
}


void Cube(double x, double y, double z, double width, double height, double length, double angle, double ax, double ay, double az) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, ax, ay, az);
    glScalef(width, height, length);
    glColor3f(0, 0, 1);

    glBegin(GL_QUADS);
    glVertex3f(-1, 0, +1);
    glVertex3f(+1, 0, +1);
    glVertex3f(+1, +1, +1);
    glVertex3f(-1, +1, +1);
    glVertex3f(+1, 0, -1);
    glVertex3f(-1, 0, -1);
    glVertex3f(-1, +1, -1);
    glVertex3f(+1, +1, -1);
    glVertex3f(+1, 0, +1);
    glVertex3f(+1, 0, -1);
    glVertex3f(+1, +1, -1);
    glVertex3f(+1, +1, +1);
    glVertex3f(-1, 0, -1);
    glVertex3f(-1, 0, +1);
    glVertex3f(-1, +1, +1);
    glVertex3f(-1, +1, -1);
    glVertex3f(-1, +1, +1);
    glVertex3f(+1, +1, +1);
    glVertex3f(+1, +1, -1);
    glVertex3f(-1, +1, -1);
    glVertex3f(-1, 0, -1);
    glVertex3f(+1, 0, -1);
    glVertex3f(+1, 0, +1);
    glVertex3f(-1, 0, +1);
    glEnd();
    glPopMatrix();
}


void Roof(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, ax, ay, az);
    glScalef(width, height, depth);

    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1, 0, 1);
    glVertex3f(1, 0, 1);
    glVertex3f(0, 1, 0);
    glVertex3f(-1, 0, -1);
    glVertex3f(1, 0, -1);
    glVertex3f(0, 1, 0);
    glVertex3f(-1, 0, -1);
    glVertex3f(-1, 0, 1);
    glVertex3f(0, 1, 0);
    glVertex3f(1, 0, 1);
    glVertex3f(1, 0, -1);
    glVertex3f(0, 1, 0);
    glEnd();
    glPopMatrix();
}


void House(double x, double y, double z, double dx, double dy, double dz) {
    Cube(x, y, z, dx, dy, dz, 0, 0, 0, 0);
    Roof(x, y + dy, z, dx, dy / 2, dz, 0, 0, 0, 0);
}


void Tree(double x, double y, double z, double size) {
    
    Cylinder(x, y, z, 0.1 * size, 0.5 * size);
 
    Cone(x, y + 0.5 * size, z, 0.3 * size, 0.3 * size);
    Cone(x, y + 0.8 * size, z, 0.25 * size, 0.25 * size);
    Cone(x, y + 1.05 * size, z, 0.2 * size, 0.2 * size);
} 


void Star(double x, double y, double z, double radius) {
    Sphere(x, y, z, radius);
}


void projection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (mode == 0) {
        glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim);
    } else if (mode == 1 || mode == 2) {
        gluPerspective(fov, asp, dim / 4, 4 * dim);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();

    if (mode == 1) {
        double Ex = -2 * dim * Sin(th) * Cos(ph);
        double Ey = +2 * dim * Sin(ph);
        double Ez = +2 * dim * Cos(th) * Cos(ph);
        gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
    } else if (mode == 0) {
        glRotatef(ph, 1, 0, 0);
        glRotatef(th, 0, 1, 0);
    } else if (mode == 2) {
        gluLookAt(EX, EY, EZ, AX, AY, AZ, 0, 1, 0);
    }

    House(-1, 0, 0, 0.3, 0.2, 0.3);
    House(0, 0, 0, 0.3, 0.2, 0.3);
    House(1, 0, 0, 0.3, 0.2, 0.3);
    House(2, 0, 0, 0.3, 0.2, 0.3);
    House(-1, 0, 1, 0.3, 0.2, 0.3);
    House(1, 0, 1, 0.3, 0.2, 0.3);
    House(0, 0, 2, 0.3, 0.2, 0.3);


    Tree(-0.5, 0, -0.5, 0.5);
    Tree(0.5, 0, -0.5, 0.5);
    Tree(-1.5, 0, 1.5, 0.7);
    Tree(1.5, 0, 1.5, 0.7);
    Tree(0, 0, -1.5, 0.6);
    Tree(2, 0, -0.5, 0.5);

    glColor3f(1, 1, 1);  
    Star(-2.0, 2.5, -1.0, 0.05);  
    Star(1.5, 2.7, 0.5, 0.05);
    Star(-0.5, 2.9, -0.5, 0.05);
    Star(0.8, 2.8, 1.5, 0.05);
    Star(-1.2, 2.6, 1.0, 0.05);


    glWindowPos2i(5, 5);
    Print("View Angle=%d,%d", th, ph);

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
    }

    glWindowPos2i(5, 5);
    //Print("Mode=%s  Angle=%d,%d  Axes=%s  FOV=%d", views[mode], th, ph, axes ? "On" : "Off", fov);

    glFlush();
    glutSwapBuffers();
}

void special(int key, int x, int y) {
    if (key == GLUT_KEY_RIGHT) th += 5;
    else if (key == GLUT_KEY_LEFT) th -= 5;
    else if (key == GLUT_KEY_UP) ph += 5;
    else if (key == GLUT_KEY_DOWN) ph -= 5;
    th %= 360;
    ph %= 360;
    projection();
    glutPostRedisplay();
}

void key(unsigned char ch, int x, int y) {
    double step = 0.1;  // Step size for movement
    if (ch == 27) exit(0);  // Exit on ESC
    else if (ch == '0') th = ph = 0;  // Reset view angles
    else if (ch == 'm') mode = (mode + 1) % 3;  // Switch view mode
    else if (ch == 'w' || ch == 'W') {  // Move forward
        EX += step * Cos(th);
        EZ -= step * Sin(th);
    }
    else if (ch == 's' || ch == 'S') {  // Move backward
        EX -= step * Cos(th);
        EZ += step * Sin(th);
    }
    else if (ch == 'a' || ch == 'A') {  // Move left 
        EX -= step * Cos(th + 90);
        EZ += step * Sin(th + 90);
    }
    else if (ch == 'd' || ch == 'D') {  // Move right 
        EZ -= step * Sin(th + 90);
    }
    projection();
    glutPostRedisplay();
}



void reshape(int width, int height) {
    asp = (height > 0) ? (double)width / height : 1;
    glViewport(0, 0, width, height);
    projection();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Town - Varshitha Sathish");
#ifdef USEGLEW
    glewInit();
    #endif
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    projection();
    glutMainLoop();
    return 0;
}
