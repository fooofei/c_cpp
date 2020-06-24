#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "sha1.h"


#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif

enum {
    MAX_FILE_READ_BUFFER = 4096,
};

#define ROL32(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#ifdef LITTLE_ENDIAN
#define SHABLK0(i) (block->l[i] = (ROL32(block->l[i], 24) & 0xFF00FF00) | (ROL32(block->l[i], 8) & 0x00FF00FF))
#else
#define SHABLK0(i) (block->l[i])
#endif


#define SHABLK(i) (block->l[i & 15] = \
         ROL32(block->l[(i + 13) & 15] ^ block->l[(i + 8) & 15] ^ block->l[(i + 2) & 15] ^ block->l[i & 15], 1))

#define R0(v, w, x, y, z, i) {                                                                     \
        z += ((w & (x ^ y)) ^ y) + SHABLK0(i) + 0x5A827999 + ROL32(v, 5); \
        w = ROL32(w, 30);                                                 \
    }
#define R1(v, w, x, y, z, i) {                                                                    \
        z += ((w & (x ^ y)) ^ y) + SHABLK(i) + 0x5A827999 + ROL32(v, 5); \
        w = ROL32(w, 30);                                                \
    }
#define R2(v, w, x, y, z, i) {                                                            \
        z += (w ^ x ^ y) + SHABLK(i) + 0x6ED9EBA1 + ROL32(v, 5); \
        w = ROL32(w, 30);                                        \
    }
#define R3(v, w, x, y, z, i) {                                                                          \
        z += (((w | x) & y) | (w & x)) + SHABLK(i) + 0x8F1BBCDC + ROL32(v, 5); \
        w = ROL32(w, 30);                                                      \
    }
#define R4(v, w, x, y, z, i) {                                                            \
        z += (w ^ x ^ y) + SHABLK(i) + 0xCA62C1D6 + ROL32(v, 5); \
        w = ROL32(w, 30);                                        \
    }

enum {
    _sha1_digest_hex_size = 20,
};

struct sha1_context {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t buffer[64];
    uint8_t digest[_sha1_digest_hex_size];
    uint8_t transform_block[64];
    bool finish;
};


typedef union {
    uint8_t c[64];
    uint32_t l[16];
} sha1_workspace_block_t;


void sha1_create(struct sha1_context **sc)
{
    *sc = calloc(1, sizeof(**sc));
}

void sha1_release(struct sha1_context **sc)
{
    free(*sc);
    *sc = 0;
}


void sha1_clear(struct sha1_context *sc)
{
    memset(sc, 0, sizeof(*sc));
    uint32_t t[] = {
        0x67452301,
        0xEFCDAB89,
        0x98BADCFE,
        0x10325476,
        0xC3D2E1F0
    };
    memcpy(sc->state, t, sizeof(t));

    sc->finish = true;
}


static void _sha1_transform(struct sha1_context *sc, const uint8_t buffer[64])
{
    uint32_t a = 0, b = 0, c = 0, d = 0, e = 0;

    sha1_workspace_block_t *block;
    uint32_t *state;

    block = (sha1_workspace_block_t *)sc->transform_block;
    memcpy(block, buffer, 64);
    state = sc->state;

    // Copy state[] to working vars
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    // 4 rounds of 20 operations each. Loop unrolled.
    R0(a, b, c, d, e, 0);
    R0(e, a, b, c, d, 1);
    R0(d, e, a, b, c, 2);
    R0(c, d, e, a, b, 3);
    R0(b, c, d, e, a, 4);
    R0(a, b, c, d, e, 5);
    R0(e, a, b, c, d, 6);
    R0(d, e, a, b, c, 7);
    R0(c, d, e, a, b, 8);
    R0(b, c, d, e, a, 9);
    R0(a, b, c, d, e, 10);
    R0(e, a, b, c, d, 11);
    R0(d, e, a, b, c, 12);
    R0(c, d, e, a, b, 13);
    R0(b, c, d, e, a, 14);
    R0(a, b, c, d, e, 15);
    R1(e, a, b, c, d, 16);
    R1(d, e, a, b, c, 17);
    R1(c, d, e, a, b, 18);
    R1(b, c, d, e, a, 19);
    R2(a, b, c, d, e, 20);
    R2(e, a, b, c, d, 21);
    R2(d, e, a, b, c, 22);
    R2(c, d, e, a, b, 23);
    R2(b, c, d, e, a, 24);
    R2(a, b, c, d, e, 25);
    R2(e, a, b, c, d, 26);
    R2(d, e, a, b, c, 27);
    R2(c, d, e, a, b, 28);
    R2(b, c, d, e, a, 29);
    R2(a, b, c, d, e, 30);
    R2(e, a, b, c, d, 31);
    R2(d, e, a, b, c, 32);
    R2(c, d, e, a, b, 33);
    R2(b, c, d, e, a, 34);
    R2(a, b, c, d, e, 35);
    R2(e, a, b, c, d, 36);
    R2(d, e, a, b, c, 37);
    R2(c, d, e, a, b, 38);
    R2(b, c, d, e, a, 39);
    R3(a, b, c, d, e, 40);
    R3(e, a, b, c, d, 41);
    R3(d, e, a, b, c, 42);
    R3(c, d, e, a, b, 43);
    R3(b, c, d, e, a, 44);
    R3(a, b, c, d, e, 45);
    R3(e, a, b, c, d, 46);
    R3(d, e, a, b, c, 47);
    R3(c, d, e, a, b, 48);
    R3(b, c, d, e, a, 49);
    R3(a, b, c, d, e, 50);
    R3(e, a, b, c, d, 51);
    R3(d, e, a, b, c, 52);
    R3(c, d, e, a, b, 53);
    R3(b, c, d, e, a, 54);
    R3(a, b, c, d, e, 55);
    R3(e, a, b, c, d, 56);
    R3(d, e, a, b, c, 57);
    R3(c, d, e, a, b, 58);
    R3(b, c, d, e, a, 59);
    R4(a, b, c, d, e, 60);
    R4(e, a, b, c, d, 61);
    R4(d, e, a, b, c, 62);
    R4(c, d, e, a, b, 63);
    R4(b, c, d, e, a, 64);
    R4(a, b, c, d, e, 65);
    R4(e, a, b, c, d, 66);
    R4(d, e, a, b, c, 67);
    R4(c, d, e, a, b, 68);
    R4(b, c, d, e, a, 69);
    R4(a, b, c, d, e, 70);
    R4(e, a, b, c, d, 71);
    R4(d, e, a, b, c, 72);
    R4(c, d, e, a, b, 73);
    R4(b, c, d, e, a, 74);
    R4(a, b, c, d, e, 75);
    R4(e, a, b, c, d, 76);
    R4(d, e, a, b, c, 77);
    R4(c, d, e, a, b, 78);
    R4(b, c, d, e, a, 79);

    // Add the working vars back into state[]
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;

    // Wipe variables
    a = 0;
    b = 0;
    c = 0;
    d = 0;
    e = 0;

    memset(sc->transform_block, 0, sizeof(sc->transform_block));
}


void sha1_final(struct sha1_context *sc)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint8_t finalcount[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    for (i = 0; i < 8; i++) {
        finalcount[i] = (uint8_t)((sc->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255); // Endian independent
    }

    sha1_update(sc, (uint8_t *)"\200", 1);

    for (; (sc->count[0] & 504) != 448;) {
        sha1_update(sc, (uint8_t *)"\0", 1);
    }


    sha1_update(sc, finalcount, 8); // Cause a SHA1Transform()

    for (i = 0; i < 20; i++) {
        sc->digest[i] = (uint8_t)((sc->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }

    _sha1_transform(sc, sc->buffer);

    sc->finish = true;
}
void sha1_update(struct sha1_context *sc, const void *addr1, size_t size)
{
    const uint8_t *addr = (const uint8_t *)addr1;
    sc->finish = false;

    if (!(addr && size)) {
        return;
    }

    uint32_t i = 0;
    uint32_t j = 0;

    j = (sc->count[0] >> 3) & 63;

    if ((sc->count[0] += size << 3) < (size << 3)) {
        sc->count[1]++;
    }

    sc->count[1] += (size >> 29);

    if ((j + size) > 63) {
        memcpy(&sc->buffer[j], addr, (i = 64 - j));
        _sha1_transform(sc, sc->buffer);

        for (; i + 63 < size; i += 64) {
            _sha1_transform(sc, &addr[i]);
        }

        j = 0;
    } else {
        i = 0;
    }

    memcpy(&sc->buffer[j], &addr[i], size - i);
}


static const char g_HEX[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

void sha1_digest(struct sha1_context *sc, char digest[])
{
    const uint8_t *p = sc->digest;
    size_t i;

    if (!sc->finish) {
        return;
    }

    for (i = 0; i < _sha1_digest_hex_size; ++i, p += 1) {
        digest[i * 2] = g_HEX[*p / 0x10];
        digest[i * 2 + 1] = g_HEX[*p % 0x10];
    }
    digest[i * 2] = 0;
}
