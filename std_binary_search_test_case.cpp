


#include <algorithm>

#include "common.h"



void
_test_std_framework(
    const uint32_t * b_dst, const uint32_t * e_dst
    , const uint32_t * b_search, const uint32_t * e_search
    , const uint32_t * (*pfn)(const uint32_t *, const uint32_t *, const uint32_t &)
)
{

    const uint32_t * p;
    const uint32_t * r;

    for (p = b_search; p<e_search; p += 2)
    {
        r = pfn(b_dst, e_dst, *p);

        if (!(r - b_dst == (*(p + 1))))
        {
            fprintf(stderr, "unexpect -> %s:%d, search %u, expect index=%u, result index=%u\n", __FILE__, __LINE__
                , *p
                , *(p + 1)
                , r - b_dst
            );
            fflush(stderr);
        }

    }

}

void
_test1()
{
    // lower_bound 找的是第一个不小于 (>=)
    uint32_t array[] = { 1,2,3,3,3,5,5,5,6,(uint32_t)(-1) };// -1 用来占位，防止末尾解引用发生崩溃

                                                          // search, result pair
    uint32_t array_search[] = {
        1,0
        ,6, (uint32_t)(_array_end(array) - 1 - array - 1)
        ,3,2
        ,0,0
        ,4,5
        ,9,(uint32_t)(_array_end(array) - array - 1)
    };

    _test_std_framework(_array_begin_end(array) - 1, _array_begin_end(array_search), std::lower_bound);
}

/*
lower_bound 不适合 从大到小 的数组
*/
void
_test2()
{
    // lower_bound
    uint32_t array[] = { 6, 5,5,5,5,3,3,3,3,2,1 , (uint32_t)(-1) };// -1 用来占位，防止末尾解引用发生崩溃
    uint32_t array_search[] = {
        1,(uint32_t)(_array_end(array) - 1 - array - 1)  // NOT
        ,6,0  // NOT
        ,3,0
        ,0,0
        ,4,0 // NOT
        ,9,(uint32_t)(_array_end(array) - array - 1) };


    _test_std_framework(_array_begin_end(array) - 1, _array_begin_end(array_search), std::lower_bound);
}


/* upper_bound 找第一个大于 (>) */
void
_test3()
{
    uint32_t array[] = { 1,2,3,3,3,5,5,5,6,(uint32_t)(-1) };// -1 用来占位，防止末尾解引用发生崩溃

                                                          // search, result pair
    uint32_t array_search[] = {
        1,1
        ,6, (uint32_t)(_array_end(array) - 1 - array)
        ,3,5
        ,0,0
        ,4,5
        ,9,(uint32_t)(_array_end(array) - array - 1)
    };


    _test_std_framework(_array_begin_end(array) - 1, _array_begin_end(array_search), std::upper_bound);
}

/* upper_bound 找第一个大于 (>)
upper_bound 不适合 从大到小 的数组
*/
void
_test4()
{
    uint32_t array[] = { 6, 5,5,5,5,3,3,3,3,2,1 , (uint32_t)(-1) };// -1 用来占位，防止末尾解引用发生崩溃

    uint32_t array_search[] = {
        1,0
        ,6, (uint32_t)(_array_end(array) - 1 - array)
        ,3,0 // NOT
        ,0,0
        ,4,0 // NOT
        ,9,(uint32_t)(_array_end(array) - array - 1)
    };


    _test_std_framework(_array_begin_end(array) - 1, _array_begin_end(array_search), std::upper_bound);
}

void 
test_std_binary_search()
{
    _test1();
    _test3();
    printf("pass test_std_binary_search()\n");
}
