// 报文速率限制，报文发送速率展示

#ifndef CLOCK_TICK_H
#define CLOCK_TICK_H

#include <stdint.h>
#include <time.h>

struct tick {
    struct timespec now;
    struct timespec start_time;
    uint64_t sent_packets;
    uint64_t sent_packets_ok;
    uint64_t sent_querys;
    uint64_t sent_querys_ok;
    struct {
        uint64_t sent_packets;
        uint64_t sent_packets_ok;
        struct timespec start_time;
    } r;
    char buf[0x200];
};

void tick_init(struct tick *tk);
// every packet call this
void tick_now(struct tick *tk);
// 发送报文时 如何限制速率
void tick_now_limitspeed(struct tick *tk, const uint64_t pps_max);

void timespec_add(struct timespec *val, uint64_t delta);

#endif
