#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

// OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

// Default resolution
// For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

// --- Global Variables ---
// Initial View Angles
const int DEF_TH = -5;
const int DEF_PH = 35;

int th;             // Azimuth of view angle
int ph;             // Elevation of view angle
int axes = 1;       // Display axes
int legend = 1;     // Display legend
double dim = 50.0;  // Dimension of the viewing box.

// Lorenz Parameters
double s;
double b;
double r;

/*
 * Convenience routine to output raster text
 */
#define LEN 8192   // Maximum length of text string
void Print(const char* format , ...)
{
    char    buf[LEN];
    char* ch=buf;
    va_list args;
    //  Turn the parameters into a character string
    va_start(args,format);
    vsnprintf(buf,LEN,format,args);
    va_end(args);
    //  Display the characters one at a time at the current raster position
    while (*ch)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 * Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

/*
 * Reset all parameters to their initial state
 */
void reset_parameters()
{
    th = DEF_TH;
    ph = DEF_PH;
    s = 10.0;
    b = 8.0/3.0;
    r = 28.0;
}

/*
 * This function is called by GLUT to display the scene
 */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    glRotatef(ph, 1, 0, 0);
    glRotatef(th, 0, 1, 0);

    // --- Draw Lorenz Attractor ---
    int i;
    double x = 1, y = 1, z = 1;
    double dt = 0.001;
    glTranslated(0, 0, -r);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < 50000; i++)
    {
        double dx = s * (y - x);
        double dy = x * (r - z) - y;
        double dz = x * y - b * z;
        x += dt * dx;
        y += dt * dy;
        z += dt * dz;
        float red_comp   = (x + 20.0f) / 40.0f;
        float green_comp = (y + 30.0f) / 60.0f;
        float blue_comp  = z / 50.0f;
        glColor3f(red_comp, green_comp, blue_comp);
        glVertex3d(x, y, z);
    }
    glEnd();

    // --- Draw Axes ---
    if (axes)
    {
        glLoadIdentity();
        glRotatef(ph, 1, 0, 0);
        glRotatef(th, 0, 1, 0);
        const double len = 30.0;
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0); glVertex3d(0.0, 0.0, 0.0); glVertex3d(len, 0.0, 0.0);
        glColor3f(0.0, 1.0, 0.0); glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, len, 0.0);
        glColor3f(0.0, 0.0, 1.0); glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 0.0, len);
        glEnd();
        glColor3f(1.0, 1.0, 1.0);
        glRasterPos3d(len, 0.0, 0.0); Print("X");
        glRasterPos3d(0.0, len, 0.0); Print("Y");
        glRasterPos3d(0.0, 0.0, len); Print("Z");
    }

    // --- Display On-Screen Text ---
    glDisable(GL_DEPTH_TEST);
    glColor3f(1.0, 1.0, 1.0);
    glWindowPos2i(5, 5);  Print("View Angle: %d, %d", th, ph);
    glWindowPos2i(5, 25); Print("Lorenz Parameters: s=%.2f b=%.2f r=%.2f", s, b, r);

    // --- Display Legend ---
    if (legend)
    {
        int height = glutGet(GLUT_WINDOW_HEIGHT);
        int y_pos = height - 20;
        glColor3f(0.8, 0.8, 0.8);
        glWindowPos2i(5, y_pos); Print("Controls"); y_pos -= 18;
        glWindowPos2i(5, y_pos); Print("--------"); y_pos -= 18;
        glWindowPos2i(5, y_pos); Print("h: Toggle Legend"); y_pos -= 18;
        glWindowPos2i(5, y_pos); Print("d: Reset All Parameters"); y_pos -= 18;
        glWindowPos2i(5, y_pos); Print("0: Reset View Angle"); y_pos -= 18;
        glWindowPos2i(5, y_pos); Print("a: Toggle Axes"); y_pos -= 18;
        glWindowPos2i(5, y_pos); Print("Arrows: Rotate View"); y_pos -= 18;
        glWindowPos2i(5, y_pos); Print("s/S: Change Sigma"); y_pos -= 18;
        glWindowPos2i(5, y_pos); Print("b/B: Change Beta"); y_pos -= 18;
        glWindowPos2i(5, y_pos); Print("r/R: Change Rho"); y_pos -= 18;
    }

    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
}

/*
 * GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y)
{
    if (key == GLUT_KEY_RIGHT) th += 5;
    else if (key == GLUT_KEY_LEFT) th -= 5;
    else if (key == GLUT_KEY_UP) ph += 5;
    else if (key == GLUT_KEY_DOWN) ph -= 5;
    th %= 360;
    ph %= 360;
    glutPostRedisplay();
}

/*
 * GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y)
{
    if (ch == 27)
        exit(0);
    else if (ch == 'h' || ch == 'H')
        legend = 1 - legend;
    // Reset only view angle
    else if (ch == '0')
    {
        th = DEF_TH;
        ph = DEF_PH;
    }
    // Reset all parameters
    else if (ch == 'd' || ch == 'D')
    {
        reset_parameters();
    }
    else if (ch == 'a' || ch == 'A')
        axes = 1 - axes;
    else if (ch == 's') s += 0.1;
    else if (ch == 'S') s -= 0.1;
    else if (ch == 'b') b += 0.1;
    else if (ch == 'B') b -= 0.1;
    else if (ch == 'r') r += 1.0;
    else if (ch == 'R') r -= 1.0;
    
    glutPostRedisplay();
}

/*
 * GLUT calls this routine when the window is resized
 */
void reshape(int width, int height)
{
    glViewport(0, 0, RES * width, RES * height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double asp = (height > 0) ? (double)width / height : 1;
    glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*
 * Start up GLUT
 */
int main(int argc, char* argv[])
{
    // Initialize all parameters to their default values
    reset_parameters();

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Lorenz Attractor - Sanjay Baskaran");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    glutMainLoop();
    return 0;
}