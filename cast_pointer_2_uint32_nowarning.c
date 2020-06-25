// the file shows how to cast one pointer to uint32_t safely no warning

#include <stdio.h>
#include <stdint.h>

int main()
{
    int v = 0;
    int *p = &v;

    /* if no intptr_t, here will have warning in x64 platforms */
    uint32_t x = (uint32_t)(intptr_t)p;

    p = (int *)(intptr_t)x;

    printf("int type cast no warning\n");
    return 0;
}
