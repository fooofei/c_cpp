#include <stdio.h>
#include <string.h>

#include "../sha1.h"


#define EXPECT(expr) do {                                              \
        if (!(expr)) {                                                 \
            fflush(stdout);                                            \
            fflush(stderr);                                            \
            fprintf(stderr, "unexpect-> %s:%d\n", __FILE__, __LINE__); \
        }                                                              \
    } while (0)


void _sha1_test_framework(const char *input, const char *expect_digest)
{
    struct sha1_context *sc;
    sc = 0;
    char digest[41] = { 0 };
    size_t per_size = 3;
    size_t input_size = strlen(input);

    sha1_create(&sc);
    sha1_clear(sc);

    for (; input_size > 0;) {
        per_size = input_size > per_size ? per_size : input_size;
        input_size -= per_size;
        sha1_update(sc, input, per_size);
        input += per_size;
    }

    sha1_final(sc);

    sha1_digest(sc, digest);
    sha1_release(&sc);

    EXPECT(0 == memcmp(digest, expect_digest, 40));
}

void test_sha1_1()
{
    _sha1_test_framework("hello", "aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d");

    printf("pass test_sha1_1()\n");
}

void test_sha1_2()
{
    _sha1_test_framework("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "84983e441c3bd26ebaae4aa1f95129e5e54670f1");

    printf("pass test_sha1_2()\n");
}


int main()
{
    test_sha1_1();
    test_sha1_2();
    return 0;
}
