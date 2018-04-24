#ifndef COMMON_H_
#define COMMON_H_



#define _array_size(array) sizeof(array)/sizeof((array)[0])
#define _array_end(array) (array)+sizeof(array)/sizeof((array)[0])
#define _array_begin_end(array) (array),_array_end(array)


#define EXPECT(expr) \
    do { \
    if(!(expr)) \
        { \
        fprintf(stderr, "unexpect %s  (%s:%d)\n",#expr, __FILE__, __LINE__); \
        fflush(stderr);\
        } \
    } while (0)

#ifndef max
#define max(x,y)  (((x)>(y))?(x):(y))
#endif

#ifndef min
#define min(x,y)    (((x)<(y))?(x):(y))
#endif


#ifndef _countof
#define _countof(a) (sizeof(a)/sizeof(a[0]))
#endif


#endif // COMMON_H_
