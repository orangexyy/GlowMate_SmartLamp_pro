#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mk_queue.h"

/**
 * @brief 初始化环形队列
 * 
 * @param queue 队列结构体指针
 * @param src_buf 数据缓冲区指针
 * @param src_buf_maxsize 缓冲区大小
 * @param element_size 单个元素的大小（字节）
 * @return void
 */
void mk_queue_init(TS_QUEUE* queue, void* src_buf, uint16_t src_buf_maxsize, uint16_t element_size)
{
    queue->write_index = 0;
    queue->read_index = 0;
    queue->buffer = src_buf;
    queue->max_size = src_buf_maxsize;
    queue->element_size = element_size;
}

/**
 * @brief 向队列写入数据
 * 
 * @param queue 队列结构体指针
 * @param data 要写入的数据指针
 * @param len 要写入的元素个数
 * @return true 写入成功
 * @return false 写入失败（队列已满或参数无效）
 * @note 写入长度不能超过缓冲区大小
 */
bool mk_queue_write(TS_QUEUE* queue, const void* data, uint16_t len)
{
    // 检查参数有效性
    if (queue == NULL || data == NULL || len == 0) {
        return false;
    }

    // 检查长度是否超过缓冲区大小
    if (len > queue->max_size) {
        return false;
    }

    uint8_t* src = (uint8_t*)data;
    uint8_t* dst = (uint8_t*)queue->buffer;

    // 检查剩余空间是否足够
    if(len > mk_queue_unused_count(queue))
    {
        return false;
    }

    // 检查是否需要跨边界写入
    if((queue->write_index + len) <= queue->max_size)
    {
        // 不需要跨边界，直接写入
        memcpy(dst + queue->write_index * queue->element_size, 
               src, 
               len * queue->element_size);
        queue->write_index += len;
    }
    else
    {
        // 需要跨边界写入，分两次写入
        uint16_t len1 = queue->max_size - queue->write_index;
        // 写入第一部分（到缓冲区末尾）
        memcpy(dst + queue->write_index * queue->element_size,
               src,
               len1 * queue->element_size);
        // 写入第二部分（从缓冲区开始）
        memcpy(dst,
               src + len1 * queue->element_size,
               (len - len1) * queue->element_size);
        queue->write_index = len - len1;
    }
    return true;
}

/**
 * @brief 从队列读取数据
 * 
 * @param queue 队列结构体指针
 * @param data 数据存储缓冲区指针
 * @param len 要读取的元素个数
 * @return uint32_t 实际读取的元素个数
 * @note 如果请求长度超过缓冲区大小，将限制为缓冲区大小
 */
uint32_t mk_queue_read(TS_QUEUE* queue, void* data, uint16_t len)
{
    // 检查参数有效性
    if (queue == NULL || data == NULL || len == 0) {
        return 0;
    }

    // 检查长度是否超过缓冲区大小
    if (len > queue->max_size) {
        len = queue->max_size;
    }

    uint8_t* src = (uint8_t*)queue->buffer;
    uint8_t* dst = (uint8_t*)data;

    uint16_t queue_num = mk_queue_count(queue);
    if(queue_num < len)
    {
        len = queue_num;
    }

    if(queue->buffer != NULL)
    {
        // 检查是否需要跨边界读取
        if((queue->read_index + len) <= queue->max_size)
        {
            // 不需要跨边界，直接读取
            memcpy(dst,
                   src + queue->read_index * queue->element_size,
                   len * queue->element_size);
            queue->read_index += len;
        }
        else
        {
            // 需要跨边界读取，分两次读取
            uint16_t len1 = queue->max_size - queue->read_index;
            // 读取第一部分（到缓冲区末尾）
            memcpy(dst,
                   src + queue->read_index * queue->element_size,
                   len1 * queue->element_size);
            // 读取第二部分（从缓冲区开始）
            memcpy(dst + len1 * queue->element_size,
                   src,
                   (len - len1) * queue->element_size);
            queue->read_index = len - len1;
        }
    }
    else
    {
        len = 0;
    }

    return len;
}

/**
 * @brief 获取队列总容量
 * 
 * @param queue 队列结构体指针
 * @return uint32_t 队列可存储的最大元素个数
 */
uint32_t mk_queue_size(TS_QUEUE* queue)
{
    return queue->max_size;
}

/**
 * @brief 获取队列当前元素个数
 * 
 * @param queue 队列结构体指针
 * @return uint32_t 当前队列中的元素个数
 */
uint32_t mk_queue_count(TS_QUEUE* queue)
{
    if(queue == NULL)
    {
        return 0;
    }
    
    if(queue->write_index >= queue->read_index)
    {
        return queue->write_index - queue->read_index;
    }
    else
    {
        return queue->max_size - (queue->read_index - queue->write_index);
    }
}

/**
 * @brief 获取队列剩余空间
 * 
 * @param queue 队列结构体指针
 * @return uint32_t 队列中可用的元素个数
 */
uint32_t mk_queue_unused_count(TS_QUEUE* queue)
{
    if(queue == NULL)
    {
        return 0;
    }
    return mk_queue_size(queue) - mk_queue_count(queue);
}

/**
 * @brief 检查队列是否已满
 * 
 * @param queue 队列结构体指针
 * @return true 队列已满
 * @return false 队列未满
 */
bool mk_queue_is_full(TS_QUEUE* queue)
{
    return mk_queue_unused_count(queue) == 0;
}

/**
 * @brief 检查队列是否为空
 * 
 * @param queue 队列结构体指针
 * @return true 队列为空
 * @return false 队列非空
 */
bool mk_queue_is_empty(TS_QUEUE* queue)
{
    return mk_queue_count(queue) == 0;
}

