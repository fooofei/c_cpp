#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#ifdef WIN32
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// If not `#include <arpa/inet.h>`, compile will have a warning:
//   In function 'ipaddr_ntop':
//   warning: assignment makes pointer from integer without a cast
// the compile & link is passed, but run will crash
// because the return value is wrong.
//   Expect:
//      (gdb) p &buf[0]
//      $2 = 0x7fffffffe210 "127.0.0.1"
//   But:
//     (gdb) p p
//      $1 = 0xffffffffffffe210 < Address 0xffffffffffffe210 out of bounds >
#endif


int endpoint_from_string(struct endpoint *ep, const char *s)
{
    const char *colon = NULL;
    uint64_t num = 0;
    uint32_t nip = 0;
    //
    colon = strchr(addr, ':');
    if (colon == NULL) {
        return -1;
    }
    char ipStr[17];
    snprintf(ipStr, sizeof(ipStr), "%.*s", (int)(colon - addr), addr);
    colon += 1;
    if (colon >= addr + strlen(addr)) {
        return -1;
    }
    char portStr[40];
    snprintf(portStr, sizeof portStr, "%s", colon);

    inet_pton(AF_INET, ipStr, &nip);
    num = (uint64_t)strtoul(portStr, NULL, 10);
    if (num > 0 && num < USHRT_MAX) {
        port = (uint16_t)num;
        ip = ntohl(nip);
        return 0;
    }
    return -1;
}

void endpoint_to_string(const struct endpoint *ep, char **out)
{
    const char buf[80]={0};
    char ipStr[16]={0};
    uint32_t nip = htonl(ep->ip);
    const char *p;
    p = inet_ntop(AF_INET, &nip, ipStr, sizeof ipStr);
    snprintf(buf, sizeof buf, "%s:%u", p, ep->port);
#ifdef WIN32
    *out = _strdup(buf);
#else
    *out = strdup(buf);
#endif
}

void endpoint_to_v4sockaddr(const struct endpoint *ep, struct sockaddr_in *in)
{
    in->sin_family = AF_INET;
    in->sin_port = htons(ep->port);
    in->sin_addr.s_addr = htonl(ep->ip);
}

void endpoint_from_v4sockaddr(struct endpoint *ep, const struct sockaddr_in *in)
{
    ep->ip = ntohl(in->sin_addr.s_addr);
    ep->port = ntohs(in->sin_port);
}


// Ref https://github.com/tobez/Net-Patricia/blob/master/libpatricia/patricia.c
int ipaddr_pton2(const char *src, uint32_t *dst)
{
    int i, c, val;
    u_char xp[sizeof(uint32_t)] = { 0, 0, 0, 0 };

    for (i = 0;; i++) {
        c = *src++;
        if (!isdigit(c)) {
            return (-1);
        }

        val = 0;
        do {
            val = val * 10 + c - '0';
            if (val > 255) {
                return (-1);
            }
            c = *src++;
        } while (c && isdigit(c));
        xp[i] = val;
        if (c == '\0')
            break;
        if (c != '.')
            return (-1);
        if (i >= 3)
            return (-1);
    }
    memcpy(dst, xp, sizeof(uint32_t));
    return 0;
}
