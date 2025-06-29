# Alpine Wilderness Simulator
### By: Sanjay Baskaran

An interactive 3D simulation of a natural landscape inspired by the Boulder area, featuring dynamic weather, day/night cycles, and rich environmental interactions. This project demonstrates advanced OpenGL graphics techniques with procedural terrain generation, sophisticated particle systems, and immersive atmospheric effects.

## Key Features

###  **Procedural Terrain Generation**
- **Multi-octave Perlin noise** for realistic elevation
- **Dynamic texture blending** based on height and slope for varied terrain types
- **Collision detection** for camera movement and particle interactions
- **Water rendering** with animated surface and adjustable water level

###  **Dynamic Environment & Atmospheric Effects**
- **Realistic day/night cycle** with dynamic sky color transitions
- **Volumetric cloud system** with procedural clouds
- **GPU-based snow particle system** with 20,000 particles and terrain accumulation
- **Atmospheric fog** that changes density and color with time of day
- **Sun and moon positioning** with emission lighting and realistic orbits

###  **Advanced Dual-Mode Camera System**
- **First Person Mode**: Ground-level exploration with terrain collision detection
- **Free Orbit Mode**: Aerial view with smooth rotation and zoom controls
- **Mouse and keyboard controls** with intuitive navigation
- **Camera interpolation** between modes with state preservation
- **Terrain boundary constraints** to prevent camera clipping

###  **Sophisticated Vegetation & Environmental Systems**
- **L-System fractal tree generation** with recursive branching and wind animation
- **Dynamic grass system** with 500,000 blades and individual sway effects
- **Procedural boulder placement** with shape variation and collision avoidance
- **Texture-based rendering** for bark, leaves, and terrain materials
- **Wind effects** on trees, grass, and atmospheric particles

###  **Advanced Rendering & Shaders**
- **Custom shader pipeline** for terrain, vegetation, particles, and atmospheric effects
- **GPU-based particle physics** using transform feedback for performance
- **Dynamic lighting system** with day/night transitions and material properties
- **Point sprite rendering** with custom snowflake and particle shaders
- **Texture mapping** for realistic material appearance

###  **Interactive Weather & Sound**
- **Toggleable snow weather system** with realistic particle physics
- **Ambient forest sound system** with SDL2 audio integration
- **Wind simulation** affecting all environmental elements
- **Weather state management** with smooth transitions
- **Particle accumulation** and respawning on terrain

##  Quick Demo

Experience the key features in this order:

1. **Launch in Free Orbit mode** to see overall terrain and sky system
2. **Switch to First Person** (`1`) to explore ground level
3. **Toggle weather** (`W`) to see snow effects and particle system
4. **Enable fog** (`b`) for atmospheric effects and time-of-day transitions
5. **Press W** to explore in wireframe mode

## Build Instructions

### Prerequisites
- OpenGL 2.1+
- GLUT/FreeGLUT
- GCC compiler
- Make
- SDL2 (for sound system)

### Compilation
```bash
make clean
make
./final
```

## Controls

### Camera Controls
- **Arrow Keys**: 
  - **Free Orbit Mode**: Rotate view
  - **First Person Mode**: Strafe/Walk (Forward = Up, Backward = Down, Left = Left, Right = Right)
- **Mouse Drag (Left Button)**: Look around (First Person mode only)
- **1**: Switch to First Person View
- **2**: Switch to Free Orbit View
- **z** / **Z**: Quick zoom **in** / **out** (Free Orbit mode only)
- **r**: Reset camera and simulation parameters to defaults

### Environment Controls
- **T**: Toggle time animation (day/night cycle)
- **k/l**: Decrease/increase time speed (range: 0.1x to 5.0x, step: 0.1x)
- **b**: Toggle atmospheric fog
- **W**: Toggle weather system (Fall ↔ Winter, enables snow in Winter)
- **[ ]**: Lower/raise water level (±1.0 unit)
- **n**: Toggle snow particles
- **m**: Toggle ambient sound

### Display Controls
- **w**: Toggle wireframe mode
- **a**: Toggle coordinate axes display
- **ESC**: Exit application


## Code Attribution

### Core OpenGL Framework and Utilities
- **Basic OpenGL setup and utility functions**: CSCI-4229/5229 class examples by professor Willem A. (Vlakkies) Schreuder
- **Shader management system**: Adapted from https://learnopengl.com/Getting-started/Shaders

### Terrain and Environment Systems
- **Perlin noise implementation**: Adapted from Ken Perlin's improved noise algorithm [http://mrl.nyu.edu/~perlin/noise/]
- **Water rendering system**: Adapted from OpenGL-Tutorial.org techniques

### Camera and Navigation System
- **Core camera implementation**: Adapted from LearnOpenGL's Camera tutorial [https://learnopengl.com/Getting-started/Camera]
- **Multiple view modes and collision detection**: Enhanced with Claude assistance

### Vegetation and Environmental Systems
- **L-System fractal trees**: Reference algorithms from [https://gpfault.net/posts/generating-trees.txt.html] and [https://thecodingtrain.com/challenges/16-l-system-fractal-trees]
- **Wind animation system**: Enhanced with Claude assistance

### Particle and Weather Effects
- **Base particle system**: Adapted from OpenGL-Tutorial.org [https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-18-billboards-particles/]
- **GPU transform feedback**: Standard OpenGL particle system techniques
- **Snow accumulation system**: Enhanced with Claude assistance

### Lighting and Atmospheric Effects
- **Dynamic lighting system**: Based on LearnOpenGL's Advanced Lighting tutorials
- **Day/night cycle implementation**: Assisted by Claude

### AI Assistance
This project was developed independently with AI assistance for specific development tasks:

- **GitHub Copilot**: Extensively used throughout development for code completion and suggestions
- **Claude**: Assisted with specific function implementations and algorithms, documented in function comments
- **Code refactoring and cleanup** during development phases
- **Debugging assistance** for shader compilation issues and OpenGL warnings
- **Minor syntax corrections** and code organization
- **Documentation formatting** and README structure


## Before Demo

- **GPU-based rain particle system** to complement existing snow effects
- **Additional terrain objects** including insects, butterflies, or other wildlife (number to be determined based on time constraints)

