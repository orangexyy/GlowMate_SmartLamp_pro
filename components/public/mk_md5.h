#ifndef MK_MD5_H
#define MK_MD5_H

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    uint32_t      buf[4];
    uint32_t      bits[2];
    unsigned char in[64];
} mk_md5_ctx;

void mk_md5_init(mk_md5_ctx *c);
void mk_md5_update(mk_md5_ctx *c, const unsigned char *data, size_t len);
void mk_md5_final(mk_md5_ctx *c, unsigned char[16]);

#endif
