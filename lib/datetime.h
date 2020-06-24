#ifndef CORE_DATETIME_H
#define CORE_DATETIME_H


/*
c++11 above can use chrono library,
http://en.cppreference.com/w/cpp/chrono

time include two kind, a time_t time, is large interger 64bit.
a calendar time called UTC-time, is Universal Time Coordinated.
local time in Beijing is UTC+8.

in std namespace (c library), time type is std::time_t (struct time_t)
in std namespace (c library), UTC-time type is struct tm
in std namespace (c library), local time type is struct tm


use std::time() to get current time(typed of struct time_t),
use std::gmtime() to convert  std::time_t to struct tm, UTC-time.
gmtime() may not be thread-safe, the thread-safe version is std::gmtime_s()
which is c++11 above, the posix special version is gmtime_r() is c++11 above in <time.h>

use std::localtime() to convert std::time_t to local time, also save as struct tm,
may not be thread-safe, the thread-safe version is localtime_r in posix (localtime_s in c++11).
注意 在 linux 测试 localtime()获取的时间跟 /etc/localtime 有关。更改 /etc/localtime 之后，也就是更改时区，
localtime() API 返回的结果也会改变，并不是从时间戳到 UTC 的直接转换。
因此 推荐使用 UTC time，也就是 gmtime*()(gmtime_s() on Windows, gmtime_r() on linux) 这个 API
测试：localtime -> ../usr/share/zoneinfo/America/New_York 更改映射几个 测试


use std::strftime() to convert struct tm to string, also std::asctime(), also ctime()

std::get_time() and std::put_time() is c++11 above helper function to convert
struct tm to string

std::time_t's invalid value is -1
use difftime() to get the two time interval, in seconds

notice the difference:
errno_t gmtime_s(struct tm*,time_t const*); is the MSVC implemention
struct tm *gmtime_s(const time_t *restrict time, struct tm *restrict result); is the c++11 standard

gettimeofday() 被弃用了 不要使用
http://pubs.opengroup.org/onlinepubs/9699919799/functions/gettimeofday.html
https://blog.habets.se/2010/09/gettimeofday-should-never-be-used-to-measure-time.html
于是又听到了 monotonic time & wall time 的区分
https://blog.csdn.net/tangchenchan/article/details/47989473

struct timespec { time_t tv_sec;  long tv_nsec;  纳秒 }
struct timeval { time_t tv_sec; long tv_usec; 微妙 }

*/


/* all api is thread-safe */

#include <time.h>
#include <stdint.h>

/* return 0 for success */

int timenow(time_t *);

// convert time_t to UTC-time
int gmtime2(const time_t *t, struct tm *result);

// convert time_t to localtime (bejing time is UTC+8)
int localtime2(const time_t *t, struct tm *result);


// if buffer's memory passed, then use it. if not, the memory alloced by strftime2()
// if buffer's memory is alloced by strftime2(), please use free() to release memory
int strftime2(const struct tm *t, const char *fmt, char **buffer, size_t *size);


// convert time_t ->(localtime) -> str
int default_strftime(const time_t *t, char **buffer, size_t *size);


int default_timenow(char **buffer, size_t *size);

// format milliseconds to string as format hour:minutes:seconds.milliseconds
int format_milliseconds(double const * milliseconds, char **buffer, size_t *size);


int elapsed(const time_t *begin, const time_t *end, char **buffer, size_t *size);


#ifdef WIN32
typedef int64_t clock2_t;
#else
typedef struct timeval clock2_t;
#endif

int clock2_now(clock2_t *);
double clock2_elapsed_milliseconds(const clock2_t *begin, const clock2_t *end);
int clock2_elapsed(const clock2_t *begin, const clock2_t *end, char **buffer, size_t *size);

/*
  an example of clock2_elapsed()

    clock2_t begin;
    clock2_t end;
    enum{_buf_size=0x100,};
    char buf[_buf_size];
    char * pbuf;
    size_t size;
    clock2_now(&begin);

    // do work ...

    clock2_now(&end);
    size = _buf_size;
    pbuf = buf;
    clock2_elapsed(&begin, &end, &pbuf, &size); // WARNING: there cannot be buf, because buf is the array name,
     // the array name cannot be char **

    fprintf(stdout, "[+] curl cost time %s %s:%d %s()\n", pbuf, __FILE__, __LINE__, __FUNCTION__);
    fflush(stdout);
*/


#ifdef __cplusplus
int strftime2(const struct tm *t, const char *fmt, std::string &s);
#endif

#endif // CORE_DATETIME_H
