#include "CSCIx229.h"

void Project(int projMode, double asp, double dim, int fov)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   if (projMode)
      gluPerspective(fov, asp, dim/16, 16*dim);
   else
      glOrtho(-asp*dim, +asp*dim, -dim, +dim, -dim, +dim);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}
