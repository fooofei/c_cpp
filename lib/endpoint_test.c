#include <stdio.h>

#include "endpoint.h"

struct ipaddress_pair {
    const char *p;
    uint32_t h;
};

static const struct ipaddress_pair g_test_data[] = {
    { "127.0.0.1:0", 2130706433 },
    { "192.145.109.100:0", 3230756196 },
    { "192.168.1.3:0", 3232235779 },
    { 0, 0, 0 }
};


#define EXPECT(expr) do {                                                                          \
        if (!(expr)) {                                                            \
            fprintf(stderr, "unexpect %s  (%s:%d)\n", #expr, __FILE__, __LINE__); \
            fflush(stderr);                                                       \
        }                                                                         \
    } while (0)

void test_ipaddress()
{
    char *paddr;
    struct endpoint ep = { 0 };


    const struct ipaddress_pair *p;

    for (p = g_test_data; p->p; p += 1) {
        paddr = 0;
        naddr = 0;
        haddr = 0;

        ep.ip = p->h;
        ep.port = 0;
        endpoint_to_string(&ep, &paddr);
        EXPECT(0 == strcmp(paddr, p->p));
        free(paddr);

        endpoint_from_string(&ep, p->p);
        EXPECT(ep->ip == p->h);
    }
    printf("pass %s()\n", __FUNCTION__);
}


int main()
{
    test_ipaddress();
    return 0;
}
