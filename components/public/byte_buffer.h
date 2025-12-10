#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _BYTE_BUFFER
{
  uint32_t size;
  uint32_t index;
  uint8_t *buf;
}BYTE_BUFFER;

#ifdef __cplusplus
extern "C" {
#endif

void byte_buffer_init(BYTE_BUFFER* byte_buffer, uint8_t* buf, uint32_t buf_size, uint32_t start_index);
bool byte_buffer_read_byte_8(BYTE_BUFFER* byte_buffer, uint8_t* data);
bool byte_buffer_read_byte_16(BYTE_BUFFER* byte_buffer, uint8_t* data);
bool byte_buffer_read_short_16(BYTE_BUFFER* byte_buffer, short* data);
bool byte_buffer_read_short_24(BYTE_BUFFER* byte_buffer, short* data);
bool byte_buffer_read_long(BYTE_BUFFER* byte_buffer, long* data);
bool byte_buffer_read_float(BYTE_BUFFER* byte_buffer, float* data);
bool byte_buffer_read_buf(BYTE_BUFFER* byte_buffer, uint32_t data_size, uint8_t* data_buf);

bool byte_buffer_write_byte_8(BYTE_BUFFER* byte_buffer, uint8_t data);
bool byte_buffer_write_byte_16(BYTE_BUFFER* byte_buffer, uint8_t data);
bool byte_buffer_write_short_16(BYTE_BUFFER* byte_buffer, short data);
bool byte_buffer_write_short_24(BYTE_BUFFER* byte_buffer, short data);
bool byte_buffer_write_long(BYTE_BUFFER* byte_buffer, long data);
bool byte_buffer_write_float(BYTE_BUFFER* byte_buffer, float data);
bool byte_buffer_write_buf(BYTE_BUFFER* byte_buffer, uint32_t data_size, uint8_t* data_buf);

#ifdef __cplusplus
}
#endif

#endif
