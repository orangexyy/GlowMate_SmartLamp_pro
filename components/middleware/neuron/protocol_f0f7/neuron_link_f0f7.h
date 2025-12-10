/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_link.h
**  功能描述 :  链路层负责将上层数据封装为符合协议格式的帧，并通过硬件发送；
                接收硬件数据后解析帧结构，验证起始符、结束符和校验和，将有效数据传递给路由层。
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

#ifndef __NEURON_LINK_H__
#define __NEURON_LINK_H__

#include <stdint.h>
#include <stdbool.h> 
#include "ring_buf.h"

/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define F0F7_STREAM_SERVICE_TYPE                0x61
#define F0F7_STREAM_SERVICE_SUB_TYPE            0x61
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
// 数据类型
typedef enum
{
    NEURON_LINK_F0F7_STREAM_TYPE = 0x00,  // 流类型
    NEURON_LINK_F0F7_COMMAND_TYPE = 0x01, // 命令类型
} TE_NEURON_LINK_F0F7_TYPE;

typedef uint32_t (*f0f7_send_callback)(uint8_t port_id, uint8_t *data, uint16_t len);
typedef uint32_t (*f0f7_forward_callback)(uint8_t port_id, uint8_t protocol_type, uint8_t *data, uint16_t len);

/****************************************************************************\
                            Functions declarations
\****************************************************************************/

/**
 * @brief 注册F0F7协议链路对象
 * @param port_id 端口ID，用于标识不同的通信端口
 * @param rx_buffer 接收数据缓冲区指针
 * @param rx_buffer_size 接收缓冲区大小
 * @param send_cb 发送回调函数
 * @param forward_cb 转发回调函数 
 * 
 * 功能说明：
 * 1. 查找空闲的链路对象或已存在的端口ID对应的链路对象
 * 2. 初始化链路对象的基本参数
 * 3. 设置接收缓冲区和初始状态
 */
int neuron_link_f0f7_register(uint8_t port_id, uint8_t *rx_buffer, uint32_t rx_buffer_size, f0f7_send_callback send_cb, f0f7_forward_callback forward_cb);

/**
 * @brief 解析接收到的数据，提取有效的数据帧
 * @param port_id 端口ID，标识数据来源端口
 * @param data 接收到的原始数据缓冲区
 * @param len 接收数据的长度 
 * 
 * 功能说明：
 * 使用状态机方式解析F0F7协议数据帧：
 * 1. 查找帧起始符F0
 * 2. 接收数据直到帧结束符F7
 * 3. 验证校验和的正确性
 * 4. 处理各种异常情况（超时、非法字符、数据过长等）
 */
int neuron_link_f0f7_unpack(uint8_t port_id, uint8_t *data, uint16_t len);

/**
 * @brief 将上层数据封装成F0F7协议格式的数据帧并发送
 * @param port_id 端口ID，标识发送端口
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度 
 */
int neuron_link_f0f7_pack(uint8_t port_id, uint8_t *data, uint16_t len);

/**
 * @brief 将上层数据封装成F0F7协议格式的数据帧并发送
 * @param port_id 端口ID，标识发送端口
 * @param type 类型
 * @param sub_type 子类型
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度
 * @return int 封装发送成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_f0f7_pack_raw(uint8_t port_id, uint8_t type, uint8_t sub_type, uint8_t *data, uint16_t len);

/**
 * @brief 将上层数据封装成F0F7协议格式的数据帧
 * @param data 待发送的数据缓冲区, 从type开始到数据部分
 * @param len 待发送数据的长度
 * @param pack_data 封装后的数据缓冲区
 * @return int 封装后的数据长度
 */
int neuron_link_f0f7_pack_data(uint8_t *data, uint16_t len, uint8_t *pack_data);

#endif /* __NEURON_LINK_H__ */
/**************************** End of File (H) ****************************/ 
