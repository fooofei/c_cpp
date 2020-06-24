#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdint.h>

struct endpoint {
    uint32_t ip; // host order
    uint16_t port;
};

int endpoint_from_string(struct endpoint *ep, const char *s);
void endpoint_to_string(const struct endpoint *ep, char **out);
void endpoint_to_v4sockaddr(const struct endpoint *ep, struct sockaddr_in *in);
void endpoint_from_v4sockaddr(struct endpoint *ep, const struct sockaddr_in *in);

#endif
