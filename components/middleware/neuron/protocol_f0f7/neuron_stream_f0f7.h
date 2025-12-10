/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_stream_f0f7.h
**  功能描述 :  流链路层负责流链路数据传输，包括数据接收、处理和发送。
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

#ifndef __NEURON_STREAM_F0F7_H__
#define __NEURON_STREAM_F0F7_H__

#include <stdint.h>
#include <stdbool.h> 
#include "ring_buf.h"

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef enum
{
    NEURON_STREAM_F0F7_STATUS_IDLE = 0,
    NEURON_STREAM_F0F7_STATUS_START, 
    NEURON_STREAM_F0F7_STATUS_END,
} NEURON_STREAM_F0F7_STATUS;

typedef void (*neuron_stream_f0f7_cb_t)(NEURON_STREAM_F0F7_STATUS status);

/****************************************************************************\
                           Global functions definitions
\****************************************************************************/

/**
 * @brief 注册流链路
 * @param port_id 端口ID
 * @param rx_buffer 接收缓冲区
 * @param rx_buffer_size 接收缓冲区大小
 * @param data_cb 数据回调函数, 非必须实现, 如果需要对接收到的数据进行特殊处理，
 *                  如GS系列获取文件seek位置, 可以实现这个回调函数
 * @return int 注册结果
 */
int neuron_stream_f0f7_register(uint8_t port_id, uint8_t *rx_buffer, uint32_t rx_buffer_size, neuron_stream_f0f7_cb_t data_cb);

/**
 * @brief 获取流链路环形缓冲区句柄, 如果自己处理流数据时可以调用这个函数获取环形缓冲区句柄
 * @return TS_RING_BUF_DEF 环形缓冲区结构体
 */
TS_RING_BUF_DEF *neuron_stream_f0f7_get_ringbuf_handle(void);

/**
 * @brief 获取流链路环形缓冲区剩余空间
 * @return uint32_t 剩余空间大小
 */
uint32_t neuron_stream_f0f7_get_available_space(void);

/**
 * @brief 获取包序号
 * @return char 包序号
 */
char neuron_stream_f0f7_get_package_index(void);

/**
 * @brief 判断流链路环形缓冲区是否为空
 * @return bool 是否为空
 */
bool neuron_stream_f0f7_is_empty(void);

/**
 * @brief 清空流链路环形缓冲区
 */
void neuron_stream_f0f7_clear_ringbuf(void);

/**
 * @brief 获取解析的gcode行数
 * @return uint32_t 行数
 */
uint32_t neuron_stream_f0f7_get_line_count(void);

/**
 * @brief 设置解析的gcode行数
 * @param line_count 行数
 */
void neuron_stream_f0f7_set_line_count(uint32_t line_count);

/**
 * @brief 处理流链路环形缓冲区数据
 * @param port_id 端口ID
 * @return uint32_t 处理结果
 */
void neuron_stream_f0f7_process_gcode(uint8_t port_id);

#endif /* __NEURON_STREAM_F0F7_H__ */
/**************************** End of File (H) ****************************/ 
