// Tree leaf vertex shader - handles leaf positioning and texture coordinates

#version 120

varying vec3 Normal;
varying float Height;
varying vec2 TexCoord;
varying vec3 WorldPos;

void main() {
    Normal = normalize(gl_NormalMatrix * gl_Normal);
    Height = gl_Vertex.y;
    TexCoord = gl_MultiTexCoord0.st;
    WorldPos = vec3(gl_ModelViewMatrix * gl_Vertex);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
} 