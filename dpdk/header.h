
/*
 这个工程用来证明  ring 的 peek 用法，不是进程 A enqueue 进程 B dequeue 的用法
 共享内存中的变量 cons_cur 虽然跨了进程 但是也无需同步保护
 
 1 peek_ring 中窥探偏移未加锁保护，限制了这个策略暂且只适合 1 个peek 进程
  已经测试 A 进程 enqueue dequeue， B 进程 peek，不会有问题
 2 peek 进程一定要注意在 peek 结束完成后才能对 窥探偏移增加计数 不然 peek 的数据都是错误的
 
 
*/

#pragma once

#include <pthread.h>
#include <semaphore.h>


struct rte_ring;

// `peek ring` enqueued and dequeued in master,
//  the slave peek read it, not take it.
struct peek_ring
{
    struct rte_ring * rx_queue;
    //pthread_mutex_t mutex;
    //sem_t  sem;
    uint32_t cons_cur; // 这个写法限制了这个策略只适合 1 个peek 进程
};

// sem 显然没有 mutex 更快

#define NO_SYNC

#ifdef NO_SYNC
static inline void peek_ring_cons_cur_init(struct peek_ring * self)
{
    (void)self;
    self->cons_cur = self->rx_queue->cons.head; // 什么作用
    fprintf(stdout, "self->cons_cur = %u\n", self->cons_cur);
}
static inline void peek_ring_cons_cur_destroy(struct peek_ring * self)
{
    (void)self;
}
static inline void peek_ring_cons_cur_inc(struct peek_ring * self)
{
    self->cons_cur += 1;
}
static inline uint32_t peek_ring_get_cons_cur(struct peek_ring * self)
{
    return self->cons_cur;
}
#endif


/*
mutex
*/

#ifdef USE_MUTEX
static inline void peek_ring_cons_cur_inc(struct peek_ring * self)
{
    // pthread_mutex_lock(&self->mutex);
    self->cons_cur += 1;
    //pthread_mutex_unlock(&self->mutex);
}

static inline uint32_t peek_ring_get_cons_cur(struct peek_ring * self)
{
    uint32_t cons_cur;
    //pthread_mutex_lock(&self->mutex);
    cons_cur = self->cons_cur;
    // pthread_mutex_unlock(&self->mutex);
    return cons_cur;
}

static inline void peek_ring_cons_cur_init(struct peek_ring * self)
{
    // Use mutex for process sync.
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&self->mutex, &mutex_attr);
    //pthread_mutex_init(&self->mutex, NULL); // 不加的话 两边就无法进行 是bug
    self->cons_cur = self->rx_queue->cons.head;
}
static inline void peek_ring_cons_cur_destroy(struct peek_ring * self)
{
    pthread_mutex_destroy(&self->mutex);
}
#endif


#ifdef USE_SEM

static inline void peek_ring_cons_cur_inc(struct peek_ring * self)
{
    sem_wait(&self->sem);
    self->cons_cur += 1;
    sem_post(&self->sem);
}

static inline uint32_t peek_ring_get_cons_cur(struct peek_ring * self)
{
    uint32_t cons_cur;
    sem_wait(&self->sem);
    cons_cur = self->cons_cur;
    sem_post(&self->sem);
    return cons_cur;
}

static inline void peek_ring_cons_cur_init(struct peek_ring * self)
{
    // first 1 for share between processes , 0 for shared between threads of one process
    sem_init(&self->sem, 1, 1);
    self->cons_cur = self->rx_queue->cons.head;
}
static inline void peek_ring_cons_cur_destroy(struct peek_ring * self)
{
    sem_destroy(&self->sem);
}
#endif

/*
 Return can peek count of the ring.
*/
static inline uint32_t peek_ring_get_count(struct peek_ring * self)
{
    uint32_t count;
    uint32_t cons_cur = peek_ring_get_cons_cur(self);

    count = self->rx_queue->prod.tail - cons_cur;

    return count;
}

#define MASTER_MEMORY_ZONE_NAME "master_zone"
