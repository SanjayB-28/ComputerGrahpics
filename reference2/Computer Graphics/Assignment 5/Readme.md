# OpenGL Lighting Scene with Custom Object

This program demonstrates a 3D scene with custom objects, interactive lighting, and various view modes.

## Features

- Custom 3D object (twisted torus) with proper lighting and shading
- Interactive lighting controls
- Multiple view modes: orthogonal and perspective projections
- User-controlled camera movement
- Adjustable lighting parameters (ambient, diffuse, specular, emission)

## Objects

1. Twisted Torus: A custom-designed 3D object showcasing complex geometry and lighting effects
2. Light Source: Represented by a small sphere, movable around the scene

## Controls

- Arrow keys: Rotate the view
- 'z' or 'Z': Zoom in/out
- 'x' or 'X': Toggle axes display
- 'l' or 'L': Toggle lighting on/off
- 'p' or 'P': Switch between perspective and orthogonal projection
- 'm' or 'M': Toggle light movement
- '<' and '>': Manually move the light source
- '[' and ']': Adjust light height
- 'a'/'A', 'd'/'D', 's'/'S', 'e'/'E': Adjust ambient, diffuse, specular, and emission light intensities
- 'n'/'N': Adjust shininess
- 'z'/'Z': Zoom out/in
- 'r' or 'R': Reset all settings to default
- Esc: Exit the program

## Running the Program

1. Unzip the uploaded file to a directory of your choice.

2. Open a terminal/command prompt and navigate to the directory containing the unzipped files.

3. Run the program:
    - On Windows:
      ```
      hw5.exe
      ```
    - On macOS/Linux:
      ```
      ./hw5
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
      gcc -O3 -Wall -Wno-deprecated-declarations -DRES=1 -o hw5 hw5.c CSCIx229.a -framework GLUT -framework OpenGL
      ```

   #### On Linux/Ubuntu:
      ```
      gcc -O3 -Wall -o hw5 hw5.c CSCIx229.a -lglut -lGLU -lGL -lm
      ```

   #### On Windows (using MinGW):
      ```
      gcc -O3 -Wall -DUSEGLEW -o hw5.exe hw5.c CSCIx229.a -lfreeglut -lglew32 -lglu32 -lopengl32
      ```

3. Then run the program as described above.

## Time to Complete the Assignment:
### Total Time: 16 hours
- Research and Setup: 5 hours
- Designing and Implementing Custom Object: 1 hours
- Implementing Lighting and Shading: 5 hours
- User Interface and Controls: 1 hours
- Testing and Debugging: 3 hours
- Writing README: 1 hours