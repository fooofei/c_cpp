#include <stdio.h>


#include "sort.h"


void _test(uint32_t *b1, uint32_t *e1, const uint32_t *b2)
{
    quick_sort(b1, e1);

    if (0 != memcmp(b1, b2, (e1 - b1) * sizeof(uint32_t))) {
        fprintf(stderr, "unexpect %s:%d\n", __FILE__, __LINE__);
        fflush(stdout);
    }
}


#define _array_size(array) sizeof(array) / sizeof((array)[0])
#define _array_end(array) (array) + sizeof(array) / sizeof((array)[0])
#define _array_begin_end(array) (array), _array_end(array)

void test_quick_sort()
{
    uint32_t array1[] = { 19, 12, 6, 15 };
    uint32_t array1_expect[] = { 6, 12, 15, 19 };
    _test(_array_begin_end(array1), array1_expect);


    uint32_t array4[] = { 3 };
    uint32_t array4_expect[] = { 3 };
    _test(_array_begin_end(array4), array4_expect);

    uint32_t array5[] = { 3, 2 };
    uint32_t array5_expect[] = { 2, 3 };
    _test(_array_begin_end(array5), array5_expect);

    uint32_t array7[] = { 1, 3, 2 };
    uint32_t array7_expect[] = { 1, 2, 3 };
    _test(_array_begin_end(array7), array7_expect);

    uint32_t array6[] = { 3, 15, 9, 10, 15 };
    uint32_t array6_expect[] = { 3, 9, 10, 15, 15 };
    _test(_array_begin_end(array6), array6_expect);

    uint32_t array2[] = { 2, 2, 12, 5, 8, 4, 18, 5, 4, 3 };
    uint32_t array2_expect[] = { 2, 2, 3, 4, 4, 5, 5, 8, 12, 18 };
    _test(_array_begin_end(array2), array2_expect);


    uint32_t array3[] = { 18, 5, 18, 15, 17, 12, 14, 18, 9, 16 };
    uint32_t array3_expect[] = { 5, 9, 12, 14, 15, 16, 17, 18, 18, 18 };
    _test(_array_begin_end(array3), array3_expect);

    printf("pass test_quick_sort()\n");
}


int main()
{
    test_quick_sort();
    return 0;
}
