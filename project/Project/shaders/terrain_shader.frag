#version 120

varying vec3 Normal;
varying float Height;

void main()
{
   // Normalize normal vector
   vec3 N = normalize(Normal);

   // Light direction (from above)
   vec3 L = normalize(vec3(0,1,0));

   // Calculate light intensity
   float intensity = max(dot(N,L), 0.0);

   // Base color based on height
   vec3 color;
   if (Height > 0.5)
      color = mix(vec3(0.5,0.5,0.5), vec3(1,1,1), min((Height-0.5)/0.5, 1.0));  // Rock to snow
   else if (Height > 0.0)
      color = mix(vec3(0.0,0.7,0.0), vec3(0.5,0.5,0.5), Height/0.5);  // Grass to rock
   else
      color = vec3(0.76,0.7,0.5);  // Sand

   // Output color with lighting
   gl_FragColor = vec4(min(intensity, 0.7) * color, 1.0);
}