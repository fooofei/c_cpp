
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "clock-tick.h"
#include "util/vstring.h"

#define scale_m (1000 * 1000)
#define scale_g (1000 * 1000 * 1000)

void tick_init(struct tick_s* self)
{
    // 不要使用 gettimeofday
    memset(self, 0, sizeof(*self));
    clock_gettime(CLOCK_MONOTONIC, &self->start_time);
    clock_gettime(CLOCK_MONOTONIC, &self->r.start_time);
}

static inline double timeval_delta_millsec(const struct timeval* end, const struct timeval* begin)
{
    return (double)(end->tv_sec - begin->tv_sec) * 1000 + (double)(end->tv_usec - begin->tv_usec) / 1000;
}
static inline double timespec_delta_millsec(const struct timespec* end, const struct timespec* begin)
{
    return (double)(end->tv_sec - begin->tv_sec) * 1000 + (double)(end->tv_nsec - begin->tv_nsec) / (1000 * 1000);
}

static void tick_shot(struct tick_s* self)
{
    uint32_t delta_r;
    uint32_t delta;
    uint32_t value;
    double delta1;

    struct string_buf_s sb;
    self->buf[0] = 0;
    string_buf_set(&sb, self->buf, sizeof(self->buf));

    delta1 = timespec_delta_millsec(&self->now, &self->r.start_time);

    delta_r = (uint32_t)(delta1 / 1000);
    delta = (uint32_t)(timespec_delta_millsec(&self->now, &self->start_time) / 1000);

    value = self->r.sent_packets / delta_r;
    string_buf_sprintf(&sb, "-   pps_r %lu/%u=%lu ", self->r.sent_packets, delta_r, value);
    value = self->r.sent_packets_ok / delta_r;
    string_buf_sprintf(&sb, "ok %lu/%u=%lu\n", self->r.sent_packets_ok, delta_r, value);

    value = self->sent_packets / delta;
    string_buf_sprintf(&sb, "    pps %lu/%u=%lu ", self->sent_packets, delta, value);
    value = self->sent_packets_ok / delta;
    string_buf_sprintf(&sb, "ok %lu/%u=%lu\n", self->sent_packets_ok, delta, value);

    value = self->sent_querys / delta;
    string_buf_sprintf(&sb, "    qps %lu/%u=%lu ", self->sent_querys, delta, value);
    value = self->sent_querys_ok / delta;
    string_buf_sprintf(&sb, "ok %lu/%u=%lu\n\n", self->sent_querys_ok, delta, value);

    fprintf(stdout, self->buf);
    fflush(stdout);
    memset(&self->r, 0, sizeof(self->r));
    self->r.start_time = self->now;
}

void tick_now(struct tick_s* self)
{
    double delta1;

    // 报文累计到一个小数字后再看看时间
    if (self->r.sent_packets > 1000) {
        clock_gettime(CLOCK_MONOTONIC, &self->now);

        delta1 = timespec_delta_millsec(&self->now, &self->r.start_time);
        if (delta1 > 3000) {
            tick_shot(self);
        }
    }
}

void timespec_add(struct timespec* val, uint64_t delta)
{
    val->tv_sec += delta / 1000;
    val->tv_nsec += (delta % 1000) * scale_m;
    while (val->tv_nsec >= scale_g) {
        ++(val->tv_sec);
        val->tv_nsec -= scale_g;
    }
}

void tick_now_limitspeed(struct tick_s* self, const uint64_t pps_max)
{
    double delta1;
    struct timespec end_time;

    clock_gettime(CLOCK_MONOTONIC, &self->now);

    delta1 = timespec_delta_millsec(&self->now, &self->r.start_time);

    if (self->r.sent_packets_ok > pps_max * 3 && delta1 < 3000) {
        // limit the sent
        uint64_t rest = 3000 - delta1;
        if (rest / 1000 > 0) {
            end_time = self->now;
            timespec_add(&end_time, rest);
            // 发太快 等一会
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &end_time, NULL);
            clock_gettime(CLOCK_MONOTONIC, &self->now);
            delta1 = timespec_delta_millsec(&self->now, &self->r.start_time);
        }
    }

    if (delta1 > 3000) {
        tick_shot(self);
    }
}
