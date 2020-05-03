
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <rte_common.h>
#include <rte_log.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_log.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_memcpy.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_arp.h>
#include <rte_spinlock.h>
#include <rte_ring.h>
#include <rte_malloc.h>

#include "header.h"

struct context {
    uint64_t count;
    uint64_t dequeue_seq;
};

int enqueue(struct context* ctx, struct peek_ring* ring)
{
    int rc;
    // malloc 对方读取不到
    // rte_malloc 另一个进程能读取到
    uint64_t* p = rte_zmalloc("enqueue_datas", sizeof(uint64_t), 0);
    if (!p) {
        // fprintf(stdout, "Fail rte_zmalloc\n"); fflush(stdout);
        return -1;
    }
    *p = ctx->count;

    if (ring->rx_queue->prod.tail == ((1 < 3) - 1)) {
        // 队列计数 prod.head prod.tail cons.head cons.tail 经历了循环后会在这里输出
        fprintf(stdout, "ring->rx_queue->prod.tail=10\n");
        fflush(stdout);
    }

    rc = rte_ring_enqueue(ring->rx_queue, p);

    if (rc < 0) {
        // fprintf(stderr, " Fail rte_ring_enqueue() at %u\n", ctx->count);
        // fflush(stderr);
        rte_free(p);
        return rc;
    }
    ctx->count += 1;
    return 0;
}

void dequeue(struct context* ctx, struct peek_ring* ring)
{
    uint32_t cons_cur;

    cons_cur = peek_ring_get_cons_cur(ring);
    uint32_t can_dequeue_size = cons_cur - ring->rx_queue->cons.tail;
    int rc;
    uint32_t i;
    uint64_t* obj = 0;

    // can_dequeue_size = rte_ring_count(ring->rx_queue);
    if (can_dequeue_size == 0) {
        // fprintf(stdout, "dequeue can size = %u \n", can_dequeue_size); fflush(stdout);
    }
    // fprintf(stdout, "dequeue can size = %u \n", can_dequeue_size); fflush(stdout);

    for (i = 0; i < can_dequeue_size; i += 1) {
        rc = rte_ring_dequeue(ring->rx_queue, (void**)&obj);
        if (rc < 0) {
            fprintf(stderr, "Fail rte_ring_dequeue()\n");
            fflush(stderr);
        }
        if (obj) {
            // printf(" dequeue %d ", *obj);
            if (ctx->dequeue_seq != *obj) {
                fprintf(stdout,
                    "dequeue invalid ctx->dequeue_seq = %lld dequeue = %lld\n",
                    (long long)ctx->dequeue_seq,
                    (long long)*obj);
                fflush(stdout);
            }
            rte_free(obj);
            ctx->dequeue_seq += 1;
        }
    }
}

// 可以不用 master slave 共享吗？
#define MASTER_CREATE_RING_NAME "test_ring"

int main(int argc, const char* argv[])
{
    int rc;
    int argc2 = argc + 3;
    const char** argv2 = calloc(argc2, sizeof(const char*));
    int i;
    for (i = 0; i < argc; i += 1) {
        argv2[i] = argv[i];
    }
    // primary secondary auto
    argv2[i] = "-l";
    i += 1;
    argv2[i] = "0-0";
    i += 1;
    argv2[i] = "--log-level=10";
    i += 1;
    // argv2[i] = "--proc-type=secondary"; // 如果全用 secondary，在 slave 还存活的情况下开启 master
    // 报错为  EAL: Could not open /dev/hugepages/rtemap_453
    // PANIC in rte_eal_init() :
    //   Cannot init memory

    rc = rte_eal_init(argc2, (char**)argv2);
    free(argv2);
    if (rc < 0) {
        rte_panic("Fail rte_eal_init\n");
    }

    struct peek_ring* ring;
    const struct rte_memzone* zone;

    zone = rte_memzone_lookup(MASTER_MEMORY_ZONE_NAME);
    if (zone == 0) {
        zone = rte_memzone_reserve(MASTER_MEMORY_ZONE_NAME, sizeof(struct peek_ring), rte_socket_id(), RTE_MEMZONE_2MB);
    }

    if (zone == 0) {
        rte_panic("Fail rte_memzone_reserve()");
    }
    ring = zone->addr;

    memset(ring, 0, sizeof(struct peek_ring));
    // rte_ring_init 没用到， 只用到了 rte_ring_create
    ring->rx_queue = rte_ring_create(MASTER_CREATE_RING_NAME, 1 << 12, 0, 0);
    if (ring->rx_queue == NULL) {
        rte_panic("Fail rte_ring_create()");
    }

    peek_ring_cons_cur_init(ring);
    struct context ctx;
    memset(&ctx, 0, sizeof(struct context));

    for (;;) {
        rc = enqueue(&ctx, ring);
        if (rc < 0) {
            // rte_ring_list_dump(stdout);
            // break;
            // sleep(1);
        }
        // usleep(10);
        // sleep(1);
        dequeue(&ctx, ring);
    }

    peek_ring_cons_cur_destroy(ring);
    rte_ring_free(ring->rx_queue);
    rte_memzone_free(zone);

    return 0;
}
