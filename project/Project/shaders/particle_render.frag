#version 120

// Returns 1.0 for points inside a snowflake, 0.0 outside
float simpleSnowflake(vec2 uv) {
    uv = uv * 2.0 - 1.0;
    float r = length(uv);
    float a = atan(uv.y, uv.x);
    // 6 arms, but much broader
    float arm = abs(sin(3.0 * a));
    float flake = smoothstep(0.25, 0.22, r) * smoothstep(0.5, 0.2, arm);
    // Add a soft core
    flake += smoothstep(0.08, 0.0, r) * 0.5;
    return clamp(flake, 0.0, 1.0);
}

void main() {
    float flake = simpleSnowflake(gl_PointCoord);
    float alpha = flake * 0.85;
    vec3 color = mix(vec3(0.85, 0.92, 1.0), vec3(1.0), 0.5);
    gl_FragColor = vec4(color, alpha);
} 