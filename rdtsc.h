
// CPU cycles 

#ifndef RDTSC_H
#define RDTSC_H

#include <stdint.h>


// x86_64 
static inline uint64_t Rdtsc()
{
    unsigned long _hi, _lo;
    asm volatile("rdtscp" : "=a"(_lo), "=d"(_hi));
    uint64_t val = _hi << 32 | _lo;
    return val;
}

// $ cat /proc/cpuinfo |grep MHz
//  *	cpu MHz		: 2701.000
//  *	converts 2701.000 to 2701000000LL
static inline uint64_t RdtscTakeTimeUs(uint64_t start, uint64_t end)
{
    uint64_t diff = end - start;
    const uint64_t CPU_HZ = 3192 * 1000 * 1000LL;

    return (double)diff / CPU_HZ * 1000 * 1000;
}


#endif 
