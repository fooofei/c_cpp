
// 只是一个普通字节序测试，没看懂在干什么

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include <stdbool.h>

#ifdef WIN32
#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <time.h>
#include <sys/timeb.h>


#pragma pack(push)
#pragma pack(1)
struct test {
    uint32_t mark;
    union {
        uint16_t tl;
        struct {
            uint16_t length : 12;
            uint16_t type : 4;
        };
    };
};
#pragma pack(pop)


/*
#pragma pack(push)
#pragma pack(1)
struct test {
    uint32_t mark;
    union {
        uint16_t tl;
        struct {
            unsigned length : 12;
            unsigned type : 4;
        };
    };

};
#pragma pack(pop)
sizeof(struct test) = 8


#pragma pack(push)
#pragma pack(1)
struct test {
    uint32_t mark;
    union {
        uint16_t tl;
        struct {
            uint16_t length : 12, type : 4;
        };
    };

};
#pragma pack(pop)
这样写跟  uint16_t length:12 ;  uint16_t type:4; 不等价， 要注意书写

*/


void store_length(struct test *a, uint16_t len)
{
    uint16_t t = a->tl & 0xF000;
    a->tl = t | (len & 0x0FFF);
}

void store_type(struct test *a, uint16_t type)
{
    uint16_t l = a->tl & 0x0FFF;
    uint16_t t = (type & 0xF) << 12;
    a->tl = t | l;
}

int main(int argc, char **argv)
{
    char buffer[0x40];
    uint16_t len = 0x234;
    struct test b = { 0 };
    struct test a = { 0 };

    a.mark = 0xABCDEF9A;
    /* 写 a.length = 0x234 与 store_length 等价，如果其他 struct 定义就不等价 */
    a.length = 0x234;
    // store_length(&a, 0x234);
    // store_type(&a, 0x1);
    a.type = 0x1;

    assert(a.length == len);
    memcpy(buffer, &a, sizeof(a));
    // 9a ef cd ab 34 12

    a.mark = htonl(a.mark);
    a.tl = htons(a.tl);

    memcpy(buffer, &a, sizeof(a));


    memcpy(&b, buffer, sizeof b);

    b.mark = ntohl(b.mark);
    b.tl = ntohs(b.tl);
    return 0;
}
