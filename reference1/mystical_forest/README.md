# MYSTICAL FOREST

* Name:  Varshitha Sathish 
* ID:    vasa2949
* Email: varshitha.sathish@colorado.edu

## DESCRIPTION
The Mystical Forest project is an interactive OpenGL application simulating a magical forest environment with dynamic weather, glowing fairies, and swaying trees. Users can explore the environment through various movement controls, toggle weather effects, and enjoy the aesthetic animations designed to create a surreal, immersive experience.

## Mystical Forest - Features
**1. Dynamic Weather Effects**
- **Rain:** 
  - Toggle rain simulation using the `R` key.
  - Raindrops fall dynamically and reset upon reaching the ground.
- **Snow:**
  - Toggle snow simulation using the `N` key.
  - Snowflakes gently fall, creating a winter wonderland effect.
**2. Magical Environment**
- **Swaying Trees:**
  - Trees sway naturally, triggered by mouse interactions, adding a lifelike touch to the forest.
- **Glowing Fairies:**
  - Randomly moving fairies with adjustable glow intensity and smooth animation paths.
- **Bird Animation:**
  - A bird with flapping wings traverses the sky, adding life to the scene.
**3. Terrain and Vegetation**
- **Procedurally Generated Terrain:**
  - Terrain height dynamically calculated using sinusoidal functions for realism.
- **Vegetation:**
  - Includes swaying magical trees, fractal plants, glowing mushrooms, and detailed weeds.
**4. Dynamic Controls**
- **Movement:**
  - Navigate the scene using `W`, `A`, `S`, and `D` keys.
- **Camera Control:**
  - Adjust the camera perspective with mouse drag.
- **Weather Interaction:**
  - Use key toggles to enable/disable rain and snow effects.
- **Projection Modes:**
  - Switch between orthogonal and perspective projections using the `M` key.
**5. Rendering Techniques**
- **Blending Effects:**
  - Used for glowing objects like fairies, mushrooms, and butterflies.
- **Recursive Fractals:**
  - Generate realistic plants and tree branches with fractal algorithms.
- **Textures:**
  - Ground and tree surfaces are textured for enhanced realism.
- **Interactive Scene:**
  - Explore the terrain while interacting with dynamic and visually engaging elements.


## Compile and execute
```
$ make
$ .main
```
## Command

There are some key commands to interact with the program.

* W/A/S/D: Move
* R: Toggle rain
* N: Toggle snow
* Esc: Exit

## Mouse
* Left Button: Toggle swaying
* Drag: Adjust camera

## Remaining Work
* Add More Animals : Plan to include additional wildlife to enhance the scene's liveliness.  
* Implement a Water Body : Create a realistic water feature, such as a river or pond.  
* Learn and Apply Shaders : Explore shader techniques to add advanced visual effects and realism.  

## Attributions
- Refrenced `loadtexbmp.c` created by Dr. Schreuder for texture loading functionalities.
- Refrenced https://github.com/chmosquera/Real-Time-Glow-Effect for implementing glwoing effect in few trees.