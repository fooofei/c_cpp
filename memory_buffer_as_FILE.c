
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    char * buffer = 0;
    FILE * f = 0;

    enum{size=2048,};
    buffer = (char*)calloc(1,size);

#ifndef WIN32
    f = fmemopen(&buffer[0], size, "wb");
#endif
    if (f)
    {

        fwrite("nice", 4, 1, f);
        fprintf(f, "hello");
        fprintf(f, "world");
       
        fflush(f); // must call fflush
        printf("%s\n", buffer); // nice hello world

        fclose(f);
    }

    if (buffer)
    {
        free(buffer);
    }

    printf("end\n");

    return 0;
}
