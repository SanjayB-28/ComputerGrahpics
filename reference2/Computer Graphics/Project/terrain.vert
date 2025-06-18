#version 120

varying vec3 Normal;
varying float Height;
varying vec3 Position;
varying vec3 LightDir;

void main()
{
    Normal = normalize(gl_NormalMatrix * gl_Normal);
    Position = vec3(gl_ModelViewMatrix * gl_Vertex);
    Height = gl_Vertex.y;  // Use actual height
    LightDir = normalize(gl_LightSource[0].position.xyz - Position);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}