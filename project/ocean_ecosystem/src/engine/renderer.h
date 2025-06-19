#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"

void renderer_init();
void renderer_render_frame(Camera* camera, int window_width, int window_height);
void renderer_cleanup();

// Utility for loading shaders (could be in its own file later)
unsigned int create_shader_program(const char* vertex_shader_path, const char* fragment_shader_path);

#endif // RENDERER_H