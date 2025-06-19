#version 330 core

layout (location = 0) in vec3 a_pos;

out VS_OUT {
    vec3 world_pos;
} vs_out;

uniform mat4 u_model;
uniform mat4 u_mvp;

void main() {
    // Transform vertex to clip space for rendering
    gl_Position = u_mvp * vec4(a_pos, 1.0);
    
    // Separately transform vertex to world space for lighting/fog calculations
    vs_out.world_pos = vec3(u_model * vec4(a_pos, 1.0));
}