#include "sort.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>


typedef uint32_t index_t;
typedef uint32_t value_t;
typedef uint32_t *pointer_t;

pointer_t partition(pointer_t left, pointer_t right)
{
    pointer_t ppivot;
    value_t pivot;
    value_t t;

    if (!(left < right)) {
        return left;
    }

    ppivot = left;
    pivot = *ppivot;
    for (left += 1;;) {
        // 有等号是稳定排序
        for (; left < right && (*left <= pivot); left += 1) {
        }

        for (; right - 1 > ppivot && *(right - 1) >= pivot; right -= 1) {
        }
        if (left < right - 1) {
            t = *left;
            *left = *(right - 1);
            *(right - 1) = t;
        } else {
            break;
        }
    }

    left -= 1;

    if (left < right && ppivot != left) {
        t = *ppivot;
        *ppivot = *left;
        *left = t;
    }
    return left;
}


void quick_sort(pointer_t begin, pointer_t end)
{
    pointer_t p;
    if (begin < end && begin + 1 < end) {
        p = partition(begin, end);
        if (p > begin) {
            quick_sort(begin, p);
        }
        if (p < end) {
            quick_sort(p + 1, end);
        }
    }
}
