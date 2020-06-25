#include <stdio.h>
#include <stdint.h>


int main()
{
    uint32_t i = 0;
    uint8_t b[1028];


    /*
      linux
        debug - core dumped
        release - no-core
      Windows x86
        debug - no crash
        release - no crash
    */
    b[i] = b[i - 1];

    return 0;
}
