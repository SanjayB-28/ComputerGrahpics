#include "renderer.h"
#include "math_utils.h"
#include "CSCIx229.h" // For OpenGL functions
#include <stdio.h>
#include <stdlib.h>

// --- Globals for this module ---
static unsigned int underwater_shader;
static GLint u_mvp_location = -1;
static GLint u_model_location = -1;
static GLint u_camera_pos_location = -1;

// Simple quad for the "floor"
static unsigned int floor_vao = 0;
static unsigned int floor_vbo = 0;


char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        // This is not a fatal error, just return NULL. The caller will handle it.
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

unsigned int create_shader_program(const char* vertex_shader_path, const char* fragment_shader_path) {
    char* vs_source = read_file(vertex_shader_path);
    if (vs_source == NULL) {
        Fatal("Failed to load vertex shader: %s\n", vertex_shader_path);
    }
    char* fs_source = read_file(fragment_shader_path);
    if (fs_source == NULL) {
        free(vs_source); // Clean up the vertex shader source
        Fatal("Failed to load fragment shader: %s\n", fragment_shader_path);
    }

    // Compile vertex shader
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, (const char**)&vs_source, NULL);
    glCompileShader(vs);
    ErrCheck("Vertex Shader Compilation");
    free(vs_source);

    // Compile fragment shader
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, (const char**)&fs_source, NULL);
    glCompileShader(fs);
    ErrCheck("Fragment Shader Compilation");
    free(fs_source);

    // Link shaders
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    ErrCheck("Shader Program Linking");

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void renderer_init() {
    underwater_shader = create_shader_program("shaders/underwater.vert", "shaders/underwater.frag");
    u_mvp_location = glGetUniformLocation(underwater_shader, "u_mvp");
    u_model_location = glGetUniformLocation(underwater_shader, "u_model");
    u_camera_pos_location = glGetUniformLocation(underwater_shader, "u_camera_pos");

    // --- Create a large quad to act as a temporary seafloor ---
    float floor_vertices[] = {
        // positions
        -250.0f, -10.0f, -250.0f,
         250.0f, -10.0f, -250.0f,
         250.0f, -10.0f,  250.0f,
         250.0f, -10.0f,  250.0f,
        -250.0f, -10.0f,  250.0f,
        -250.0f, -10.0f, -250.0f,
    };

    glGenVertexArrays(1, &floor_vao);
    glGenBuffers(1, &floor_vbo);
    glBindVertexArray(floor_vao);
    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Basic OpenGL state
    glEnable(GL_DEPTH_TEST);
}

void renderer_render_frame(Camera* camera, int window_width, int window_height) {
    // 1. Set up viewport and clear screen
    glViewport(0, 0, window_width, window_height);
    Vector3 clear_color = {0.0f, 0.05f, 0.1f}; // Deep blue
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Get matrices
    Matrix4x4 projection_matrix, view_matrix, model_matrix, mvp_matrix;
    camera_get_projection_matrix((float)window_width / (float)window_height, &projection_matrix);
    camera_get_view_matrix(camera, &view_matrix);

    // 3. Render the floor
    glUseProgram(underwater_shader);

    model_matrix = mat4_identity(); // Simple identity for the floor
    mvp_matrix = mat4_multiply(projection_matrix, mat4_multiply(view_matrix, model_matrix));

    if (u_mvp_location != -1) glUniformMatrix4fv(u_mvp_location, 1, GL_FALSE, mvp_matrix.m);
    if (u_model_location != -1) glUniformMatrix4fv(u_model_location, 1, GL_FALSE, model_matrix.m);
    if (u_camera_pos_location != -1) glUniform3f(u_camera_pos_location, camera->position.x, camera->position.y, camera->position.z);

    glBindVertexArray(floor_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    ErrCheck("render_frame");
}

void renderer_cleanup() {
    glDeleteProgram(underwater_shader);
    glDeleteVertexArrays(1, &floor_vao);
    glDeleteBuffers(1, &floor_vbo);
}