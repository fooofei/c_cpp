#ifndef SEARCH_H
#define SEARCH_H

#include <stdint.h>


/*
(addr,size) is [small, large) sorted array.
same with std::lower_bound, return the >= element of index,

return the element index, begin with 0.
if not found, return the size.
*/
uint32_t lower_bound(const uint32_t *addr, uint32_t size, const uint32_t val);


/*
(addr,size) is [small, large) sorted array.
same with std::upper_bound, return the > element of index,

return the element index, begin with 0.
if not found, return the size.
*/
uint32_t upper_bound(const uint32_t *addr, uint32_t size, const uint32_t val);

// 返回第一个 true 的索引
int bearch(const uint32_t *addr, int first, int last, uint32_t val);

#endif