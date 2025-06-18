#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define MAX_VERTICES 10000
#define MAX_INDICES 30000

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    Vertex vertices[MAX_VERTICES];
    int indices[MAX_INDICES];
    int num_vertices;
    int num_indices;
    float r, g, b; // Color
} Object;

// Global variables
float cameraX = 0.0f, cameraY = 3.0f, cameraZ = 10.0f;
float cameraAngleX = 20.0f, cameraAngleY = 0.0f;
int lastMouseX, lastMouseY;
int mouseDown = 0;

// Objects in the scene
Object windmill, house, tree, torus, ground;

void createWindmill() {
    windmill.num_vertices = 0;
    windmill.num_indices = 0;

    // Base (tower)
    float baseRadius = 0.2f;
    int baseSegments = 16;
    float baseHeight = 1.5f;

    for (int i = 0; i <= baseSegments; i++) {
        float theta = 2.0f * M_PI * (float)i / (float)baseSegments;
        float x = baseRadius * cos(theta);
        float z = baseRadius * sin(theta);
        windmill.vertices[windmill.num_vertices++] = (Vertex){x, 0, z};
        windmill.vertices[windmill.num_vertices++] = (Vertex){x, baseHeight, z};
    }

    for (int i = 0; i < baseSegments; i++) {
        int bottomLeft = 2 * i;
        int bottomRight = 2 * i + 2;
        int topLeft = 2 * i + 1;
        int topRight = 2 * i + 3;
        windmill.indices[windmill.num_indices++] = bottomLeft;
        windmill.indices[windmill.num_indices++] = bottomRight;
        windmill.indices[windmill.num_indices++] = topRight;
        windmill.indices[windmill.num_indices++] = bottomLeft;
        windmill.indices[windmill.num_indices++] = topRight;
        windmill.indices[windmill.num_indices++] = topLeft;
    }

    // Blades
    float bladeLength = 1.2f;
    float bladeWidth = 0.1f;
    float bladeHeight = 0.02f;
    int numBlades = 3;

    for (int i = 0; i < numBlades; i++) {
        float angle = 2.0f * M_PI * (float)i / (float)numBlades;
        float c = cos(angle);
        float s = sin(angle);
        Vertex v1 = {0, baseHeight, 0};
        Vertex v2 = {bladeLength * c, baseHeight, bladeLength * s};
        Vertex v3 = {bladeLength * c, baseHeight + bladeHeight, bladeLength * s};
        Vertex v4 = {0, baseHeight + bladeHeight, 0};
        int baseIndex = windmill.num_vertices;
        windmill.vertices[windmill.num_vertices++] = v1;
        windmill.vertices[windmill.num_vertices++] = v2;
        windmill.vertices[windmill.num_vertices++] = v3;
        windmill.vertices[windmill.num_vertices++] = v4;
        windmill.indices[windmill.num_indices++] = baseIndex;
        windmill.indices[windmill.num_indices++] = baseIndex + 1;
        windmill.indices[windmill.num_indices++] = baseIndex + 2;
        windmill.indices[windmill.num_indices++] = baseIndex;
        windmill.indices[windmill.num_indices++] = baseIndex + 2;
        windmill.indices[windmill.num_indices++] = baseIndex + 3;
    }

    windmill.r = 0.9f; windmill.g = 0.9f; windmill.b = 0.9f; // Light gray color
}

void createHouse() {
    house.num_vertices = 0;
    house.num_indices = 0;

    // Base
    house.vertices[house.num_vertices++] = (Vertex){-0.5f, 0.0f, -0.5f};
    house.vertices[house.num_vertices++] = (Vertex){0.5f, 0.0f, -0.5f};
    house.vertices[house.num_vertices++] = (Vertex){0.5f, 0.0f, 0.5f};
    house.vertices[house.num_vertices++] = (Vertex){-0.5f, 0.0f, 0.5f};
    house.vertices[house.num_vertices++] = (Vertex){-0.5f, 0.8f, -0.5f};
    house.vertices[house.num_vertices++] = (Vertex){0.5f, 0.8f, -0.5f};
    house.vertices[house.num_vertices++] = (Vertex){0.5f, 0.8f, 0.5f};
    house.vertices[house.num_vertices++] = (Vertex){-0.5f, 0.8f, 0.5f};

    // Roof
    house.vertices[house.num_vertices++] = (Vertex){0.0f, 1.2f, -0.6f};
    house.vertices[house.num_vertices++] = (Vertex){0.0f, 1.2f, 0.6f};

    // Indices for walls and roof
    int wall_indices[] = {
        0,1,5, 0,5,4, 1,2,6, 1,6,5, 2,3,7, 2,7,6, 3,0,4, 3,4,7,
        4,5,8, 5,6,9, 6,7,9, 7,4,8, 8,9,4, 9,8,5
    };

    for (int i = 0; i < 42; i++) {
        house.indices[house.num_indices++] = wall_indices[i];
    }

    house.r = 0.8f; house.g = 0.6f; house.b = 0.4f; // Warm beige color
}

void createTree() {
    tree.num_vertices = 0;
    tree.num_indices = 0;

    // Trunk
    for (int i = 0; i < 8; i++) {
        float angle = 2.0f * M_PI * i / 8;
        tree.vertices[tree.num_vertices++] = (Vertex){0.1f * cos(angle), 0.0f, 0.1f * sin(angle)};
        tree.vertices[tree.num_vertices++] = (Vertex){0.1f * cos(angle), 0.5f, 0.1f * sin(angle)};
    }

    // Indices for trunk
    for (int i = 0; i < 8; i++) {
        int next = (i + 1) % 8;
        tree.indices[tree.num_indices++] = i * 2;
        tree.indices[tree.num_indices++] = next * 2;
        tree.indices[tree.num_indices++] = i * 2 + 1;
        tree.indices[tree.num_indices++] = next * 2;
        tree.indices[tree.num_indices++] = next * 2 + 1;
        tree.indices[tree.num_indices++] = i * 2 + 1;
    }

    // Leaves (as multiple cones)
    for (int level = 0; level < 3; level++) {
        int leaves_base = tree.num_vertices;
        float y_offset = 0.5f + level * 0.3f;
        float radius = 0.5f - level * 0.15f;

        for (int i = 0; i < 8; i++) {
            float angle = 2.0f * M_PI * i / 8;
            tree.vertices[tree.num_vertices++] = (Vertex){radius * cos(angle), y_offset, radius * sin(angle)};
        }

        tree.vertices[tree.num_vertices++] = (Vertex){0.0f, y_offset + 0.3f, 0.0f}; // Top of the cone

        for (int i = 0; i < 8; i++) {
            int next = (i + 1) % 8;
            tree.indices[tree.num_indices++] = leaves_base + i;
            tree.indices[tree.num_indices++] = leaves_base + next;
            tree.indices[tree.num_indices++] = leaves_base + 8;
        }
    }

    float greenVariation = ((float)rand() / RAND_MAX) * 0.2f - 0.1f;
    tree.r = 0.0f;
    tree.g = 0.5f + greenVariation;
    tree.b = 0.0f;
}

void createGround() {
    ground.num_vertices = 4;
    ground.num_indices = 6;

    ground.vertices[0] = (Vertex){-5.0f, 0.0f, -5.0f};
    ground.vertices[1] = (Vertex){ 5.0f, 0.0f, -5.0f};
    ground.vertices[2] = (Vertex){ 5.0f, 0.0f, 5.0f};
    ground.vertices[3] = (Vertex){-5.0f, 0.0f, 5.0f};

    ground.indices[0] = 0; ground.indices[1] = 1; ground.indices[2] = 2;
    ground.indices[3] = 0; ground.indices[4] = 2; ground.indices[5] = 3;

       ground.r = 0.76f; ground.g = 0.69f; ground.b = 0.50f; // Light tan color
}

void drawObject(const Object* obj) {
    glColor3f(obj->r, obj->g, obj->b);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < obj->num_indices; i++) {
        const Vertex* v = &obj->vertices[obj->indices[i]];
        glVertex3f(v->x, v->y, v->z);
    }
    glEnd();
}

void drawScene(float scaleX, float scaleY, float scaleZ, float rotateY) {
    glPushMatrix();
    glScalef(scaleX, scaleY, scaleZ);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);

    // Draw windmill
    glPushMatrix();
    glTranslatef(1.5f, 0.0f, -1.0f);
    drawObject(&windmill);
    glPopMatrix();

    // Draw house
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, 0.0f);
    glScalef(1.2f, 1.2f, 1.2f);
    drawObject(&house);
    glPopMatrix();

    // Draw trees
    glPushMatrix();
    glTranslatef(-2.0f, 0.0f, -1.5f);
    drawObject(&tree);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.5f, 0.0f, 1.5f);
    glScalef(1.2f, 1.2f, 1.2f);
    drawObject(&tree);
    glPopMatrix();

    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(-cameraX, -cameraY, -cameraZ);
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);

    // Draw ground
    drawObject(&ground);

    // Draw first scene
    glPushMatrix();
    glTranslatef(-2.5f, 0.0f, 0.0f);
    drawScene(1.0f, 1.0f, 1.0f, 0.0f);
    glPopMatrix();

    // Draw second scene (scaled and rotated)
    glPushMatrix();
    glTranslatef(2.5f, 0.0f, -1.0f);
    drawScene(0.8f, 0.8f, 0.8f, 45.0f);
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Escape key
            exit(0);
            break;
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            cameraY += 0.1f;
            break;
        case GLUT_KEY_DOWN:
            cameraY -= 0.1f;
            break;
        case GLUT_KEY_LEFT:
            cameraX -= 0.1f;
            break;
        case GLUT_KEY_RIGHT:
            cameraX += 0.1f;
            break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDown = 1;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            mouseDown = 0;
        }
    }
}

void motion(int x, int y) {
    if (mouseDown) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;
        cameraAngleY += 0.5f * deltaX;
        cameraAngleX += 0.5f * deltaY;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("HW3 Manojdeep");

    glEnable(GL_DEPTH_TEST);

    createWindmill();
    createHouse();
    createTree();
    createGround();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}