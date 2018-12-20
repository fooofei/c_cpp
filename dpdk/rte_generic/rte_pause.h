/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2017 Cavium, Inc
 */

#ifndef _RTE_PAUSE_H_
#define _RTE_PAUSE_H_
#include <emmintrin.h>
/**
 * @file
 *
 * CPU pause operation.
 *
 */

/**
 * Pause CPU execution for a short while
 *
 * This call is intended for tight loops which poll a shared resource or wait
 * for an event. A short pause within the loop may reduce the power consumption.
 */
static inline void rte_pause(void){
    _mm_pause();
}

#endif /* _RTE_PAUSE_H_ */
