
#ifndef CLOCK_TICK_H
#define CLOCK_TICK_H

#include <stdint.h>
#include <time.h>

struct tick_s
{
    struct timespec now;
    struct timespec start_time;
    uint64_t sent_packets;
    uint64_t sent_packets_ok;
    uint64_t sent_querys;
    uint64_t sent_querys_ok;
    struct
    {
        uint64_t sent_packets;
        uint64_t sent_packets_ok;
        struct timespec start_time;
    }r;
    char buf[0x200];
};

void tick_init(struct tick_s * self);
// every packet call this
void tick_now(struct tick_s * self);
void tick_now_limitspeed(struct tick_s * self, const uint64_t pps_max);

void timespec_add(struct timespec * val, uint64_t delta);

#endif
