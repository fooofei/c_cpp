#ifndef FMEM_H
#define FMEM_H

#include <stdio.h>

// not test on macOS
// https://github.com/NimbusKit/memorymapping/blob/master/src/fmemopen.c maybe useful
// http://kvasir.sr.bham.ac.uk/redmine/projects/finesse/repository/revisions/d843ad5f1dc0cd488cf4135712abe7f52c155dc8/entry/src/fmemopen.c
// fopencookie

// set FILE to use memory buffer
int fmemopen2(FILE **self, void *ptr, size_t size, const char *mode);

#endif // FMEM_H
