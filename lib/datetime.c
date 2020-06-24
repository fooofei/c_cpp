#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

#include "datetime.h"


int timenow(time_t *out)
{
    // time() return *out
    if (-1 == time(out)) {
        return -1;
    }
    return 0;
}

int gmtime2(const time_t *t, struct tm *result)
{
#ifdef WIN32
    return gmtime_s(result, t) == 0 ? 0 : -1;
#else
    // only for posix
    return gmtime_r(t, result) == 0 ? -1 : 0;
#endif
}

int localtime2(const time_t *t, struct tm *result)
{
#ifdef WIN32
    return localtime_s(result, t) == 0 ? 0 : -1;
#else
    return localtime_r(t, result) == 0 ? -1 : 0;
#endif
}

int strftime2(const struct tm *t, const char *fmt, char **buffer, size_t *size)
{
    size_t r;
    if (*buffer) {
        r = strftime(*buffer, *size, fmt, t);
        if (r > 0) {
            *size = r;
            return 0;
        } else {
            *size = 0;
            return -1;
        }
    } else {
        char *bufferv = 0;
        size_t sizev = 0;

        for (sizev = 0x10; sizev < 0x100000; sizev *= 2) {
            bufferv = malloc(sizev);
            if (!bufferv)
                return -1;
            r = strftime(bufferv, sizev, fmt, t);

            if (r > 0) {
                *size = r;
                *buffer = bufferv;
                // free() outside
                return 0;
            }
            free(bufferv);
            bufferv = 0;
        }

        return -1;
    }
}


int default_strftime(const time_t *t, char **buffer, size_t *size)
{
    int ret;
    struct tm localt;

    memset(&localt, 0, sizeof(localt));

    ret = localtime2(t, &localt);
    if (ret != 0)
        return ret;

    return strftime2(&localt, "%Y/%m/%d %H:%M:%S", buffer, size);
}

int default_timenow(char **buffer, size_t *size)
{
    int ret;
    time_t t;

    t = 0;
    ret = timenow(&t);
    if (0 != ret)
        return ret;

    return default_strftime(&t, buffer, size);
}

int format_milliseconds(double const * milliseconds, char **buffer, size_t *size)
{
    typedef int (*pfn_sprintf)(char *, size_t, char const *, ...);
    pfn_sprintf fn = 0;
    uint64_t millsec;
    uint64_t seco;
    uint64_t minu;
    uint64_t hour;
    int ret;
    const char *fmt = "%02" PRIu64 ":%02" PRIu64 ":%02" PRIu64 ".%" PRIu64 "";

    uint64_t temp = (uint64_t)*milliseconds;
    millsec = (temp % 1000);
    temp = temp / 1000;
    seco = temp % 60;
    temp = temp / 60;
    minu = temp % 60;
    temp = temp / 60;
    hour = temp;


#ifdef WIN32
    //   snprintf() is defined in Windows Kits 10-VisualStudio2017, but not in VisualStudio2012.
    //  _snprintf_c() works
    //  _snprintf()  will have warning
    //  sprintf_s() works
    //  by the way, there are so many sprintf*, recommanded use sprintf_s()

    fn = sprintf_s;
#else
    fn = snprintf;
#endif

    if (*buffer) {
        ret = fn(*buffer, *size, fmt, hour, minu, seco, millsec);
        if (ret > 0) {
            *size = ret;
            return 0;
        } else {
            *size = 0;
            return -1;
        }
    } else {
        char *bufferv = 0;
        size_t sizev = 0;

        for (sizev = 0x20; sizev < 0x100000; sizev *= 2) {
            bufferv = malloc(sizev);
            if (!bufferv)
                return -1;
            ret = fn(bufferv, sizev, fmt, hour, minu, seco, millsec);
            if (ret > 0) {
                *size = ret;
                *buffer = bufferv;
                return 0;
            }
            free(bufferv);
            bufferv = 0;
        }
        return -1;
    }
}

int elapsed(const time_t *begin, const time_t *end, char **buffer, size_t *size)
{
    double f = difftime(*begin, *end);
    return format_milliseconds(&f, buffer, size);
}


int clock2_now(clock2_t *t)
{
#ifdef WIN32
    return QueryPerformanceCounter((LARGE_INTEGER *)t) ? 0 : -1;
#else
    // gettimeofday() return 0 for success
    return gettimeofday(t, 0);
#endif
}

#ifdef WIN32
static double __elapsed_milliseconds_win(const LARGE_INTEGER *begin, const LARGE_INTEGER *end)
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return (end->QuadPart - begin->QuadPart) * 1000.0 / freq.QuadPart;
}
#endif

double clock2_elapsed_milliseconds(const clock2_t *begin, const clock2_t *end)
{
#ifdef WIN32
    return __elapsed_milliseconds_win((const LARGE_INTEGER *)begin, (const LARGE_INTEGER *)end);
#else
    return ((end->tv_sec - begin->tv_sec) * 1000.0 + (end->tv_usec - begin->tv_usec) / 1000.0);
#endif
}


int clock2_elapsed(const clock2_t *begin, const clock2_t *end, char **buffer, size_t *size)
{
    double r;
    r = clock2_elapsed_milliseconds(begin, end);
    return format_milliseconds(&r, buffer, size);
}
