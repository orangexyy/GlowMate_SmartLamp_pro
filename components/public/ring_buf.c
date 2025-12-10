/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  ring_buf.c
**  功能描述 :  环形缓冲区实现
**  作    者 :   
**  日    期 :   
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

#include "ring_buf.h"
#include <string.h>
#include <stdbool.h>

/**
 * @brief 初始化环形缓冲区
 * @param ring_buf 环形缓冲区结构体指针
 * @param src_buf 数据存储区指针
 * @param src_buf_maxsize 数据存储区大小
 * 
 * 功能说明：
 * 初始化环形缓冲区的各项参数，包括读写索引和缓冲区地址
 * 注意：实际可用大小为src_buf_maxsize-1，预留一个字节来区分满和空状态
 */
void ring_buf_init(TS_RING_BUF_DEF *ring_buf, uint8_t *src_buf, uint32_t src_buf_maxsize)
{
    ring_buf->write_index = 0;
    ring_buf->read_index = 0;
    ring_buf->buff_addr = src_buf;
    ring_buf->max_size = src_buf_maxsize - 1;  // 预留一个字节来区分满和空状态
    ring_buf->is_empty = 1;  // 初始状态为空
}

/**
 * @brief 从环形缓冲区读取数据
 * @param src_ringbuf 环形缓冲区结构体指针
 * @param len 需要读取的数据长度
 * @param dst_buf 输出数据缓冲区
 * @return uint32_t 实际读取到的数据长度
 * 
 * 功能说明：
 * 1. 检查缓冲区中可读数据量
 * 2. 根据读索引位置决定是否需要分段读取
 * 3. 更新读索引位置
 */
uint32_t ring_buf_read(TS_RING_BUF_DEF *src_ringbuf, uint32_t len, uint8_t *dst_buf)
{
    uint32_t size = ring_buf_count(src_ringbuf);
    if (size == 0) {
        return 0;
    }

    if (size < len)
    {
        len = size;
    }

    if ((src_ringbuf->read_index + len) <= (src_ringbuf->max_size))
    {
        // 数据连续，一次性拷贝
        memcpy(dst_buf, src_ringbuf->buff_addr + src_ringbuf->read_index, len);
        src_ringbuf->read_index += len;
    }
    else
    {
        // 数据跨越缓冲区边界，分两次拷贝
        uint32_t len1 = src_ringbuf->max_size - src_ringbuf->read_index;
        memcpy(dst_buf, src_ringbuf->buff_addr + src_ringbuf->read_index, len1);
        memcpy(dst_buf + len1, src_ringbuf->buff_addr, len - len1);
        src_ringbuf->read_index = len - len1;  // 下一个可读位置
    }
    
    // 检查是否读取完所有数据
    if (src_ringbuf->read_index == src_ringbuf->write_index) {
        src_ringbuf->is_empty = 1;  // 读取完所有数据后，缓冲区为空
    }
    
    return len;
}

/**
 * @brief 预览环形缓冲区数据（不移动读指针）
 * @param src_ringbuf 环形缓冲区结构体指针
 * @param len 需要预览的数据长度
 * @param dst_buf 输出数据缓冲区
 * @return uint32_t 实际预览到的数据长度
 * 
 * 功能说明：
 * 从队列中读取数据，但实际数据项未从队列中取出，读指针不变
 */
uint32_t ring_buf_peek(TS_RING_BUF_DEF *src_ringbuf, uint32_t len, uint8_t *dst_buf)
{
    uint32_t size = ring_buf_count(src_ringbuf);
    
    if (size < len)
    {
        len = size;
    }

    if (len == 0) {
        return 0;
    }

    // 使用预览索引进行数据拷贝
    uint32_t peek_index = src_ringbuf->peek_index;
    if ((peek_index + len) <= (src_ringbuf->max_size))
    {
        // 数据连续，一次性拷贝
        memcpy(dst_buf, src_ringbuf->buff_addr + peek_index, len);
    }
    else
    {
        // 数据跨越缓冲区边界，分两次拷贝
        uint32_t len1 = src_ringbuf->max_size - peek_index;
        memcpy(dst_buf, src_ringbuf->buff_addr + peek_index, len1);
        memcpy(dst_buf + len1, src_ringbuf->buff_addr, len - len1);
    }
    
    // 更新预览索引
    peek_index += len;
    if (peek_index >= src_ringbuf->max_size) {
        peek_index = 0;  // 回绕到开始位置
    }
    src_ringbuf->peek_index = peek_index;
    
    return len;
}

/**
 * @brief 重置预览索引
 * @param src_ringbuf 环形缓冲区结构体指针
 * 
 * 功能说明：
 * 将预览索引重置为读索引位置
 */
void ring_buf_peeked_reset(TS_RING_BUF_DEF *src_ringbuf)
{
    src_ringbuf->peek_index = src_ringbuf->read_index;
}

/**
 * @brief 确认预览的数据（将读索引移动到预览索引位置）
 * @param src_ringbuf 环形缓冲区结构体指针
 * 
 * 功能说明：
 * 将读索引移动到预览索引位置，确认预览的数据已被处理
 */
void ring_buf_peeked_flush(TS_RING_BUF_DEF *src_ringbuf)
{
    src_ringbuf->read_index = src_ringbuf->peek_index;
    
    // 检查是否读取完所有数据
    if (src_ringbuf->read_index == src_ringbuf->write_index) {
        src_ringbuf->is_empty = 1;  // 读取完所有数据后，缓冲区为空
    }
}

/**
 * @brief 获取已预览的数据量
 * @param src_ringbuf 环形缓冲区结构体指针
 * @return uint32_t 已预览的数据量
 * 
 * 功能说明：
 * 计算从读索引到预览索引之间的数据量
 */
uint32_t ring_buf_peeked_counter(TS_RING_BUF_DEF *src_ringbuf)
{
    uint32_t count;
    if (src_ringbuf->peek_index >= src_ringbuf->read_index)
    {
        // 预览索引在读索引之后
        count = src_ringbuf->peek_index - src_ringbuf->read_index;
    }
    else
    {
        // 预览索引在读索引之前（跨越了缓冲区边界）
        count = src_ringbuf->max_size - (src_ringbuf->read_index - src_ringbuf->peek_index);
    }
    return count;
}

/**
 * @brief 获取可预览的数据量
 * @param src_ringbuf 环形缓冲区结构体指针
 * @return uint32_t 可预览的数据量
 * 
 * 功能说明：
 * 计算从预览索引到写索引之间的可预览数据量
 */
uint32_t ring_buf_peek_count(TS_RING_BUF_DEF *src_ringbuf)
{
    uint32_t count = 0;
    if (src_ringbuf->write_index >= src_ringbuf->peek_index)
    {
        // 写索引在预览索引之后
        count = src_ringbuf->write_index - src_ringbuf->peek_index;
    }
    else
    {
        // 写索引在预览索引之前（跨越了缓冲区边界）
        count = src_ringbuf->max_size - (src_ringbuf->peek_index - src_ringbuf->write_index);
    }
    return count;
}

/**
 * @brief 改变预览位置
 * @param src_ringbuf 环形缓冲区结构体指针
 * @param offset 偏移量
 * 
 * 功能说明：
 * 将预览索引从当前读索引位置偏移指定距离
 */
void ring_buf_peek_pos_change(TS_RING_BUF_DEF *src_ringbuf, uint32_t offset)
{
    uint32_t new_peek_index = src_ringbuf->read_index + offset;
    
    // 处理回绕
    if (new_peek_index >= src_ringbuf->max_size) {
        new_peek_index = new_peek_index % src_ringbuf->max_size;
    }
    
    src_ringbuf->peek_index = new_peek_index;
}

/**
 * @brief 向环形缓冲区写入数据
 * @param des_ringbuf 环形缓冲区结构体指针
 * @param src 源数据缓冲区
 * @param len 需要写入的数据长度
 * @return _Bool 写入成功返回true，失败返回false
 * 
 * 功能说明：
 * 1. 检查缓冲区剩余空间是否足够
 * 2. 根据写索引位置决定是否需要分段写入
 * 3. 更新写索引位置
 */
_Bool ring_buf_write(TS_RING_BUF_DEF *des_ringbuf, uint8_t *src, uint32_t len)
{
    if (len > ring_buf_unused_count(des_ringbuf))
    {
        return false;
    }
    if ((des_ringbuf->write_index + len) <= (des_ringbuf->max_size))
    {
        // 数据连续，一次性写入
        memcpy(des_ringbuf->buff_addr + des_ringbuf->write_index, src, len);
        des_ringbuf->write_index += len;
    }
    else
    {
        // 数据跨越缓冲区边界，分两次写入
        uint32_t len1 = des_ringbuf->max_size - des_ringbuf->write_index;
        memcpy(des_ringbuf->buff_addr + des_ringbuf->write_index, src, len1);
        memcpy(des_ringbuf->buff_addr, src + len1, len - len1);
        des_ringbuf->write_index = len - len1;  // 下一个可写位置
    }
    des_ringbuf->is_empty = 0;  // 写入数据后，缓冲区非空
    return true;
}

/**
 * @brief 获取环形缓冲区剩余空间大小
 * @param src_ringbuf 环形缓冲区结构体指针
 * @return uint32_t 剩余空间大小
 * 
 * 功能说明：
 * 计算缓冲区中可写入的剩余空间大小
 */
uint32_t ring_buf_unused_count(TS_RING_BUF_DEF *src_ringbuf)
{
    return (src_ringbuf->max_size - ring_buf_count(src_ringbuf));
}

/**
 * @brief 获取环形缓冲区中数据长度
 * @param src_ringbuf 环形缓冲区结构体指针
 * @return uint32_t 缓冲区中的数据长度
 * 
 * 功能说明：
 * 根据读写索引计算缓冲区中当前存储的数据量
 */
uint32_t ring_buf_count(TS_RING_BUF_DEF *src_ringbuf)
{
    // 如果缓冲区为空，直接返回0
    if (src_ringbuf->is_empty) {
        return 0;
    }
    
    uint32_t count;
    if (src_ringbuf->write_index > src_ringbuf->read_index)
    {
        // 写索引在读索引之后
        count = (src_ringbuf->write_index - src_ringbuf->read_index);
    }
    else if (src_ringbuf->write_index < src_ringbuf->read_index)
    {
        // 写索引在读索引之前（跨越了缓冲区边界）
        count = (src_ringbuf->max_size) - (src_ringbuf->read_index - src_ringbuf->write_index);
    }
    else
    {
        // 写索引等于读索引，且缓冲区非空，说明是满缓冲区
        count = src_ringbuf->max_size;
    }
    return count;
}

/**
 * @brief 判断环形缓冲区是否为空
 * @param src_ringbuf 环形缓冲区结构体指针
 * @return _Bool 缓冲区为空返回true，非空返回false
 * 
 * 功能说明：
 * 快速判断缓冲区是否为空，直接使用is_empty标志位
 */
_Bool ring_buf_is_empty(TS_RING_BUF_DEF *src_ringbuf)
{
    return (src_ringbuf->is_empty == 1);
}

/**
 * @brief 判断环形缓冲区是否已满
 * @param src_ringbuf 环形缓冲区结构体指针
 * @return _Bool 缓冲区已满返回true，未满返回false
 * 
 * 功能说明：
 * 判断缓冲区是否已满，通过检查剩余空间是否为0
 */
_Bool ring_buf_is_full(TS_RING_BUF_DEF *src_ringbuf)
{
    return (ring_buf_unused_count(src_ringbuf) == 0);
}

/**
 * @brief 清空环形缓冲区
 * @param src_ringbuf 环形缓冲区结构体指针
 * 
 * 功能说明：
 * 重置所有索引为0，清空缓冲区中的数据
 */
void ring_buf_flush(TS_RING_BUF_DEF *src_ringbuf)
{
    src_ringbuf->write_index = 0;
    src_ringbuf->read_index = 0;
    src_ringbuf->peek_index = 0;
    src_ringbuf->is_empty = 1;  // 清空后，缓冲区为空
}
