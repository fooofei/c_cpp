
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

#include "header.h"

struct context
{
    uint64_t count;
};


void peek(struct context * ctx, struct peek_ring * ring)
{
    uint32_t count;
    uint32_t cons_cur = peek_ring_get_cons_cur(ring);

    count = ring->rx_queue->prod.tail - cons_cur;

    if (count == 0)
    {
        //fprintf(stdout, "No data\n"); 
        //fflush(stdout);
    }

    uint32_t i;

    for (i=0;i<count; i+=1)
    {
        uint64_t * p = ring->rx_queue->ring[(cons_cur + i) & ring->rx_queue->prod.mask];
       
        

        if (ctx->count != *p)
        {
            // gdb 调试 *p 有值 但是为什么还是 print 0 呢？ 因为 就不应该在这里之前 cons_cur += 1
            long long v2 = *p;
            fprintf(stdout, "peek invalid ctx->count = %lld peek = %lld\n", (long long )ctx->count, v2);
            if (*p == 0)
            {
                fprintf(stdout, "cons_cur=%u i=%u (cons_cur + i) & ring->rx_queue->prod.mask=%u\n", cons_cur, i,
                    (cons_cur + i) & ring->rx_queue->prod.mask
                );
            }
            fflush(stdout);
        }
        ctx->count += 1;

        if (ctx->count % 1000000 ==0)
        {
            fprintf(stdout, "peek %lld ", (long long)*p); fflush(stdout);
        }

        // 注意顺序  放到末尾通知
        //ring->cons_cur += 1;

        // CAS not work
        //__sync_fetch_and_add(&ring->cons_cur, 1); // 连同步都不需要了 CAS 也就用不到了 /

        // 确实少了很多
        peek_ring_cons_cur_inc(ring);
    }


}


int main(int argc, const char * argv[])
{
    int rc;

    const char ** argv2 = calloc(argc + 3, sizeof(const char *));
    int i;
    for (i = 0; i < argc; i += 1)
    {
        argv2[i] = argv[i];
    }
    // primary secondary auto
    argv2[i] = "--proc-type=secondary"; i += 1;
    argv2[i] = "-l"; i += 1;
    argv2[i] = "1-1";

    rc = rte_eal_init(argc+3, (char**)argv2);
    free(argv2);
    if (rc < 0)
    {
        rte_panic("Fail rte_eal_init\n");
    }
    
    const struct rte_memzone * zone;

    struct context ctx;
    memset(&ctx, 0, sizeof(ctx));
    
    for (;;)
    {
        zone = rte_memzone_lookup(MASTER_MEMORY_ZONE_NAME);

        if (zone)
        {
            struct peek_ring * ring = zone->addr;
            for (;;)
            {
                peek(&ctx,ring);

                //usleep(10);
            }
        }

        fprintf(stdout, "try rte_memzone_lookup\n");
        fflush(stdout);
        sleep(1);
    }
   


    for (;0;)
    {
        struct peek_ring ring;
       // ring.rx_queue = rte_ring_lookup(MASTER_CREATE_RING_NAME);
        if (ring.rx_queue )
        {
            for (;;)
            {
                //peek(&ring);

                usleep(10);
            }

          
        }

        fprintf(stdout, "try rte_ring_lookup\n");
        fflush(stdout); 
        sleep(1);
        // rte_panic("Fail rte_ring_lookup()");
       
    }

   
    return 0;
}
