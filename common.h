#ifndef COMMON_H_
#define COMMON_H_



#define _array_size(array) sizeof(array)/sizeof((array)[0])
#define _array_end(array) (array)+sizeof(array)/sizeof((array)[0])
#define _array_begin_end(array) (array),_array_end(array)


#define EXPECT(expr) \
    do { \
    if(!(expr)) \
        { \
        fflush(stdout); \
        fflush(stderr); \
        fprintf(stderr, "unexpect-> %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)


#endif // COMMON_H_
