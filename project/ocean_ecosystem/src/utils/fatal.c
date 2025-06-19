#include "CSCIx229.h"

//
//  Convenience routine to output fatal error message
//  (from ex8 of CSCI x229)
//
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}