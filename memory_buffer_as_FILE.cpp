
#include <stdio.h>
#include <stdint.h>
#include <string>

int main()
{

    std::string buffer;
    buffer.resize(2048);
    FILE * f = 0;

    f = fmemopen(&buffer[0], buffer.size(), "wb");


    if (f)
    {

        fwrite("nice", 4, 1, f);
        fprintf(f, "hello");

        fprintf(f, "world");

        fflush(f);

        printf("%s\n", buffer.c_str()); // nice hello world


        fclose(f);
    }

    printf("end\n");



   
    return 0;
}
