#include <stdio.h>


#include "asprintf.h"

#define EXPECT(expr) do {                                                         \
        if (!(expr)) {                                                            \
            fprintf(stderr, "unexpect %s  (%s:%d)\n", #expr, __FILE__, __LINE__); \
            fflush(stderr);                                                       \
        }                                                                         \
    } while (0)

void test_asprintf()
{
    char *p;
    int size;

    p = 0;
    size = asprintf(&p, "%s", "Hello");
    EXPECT(size == 5);
    EXPECT(0 == memcmp(p, "Hello", 5));
    free(p);

    printf("pass %s()\n", __FUNCTION__);
}

struct s_format_type_check {
    int a[0x200];
    int b;
    int c[0x200];
};

void test_format_type_check()
{
    char *p;

    struct s_format_type_check *st = calloc(1, sizeof(struct s_format_type_check));
    // i.b = 2;
    int size;

    memset(st, 0x01, sizeof(*st));
    p = 0;
    size = asprintf(&p, "%d %lld %d", st->a[0], (long long)st->b, st->c[0]);
    // size = asprintf(&p, "%d %lld %d", st->a[0], st->b, st->c[0]); // there will be error
    // MSVC 稳定复现这个 bug ， 但是 gcc 不稳定复现
    EXPECT(size = 26);
    EXPECT(0 == memcmp(p, "16843009 16843009 16843009", 26));
    free(p);
    free(st);
    printf("pass %s()\n", __func__);
}

int main()
{
    test_asprintf();
    test_format_type_check();
    return 0;
}
