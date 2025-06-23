// ---------------------------------------------
// fatal.c - Fatal error handling utility
// ---------------------------------------------

#include "CSCIx229.h"

// --- Error handling utilities ---
/*
   Prints formatted error message to stderr and terminates program
   Used for unrecoverable errors that require immediate shutdown
*/
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}
