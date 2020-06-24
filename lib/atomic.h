#ifndef ATOMIC_H
#define ATOMIC_H

#include <stdint.h>
#include <stdbool.h>

/*
 __sync_lock_test_and_set is more efficient than mutex

 https://github.com/cloudwu/ltask/blob/master/simplelock.h
 http://www.boost.org/doc/libs/1_56_0/boost/container/detail/mutex.hpp
 https://books.google.com/books?id=NF-C2ZQZXekC&pg=PA311&dq=__sync_val_compare_and_swap&hl=zh-CN&sa=X&ved=0ahUKEwiZuLbP5tvYAhUo9YMKHfkZB5oQ6AEIMDAB#v=onepage&q=__sync_val_compare_and_swap&f=false
 https://github.com/fuzzything44/ManaForge-Engine/blob/master/src/OpenALAudioSystem/OpenAL/include/atomic.h
// linux macro //#elif defined(__linux__)
//
// APPLE Specific
// #elif defined(__APPLE__) && defined(__MACH__)
//  deprecated  OSAtomicCompareAndSwapPtrBarrier
//  return atomic_compare_exchange_strong(dst, oval, nval);
*/

// atomics

#ifdef WIN32

#include <Windows.h>

#define atomic_inc_return(var) InterlockedIncrement(var)
#define atomic_inc_return16(var) InterlockedIncrement16(var)
#define atomic_dec_return(var) InterlockedDecrement(var)
#define atomic_dec_return16(var) InterlockedDecrement16(var)


#define atomic_sync() MemoryBarrier()


#define atomic_var_compare_and_swap64(ptr, oldvar, newvar) \
    InterlockedCompareExchange64((LONGLONG volatile *)ptr, (LONGLONG)newvar, (LONGLONG)oldvar)
#define atomic_var_compare_and_swap32(ptr, oldvar, newvar) \
    InterlockedCompareExchange((LONG volatile *)ptr, (LONG)newvar, (LONG)oldvar)
#define atomic_var_compare_and_swap_pointer(ptr, oldvar, newvar) \
    InterlockedCompareExchangePointer((PVOID volatile *)ptr, (PVOID)newvar, (PVOID)oldvar)

#define atomic_bool_compare_and_swap64(ptr, oldvar, newvar) \
    (atomic_var_compare_and_swap64(ptr, oldvar, newvar) == ((LONGLONG)oldvar))
#define atomic_bool_compare_and_swap32(ptr, oldvar, newvar) \
    (atomic_var_compare_and_swap32(ptr, oldvar, newvar) == ((LONG)oldvar))

#define atomic_bool_compare_and_swap_pointer(ptr, oldvar, newvar) \
    (atomic_var_compare_and_swap_pointer(ptr, oldvar, newvar) == ((PVOID)oldvar))

// return prior value
#define atomic_spinlock64(ptr) while (InterlockedExchange64((LONGLONG volatile *)ptr, 1)) { \
    }
#define atomic_spinlock32(ptr) while (InterlockedExchange((LONG volatile *)ptr, 1)) { \
    }

#define atomic_spinunlock64(ptr) InterlockedExchange64((LONGLONG volatile *)ptr, 0)
#define atomic_spinunlock32(ptr) InterlockedExchange((LONG volatile *)ptr, 0)

// named atomic_cas_long


#else

// any type
// gcc options -march=i686 ?
#define atomic_inc_return(var) __sync_add_and_fetch(var, 1)
#define atomic_dec_return(var) __sync_sub_and_fetch(var, 1)

// #define atomic_spinlock(ptr) while (__sync_lock_test_and_set(ptr,1)) {}
// or  put the task to CPU tasks queue end
// #define atomic_spinlock(ptr) while (__sync_lock_test_and_set(ptr,1)) { sched_yield(); }
// #define atomic_spinunlock(ptr)  __sync_lock_release(ptr)

// Doc http://www.cnblogs.com/shines77/p/4238155.html
// Doc https://idea.popcount.org/2012-09-12-reinventing-spinlocks/   talk about the unlock()
// equivalent with  mutex lock unlock
// http://www.informit.com/articles/article.aspx?p=1626979&seqNum=5
#define atomic_spinlock(ptr) while (!__sync_bool_compare_and_swap((ptr), 0, 1)) { \
        sched_yield();                                   \
    }
#define atomic_spinunlock(ptr) do {                       \
        *ptr = 0;              \
    } while (0)

// also worked on CentOS and iOS
#define atomic_var_compare_and_swap(ptr, oldvar, newvar) __sync_val_compare_and_swap(ptr, oldvar, newvar)
#define atomic_bool_compare_and_swap(ptr, oldvar, newvar) __sync_bool_compare_and_swap(ptr, oldvar, newvar)

#define atomic_var_compare_and_swap32 atomic_var_compare_and_swap
#define atomic_bool_compare_and_swap32 atomic_bool_compare_and_swap
#define atomic_spinlock32 atomic_spinlock
#define atomic_spinunlock32 atomic_spinunlock
#define atomic_var_compare_and_swap_pointer atomic_var_compare_and_swap
#define atomic_bool_compare_and_swap_pointer atomic_bool_compare_and_swap

#define atomic_dec_return16 atomic_dec_return
#define atomic_inc_return16 atomic_inc_return

#define atomic_sync() __sync_synchronize()


#endif

// atomic set *ptr = val, return the old value.
static inline void *atomic_write(void **ptr, void *val)
{
    bool success = 0;

    while (!success) {
        void *tmp = *ptr;
        success = atomic_bool_compare_and_swap(ptr, tmp, val);
    }
}

// return the new value
int64_t atomic_add_return64(int64_t *ptr, int64_t val)
{
#if defined(WIN32)
    return (int64_t)InterlockedExchangeAdd64((LONG64 volatile *)ptr, (LONG64)val) + val;
#else
    return __sync_add_and_fetch(ptr, val);
#endif
}

#endif // ATOMIC_H
