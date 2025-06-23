#version 120

varying vec3 Normal;
varying float Height;
varying vec3 Position;
varying vec3 LightDir;

void main()
{
    Normal = normalize(gl_NormalMatrix * gl_Normal);
    Height = gl_Vertex.y;  // Use actual height
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}