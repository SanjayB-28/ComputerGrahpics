#version 120
attribute vec3 position;
attribute float swaySeed;
attribute float offsetX;
attribute float offsetY;
attribute float bladeHeight;
attribute float bladeWidth;
attribute float colorVar;
attribute float rotation;
uniform float time;
uniform float windStrength;
varying float vAlpha;
varying vec3 vNormal;
varying float vColorVar;
varying float vHalfSide;
varying vec2 vTexCoord;
varying float vColorIndex;
void main() {
    float x = offsetY >= bladeHeight - 0.001 ? 0.0 : offsetX;
    float curve = 0.35 * sin(offsetY / bladeHeight * 1.57);
    float sway = sin(time * 1.5 + position.x * 0.2 + position.z * 0.3 + swaySeed * 6.28) * 0.2 * windStrength;
    float tipFactor = offsetY / bladeHeight;
    float swayX = sway * tipFactor;
    float twist = 0.15 * sin(offsetY * 6.0 / bladeHeight + swaySeed * 3.0);
    vec3 local = vec3(x + twist, offsetY, curve);
    float c = cos(rotation);
    float s = sin(rotation);
    mat3 rotY = mat3(
        c, 0, -s,
        0, 1, 0,
        s, 0,  c
    );
    local = rotY * local;
    vec3 pos = position + local;
    pos.x += swayX;
    vAlpha = 1.0 - abs(sway) * 0.5;
    vNormal = normalize(rotY * vec3(0.0, 1.0, 0.0));
    vColorVar = colorVar;
    vHalfSide = offsetX < 0.0 ? -1.0 : 1.0;
    vTexCoord = vec2((offsetX/bladeWidth)+0.5, offsetY/bladeHeight); 
    vColorIndex = floor(colorVar * 4.0 + 2.0) / 4.0; 
    gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
} 