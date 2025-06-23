// print.c - Text output utility for OpenGL

#include "CSCIx229.h"

// Maximum text buffer size
#define LEN 8192

// Renders formatted text at raster position
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   
   // Format string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   
   // Render characters
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}