#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Initialize fractal tree system (if needed)
void fractalTreeInit();

// Draw a single fractal tree at (x, y, z) with a given scale and recursion depth
void fractalTreeDraw(double x, double y, double z, double scale, int depth, unsigned int treeSeed, int leafColorIndex);

// Cleanup fractal tree system (if needed)
void fractalTreeCleanup();

#ifdef __cplusplus
}
#endif 