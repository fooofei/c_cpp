#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "md5.h"


/* Constants for MD5Transform routine. */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
*/
#define FF(a, b, c, d, x, s, ac) {                                       \
        (a) += F((b), (c), (d)) + (x) + ac; \
        (a) = ROTATE_LEFT((a), (s));        \
        (a) += (b);                         \
    }
#define GG(a, b, c, d, x, s, ac) {                                       \
        (a) += G((b), (c), (d)) + (x) + ac; \
        (a) = ROTATE_LEFT((a), (s));        \
        (a) += (b);                         \
    }
#define HH(a, b, c, d, x, s, ac) {                                       \
        (a) += H((b), (c), (d)) + (x) + ac; \
        (a) = ROTATE_LEFT((a), (s));        \
        (a) += (b);                         \
    }
#define II(a, b, c, d, x, s, ac) {                                       \
        (a) += I((b), (c), (d)) + (x) + ac; \
        (a) = ROTATE_LEFT((a), (s));        \
        (a) += (b);                         \
    }

#define _min(a, b) ((a) < (b) ? (a) : (b))

const uint8_t g_PADDING[64] = { 0x80 };

enum {
    _md5_digest_hex_size = 16,
};

struct md5_context {
    uint32_t state[4];
    uint32_t count[2];
    uint8_t buffer[64];
    uint8_t digest[_md5_digest_hex_size];
    uint32_t transform_block[16];
    bool finish;
};


void md5_create(struct md5_context **mc)
{
    *mc = calloc(1, sizeof(**mc));
}

void md5_release(struct md5_context **mc)
{
    free(*mc);
    *mc = 0;
}

void md5_clear(struct md5_context *mc)
{
    memset(mc, 0, sizeof(*mc));

    uint32_t t[] = {
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476
    };
    memcpy(mc->state, t, sizeof(t));
    mc->finish = true;
}

/* Decodes input (byte) into output (ulong). Assumes length is
a multiple of 4.
*/
static void _md5_decode(const uint8_t *input, uint32_t *output, uint32_t size)
{
    uint32_t i;
    uint32_t j;
    for (i = 0, j = 0; j < size; ++i, j += 4) {
        output[i] = (input[j]) | ((input[j + 1]) << 8) | ((input[j + 2]) << 16) | ((input[j + 3]) << 24);
    }
}

/* Encodes input (ulong) into output (byte). Assumes length is
a multiple of 4.
*/
static void _md5_encode(const uint32_t *input, uint8_t *output, uint32_t size)
{
    uint32_t i;
    uint32_t j;
    for (i = 0, j = 0; j < size; ++i, j += 4) {
        output[j] = (input[i] & 0xff);
        output[j + 1] = ((input[i] >> 8) & 0xff);
        output[j + 2] = ((input[i] >> 16) & 0xff);
        output[j + 3] = ((input[i] >> 24) & 0xff);
    }
}

/* MD5 basic transformation. Transforms _state based on block. */

static void _md5_transform(struct md5_context *mc, const uint8_t block[64])
{
    uint32_t *state = mc->state;

    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t *x = mc->transform_block;

    _md5_decode(block, x, 64);

    /* Round 1 */
    FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
    FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
    FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
    FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
    FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
    FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
    FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
    FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
    FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
    FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
    FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
    GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
    GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
    GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
    GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
    GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
    GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
    GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
    GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
    GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
    GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
    HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
    HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
    HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
    HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
    HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
    HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
    HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
    HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
    HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
    II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
    II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
    II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
    II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
    II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
    II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
    II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
    II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

/* MD5 finalization. Ends an MD5 message-_digest operation, writing the
the message _digest and zeroizing the context.
*/

void md5_final(struct md5_context *mc)
{
    uint8_t bits[8];
    uint32_t oldState[4];
    uint32_t oldCount[2];
    uint32_t index, padLen;

    /* Save current state and count. */
    memcpy(oldState, mc->state, 16);
    memcpy(oldCount, mc->count, 8);

    /* Save number of bits */
    _md5_encode(mc->count, bits, 8);

    /* Pad out to 56 mod 64. */
    index = (uint32_t)((mc->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    md5_update(mc, g_PADDING, padLen);

    /* Append length (before padding) */
    md5_update(mc, bits, 8);

    /* Store state in digest */
    _md5_encode(mc->state, mc->digest, 16);

    /* Restore current state and count. */
    memcpy(mc->state, oldState, 16);
    memcpy(mc->count, oldCount, 8);

    mc->finish = true;
}

/* MD5 block update operation. Continues an MD5 message-digest
operation, processing another message block, and updating the
context.
*/

void md5_update(struct md5_context *mc, const void *addr1, size_t size)
{
    const uint8_t *addr = (const uint8_t *)addr1;
    mc->finish = false;

    if (!(addr && size)) {
        return;
    }


    uint32_t i;
    uint32_t index;
    uint32_t partLen;

    /* Compute number of bytes mod 64 */
    index = ((mc->count[0] >> 3) & 0x3f);

    /* update number of bits */
    if ((mc->count[0] += (size << 3)) < (size << 3)) {
        ++mc->count[1];
    }
    mc->count[1] += (size >> 29);

    partLen = 64 - index;

    /* transform as many times as possible. */
    if (size >= partLen) {
        memcpy(&mc->buffer[index], addr, partLen);
        _md5_transform(mc, mc->buffer);

        for (i = partLen; i + 63 < size; i += 64) {
            _md5_transform(mc, &addr[i]);
        }
        index = 0;
    } else {
        i = 0;
    }

    /* Buffer remaining input */
    if (i < size) {
        memcpy(&mc->buffer[index], &addr[i], _min(64 - index, size - i));
    }
}


static const char g_HEX[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

void md5_digest(struct md5_context *mc, char digest[])
{
    const uint8_t *p = mc->digest;
    size_t i;

    if (!mc->finish) {
        return;
    }

    for (i = 0; i < _md5_digest_hex_size; ++i, p += 1) {
        digest[i * 2] = g_HEX[*p / 0x10];
        digest[i * 2 + 1] = g_HEX[*p % 0x10];
    }
    digest[i * 2] = 0;
}
