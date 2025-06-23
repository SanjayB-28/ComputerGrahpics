// ---------------------------------------------
// projection.c - OpenGL projection matrix setup
// ---------------------------------------------

#include "CSCIx229.h"

// Sets projection matrix (perspective or orthographic)
// fov: Field of view (0 for orthographic)
// asp: Aspect ratio
// dim: Scene dimensions
void Project(double fov,double asp,double dim)
{
   // Projection matrix
   glMatrixMode(GL_PROJECTION);
   // Reset
   glLoadIdentity();
   // Perspective
   if (fov)
      gluPerspective(fov,asp,dim/16,16*dim);
   // Orthographic
   else
      glOrtho(-asp*dim,asp*dim,-dim,+dim,-dim,+dim);
   // Modelview matrix
   glMatrixMode(GL_MODELVIEW);
   // Reset matrix
   glLoadIdentity();
}

