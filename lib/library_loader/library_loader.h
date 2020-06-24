#include <stdint.h>


struct library_loader;

// open <path>'s library, return by ins
// path is utf-8 encoding
int library_open(struct library_loader **ins, const char *path);

void library_close(struct library_loader **ins);

int library_sym(struct library_loader *ins, const char *symbol, void **proc);

const char *library_fullpath(struct library_loader *ins);
