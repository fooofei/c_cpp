

// DPDK ring 内存默认从共享内存申请 并且记载了申请信息 下次申请会失败 不方便做进程重启
// 这个 ring 使用堆内存

struct rte_ring * rte_ring_create2(unsigned count, unsigned flags)
{
	struct rte_ring *r;
	ssize_t ring_size;
    int ret;

	ring_size = rte_ring_get_memsize(count);
	if (ring_size < 0) {
		rte_errno = ring_size;
		return NULL;
	}

    r = cc_malloc(ring_size);
    if(r == NULL)
    {
        return NULL;
    }
    ret = rte_ring_init(r, "", count, flags);
    if(ret <0)
    {
        rte_errno = ret;
        return NULL;
    }
    return r;
}



void rte_ring_free2(struct rte_ring *r)
{
    cc_free(r);
}
