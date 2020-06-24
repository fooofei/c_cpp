#ifndef SORT_H
#define SORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* sort at original memory */
void quick_sort(uint32_t *begin, uint32_t *end);

void test_quick_sort();

#ifdef __cplusplus
}
#endif

#endif