#version 330 core

in VS_OUT {
    vec3 world_pos;
} fs_in;

out vec4 frag_color;

uniform vec3 u_camera_pos;

// Underwater color settings
const vec3 SHALLOW_COLOR = vec3(0.1, 0.4, 0.5);
const vec3 DEEP_COLOR = vec3(0.0, 0.05, 0.1);
const float FOG_DENSITY = 0.05;

void main() {
    // Calculate distance-based fog
    float dist = distance(fs_in.world_pos, u_camera_pos);
    float fog_factor = 1.0 - exp(-pow(dist * FOG_DENSITY, 2.0));
    fog_factor = clamp(fog_factor, 0.0, 1.0);

    // Final color is a mix between the "surface" color and the deep water color
    frag_color = vec4(mix(SHALLOW_COLOR, DEEP_COLOR, fog_factor), 1.0);
}