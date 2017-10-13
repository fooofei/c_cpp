
#include <stdio.h>
#include <stdlib.h>


int 
foo(void ** outp, size_t * outl)
{

  *outp = malloc(10);
  *outl = 10;

  return 0;
}