#ifndef MK_BASE64_H
#define MK_BASE64_H

int mk_base64_update(unsigned char p, char *to, int len);
int mk_base64_final(char *to, int len);
int mk_base64_encode(const unsigned char *p, int n, char *to);
int mk_base64_decode(const char *src, int n, char *dst);

#endif
