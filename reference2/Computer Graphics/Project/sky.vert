varying vec3 vPosition;
varying float vHeight;

void main() {
    // Pass position to fragment shader
    vPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    // Pass height to fragment shader
    vHeight = gl_Vertex.y;
    
    // Transform vertex position
    gl_Position = ftransform();
}