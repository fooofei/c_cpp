
> rte_ring 在用户态使用的问题

  DPDK中的rte_ring实现了无锁的生产者消费者模型的队列，经过各种性能测试，在队列出队入队频繁的场景下，
  比spinlock处理出队入队要快10倍左右。

  但在用户态时，使用多生产者多消费者的模型，会出现多个线程执行在一个核上；由于线程时间片切换，
  会出现第一个生产者没有返回就被切换出去，导致后续的所有生产者忙等直到时间片耗尽退出，这种持续恶化可能无法恢复过来。

  官方给出的两个方案：
  1、时间片轮转的场景中，使用单生产者单消费者模型。

  2、时间片轮转的场景中，在使用入队或者出队之前，主动进行一次线程切换，来确保证执行入队出队过程不会出现切换。


  在模型允许的情况下，优先选择单生产者单消费者模型。方案二可能影响性能。
  
> rte_ring 的知识点

  又叫做 环形队列 无锁队列

> 无符号二进制数的环性 
  uint32_t A - uint32_t B  结果还是 uint32_t 即便 A 比 B 小
  https://blog.csdn.net/chen98765432101/article/details/69367633
  有符号数 原码是人类可读的书写方式 反码是历史进程中的中间产物 缺陷是有 +0 -0 两个数存在
  补码是计算机对有符号数的表达方式 不易读
    
> Compare And Set CAS  CPU 上 非lock 的同步机制
  我对这个 CAS 一个疑问是据说这个跟 CPU 有关，那么跨了两个 CPU 槽 即 socket（1个socket有多个 core）
  CAS 还有效果吗？
  
  
> rte_ring 结构中 cons.head cons.tail prod.head prod.tail

  为什么是 head tail 组合， 只有一个 cons_off 不行吗？

  head tail 组合是为了支持多个，有多个 prod 多个 cons。

> head tail 相等 表示什么

  cons.head == cons.tail 表示 ring 中没有消费者进来，注意不是说没进来*过*
  而是说进来了但没出去这个状态
  在多 cons 进入 ring head tail 就会不相等，当且仅当 所有 cons 都不在 ring 的时候，
  cons.head 又会 = cons.tail

  同理 prod.head prod.tail 的关系也是这样

> 在多 cons 多 prod 进 ring 后， head tail 的关系是怎样的 （dequeue 举例）

  拿 cons 来说，我要 dequeue 了

  1 先计算还有没有数量够我取出来的，用 cons_head  prod_tail 计算

  为什么是 cons_head ，下面说。 cons_head 一定要用 local variable。

  假如不够，就退出，假如够，就继续。

  dequeue 涉及修改的就只有 cons.head cons.tail 俩。

  2 在够之后，就马上修改 cons.head =cons_head + n，说我要开始入 ring 数据了，这个要 CAS 保护起来，

  当另一个线程或者进程 cons 过来也取数据的时候，他也是修改 cons.head ，也要 CAS 保护，

  这保证了原子性质。

  3 数据都入 ring 了，下面修改 cons.tail ，修改前要判断ring 中的 cons.tail 是不是等于
  local variable 中的那个 cons_head ，为什么要这么判断？
  因为一个 ring 默认没有 cons 的时候， head = tail，这个上面讲过，因此，当此次 dequeue
  认为仅有自己在入 ring 没有别人了，才去修改 cons.tail。

  那如果有呢？

  他只关心谁在他之前过来的，在之后的他都不关心，这是一个等待链。后面的等前面的。

  注意 cons.tail 的修改前面有代码在 rte_pause() 就是在等待。

  并且 cons.tail 修改都不加 CAS 保护，从这一面也能看到 如果只有 1个线程有机会修改一个变量值，
  其他有多个线程只是读(while 在那里读)，也不用保护起来，这是加不加锁的讨论。

  cons_head 就是刚开始入 ring 前保存的 local variable ，它就是个快照。
  当他看到 cons.tail = cons_head 时，意义为在它之前 cons 都已经完成了 dequeue 操作了，
  然后当前的自己才去修改 cons.tail 说我也完成了 dequeue 操作，于是结束。然后它后面的 cons，才继续说完成。

  这是一个等待链。




> rte_ring_peek

  ```
  // ref http://dpdk.org/dev/patchwork/patch/6692/
   /**
   * Just take a peek on the top object from a ring but not consume it.
   * Note: This interface is MC and multi-thread not safe.
   *       It can only be used for ring with RING_F_SC_DEQ attribute.
   *
   * @param r
   *   A pointer to the ring structure.
   * @param obj_p
   *   A pointer to a void * pointer (object) that will be filled.
   * @return
   *   - 0: Success, object is peeked.
   *   - -ENOENT: Not entries in the ring.
   *   - - EPERM: Operation not permitted/supported
   */
  static inline int __attribute__((always_inline))
  rte_ring_peek(const struct rte_ring *r, void **obj_p)
  {
    uint32_t cons_head, prod_tail;

    if (r->cons.sc_dequeue) {
      cons_head = r->cons.head;
      prod_tail = r->prod.tail;

      if (prod_tail - cons_head == 0) {
        return -ENOENT;
      }
      *obj_p = r->ring[cons_head & r->prod.mask];
    } else {
      return -EPERM;
    }
    return 0;
  }
  ```


> dequeue fixed size
  ```
  n = rte_ring_dequeue_burst(ring, (void**)objs, nb_max_objs);
  n可能为0，可能为 (0,nb_max_objs) 的中间值，可能为 nb_max_objs，因为使用的flags 是RTE_RING_QUEUE_VARIABLE
  ```
  对比
  ```
  n = rte_ring_dequeue_bulk(ring, (void**)objs, nb_max_objs);
  n可能为0，可能为 nb_max_objs，再无其他值，因为使用的flags 是RTE_RING_QUEUE_FIXED
```



