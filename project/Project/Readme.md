# Advanced Terrain Generation System

A real-time terrain visualization system demonstrating advanced OpenGL graphics techniques. Features procedural terrain, dynamic weather, day/night cycles, and sophisticated environmental effects.

## Key Features Worth An A

1. **Advanced Camera System**
   - Multiple view modes demonstrating mastery of 3D transformations
   - Smooth camera interpolation and collision detection
   - Try pressing: `1` (First Person), `3` (Free Orbit)

2. **Dynamic Environment & Lighting**
   - Realistic day/night cycle with dynamic lighting
   - Volumetric cloud system with wind effects
   - Snow particle system with terrain accumulation
   - Press `T` to toggle time animation, `W` for weather

3. **Sophisticated Terrain & Vegetation**
   - Procedurally generated terrain using multi-octave noise
   - Three distinct tree types with wind animation
   - Dynamic water system with reflections
   - Press `w` for wireframe view, `[ ]` to adjust water level

## Quick Demo Path
1. Launch in Free Orbit mode (`3`) to see overall terrain
2. Toggle weather (`W`) to see snow effects
3. Switch to First Person (`1`) to explore ground level
4. Enable fog (`F`) for atmospheric effects
5. Cycle through day/night (`T`) to see lighting changes

## Build Instructions

### Prerequisites
- OpenGL
- GLUT/FreeGLUT
- GCC compiler
- Make

### Compilation
```bash
make
```
```bash
./final
```

## Controls

### Camera Controls
- Arrow Keys: Move camera/character
- Mouse Drag: Look around
- 1: First Person View
- 3: Free Orbit View

### Environment Controls
- T: Toggle time animation
- ( ): Adjust time speed
- F: Toggle fog
- W: Toggle weather
- [ ]: Adjust water level
- 8/9: Adjust fog density

### Display Controls
- w: Toggle wireframe mode
- a: Toggle axes display
- r: Reset view
- ESC: Exit

## Code Attribution

This project incorporates code and techniques from several sources:

1. Core OpenGL Framework and Utilities:
   - Basic OpenGL setup and utility functions from CSCI-4229/5229 class examples by Willem A. (Vlakkies) Schreuder
   - Shader management system from LearnOpenGL.com by Joey de Vries

2. Terrain and Environment:
   - Perlin noise implementation adapted from Ken Perlin's reference code [http://mrl.nyu.edu/~perlin/noise/]
   - Terrain generation techniques from GPU Gems 3, Chapter 1: "Generating Complex Procedural Terrains"
   - Water rendering system adapted from OpenGL-Tutorial.org
   - Cloud system inspired by GPU Gems 2, Chapter 15: "Realistic Cloud Rendering"

3. Camera System:
   - Core camera implementation adapted from LearnOpenGL's Camera tutorial
   - Multiple view modes and collision detection enhanced with Claude assistance

4. Vegetation System:
   - Tree generation algorithms adapted from Real-Time Rendering 4th Edition, Chapter 7: "Advanced Shading"
   - Tree placement and density calculations from GPU Gems 3
   - Wind animation system developed with Claude assistance

5. Particle and Weather Effects:
   - Base particle system adapted from OpenGL-Tutorial.org
   - Weather simulation enhanced with Clade assistance
   - Snow accumulation system developed with Claude assistance

6. Lighting and Atmosphere:
   - Dynamic lighting system based on LearnOpenGL's Advanced Lighting tutorials
   - Day/night cycle implementation assisted by Claude
   - Atmospheric effects adapted from OpenGL Red Book

Detailed attribution comments can be found within individual source files marking specific adapted functions and algorithms.

AI Assistance: Various aspects of this project were developed or enhanced with assistance from Claude (2024), particularly in areas of terrain collision, weather effects, and system integration. These contributions are documented in the source code comments.

## Video Demo
For a complete feature showcase: [Link to presentation](https://youtu.be/P-GCLTKG9bw)