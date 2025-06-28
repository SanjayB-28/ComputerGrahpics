#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void fractalTreeInit();
void fractalTreeDraw(double x, double y, double z, double scale, int depth, unsigned int treeSeed, int leafColorIndex);

#ifdef __cplusplus
}
#endif 