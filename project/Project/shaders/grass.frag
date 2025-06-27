#version 120
varying float vAlpha;
varying vec3 vNormal;
varying float vColorVar;
varying float vHalfSide;
varying vec2 vTexCoord;
varying float vColorIndex;
uniform vec3 sunDir;
uniform vec3 ambient;
uniform sampler2D grassTex;
void main() {
    // Color palette: 4 options
    vec3 palette[4];
    palette[0] = vec3(0.13, 0.45, 0.13); // deep green
    palette[1] = vec3(0.25, 0.55, 0.18); // lighter green
    palette[2] = vec3(0.60, 0.60, 0.18); // yellow-green
    palette[3] = vec3(0.45, 0.32, 0.13); // brownish
    int idx = int(clamp(vColorIndex * 4.0, 0.0, 3.0));
    vec3 baseColor = palette[idx] + vColorVar * 0.18;
    vec4 texColor = texture2D(grassTex, vTexCoord);
    // Blend baseColor and texture color for subtlety
    baseColor = mix(baseColor, texColor.rgb, 0.5);
    // Make left half darker
    if (vHalfSide < 0.0) {
        baseColor *= 0.8;
    }
    float diff = max(dot(normalize(vNormal), normalize(sunDir)), 0.0);
    vec3 color = baseColor * (0.7 * diff + 0.3) + ambient * 0.7;
    gl_FragColor = vec4(color, 0.7 * vAlpha * texColor.a);
} 