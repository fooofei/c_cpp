#ifndef ASPRINTF_H
#define ASPRINTF_H

#include <string.h>

// from https://stackoverflow.com/questions/40159892/using-asprintf-on-windows

#ifndef _vscprintf
/* For some reason, MSVC fails to honour this #ifndef. */
/* Hence function renamed to _vscprintf_so(). */
int _vscprintf_so(const char *format, va_list pargs);
#endif // _vscprintf

#ifndef vasprintf
int vasprintf(char **strp, const char *fmt, va_list ap);
#endif // vasprintf

#ifndef asprintf
int asprintf(char *strp[], const char *fmt, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 2, 3))) // 加了 GCC 也不提示 GCC选项 -Wformat 无效  -Wno-format-unknown-specifier 无效
#endif
    ;

#endif
