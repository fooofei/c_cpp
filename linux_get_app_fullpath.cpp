
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int get_app_fullpath(char * buffer, unsigned * size)
{
    FILE * fp=NULL;

    if (!size) return -1;

    fp = fopen("/proc/self/maps", "r");
    void* p_symbol = (void*)"";
    int rt = -1;
    if (fp)
    {
        char line[0x800] = { 0 };
        for (;!feof(fp);)
        {
            memset(line, 0, sizeof(line));
            char * p = fgets(line, sizeof(line), fp);
            if (p)
            {
                char * p1 = strstr(line, " r-xp ");
                if (p1)
                {
                    char * p_slash = strstr(p1, "/");
                    if (p_slash)
                    {
                        unsigned long start = 0, end = 0;
                        sscanf(line, "%lx-%lx ", &start, &end);
                        if (p_symbol >= (void*)start && p_symbol < (void*)end)
                        {
                            char * path_end = strrchr(p_slash, '\n');
                            if (path_end) *path_end = 0;
                            unsigned path_size = (unsigned)strlen(p_slash);
                            if (path_size)
                            {
                                unsigned in_size = *size;
                                *size = path_size + 1;
                                if (buffer)
                                {
                                    if (!(in_size > path_size)) {
                                        rt = -1; break;
                                    }
                                    memcpy(buffer, p_slash, path_size);
                                    buffer[path_size] = 0;
                                    *size = path_size;
                                    rt = 0;
                                    break;
                                }
                                rt = 0;
                                break;
                            }
                        }
                    }
                    
                }
            }
        }

        fclose(fp);
    }

    return rt;
}



int main()
{
    int r = 0;
    unsigned size = 0;

    r = get_app_fullpath(NULL, &size);
    if (r==0 && size)
    {
        char * buf = (char *)malloc(size);
        r = get_app_fullpath(buf, &size);
        if (r==0)
        {
            printf("%s\n", buf);
        }
        free(buf);
    }

    return 0;
}