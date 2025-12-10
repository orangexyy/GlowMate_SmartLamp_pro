/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_protocol_raw.h
**  功能描述 :  RAW协议头文件
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

#ifndef __NEURON_PROTOCOL_RAW_H__
#define __NEURON_PROTOCOL_RAW_H__

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************\
                            Macro definitions
\****************************************************************************/
 
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef uint32_t (*raw_unpack_callback)(uint8_t port_id, uint8_t *data, uint16_t len);
typedef uint32_t (*raw_send_callback)(uint8_t port_id, uint8_t *data, uint16_t len);
/****************************************************************************\
                            Functions declarations
\****************************************************************************/

/**
 * @brief 注册数据解析回调函数
 * @param port_id 端口ID，标识数据来源端口
 * @param rx_buffer 接收缓冲区指针, raw协议不需要接收缓冲区
 * @param rx_buffer_size 接收缓冲区大小, 取一帧数据的最大值, raw协议不需要接收缓冲区
 * @param send_cb 发送回调函数
 * @return int 注册成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_raw_register(uint8_t port_id, uint8_t *rx_buffer, uint32_t rx_buffer_size, raw_send_callback send_cb);

/**
 * @brief 解析接收到的数据，提取有效的数据帧
 * @param port_id 端口ID，标识数据来源端口
 * @param data 接收到的原始数据缓冲区
 * @param len 接收数据的长度
 * @return int 处理成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_raw_unpack(uint8_t port_id, uint8_t *data, uint16_t len);

/**
 * @brief 将上层数据封装成F3F4协议格式的数据帧并发送
 * @param port_id 端口ID，标识发送端口
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度 
 */
int neuron_link_raw_pack(uint8_t port_id, uint8_t *data, uint16_t len);

/**
 * @brief 注册自定义解析数据回调函数
 * @param unpack_cb 数据解析回调函数
 * @return int 注册成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_raw_register_callback(uint8_t port_id, raw_unpack_callback unpack_cb);

#endif /* __NEURON_PROTOCOL_RAW_H__ */
/**************************** End of File (H) ****************************/ 
