# OpenGL Texture and Lighting Scene with Custom Object

This OpenGL program creates a 3D spiral tube with texture mapping and dynamic lighting. It demonstrates the use of texture coordinates, lighting calculations, and user interaction in a 3D environment.


## Features

- 3D spiral tube with texture mapping
- Dynamic lighting with adjustable parameters
- User-controlled camera and light source
- Switchable orthographic/perspective projections
- Adjustable field of view and display properties

## Objects

1. Spiral tube: A custom-designed 3D object showcasing complex geometry and lighting effects
2. Light Source: Represented by a small sphere, movable around the scene

## Controls

- Arrow keys: Rotate the view
- 0: Reset view angle
- x/X: Toggle axes
- l/L: Toggle lighting
- p/P: Switch projection (orthogonal/perspective)
- m/M: Toggle light movement
- </> : Move light
- +/- : Change field of view
- [/] : Adjust light elevation
- a/A: Decrease/increase ambient light
- d/D: Decrease/increase diffuse light
- s/S: Decrease/increase specular light
- e/E: Decrease/increase emission
- n/N: Decrease/increase shininess
- z/Z: Zoom out/in
- r/R: Reset all settings to default
- Esc: Exit the program

## Running the Program

1. Unzip the uploaded file to a directory of your choice.

2. Open a terminal/command prompt and navigate to the directory containing the unzipped files.

3. Run the program:
    - On Windows:
      ```
      hw6.exe
      ```
    - On macOS/Linux:
      ```
      ./hw6
      ```

If you need to recompile the program:

1. Ensure you have the necessary development tools installed (gcc, make, OpenGL, and GLUT libraries).

2. In the terminal, run:
    ```
    make clean
    make
    ```

   or

   #### On macOS:
      ```
      gcc -O3 -Wall -Wno-deprecated-declarations -DRES=1 -o hw6 hw6.c CSCIx229.a -framework GLUT -framework OpenGL
      ```

   #### On Linux/Ubuntu:
      ```
      gcc -O3 -Wall -o hw6 hw6.c CSCIx229.a -lglut -lGLU -lGL -lm
      ```

   #### On Windows (using MinGW):
      ```
      gcc -O3 -Wall -DUSEGLEW -o hw6.exe hw6.c CSCIx229.a -lfreeglut -lglew32 -lglu32 -lopengl32
      ```

3. Then run the program as described above.

## Time to Complete the Assignment:
### Total Time: 10 hours
- Research and Setup: 2 hours
- Implementing Lighting and Shading: 2 hours
- Implementing textures: 2 hours
- User Interface and Controls: 1 hours
- Testing and Debugging: 2 hours
- Writing README: 1 hours