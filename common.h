#ifndef COMMON_H_
#define COMMON_H_


#define ARRAY_SIZE(array) sizeof(array) / sizeof((array)[0])
#define ARRAY_END(array) (array) + sizeof(array) / sizeof((array)[0])
#define ARRAY_BEGIN_END(array) (array), ARRAY_END(array)


#define EXPECT(expr) do {                                                                          \
        if (!(expr)) {                                                            \
            fprintf(stderr, "unexpect %s  (%s:%d)\n", #expr, __FILE__, __LINE__); \
            fflush(stderr);                                                       \
        }                                                                         \
    } while (0)

#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif


#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof(a[0]))
#endif


#endif // COMMON_H_
