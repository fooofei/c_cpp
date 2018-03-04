#include <stdio.h>

void test_core_pattern_pipe()
{
    const char * file = "/docker_host/cores/.calledc";
    FILE * f=0;
    f = fopen(file,"wb");
    if(f)
    {
        fprintf(f,"%s","x");
        fclose(f);
    }
}


int main()
{
    test_core_pattern_pipe();
    return 0;
}