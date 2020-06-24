#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#include "fmem.h"

#define EXPECT(expr) do {                                                                          \
        if (!(expr)) {                                                            \
            fprintf(stderr, "unexpect %s  (%s:%d)\n", #expr, __FILE__, __LINE__); \
            fflush(stderr);                                                       \
        }                                                                         \
    } while (0)


// 现在还不可用

static void test_fmem()
{
    char *buffer = 0;
    FILE *f = 0;
    int ret;
    size_t write;
    bool pass = false;

    enum {
        size = 5,
    };
    buffer = (char *)calloc(1, size);

    // macOS 非常正经 不会在空间不足时又从头开始写入
    fmemopen2(&f, &buffer[0], size, "wb");

    if (f) {
        write = fwrite("nice", 1, 4, f);
        fflush(f);
        EXPECT(write == 4);
        ret = fprintf(f, "hello");
        fflush(f);
        EXPECT(ret == 5); // 即使是空间不足  返回值也是 >0
        ret = fprintf(f, "world");
        fflush(f);
        EXPECT(ret == 5);


        fflush(f); // must call fflush
        printf("%s\n", buffer); // nice hello world

#ifdef WIN32
        pass = (0 == memcmp(buffer, "orld", 4)); // 现在又是 dorl
#else
        pass = (0 == memcmp(buffer, "niceh", 5));
#endif
        // pass = (0 == strcmp(buffer, "nicehelloworld"));
        EXPECT(pass);

        fclose(f);
    }

    if (buffer) {
        free(buffer);
    }

    printf("%s %s()\n", (pass ? "pass" : "fail"), __FUNCTION__);
}

int main()
{
    test_fmem();
    return 0;
}
