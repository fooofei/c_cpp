#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "clock-tick.h"
#include "util/vstring.h"

const int SCALE_M = 1000 * 1000;
const int SCALE_G = 1000 * 1000 * 1000;

void tick_init(struct tick *tk)
{
    // 不要使用 gettimeofday
    memset(tk, 0, sizeof *tk);
    clock_gettime(CLOCK_MONOTONIC, &tk->start_time);
    clock_gettime(CLOCK_MONOTONIC, &tk->r.start_time);
}

static inline double timeval_delta_millsec(const struct timeval *end, const struct timeval *begin)
{
    return (double)(end->tv_sec - begin->tv_sec) * 1000 + (double)(end->tv_usec - begin->tv_usec) / 1000;
}

static inline double timespec_delta_millsec(const struct timespec *end, const struct timespec *begin)
{
    return (double)(end->tv_sec - begin->tv_sec) * 1000 + (double)(end->tv_nsec - begin->tv_nsec) / (1000 * 1000);
}

static void tick_shot(struct tick *tk)
{
    uint32_t delta_r;
    uint32_t delta;
    uint32_t value;
    double delta1;

    // TODO 还不能直接使用，需要改造
    struct string_buf_s sb;
    tk->buf[0] = 0;
    string_buf_set(&sb, tk->buf, sizeof(tk->buf));

    delta1 = timespec_delta_millsec(&tk->now, &tk->r.start_time);

    delta_r = (uint32_t)(delta1 / 1000);
    delta = (uint32_t)(timespec_delta_millsec(&tk->now, &tk->start_time) / 1000);

    value = tk->r.sent_packets / delta_r;
    string_buf_sprintf(&sb, "-   pps_r %lu/%u=%lu ", tk->r.sent_packets, delta_r, value);
    value = tk->r.sent_packets_ok / delta_r;
    string_buf_sprintf(&sb, "ok %lu/%u=%lu\n", tk->r.sent_packets_ok, delta_r, value);

    value = tk->sent_packets / delta;
    string_buf_sprintf(&sb, "    pps %lu/%u=%lu ", tk->sent_packets, delta, value);
    value = tk->sent_packets_ok / delta;
    string_buf_sprintf(&sb, "ok %lu/%u=%lu\n", tk->sent_packets_ok, delta, value);

    value = tk->sent_querys / delta;
    string_buf_sprintf(&sb, "    qps %lu/%u=%lu ", tk->sent_querys, delta, value);
    value = tk->sent_querys_ok / delta;
    string_buf_sprintf(&sb, "ok %lu/%u=%lu\n\n", tk->sent_querys_ok, delta, value);

    fprintf(stdout, tk->buf);
    fflush(stdout);
    memset(&tk->r, 0, sizeof(tk->r));
    tk->r.start_time = tk->now;
}

void tick_now(struct tick *tk)
{
    double delta1;

    // 报文累计到一个小数字后再看看时间
    if (tk->r.sent_packets > 1000) {
        clock_gettime(CLOCK_MONOTONIC, &tk->now);

        delta1 = timespec_delta_millsec(&tk->now, &tk->r.start_time);
        if (delta1 > 3000) {
            tick_shot(tk);
        }
    }
}

void timespec_add(struct timespec *val, uint64_t delta)
{
    val->tv_sec += delta / 1000;
    val->tv_nsec += (delta % 1000) * SCALE_M;
    while (val->tv_nsec >= SCALE_G) {
        ++(val->tv_sec);
        val->tv_nsec -= SCALE_G;
    }
}

void tick_now_limitspeed(struct tick *tk, const uint64_t pps_max)
{
    double delta1;
    struct timespec end_time;

    clock_gettime(CLOCK_MONOTONIC, &tk->now);

    delta1 = timespec_delta_millsec(&tk->now, &tk->r.start_time);

    // 虽然卡的是每秒速率，但是延长到 3s 观察速率
    if (tk->r.sent_packets_ok > pps_max * 3 && delta1 < 3000) {
        // limit the sent
        uint64_t rest = 3000 - delta1;
        if (rest / 1000 > 0) {
            end_time = tk->now;
            timespec_add(&end_time, rest);
            // 发太快 等一会
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &end_time, NULL);
            clock_gettime(CLOCK_MONOTONIC, &tk->now);
            delta1 = timespec_delta_millsec(&tk->now, &tk->r.start_time);
        }
    }

    if (delta1 > 3000) {
        tick_shot(tk);
    }
}
