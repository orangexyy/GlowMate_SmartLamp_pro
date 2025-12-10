/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_protocol_f3f4.h
**  功能描述 :  F3F4协议头文件
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

#ifndef __NEURON_PROTOCOL_F3F4_H__
#define __NEURON_PROTOCOL_F3F4_H__

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************\
                            Macro definitions
\****************************************************************************/
 
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef uint32_t (*f3f4_send_callback)(uint8_t port_id, uint8_t *data, uint16_t len);
/****************************************************************************\
                            Functions declarations
\****************************************************************************/

/**
 * @brief 注册F3F4协议链路对象
 * @param port_id 端口ID，用于标识不同的通信端口
 * @param rx_buffer 接收数据缓冲区指针
 * @param rx_buffer_size 接收缓冲区大小
 * @param send_cb 发送回调函数
 * @return int 初始化成功返回MK_SUCCESS，失败返回MK_ERROR_MAX
 */
int neuron_link_f3f4_register(uint8_t port_id, uint8_t *rx_buffer, uint32_t rx_buffer_size, f3f4_send_callback send_cb);

/**
 * @brief 解析接收到的数据，提取有效的数据帧
 * @param port_id 端口ID，标识数据来源端口
 * @param data 接收到的原始数据缓冲区
 * @param len 接收数据的长度
 * @return int 处理成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_f3f4_unpack(uint8_t port_id, uint8_t *data, uint16_t len);

/**
 * @brief 将上层数据封装成F3F4协议格式的数据帧并回复
 * @param port_id 端口ID，标识发送端口
 * @param cmd 命令
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度
 * @return int 封装发送成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_f3f4_pack_reply(uint8_t port_id, uint8_t cmd, uint8_t *data, uint16_t len);

/**
 * @brief 将上层数据封装成F3F4协议格式的数据帧并发送
 * @param port_id 端口ID，标识发送端口
 * @param protocol_id 协议ID
 * @param src_addr 源地址
 * @param dst_addr 目的地址
 * @param cmd 命令
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度
 * @return int 封装发送成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_f3f4_pack_raw(uint8_t port_id, uint8_t protocol_id, uint8_t dst_addr, uint8_t cmd, uint8_t *data, uint16_t len);


/**
 * @brief 将上层数据封装成F3F4协议格式的数据帧
 * @param protocol_id 协议ID
 * @param src_addr 源地址
 * @param dst_addr 目的地址
 * @param cmd 命令
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度
 * @param pack_data 封装后的数据缓冲区
 * @return int 封装后的数据长度，大于0表示成功，小于0表示失败
 */
int neuron_link_f3f4_pack_data(uint8_t protocol_id, uint8_t dst_addr, uint8_t cmd, uint8_t *data, uint16_t len, uint8_t *pack_data);

#endif /* __NEURON_PROTOCOL_F3F4_H__ */
/**************************** End of File (H) ****************************/ 
