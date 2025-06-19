// CORRECTED INCLUDE PATH
#include "CSCIx229.h"

//
//  Convenience routine to check for errors
//
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR at %s: %s\n",where,gluErrorString(err));
}