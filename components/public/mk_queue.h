#ifndef _MK_QUEUE_H_
#define _MK_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 通用环形队列结构体
 * 
 * 该结构体实现了一个通用的环形缓冲区，可以存储任意类型的数据。
 * 支持任意大小的缓冲区。
 */
typedef struct {
	volatile uint16_t write_index;    ///< 写入位置索引
	volatile uint16_t read_index;     ///< 读取位置索引
	uint16_t max_size;                ///< 缓冲区大小
	uint16_t element_size;            ///< 单个元素的大小（字节）
	void* buffer;                     ///< 数据缓冲区指针
} TS_QUEUE;

/**
 * @brief 初始化队列
 * 
 * @param queue 队列结构体指针
 * @param src_buf 数据缓冲区指针
 * @param src_buf_maxsize 缓冲区大小
 * @param element_size 单个元素的大小（字节）
 */
void mk_queue_init(TS_QUEUE* queue, void* src_buf, uint16_t src_buf_maxsize, uint16_t element_size);

/**
 * @brief 向队列写入数据
 * 
 * @param queue 队列结构体指针
 * @param data 要写入的数据指针
 * @param len 要写入的元素个数
 * @return true 写入成功
 * @return false 写入失败（队列已满）
 */
bool mk_queue_write(TS_QUEUE* queue, const void* data, uint16_t len);

/**
 * @brief 从队列读取数据
 * 
 * @param queue 队列结构体指针
 * @param data 数据存储缓冲区指针
 * @param len 要读取的元素个数
 * @return uint32_t 实际读取的元素个数
 */
uint32_t mk_queue_read(TS_QUEUE* queue, void* data, uint16_t len);

/**
 * @brief 获取队列总容量
 * 
 * @param queue 队列结构体指针
 * @return uint32_t 队列可存储的最大元素个数
 */
uint32_t mk_queue_size(TS_QUEUE* queue);

/**
 * @brief 获取队列当前元素个数
 * 
 * @param queue 队列结构体指针
 * @return uint32_t 当前队列中的元素个数
 */
uint32_t mk_queue_count(TS_QUEUE* queue);

/**
 * @brief 获取队列剩余空间
 * 
 * @param queue 队列结构体指针
 * @return uint32_t 队列中可用的元素个数
 */
uint32_t mk_queue_unused_count(TS_QUEUE* queue);

/**
 * @brief 检查队列是否已满
 * 
 * @param queue 队列结构体指针
 * @return true 队列已满
 * @return false 队列未满
 */
bool mk_queue_is_full(TS_QUEUE* queue);

/**
 * @brief 检查队列是否为空
 * 
 * @param queue 队列结构体指针
 * @return true 队列为空
 * @return false 队列非空
 */
bool mk_queue_is_empty(TS_QUEUE* queue);

#endif /* _MK_QUEUE_H_ */
