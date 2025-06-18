# 3D Scene with Custom Objects

This OpenGL program creates a 3D scene featuring custom objects arranged in a village-like setting. The scene is duplicated with different scaling and rotation to demonstrate object transformation techniques.

## Features

- Custom 3D objects: windmill, house, and tree.
- Two instances of the scene with different scales and rotations
- Interactive camera controls

## Objects

1. Windmill: A cylindrical base with rotating blades
2. House: A simple house structure with walls and a roof
3. Tree: A trunk with multiple levels of conical foliage
4. Ground: A flat plane representing the ground

## Controls

- Arrow keys: Move the camera up, down, left, and right
- Mouse drag: Rotate the camera view
- Esc: Exit the program

## Running the Program

1. Unzip the uploaded file to a directory of your choice.

2. Open a terminal/command prompt and navigate to the directory containing the unzipped files.

3. Run the program:
    - On Windows:
      ```
      hw3.exe
      ```
    - On macOS/Linux:
      ```
      ./hw3
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
      gcc -O3 -Wall -Wno-deprecated-declarations -DRES=1 -o hw3 hw3.c -framework GLUT -framework OpenGL
      ```

   #### On Linux/Ubuntu:
      ```
      gcc -O3 -Wall -o hw3 hw3.c -lglut -lGLU -lGL -lm
      ```

   #### On Windows (using MinGW):
      ```
      gcc -O3 -Wall -DUSEGLEW -o hw3.exe hw3.c -lfreeglut -lglew32 -lglu32 -lopengl32
      ```

3. Then run the program as described above.


## Time to Complete the Assignment:
### Total Time: 10 hours
- Research and Setup: 3 hour
- Coding and Implementation: 6 hours
- Testing and Debugging: 3 hour
- Writing README: 1 hour