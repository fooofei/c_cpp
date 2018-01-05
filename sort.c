

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "sort.h"

typedef uint32_t index_t;
typedef uint32_t value_t;
typedef uint32_t * pointer_t;

void 
_sort_show_array_items_address(const uint32_t * b, const uint32_t * e)
{
    uint32_t i = 0;

    for (i = 0; b < e; i += 1, b += 1)
    {
        printf("[%u] %p %u\n", i, b, *b);
    }

    printf("\n");
    fflush(stdout);
}

pointer_t
partition(pointer_t left, pointer_t right)
{
    
    pointer_t ppivot;
    value_t pivot;
    value_t t;

    if (!(left<right))
    {
        return left;
    }

    // _sort_show_array_items_address(left, right);
   

    ppivot = left;
    pivot = *ppivot;
    for (left += 1; ; )
    {
        // 有等号是稳定排序
        for (; left < right && (*left <= pivot); left += 1);
        
        for (; right - 1 > ppivot && *(right - 1) >= pivot; right -= 1);

        if (left < right-1)
        {
            t = *left;
            *left = *(right-1);
            *(right-1) = t;
        }
        else
        {
            break;
        }
    }

    left -= 1;

    if (left < right &&  ppivot != left)
    {
        t = *ppivot;
        *ppivot = *left;
        *left = t;
    }
    return left;
}


void 
quick_sort(pointer_t begin, pointer_t end)
{
    pointer_t p;
    if (begin<end && begin +1 <end)
    {
        p = partition(begin, end);
        if (p>begin)
        {
            quick_sort(begin, p);
        }
        if (p<end)
        {
            quick_sort(p + 1, end);
        }
    }
}


void 
_test(pointer_t b1, pointer_t e1, const pointer_t b2)
{
    quick_sort(b1, e1);

    if (0 != memcmp(b1, b2, (e1 - b1) * sizeof(value_t)))
    {
        fprintf(stderr, "unexpect %s:%d\n", __FILE__, __LINE__);
        fflush(stdout);
    }
}



#define _array_size(array) sizeof(array)/sizeof((array)[0])
#define _array_end(array) (array)+sizeof(array)/sizeof((array)[0])
#define _array_begin_end(array) (array),_array_end(array)

void test_quick_sort()
{
    value_t array1[] = { 19, 12, 6, 15 };
    value_t array1_expect[] = { 6, 12, 15, 19 };
    _test(_array_begin_end(array1), array1_expect);


    value_t array4[] = { 3 };
    value_t array4_expect[] = { 3 };
    _test(_array_begin_end(array4), array4_expect);

    value_t array5[] = { 3 ,2 };
    value_t array5_expect[] = { 2,3 };
    _test(_array_begin_end(array5), array5_expect);

    value_t array7[] = { 1, 3 ,2};
    value_t array7_expect[] = { 1,2,3 };
    _test(_array_begin_end(array7), array7_expect);
   
    value_t array6[] = { 3, 15, 9, 10, 15 };
    value_t array6_expect[] = { 3, 9, 10, 15, 15 };
    _test(_array_begin_end(array6), array6_expect);

    value_t array2[] = { 2, 2, 12, 5, 8, 4, 18, 5, 4, 3 };
    value_t array2_expect[] = { 2, 2, 3, 4, 4, 5, 5, 8, 12, 18 };
    _test(_array_begin_end(array2), array2_expect);


    value_t array3[] = { 18, 5, 18, 15, 17, 12, 14, 18, 9, 16 };
    value_t array3_expect[] = { 5, 9, 12, 14, 15, 16, 17, 18, 18, 18 };
    _test(_array_begin_end(array3), array3_expect);

    printf("pass test_quick_sort()\n");
}
