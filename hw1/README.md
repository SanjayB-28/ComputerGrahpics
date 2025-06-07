# Lorenz Attractor Visualization

This program renders a 3D visualization of the Lorenz attractor, a classic system of ordinary differential equations known for its chaotic behavior. The program allows for interactive control over the camera and the parameters of the Lorenz system.

This project is written in **C** using the **OpenGL** and **GLUT** libraries for graphics.

---

## Features

- **3D Lorenz Attractor:** Renders the path of the Lorenz system as a continuous, smoothly colored line.
- **Dynamic Coloring:** The color of the attractor's path changes based on its (x, y, z) position in space, helping to visualize its complex 3D structure.
- **Full 3D Camera Control:** Rotate the view freely using the arrow keys; reset to a default angle.
- **Interactive Parameter Control:** Modify the Lorenz system parameters (σ, β, ρ) in real-time to observe their effects.
- **Toggleable UI Elements:** Show/hide coordinate axes and the on-screen help legend.

---

## Compilation and Execution

This program is cross-platform (macOS, Linux, Windows). A `Makefile` is provided for easy compilation.

### Prerequisites

- **C compiler** (e.g., `gcc`)
- **GLUT** (or **freeglut**) library

#### Platform-specific Instructions

- **macOS:** Xcode Command Line Tools provide everything you need.
- **Linux (Debian/Ubuntu):**
  ```bash
  sudo apt-get update
  sudo apt-get install build-essential freeglut3-dev
  ```
- **Windows:** Set up a development environment (e.g., MinGW) and install the freeglut libraries.

---

## Instructions

1. Ensure `lorenz.c` and the `Makefile` are in the same directory.
2. Open a terminal or command prompt and navigate to that directory.
3. Compile the program:
   ```bash
   make
   ```
4. If successful, an executable named `lorenz` (or `lorenz.exe` on Windows) will be created.
5. Run the program:
   ```bash
   ./lorenz
   ```

---

## Controls

| Key(s)         | Action                                                        |
|----------------|--------------------------------------------------------------|
| `h` / `H`      | Toggle the on-screen controls legend                         |
| `d` / `D`      | Reset all parameters (view angle, σ, β, ρ) to defaults       |
| `0`            | Reset the view angle to default                              |
| `a` / `A`      | Toggle visibility of X (Red), Y (Green), Z (Blue) axes       |
| `s`            | Increase sigma (σ)                                           |
| `S` (Shift+s)  | Decrease sigma (σ)                                           |
| `b`            | Increase beta (β)                                            |
| `B` (Shift+b)  | Decrease beta (β)                                            |
| `r`            | Increase rho (ρ)                                             |
| `R` (Shift+r)  | Decrease rho (ρ)                                             |
| Up Arrow       | Rotate view upwards (increase elevation)                     |
| Down Arrow     | Rotate view downwards (decrease elevation)                   |
| Left Arrow     | Rotate view left (decrease azimuth)                          |
| Right Arrow    | Rotate view right (increase azimuth)                         |
| `ESC`          | Exit the program                                             |

---