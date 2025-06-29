// Terrain fragment shader - handles terrain texturing and lighting

#version 120

varying vec3 Normal;
varying float Height;

void main()
{
   vec3 N = normalize(Normal);

   vec3 L = normalize(vec3(0,1,0));

   float intensity = max(dot(N,L), 0.0);

   vec3 color;
   if (Height > 0.5)
      color = mix(vec3(0.5,0.5,0.5), vec3(1,1,1), min((Height-0.5)/0.5, 1.0));
   else if (Height > 0.0)
      color = mix(vec3(0.0,0.7,0.0), vec3(0.5,0.5,0.5), Height/0.5);
   else
      color = vec3(0.76,0.7,0.5);

   gl_FragColor = vec4(min(intensity, 0.7) * color, 1.0);
}