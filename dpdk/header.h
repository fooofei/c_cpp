
/*
 �����������֤��  ring �� peek �÷����ǽ��� A enqueue ���� B dequeue ���÷�
 �����ڴ��еı��� cons_cur ��Ȼ���˽��� ����Ҳ����ͬ������
*/

#pragma once

#include <pthread.h>
#include <semaphore.h>


struct rte_ring;

// `peek ring` is master enqueue and dequeue, the slave peek read it.
struct peek_ring
{
    struct rte_ring * rx_queue;
    //pthread_mutex_t mutex;
    //sem_t  sem;
    uint32_t cons_cur;
};

// sem ��Ȼû�� mutex ����

#define NO_SYNC

#ifdef NO_SYNC
static inline void peek_ring_cons_cur_init(struct peek_ring * self)
{
    (void)self;
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
    // Not support by all platform ? 
    // ����Ч����ʾ�����粻��
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&self->mutex, &mutex_attr);
    //pthread_mutex_init(&self->mutex, NULL); // ���ӵĻ� ���߾��޷����� ��bug
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
}
static inline void peek_ring_cons_cur_destroy(struct peek_ring * self)
{
    sem_destroy(&self->sem);
}
#endif




#define MASTER_MEMORY_ZONE_NAME "master_zone"
