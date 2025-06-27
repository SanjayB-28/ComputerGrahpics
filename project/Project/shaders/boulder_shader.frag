#version 120

varying vec3 Normal;
varying float Height;
varying vec2 TexCoord;
varying vec3 WorldPos;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform int boulderColorIndex;
uniform sampler2D boulderTex;
uniform vec3 lightPos;

// Hash function for color variety
float hash(float n) { return fract(sin(n) * 43758.5453); }

void main() {
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - WorldPos);
    float intensity = max(dot(N, L), 0.0) * 0.7 + 0.3;
    // Select base color by index (GLSL 1.20 compatible)
    vec3 baseColor;
    if (boulderColorIndex == 0) baseColor = vec3(0.25, 0.23, 0.21);
    else if (boulderColorIndex == 1) baseColor = vec3(0.38, 0.36, 0.34);
    else if (boulderColorIndex == 2) baseColor = vec3(0.45, 0.44, 0.42);
    else if (boulderColorIndex == 3) baseColor = vec3(0.32, 0.29, 0.27);
    else if (boulderColorIndex == 4) baseColor = vec3(0.28, 0.25, 0.32);
    else if (boulderColorIndex == 5) baseColor = vec3(0.36, 0.33, 0.22);
    else if (boulderColorIndex == 6) baseColor = vec3(0.41, 0.39, 0.29);
    else baseColor = vec3(0.22, 0.20, 0.18);
    float h = clamp(Height, -1.0, 1.0);
    float t = hash(gl_FragCoord.x + gl_FragCoord.y + Height * 17.0);
    float s = hash(gl_FragCoord.x * 0.7 + gl_FragCoord.y * 1.3 + Height * 11.0);
    vec3 color = baseColor;
    color = mix(color, vec3(1.5,1.5,1.5), t*0.30); // stronger highlight
    color = mix(color, vec3(0.05,0.05,0.05), s*0.18); // stronger shadow
    vec3 texColor = texture2D(boulderTex, TexCoord).rgb;
    color = mix(color, texColor, 0.4); // Less texture, more color
    vec3 ambient = vec3(0.13, 0.13, 0.13);
    gl_FragColor = vec4(ambient + intensity * color * lightColor, 1.0);
} 