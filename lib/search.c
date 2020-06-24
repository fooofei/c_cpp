#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "search.h"


uint32_t lower_bound(const uint32_t *addr, uint32_t size, const uint32_t val)
{
    uint32_t mid = 0;
    uint32_t off_from_begin = 0;

    for (; size > 0;) {
        mid = size / 2;
        if (addr[mid] < val) {
            mid += 1;
            size -= mid;
            addr += mid;
            off_from_begin += mid;
        } else {
            size = mid;
        }
    }

    return off_from_begin;
}


uint32_t upper_bound(const uint32_t *addr, uint32_t size, const uint32_t val)
{
    uint32_t mid = 0;
    uint32_t off_from_begin = 0;


    for (; size > 0;) {
        mid = size / 2;
        if (!(addr[mid] > val)) {
            mid += 1;
            size -= mid;
            addr += mid;
            off_from_begin += mid;
        } else {
            size = mid;
        }
    }

    return off_from_begin;
}

// 比较函数在序列的作用效果为  false false false ... true true ...
int bearch(const uint32_t *addr, int first, int last, uint32_t val)
{
    for (; first < last;) {
        int64_t mid64 = (int64_t)first + (int64_t)last;
        int mid = (int)(mid64 / 2);

        // 从小到大升序的 不可以是 < 比较
        bool b = addr[mid] >= val;
        if (!b) {
            first = mid + 1;
        } else {
            last = mid;
        }
    }
    return first;
}