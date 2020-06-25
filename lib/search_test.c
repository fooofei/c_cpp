
#include <stdio.h>

#include "search.h"

#define _array_size(array) sizeof(array) / sizeof((array)[0])
#define _array_end(array) (array) + sizeof(array) / sizeof((array)[0])
#define _array_begin_end(array) (array), _array_end(array)

#define EXPECT(expr) do {                                                               \
        if (!(expr)) {                                                 \
            fflush(stdout);                                            \
            fflush(stderr);                                            \
            fprintf(stderr, "unexpect-> %s:%d\n", __FILE__, __LINE__); \
        }                                                              \
    } while (0)


void _test_search_framework(const uint32_t *b_dst, uint32_t size, const uint32_t *b_search, const uint32_t *e_search,
    uint32_t (*pfn)(const uint32_t *, uint32_t size, const uint32_t))
{
    const uint32_t *p;
    uint32_t r;

    for (p = b_search; p < e_search; p += 2) {
        r = pfn(b_dst, size, *p);

        if (!(r == *(p + 1))) {
            fprintf(stderr, "unexpect -> %s:%d, search %u, expect index=%u, result index=%u\n", __FILE__, __LINE__, *p,
                *(p + 1), r);
            fflush(stderr);
        }
    }
}

void _test_search_lower_bound()
{
    // lower_bound 找的是第一个不小于 (>=)
    uint32_t array[] = { 1, 2, 3, 3, 3, 5, 5, 5, 6, (uint32_t)(-1) }; // -1 用来占位，防止末尾解引用发生崩溃

    // search, result pair
    uint32_t array_search[] = {
        1, 0
        , 
        6, (uint32_t)(ARRAY_END(array) - 1 - array - 1)
        , 
        3, 2
        , 
        0, 0
        , 
        4, 5
        , 
        9, (uint32_t)(ARRAY_END(array) - array - 1)
    };

    _test_search_framework(array, _array_size(array), _array_begin_end(array_search), lower_bound);
}

void _test_search_upper_bound()
{
    uint32_t array[] = { 1, 2, 3, 3, 3, 5, 5, 5, 6, (uint32_t)(-1) }; // -1 用来占位，防止末尾解引用发生崩溃

    // search, result pair
    uint32_t array_search[] = {
        1, 1
        , 
        6, (uint32_t)(ARRAY_END(array) - 1 - array)
        , 
        3, 5
        , 
        0, 0
        , 
        4, 5
        , 
        9, (uint32_t)(ARRAY_END(array) - array - 1)
    };


    _test_search_framework(array, _array_size(array), _array_begin_end(array_search), upper_bound);
}

void test_binary_search()
{
    uint32_t array1[] = { 3 };
    EXPECT(0 == lower_bound(array1, _array_size(array1), 3));
    EXPECT(0 == lower_bound(array1, _array_size(array1), 1));
    EXPECT(_array_size(array1) == lower_bound(array1, _array_size(array1), 5));

    uint32_t array2[] = { 1, 2, 3, 3, 3, 4, 5, 6 };
    EXPECT(0 == lower_bound(array2, _array_size(array2), 1));
    EXPECT(2 == lower_bound(array2, _array_size(array2), 3));
    EXPECT(7 == lower_bound(array2, _array_size(array2), 6));
    EXPECT(_array_size(array2) == lower_bound(array2, _array_size(array2), 9));
    EXPECT(0 == lower_bound(array2, _array_size(array2), 0));

    _test_search_lower_bound();
    _test_search_upper_bound();

    printf("pass test_binary_search()\n");
}

int main()
{
    test_binary_search();
    return 0;
}