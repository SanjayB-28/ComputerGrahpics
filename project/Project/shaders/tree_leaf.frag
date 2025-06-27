#version 120

varying vec3 Normal;
varying float Height;
varying vec2 TexCoord;
varying vec3 WorldPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform sampler2D leafTex;
uniform int leafColorIndex;

void main() {
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - WorldPos);
    float intensity = max(dot(N, L), 0.0) * 0.8 + 0.2;
    // Select base color by index (GLSL 1.20 compatible)
    vec3 baseColor;
    if (leafColorIndex == 0) baseColor = vec3(0.13, 0.45, 0.13); // green
    else if (leafColorIndex == 1) baseColor = vec3(0.5, 0.9, 0.3); // light green
    else if (leafColorIndex == 2) baseColor = vec3(0.9, 0.7, 0.3); // yellow
    else if (leafColorIndex == 3) baseColor = vec3(0.8, 0.4, 0.1); // orange
    else if (leafColorIndex == 4) baseColor = vec3(0.7, 0.2, 0.2); // red
    else if (leafColorIndex == 5) baseColor = vec3(0.3, 0.3, 0.7); // blue
    else if (leafColorIndex == 6) baseColor = vec3(0.5, 0.3, 0.7); // purple
    else baseColor = vec3(0.2, 0.6, 0.4); // teal
    float h = clamp(Height, 0.0, 1.0);
    vec3 color = mix(baseColor, vec3(1.0,1.0,1.0), h*0.5); // lighter at tip
    vec3 texColor = texture2D(leafTex, TexCoord).rgb;
    color = mix(color, texColor, 0.8); // Blend with leaf texture
    vec3 ambient = vec3(0.10, 0.10, 0.10);
    gl_FragColor = vec4(ambient + intensity * color * lightColor, 1.0);
} 