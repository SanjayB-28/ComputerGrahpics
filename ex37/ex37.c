/*
 *  Asteroids
 *
 *  Demonstrates using Gouraud Shading and Selection
 *
 *  Asteroid models from DAMIT database of asteroid models
 *     https://www.aanda.org/articles/aa/full_html/2010/05/aa12693-09/aa12693-09.html
 *  and
 *     https://astro.troja.mff.cuni.cz/projects/damit/
 *
 *  Key bindings:
 *  mouse      Left=rotate, right=pan, wheel=zoom
 *  arrows     Change view angle
 *  0          Reset view angle
 *  1          Back view
 *  ESC        Exit
 */
#include "CSCIx229.h"
int th=0;        //  Azimuth of view angle
int ph=0;        //  Elevation of view angle
double fov=45;   //  Field of view
double asp=1;    //  Aspect ratio
double dim=1000; //  Size of world
double time=0;   //  Time
int mx=0,my=0;   //  Mouse coordinates
int move=0;      //  Mouse movement
double Ox=0,Oy=0,Oz=0; //  Origin

//  Vector 3 math
typedef struct {float x,y,z;} vec3;
vec3 add(vec3 v1,vec3 v2) {return (vec3){v1.x+v2.x , v1.y+v2.y , v1.z+v2.z};}
vec3 sub(vec3 v1,vec3 v2) {return (vec3){v1.x-v2.x , v1.y-v2.y , v1.z-v2.z};}
vec3 cross(vec3 v1,vec3 v2) {return (vec3){v1.y*v2.z-v2.y*v1.z , v1.z*v2.x-v2.z*v1.x , v1.x*v2.y-v2.x*v1.y};}
vec3 normalize(vec3 v)
{
   float d = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
   if (d>0) v = (vec3){v.x/d , v.y/d , v.z/d};
   return v;
}
// Random float between min and max
float frand(float min,float max) {return min+rand()*(max-min)/RAND_MAX;}

// Asteroids
#define N 8
char* name[N] = {"Daphne","Elektra","Eros","Juno","Lutetia","Lydia","Psyche","Thetis"};
typedef struct {vec3 xyz,nml;} vertex;
typedef struct
{
   int k;            // Object number
   int n;            // Number of element pointers
   unsigned int vbo; // Vertex buffer object
   unsigned int ebo; // Element buffer object
   vec3 pos;         // Position
   vec3 axis;        // Rotation axis
   float per;        // Rotation period
} ast_t;
ast_t asteroids[N];

/*
 *  Draw asteroid
 */
static void DrawAsteroid(ast_t ast,int outline)
{
   //  Apply transformations
   glPushMatrix();
   glTranslated(ast.pos.x,ast.pos.y,ast.pos.z);
   glRotated(fmod(time/ast.per,1.0)*360 , ast.axis.x,ast.axis.y,ast.axis.z);
   //  Set color with alpha value equal to object
   if (outline)
   {
      glEnable(GL_POLYGON_OFFSET_LINE);
      glPolygonOffset(-1,-1);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glColor4ub(255,0,0,ast.k);
   }
   else
      glColor4ub(255,255,255,ast.k);
   //  Map vertex and normal
   glBindBuffer(GL_ARRAY_BUFFER,ast.vbo);
   glVertexPointer(3,GL_FLOAT,6*sizeof(float),(void*)0);
   glEnableClientState(GL_VERTEX_ARRAY);
   glNormalPointer(GL_FLOAT,6*sizeof(float),(void*)(3*sizeof(float)));
   glEnableClientState(GL_NORMAL_ARRAY);
   //  Map elements and draw
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ast.ebo);
   glDrawElements(GL_TRIANGLES,ast.n,GL_UNSIGNED_INT,(void*)0);
   //  Unmap buffers
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glBindBuffer(GL_ARRAY_BUFFER,0);
   //  Undo changes
   glPopMatrix();
   if (outline)
   {
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glDisable(GL_POLYGON_OFFSET_LINE);
   }
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer (a=255)
   glClearColor(0,0,0,1);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering
   glEnable(GL_DEPTH_TEST);
   //  Set perspective
   double Ex = Ox-2*dim*Sin(th)*Cos(ph);
   double Ey = Oy+2*dim        *Sin(ph);
   double Ez = Oz+2*dim*Cos(th)*Cos(ph);
   glLoadIdentity();
   gluLookAt(Ex,Ey,Ez , Ox,Oy,Oz , 0,Cos(ph),0);
   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   //  Enable light 0
   glEnable(GL_LIGHT0);
   //  Set ambient, diffuse, specular components and position of light 0
   float Ambient[]   = {0.01 ,0.01 ,0.01 ,1.0};
   float Diffuse[]   = {0.50 ,0.50 ,0.50 ,1.0};
   float Specular[]  = {0.20 ,0.20 ,0.20 ,1.0};
   float Position[]  = {1,1,1,0}; // Directional light (w=0)
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
   //  Set specular color to white
   float white[] = {1,1,1,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,4);

   //  Draw asteroids
   for (int k=0;k<N;k++)
      DrawAsteroid(asteroids[k],0);
   //  Flush before reading screen
   glFlush();

   //  Read what is under the cursor
   unsigned char mc[4];
   glReadPixels(mx,my,1,1,GL_RGBA,GL_UNSIGNED_BYTE,mc);
   float mz;
   glReadPixels(mx,my,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&mz);
   //  Convert pixel to world coordinates
   int VP[4];
   double PRJ[16],MV[16],X,Y,Z;
   glGetDoublev(GL_PROJECTION_MATRIX,PRJ);
   glGetDoublev(GL_MODELVIEW_MATRIX,MV);
   glGetIntegerv(GL_VIEWPORT,VP);
   gluUnProject(mx,my,mz,MV,PRJ,VP,&X,&Y,&Z);

   //  Draw selected asteroid
   int obj = mc[3];
   if (obj<N) DrawAsteroid(asteroids[obj],1);
   
   //  Display parameters
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f RGBA=%.3d,%.3d,%.3d,%.3d Z=%f",th,ph,dim,mc[0],mc[1],mc[2],mc[3],mz);
   //  Display pick
   glWindowPos2i(5,25);
   if (obj<N) Print("%s xyz=%.1f,%.1f,%.1f ",name[obj],X,Y,Z);
   
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(45,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch==27)
      exit(0);
   //  Reset view angle
   else if (ch=='0')
      th = ph = Ox = Oy = Oz = 0;
   else if (ch=='1')
   {
      th = -180;
      ph = 0;
   }
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Set projection
   Project(fov,asp,dim);
}

/*
 *  Load asteroid
 */
ast_t LoadAsteroid(int k)
{
   ast_t ast = {.k=k};
   //  Build filename
   char file[256];
   strcpy(file,name[k]);
   strcat(file,".txt");
   //  Open file and read vertex and element counts
   FILE* f = fopen(file,"r");
   if (!f) Fatal("Cannot open file %s\n",file);
   int Nv,Ne;
   if (fscanf(f,"%d %d",&Nv,&Ne)!=2) Fatal("Error reading counts from %s\n",file);
   if (Nv<1 || Ne<1) Fatal("Invalid counts vertex=%d element=%d\n",Nv,Ne);
   //  Read vertexes
   vertex* V = (vertex*)malloc(Nv*sizeof(vertex));
   if (!V) Fatal("Cannot allocate %d vertexes\n",Nv);
   for (int i=0;i<Nv;i++)
      if (fscanf(f,"%f %f %f",&V[i].xyz.x,&V[i].xyz.y,&V[i].xyz.z)!=3)
         Fatal("Error reading vertex %d from %s\n",i,file);
   //  Allocate and read elements
   Ne *= 3;
   int* E = (int*)malloc(Ne*sizeof(int));
   if (!E) Fatal("Cannot allocate %d elements\n",Ne);
   for (int i=0;i<Ne;i+=3)
   {
      int a,b,c;
      if (fscanf(f,"%d %d %d",&a,&b,&c)!=3)
         Fatal("Error reading element %d from %s\n",i,file);
      E[i+0] = a-1;
      E[i+1] = b-1;
      E[i+2] = c-1;
   }
   fclose(f);
   //  Gouraud averaging of normals
   //  If triangle normals are unit length, just sum them
   //  at each vertex.  GL_NORMALIZE rescales the result.
   for (int i=0;i<Nv;i++)
      V[i].nml = (vec3){0,0,0}; 
   for (int i=0;i<Ne;i+=3)
   {
      //  Compute normal for triangle
      vec3 A = sub(V[E[i+1]].xyz , V[E[i+0]].xyz);
      vec3 B = sub(V[E[i+2]].xyz , V[E[i+1]].xyz);
      vec3 n = normalize(cross(A,B));
      //  Add triangle normal to vertexes
      for (int j=0;j<3;j++)
         V[E[i+j]].nml = add(V[E[i+j]].nml , n);
   }
   //  Copy vertex and normal data to VBO
   glGenBuffers(1,&ast.vbo);
   glBindBuffer(GL_ARRAY_BUFFER,ast.vbo);
   glBufferData(GL_ARRAY_BUFFER,Nv*sizeof(vertex),V,GL_STATIC_DRAW);
   free(V);
   //  Copy element data to EBO
   ast.n = Ne;
   glGenBuffers(1,&ast.ebo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ast.ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER,Ne*sizeof(int),E,GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
   free(E);
   //  Assign random location
   ast.pos.x = frand(-0.5*dim,+0.5*dim);
   ast.pos.y = frand(-0.5*dim,+0.5*dim);
   ast.pos.z = frand(-0.5*dim,+0.5*dim);
   //  Assign random rotation axis and period
   float th = frand(-M_PI,+M_PI);
   float ph = frand(-M_PI/2,+M_PI/2);
   ast.axis.x = -sin(th)*cos(ph);
   ast.axis.y =         +sin(ph);
   ast.axis.z = +cos(th)*cos(ph);
   ast.per    = frand(2,10);
   //  Return struct
   return ast;
}

/*
 *  Mouse coordinates
 */
void motion(int x,int y)
{
   mx = x;
   my = glutGet(GLUT_WINDOW_HEIGHT)-y;
}

/*
 *  Mouse click
 */
void mouse(int key,int status,int x,int y)
{
   //  Record new mouse position
   mx = x;
   my = glutGet(GLUT_WINDOW_HEIGHT)-y;
   //  Left button is rotate
   if (status==GLUT_DOWN && key==GLUT_LEFT_BUTTON)
      move = 1;
   //  Right button is pan
   else if (status==GLUT_DOWN && key==GLUT_RIGHT_BUTTON)
      move = -1;
   //  On button up, unset move
   else if (status==GLUT_UP)
      move = 0;
   //  Wheel reports as button 3(scroll up) and button 4(scroll down)
   else if (key==3)
      dim += 10;
   else if (key==4 && dim>100)
      dim -= 10;
   //  Update screen
   glutPostRedisplay();
}

/*
 *  Mouse drag
 */
void drag(int x,int y)
{
   //  Rotate
   if (move>0)
   {
      th += x-mx;
      ph -= glutGet(GLUT_WINDOW_HEIGHT)-y-my;
   }
   //  Pan
   else if (move<0)
   {
      int dx = +5*(x-mx);
      int dy = -5*(glutGet(GLUT_WINDOW_HEIGHT)-y - my);
      Ox += -dx*Cos(th)*Cos(ph) + dy*Sin(th)*Sin(ph);
      Oy += +dx        *Sin(ph) + dy        *Cos(ph);
      Oz += -dx*Sin(th)*Cos(ph) - dy*Cos(th)*Sin(ph);
   }
   mx = x;
   my = glutGet(GLUT_WINDOW_HEIGHT)-y;
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering and alpha at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ALPHA);
   glutInitWindowSize(600,600);
   glutCreateWindow("DAMIT Asteroids");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutMouseFunc(mouse);
   glutMotionFunc(drag);
   glutPassiveMotionFunc(motion);
   glutIdleFunc(idle);

   //  Load asteroids
   srand(123);
   for (int k=0;k<N;k++)
      asteroids[k] = LoadAsteroid(k);

   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
