# Different Projections in 3D Scene with Custom Objects

This program allows a 3D scene to be viewed in orthogonal, perspective projections, and first-person navigation mode.

## Features

- Three distinct view modes: orthogonal, perspective, and first-person.
- Interactive camera control using keyboard and mouse.
- 3D scene with multiple objects (windmill, house, trees, ground).
- Smooth transitions between view modes.

## Objects

1. Windmill: A cylindrical base with rotating blades
2. House: A simple house structure with walls and a roof
3. Tree: A trunk with multiple levels of conical foliage
4. Ground: A flat plane representing the ground

## Controls

- 'm' or 'M': To switch between modes
- Arrow keys: Move the camera up, down, left, and right
- Mouse drag: Rotate the camera view
- Esc: Exit the program

## Running the Program

1. Unzip the uploaded file to a directory of your choice.

2. Open a terminal/command prompt and navigate to the directory containing the unzipped files.

3. Run the program:
    - On Windows:
      ```
      hw4.exe
      ```
    - On macOS/Linux:
      ```
      ./hw4
      ```

If you need to recompile the program:

1. Ensure you have the necessary development tools installed (gcc, make, OpenGL, and GLUT libraries).

2. In the terminal, run:
    ```
   make clean
   ```
   ```
   make
   ```

   or

   #### On macOS:
      ```
      gcc -O3 -Wall -Wno-deprecated-declarations -DRES=1 -o hw4 hw4.c -framework GLUT -framework OpenGL
      ```

   #### On Linux/Ubuntu:
      ```
      gcc -O3 -Wall -o hw4 hw4.c -lglut -lGLU -lGL -lm
      ```

   #### On Windows (using MinGW):
      ```
      gcc -O3 -Wall -DUSEGLEW -o hw4.exe hw4.c -lfreeglut -lglew32 -lglu32 -lopengl32
      ```

3. Then run the program as described above.


## Time to Complete the Assignment:
### Total Time: 15 hours
- Research and Setup: 3 hour
- Coding and Implementation: 8 hours(Orthogonal - 2, Perspective - 2, First person - 4)
- Testing and Debugging: 3 hour
- Writing README: 1 hour