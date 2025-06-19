// CORRECTED INCLUDE PATH
#include "CSCIx229.h"
#include "engine/renderer.h"
#include "engine/camera.h"
#include "engine/math_utils.h"

// --- Global State ---
int g_win_width = 1280;
int g_win_height = 720;
int g_last_mouse_x = -1;
int g_last_mouse_y = -1;
float g_delta_time = 0.0f;
Camera g_camera;

// Input state buffer - declared here, used as `extern` in camera.c
unsigned char g_key_state[256] = {0};

// --- Callbacks ---

void display() {
    static int prev_t = 0;
    int t = glutGet(GLUT_ELAPSED_TIME);
    g_delta_time = (t - prev_t) / 1000.0f;
    prev_t = t;

    camera_update(&g_camera, g_delta_time);
    renderer_render_frame(&g_camera, g_win_width, g_win_height);

    glutSwapBuffers();
}

void reshape(int width, int height) {
    g_win_width = width;
    g_win_height = height;
}

void idle() {
    glutPostRedisplay();
}

void keyboard_down(unsigned char key, int x, int y) {
    if (key == 27) { // Escape key
        renderer_cleanup();
        exit(0);
    }
    g_key_state[key] = 1;
}

void keyboard_up(unsigned char key, int x, int y) {
    g_key_state[key] = 0;
}

void passive_mouse_motion(int x, int y) {
    if (g_last_mouse_x == -1) {
        // This is to prevent a large jump when the window gains focus
        g_last_mouse_x = x;
        g_last_mouse_y = y;
        return;
    }

    int dx = x - g_last_mouse_x;
    int dy = y - g_last_mouse_y;
    
    camera_handle_input(&g_camera, dx, dy);

    // Center cursor for infinite look-around
    if (x < 100 || x > g_win_width - 100 || y < 100 || y > g_win_height - 100) {
        glutWarpPointer(g_win_width / 2, g_win_height / 2);
        g_last_mouse_x = g_win_width / 2;
        g_last_mouse_y = g_win_height / 2;
    } else {
        g_last_mouse_x = x;
        g_last_mouse_y = y;
    }
}


// --- Main ---

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(g_win_width, g_win_height);
    glutCreateWindow("Interactive Ocean Ecosystem - Day 1");

#ifdef USEGLEW
    if (glewInit() != GLEW_OK) Fatal("Glew failed to initialize");
#endif

    renderer_init();
    camera_init(&g_camera);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard_down);
    glutKeyboardUpFunc(keyboard_up);
    glutPassiveMotionFunc(passive_mouse_motion);
    glutMotionFunc(passive_mouse_motion); // Handle mouse while button is pressed too

    glutSetCursor(GLUT_CURSOR_NONE); // Hide cursor

    ErrCheck("Initialization");
    glutMainLoop();

    renderer_cleanup();
    return 0;
}