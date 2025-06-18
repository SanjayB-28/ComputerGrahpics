// sky.frag
uniform vec3 lightPos;
uniform float dayTime;
varying vec3 vPosition;
varying float vHeight;

// Time-based color interpolation
vec3 getSkyColor(float height, float time) {
    // Base colors
    vec3 nightColor = vec3(0.05, 0.05, 0.2);
    vec3 dayColor = vec3(0.3, 0.5, 0.9);
    vec3 sunsetColor = vec3(0.8, 0.6, 0.5);
    
    // Normalized time (0-1)
    float t = time / 24.0;
    
    // Calculate sun influence
    float sunAngle = (t * 2.0 * 3.14159);
    float sunHeight = sin(sunAngle);
    
    // Interpolate between day/night colors
    vec3 baseColor;
    if (sunHeight > 0.1) {  // Day
        baseColor = dayColor;
    } else if (sunHeight < -0.1) {  // Night
        baseColor = nightColor;
    } else {  // Sunset/sunrise
        float blend = (sunHeight + 0.1) / 0.2;
        baseColor = mix(nightColor, sunsetColor, blend);
    }
    
    // Add height-based gradient
    float heightFactor = clamp(height / 2.0 + 0.5, 0.0, 1.0);
    vec3 zenithColor = mix(baseColor, vec3(0.1, 0.1, 0.3), 0.3);
    
    return mix(baseColor, zenithColor, heightFactor);
}

void main() {
    vec3 skyColor = getSkyColor(vHeight, dayTime);
    
    // Calculate sun/moon glow
    float dist = length(vPosition - lightPos);
    float glow = 1.0 / (1.0 + dist * dist * 0.1);
    
    // Add sun/moon
    if (dist < 0.1) {
        // Sun
        if (lightPos.y > 0.0) {
            skyColor = mix(skyColor, vec3(1.0, 0.95, 0.8), glow);
        }
        // Moon
        else {
            skyColor = mix(skyColor, vec3(0.9, 0.9, 1.0), glow * 0.5);
        }
    }
    
    gl_FragColor = vec4(skyColor, 1.0);
}