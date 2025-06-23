// ---------------------------------------------
// shaders.h - Shader management system
// ---------------------------------------------

#ifndef SHADERS_H
#define SHADERS_H

/* Loads vertex and fragment shaders from files
   Returns program ID after compilation/linking */
int loadShader(const char* vertexFile, const char* fragmentFile);

/* Activates shader for rendering
   Use 0 to disable shaders */
void useShader(int shader);

/* Frees shader program resources */
void deleteShader(int shader);

#endif