#ifdef __APPLE__
#include <GLUT/glut.h>  // macOS OpenGL header
#else
#include <GL/glut.h>    // OpenGL header for other platforms
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define POINTS 50000  // Number of points for Lorenz Attractor

// Lorenz Attractor parameters
double x = 0.1, y = 0, z = 0;
double sigma = 10.0, beta = 2.6666, rho = 28.0;

// Camera/view controls
float rotationX = 0.0, rotationY = 0.0, zoom = -12.0, scale = 0.1f;
bool alternateColor = false;

// Render Lorenz Attractor
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Apply zoom and rotation
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

    // Plot points
    glBegin(GL_POINTS);
    for (int i = 0; i < POINTS; i++) {
        // Lorenz equations
        double dx = sigma * (y - x);
        double dy = x * (rho - z) - y;
        double dz = x * y - beta * z;
        x += dx * 0.001;
        y += dy * 0.001;
        z += dz * 0.001;

        // Color control
        float t = (float)i / POINTS;
        if (alternateColor) {
            glColor3f(sinf(t * M_PI), cosf(t * M_PI), 1.0f - t);
        } else {
            glColor3f(t, 0.0f, 1.0f - t);
        }

        // Plot point
        glVertex3f(x * scale, y * scale, z * scale);
    }
    glEnd();

    glutSwapBuffers();
}

// Handle window resizing
void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// Handle key input for controlling parameters and interaction
void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'q':  // Quit
        case 'Q':
        case 27:
            printf("Exiting program\n");
            exit(0);
            break;
        case 'c':  // Toggle color scheme
        case 'C':
            alternateColor = !alternateColor;
            printf("Color scheme toggled\n");
            break;
        case '+':  // Zoom in
        case '=':
            zoom += 0.5f;
            printf("Zoomed in: %.2f\n", zoom);
            break;
        case '-':  // Zoom out
        case '_':
            zoom -= 0.5f;
            printf("Zoomed out: %.2f\n", zoom);
            break;
        case 's':  // Increase sigma
            sigma += 0.1;
            printf("Sigma increased to %.2f\n", sigma);
            break;
        case 'S':  // Decrease sigma
            sigma -= 0.1;
            printf("Sigma decreased to %.2f\n", sigma);
            break;
        case 'b':  // Increase beta
            beta += 0.1;
            printf("Beta increased to %.4f\n", beta);
            break;
        case 'B':  // Decrease beta
            beta -= 0.1;
            printf("Beta decreased to %.4f\n", beta);
            break;
        case 'r':  // Increase rho
            rho += 0.1;
            printf("Rho increased to %.2f\n", rho);
            break;
        case 'R':  // Decrease rho
            rho -= 0.1;
            printf("Rho decreased to %.2f\n", rho);
            break;
    }
    glutPostRedisplay();
}

// Handle arrow keys for rotating the view
void specialKeys(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:
            rotationX += 5.0f;
            printf("Rotated up: %.2f\n", rotationX);
            break;
        case GLUT_KEY_DOWN:
            rotationX -= 5.0f;
            printf("Rotated down: %.2f\n", rotationX);
            break;
        case GLUT_KEY_LEFT:
            rotationY -= 5.0f;
            printf("Rotated left: %.2f\n", rotationY);
            break;
        case GLUT_KEY_RIGHT:
            rotationY += 5.0f;
            printf("Rotated right: %.2f\n", rotationY);
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Lorenz Attractor");

    glEnable(GL_DEPTH_TEST);  // Enable depth testing
    glPointSize(1.0f);        // Set point size

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    // Print control instructions
    printf("Lorenz Attractor Visualization\n");
    printf("Controls:\n");
    printf("  Arrow keys: Rotate the view\n");
    printf("  +/- : Zoom in/out\n");
    printf("  c : Toggle color scheme\n");
    printf("  s/S : Increase/decrease Sigma\n");
    printf("  b/B : Increase/decrease Beta\n");
    printf("  r/R : Increase/decrease Rho\n");
    printf("  q or ESC: Quit the program\n");

    glutMainLoop();
    return 0;
}
