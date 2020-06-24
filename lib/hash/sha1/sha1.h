/*
  100% free public domain implementation of the SHA-1 algorithm

  === Test Vectors (from FIPS PUB 180-1) ===

  "abc"
    A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D

  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1

  A million repetitions of "a"
    34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

#ifndef HASH_SHA1_H
#define HASH_SHA1_H


struct sha1_context;

void sha1_create(struct sha1_context **sc);
void sha1_release(struct sha1_context **sc);
void sha1_clear(struct sha1_context *);
void sha1_final(struct sha1_context *);
void sha1_update(struct sha1_context *, const void *, size_t);
void sha1_digest(struct sha1_context *, char digest[40 + 1]);

#endif
