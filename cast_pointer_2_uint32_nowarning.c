
#include <stdio.h>
#include <stdint.h>

int main()
{
    int v = 0;
    int  * p = &v;

    uint32_t x = (uint32_t)(intptr_t)p;

    printf("main\n");
    return 0;
}
