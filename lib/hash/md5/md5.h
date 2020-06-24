#ifndef HASH_MD5_H
#define HASH_MD5_H

struct md5_context;

void md5_create(struct md5_context **sc);
void md5_release(struct md5_context **sc);

void md5_clear(struct md5_context *sc);
void md5_final(struct md5_context *);
void md5_update(struct md5_context *, const void *, size_t);
void md5_digest(struct md5_context *, char digest[32 + 1]);


#endif // HASH_MD5_H
