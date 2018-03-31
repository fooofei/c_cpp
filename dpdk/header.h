

#pragma once

struct rte_ring;

// `peek ring` is master enqueue and dequeue, the slave peek read it.
struct peek_ring
{
    struct rte_ring * rx_queue;
    pthread_mutex_t mutex;
    uint32_t cons_cur;
};



#define MASTER_MEMORY_ZONE_NAME "master_zone"
