#include "CSCIx229.h"

int projMode = 0;    // 0=Ortho, 1=Persp, 2=First Person
int th = 45, ph = 25;
int fov = 55;
double asp = 1;
double dim = 4.0;

const double TURN_STEP = 5.0;
const double MOVE_STEP = 0.20;

double fpX = 0.0, fpY = 1.5, fpZ = 6.0;
int fpTh = 0, fpPh = -10;

void Project(int projMode, double asp, double dim, int fov)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (projMode)
      gluPerspective(fov, asp, dim / 16, 16 * dim);
   else
      glOrtho(-asp * dim, +asp * dim, -dim, +dim, -10 * dim, +10 * dim);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void display()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();

   if (projMode == 2)
   {
      double Cx = -Sin(fpTh) * Cos(fpPh);
      double Cy = +Sin(fpPh);
      double Cz = -Cos(fpTh) * Cos(fpPh);
      gluLookAt(fpX, fpY, fpZ, fpX + Cx, fpY + Cy, fpZ + Cz, 0, 1, 0);
   }
   else
   {
      double Ex = -2 * dim * Sin(th) * Cos(ph);
      double Ey = +2 * dim * Sin(ph);
      double Ez = +2 * dim * Cos(th) * Cos(ph);
      gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, 1, 0);
   }

   drawFloor(-1.60, 20.0, 20.0, 0.10, 0.353, 0.510, 0.494);
   drawDesk(0, 0, 0, 3.0, 0.1, 1.5, 0);
   drawMonitor(0.8, 0.1, -0.8, 0.8, -15);
   drawMacBook(-1.3, 0.1, -0.4, 1, 25, 20);
   drawKeyboard(-0.4, 0.1, 0.8, 0.5, 0);
   drawMousePad(1.0, 0.1, 0.8, 0.5, 0.4, 0);
   drawMouse(1.0, 0.12, 0.8, 0.7, 0);
   drawNotebook(-2.2, 0.1, 0.8, 0.8, 0.1, 1.0, 0, 1.0, 0.5, 0.5);
   drawNotebook(2.0, 0.1, -0.6, 0.7, 0.12, 0.9, -15, 0.32, 0.5, 1.0);
   drawNotebook(-2.6, 0.1, -0.8, 0.9, 0.1, 0.6, 110, 0.5, 1.0, 0.5);
   drawNotebook(2.4, 0.1, 1.0, 0.9, 0.12, 1.1, -10, 0.70f, 0.60f, 1.00f);
   drawNotebook(2.4, 0.1 + 0.12, 1.0, 0.85, 0.10, 1.05, -25, 1.00f, 0.85f, 0.35f);

   glWindowPos2i(5, 5);
   const char* modes[] = {"Overhead Orthogonal", "Overhead Perspective", "First Person"};
   Print("Projection: %s; Dim=%.1f FOV=%d", modes[projMode], dim, fov);

   glFlush();
   glutSwapBuffers();
}

void special(int key, int x, int y)
{
   if (key == GLUT_KEY_RIGHT)
   {
      if (projMode == 2) fpTh -= TURN_STEP;
      else th -= TURN_STEP;
   }
   else if (key == GLUT_KEY_LEFT)
   {
      if (projMode == 2) fpTh += TURN_STEP;
      else th += TURN_STEP;
   }
   else if (key == GLUT_KEY_UP)
   {
      if (projMode == 2) fpPh = fmin(89, fpPh + TURN_STEP);
      else ph = ph + TURN_STEP;
   }
   else if (key == GLUT_KEY_DOWN)
   {
      if (projMode == 2) fpPh = fmax(-89, fpPh - TURN_STEP);
      else ph = ph - TURN_STEP;
   }

   th %= 360;
   ph %= 360;
   fpTh %= 360;
   Project(projMode, asp, dim, fov);
   glutPostRedisplay();
}

void key(unsigned char ch, int x, int y)
{
   if (ch == 27)
      exit(0);
   else if (ch == '0')
   {
      th = 45; ph = 25;
      fpX = 0.0; fpY = 1.5; fpZ = 6.0;
      fpTh = 0; fpPh = -10;
      dim = 4; fov = 55;
   }
   else if (ch == 'p' || ch == 'P')
      projMode = (projMode + 1) % 3;
   else if (ch == 'z')
   {
      if (projMode) fov--;
      else dim -= 0.1;
   }
   else if (ch == 'Z')
   {
      if (projMode) fov++;
      else if (dim > 1) dim += 0.1;
   }
   if (projMode == 2)
   {
      const double s = MOVE_STEP;
      if (ch == 's' || ch == 'S')
      {
         fpX -= s * -Sin(fpTh);
         fpZ -= s * -Cos(fpTh);
      }
      else if (ch == 'w' || ch == 'W')
      {
         fpX += s * -Sin(fpTh);
         fpZ += s * -Cos(fpTh);
      }
      else if (ch == 'a' || ch == 'A')
      {
         fpX += s * -Cos(fpTh);
         fpZ += s * Sin(fpTh);
      }
      else if (ch == 'd' || ch == 'D')
      {
         fpX += s * Cos(fpTh);
         fpZ += s * -Sin(fpTh);
      }
   }
   Project(projMode, asp, dim, fov);
   glutPostRedisplay();
}

void reshape(int width, int height)
{
   asp = (height > 0) ? (double)width / height : 1;
   glViewport(0, 0, width, height);
   Project(projMode, asp, dim, fov);
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(800, 600);
   glutCreateWindow("Homework 2: Sanjay Baskaran");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
   glutMainLoop();
   return 0;
}
