#include <stdio.h>
#include <string.h>

#include "../md5.h"


#define EXPECT(expr) do {                                                               \
        if (!(expr)) {                                                 \
            fflush(stdout);                                            \
            fflush(stderr);                                            \
            fprintf(stderr, "unexpect-> %s:%d\n", __FILE__, __LINE__); \
        }                                                              \
    } while (0)


void _md5_test_framework(const char *input, const char *expect_digest)
{
    struct md5_context *sc;
    sc = 0;
    char digest[32+1] = { 0 };
    size_t per_size = 3;
    size_t input_size = strlen(input);

    md5_create(&sc);
    md5_clear(sc);

    for (; input_size > 0;) {
        per_size = input_size > per_size ? per_size : input_size;
        input_size -= per_size;
        md5_update(sc, input, per_size);
        input += per_size;
    }

    md5_final(sc);

    md5_digest(sc, digest);
    md5_release(&sc);

    EXPECT(0 == memcmp(digest, expect_digest, 32));
}

void test_md5_1()
{
    _md5_test_framework("hello", "5d41402abc4b2a76b9719d911017c592");

    printf("pass test_md5_1()\n");
}

void test_md5_2()
{
    _md5_test_framework("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", "8215ef0796a20bcaaae116d3876c664a");

    printf("pass test_md5_2()\n");
}


int main()
{
    test_md5_1();
    test_md5_2();
    return 0;
}
