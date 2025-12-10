/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  ring_buf.h
**  功能描述 :  环形缓冲区头文件
**  作    者 :  hawis.qin
**  日    期 :  2020/01/09
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
                2 添加完整预览功能
\****************************************************************************/
#ifndef __RING_BUF_H__
#define __RING_BUF_H__

#include <stdint.h>
#include <stdio.h>

typedef struct
{
    volatile uint32_t write_index;
    volatile uint32_t read_index;
    volatile uint32_t peek_index;
    uint32_t max_size;
    uint8_t *buff_addr;
    volatile uint8_t is_empty;  // 标志位：1表示空，0表示非空
} TS_RING_BUF_DEF;

// 基本操作函数
void ring_buf_init(TS_RING_BUF_DEF *ring_buf, uint8_t *src_buf, uint32_t src_buf_maxsize);
uint32_t ring_buf_read(TS_RING_BUF_DEF *src_ringbuf, uint32_t len, uint8_t *dst_buf);
_Bool ring_buf_write(TS_RING_BUF_DEF *des_ringbuf, uint8_t *src, uint32_t len);
void ring_buf_flush(TS_RING_BUF_DEF *src_ringbuf);

// 状态查询函数
uint32_t ring_buf_count(TS_RING_BUF_DEF *src_ringbuf);
uint32_t ring_buf_unused_count(TS_RING_BUF_DEF *src_ringbuf);
_Bool ring_buf_is_empty(TS_RING_BUF_DEF *src_ringbuf);
_Bool ring_buf_is_full(TS_RING_BUF_DEF *src_ringbuf);

// 预览功能函数
uint32_t ring_buf_peek(TS_RING_BUF_DEF *src_ringbuf, uint32_t len, uint8_t *dst_buf);
void ring_buf_peeked_reset(TS_RING_BUF_DEF *src_ringbuf);
void ring_buf_peeked_flush(TS_RING_BUF_DEF *src_ringbuf);
uint32_t ring_buf_peeked_counter(TS_RING_BUF_DEF *src_ringbuf);
uint32_t ring_buf_peek_count(TS_RING_BUF_DEF *src_ringbuf);
void ring_buf_peek_pos_change(TS_RING_BUF_DEF *src_ringbuf, uint32_t offset);

#endif
/**************************** End of File (H) ****************************/
