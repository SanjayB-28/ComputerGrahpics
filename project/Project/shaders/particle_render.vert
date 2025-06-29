// Particle render vertex shader - handles particle positioning

#version 120

attribute vec3 pos;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
    gl_PointSize = 1.0;
} 