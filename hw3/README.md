# CSCI 5229 Computer Graphics - Homework 3

**Student:** Sanjay Baskaran  
**Assignment:** Textured & Lit 3-D Scene — “A Tiny Village”  
**Time to Complete:** ≈ 30 hours

---

## Project Overview

This program renders a small village plaza featuring rows of brick houses with shingled roofs, conifer-style trees, clouds, and a moving sun that doubles as the single positional light source. All geometry is constructed from user-defined cubes, cylinders, cones, and spheres, each with correct normals to ensure the lighting model functions as intended.

### Note on Scene Selection

Initially, I attempted to add lighting and textures to the scene I created for Homework 2. However, due to the increased complexity and limited time, I was not confident I could complete all the requirements for Homework 3 using that scene. To ensure I met all the assignment criteria and to allow time for the project, I switched to a simpler scene that effectively covers all the requirements for HW3.

---

## Controls

| Key         | Action                                         |
|-------------|------------------------------------------------|
| m / M       | Change the mode (Orthogonal, Perspective, First Person) |
| w / W       | Move forward in first person view              |
| s / S       | Move backward in first person view             |
| a / A       | Move left in first person view                 |
| d / D       | Move right in first person view                |
| Arrow Keys  | Move along Up, Down, Right, Left axes          |
| r / R       | Stop/Start the light source                    |
| j / J       | Increase light distance                        |
| k / K       | Decrease light distance                        |
| u / U       | Increase light height                          |
| g / G       | Decrease light height                          |
| 0           | Reset view, camera, and light to default       |
| x / X       | Toggle axis                                    |
| Esc         | Exit the program                               |

---

## Building and Running

To build the program, run:

```sh
make
```

To execute the program:

```sh
./hw3
```
