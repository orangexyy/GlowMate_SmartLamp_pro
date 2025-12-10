#ifndef MK_SHA1_H
#define MK_SHA1_H

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    uint32_t      state[5];
    uint32_t      count[2];
    unsigned char buffer[64];
} mk_sha1_ctx;

void mk_sha1_init(mk_sha1_ctx *);
void mk_sha1_update(mk_sha1_ctx *, const unsigned char *data, size_t len);
void mk_sha1_final(unsigned char digest[20], mk_sha1_ctx *);
void mk_hmac_sha1(const unsigned char *key, size_t key_len, const unsigned char *text, size_t text_len, unsigned char out[20]);

#endif
