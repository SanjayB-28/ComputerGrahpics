# CS5229 Computer Graphics - Homework 1

* **Student:** Snajay Basakran
* **Assignment:** A 3D OpenGL Visualization of the Lorenz Attractor
* **Time to Complete:** 6 hours

---

## Program Description

This program generates and displays a 3D visualization of the Lorenz attractor. It is written in C using the OpenGL and GLUT libraries. The core of the program is an integrator that calculates the trajectory of the system, which is then rendered as a continuous, colored line. The application allows for full interactive control over the camera and the attractor's mathematical parameters.

## Features

* **3D Lorenz Attractor:** Renders the path of the Lorenz system as a dynamic, multi-colored line. The color of the path changes based on its position in 3D space, which helps visualize its complex structure.
* **Full 3D Camera Control:** The view can be rotated freely using the arrow keys, and the view can be reset instantly to a default angle.
* **Interactive Parameter Control:** The core parameters of the Lorenz equations ($\sigma$, $\beta$, and $\rho$) can be modified in real-time to observe how they affect the attractor's shape.
## Additional Features Compared to Professor's Version

The following features were added beyond the version demonstrated in lectures:

* **Enhanced User Interface:**
   * An on-screen legend details all controls so the user does not have to refer back to the README.
   * The legend and coordinate axes can be toggled on and off for a clean view of the visualization.
   * A "Reset" function restores all parameters and the view angle to their default state.

## Compilation and Execution
To compile and run the program:

1. **Prerequisites:** Ensure you have a C compiler and the OpenGL and GLUT libraries installed on your system.
2. **Compile:** Open a terminal or command prompt, navigate to the source directory, and run:
   ```sh
   make
   ```
3. **Run:** Execute the compiled program:
   ```sh
   ./lorenz
   ```

## Controls

The program uses the keyboard for all controls. The program starts with the legend visible.

| Key(s)        | Action                                                  |
| ------------- | ------------------------------------------------------- |
| `h` / `H`     | Toggle the on-screen controls legend on or off.         |
| `d` / `D`     | Reset all parameters (view angle, $\sigma$, $\beta$, $\rho$) to their default values. |
| `0`           | Reset the view angle to its default starting position.  |
| `a` / `A`     | Toggle the visibility of the X (Red), Y (Green), and Z (Blue) axes. |
| `s`           | Increase the sigma ($\sigma$) parameter.                |
| `S` (Shift+s) | Decrease the sigma ($\sigma$) parameter.                |
| `b`           | Increase the beta ($\beta$) parameter.                 |
| `B` (Shift+b) | Decrease the beta ($\beta$) parameter.                 |
| `r`           | Increase the rho ($\rho$) parameter.                    |
| `R` (Shift+r) | Decrease the rho ($\rho$) parameter.                    |
| `Up Arrow`    | Rotate the view upwards (increase elevation).           |
| `Down Arrow`  | Rotate the view downwards (decrease elevation).         |
| `Left Arrow`  | Rotate the view to the left (decrease azimuth).         |
| `Right Arrow` | Rotate the view to the right (increase azimuth).        |
| `ESC`         | Exit the program.                                       |