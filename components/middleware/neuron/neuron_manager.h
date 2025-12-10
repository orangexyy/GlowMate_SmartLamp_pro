/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_manager.c
**  功能描述 :  Neuron协议栈管理器
**              - 提供多端口多协议统一管理
**              - 支持F0F7、F3F4、F1F6、F8F9等多种协议
**              - 提供端口注册、数据收发、协议路由等核心功能
**              - 管理端口的接收和发送缓冲区
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/  

#ifndef __NEURON_MANAGER_H__
#define __NEURON_MANAGER_H__

#include <stdint.h>
#include <stdbool.h>
#include "ring_buf.h"

/****************************************************************************\
                            Macro definitions
\****************************************************************************/
/* 协议类型定义 */
#define PROTOCOL_TYPE_F0F7      (0x01)
#define PROTOCOL_TYPE_F3F4      (0x02)  
#define PROTOCOL_TYPE_RAW       (0x80) // 原始数据，不进行协议封装

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
/* 发送函数 */
typedef uint32_t (*send_callback_t)(uint8_t port_id, uint8_t *data, uint16_t len);

/* 转发函数 */
typedef uint32_t (*forward_callback_t)(uint8_t port_id, uint8_t protocol_type, uint8_t *data, uint16_t len);

/****************************************************************************\
                           Global functions definitions
\****************************************************************************/

/**
 * @brief 初始化Neuron管理器
 * @param send_callback 发送回调函数
 * @param forward_callback 转发回调函数, 需要转发时配置该函数
 * 
 * 功能说明：
 * 1. 先调用该函数进行初始化, 所有的发送和转发都会在同一个回调函数中处理, 通过port_id区分处理
 * 2. 再调用neuron_manager_install_port安装端口
 * 3. 最后调用neuron_manager_bind_protocol绑定协议
 * 4. 调用neuron_manager_process处理数据, 该函数需要定期调用, 用于处理所有端口的数据收发
 * 5. 调用neuron_manager_receive接收数据, 把该函数放在接收处理函数中, 用于接收数据到指定端口
 * 
 * 协议相关配置宏定义在neuron_config_def.h中修改
 */
void neuron_manager_init(send_callback_t send_callback, forward_callback_t forward_callback);

/**
 * @brief 初始化并注册一个新的Neuron端口
 * @param port 端口结构体指针
 * @param p_rx_buffer 接收缓冲区指针
 * @param rx_buffer_size 接收缓冲区大小
 * @param p_tx_buffer 发送缓冲区指针
 * @param tx_buffer_size 发送缓冲区大小 
 */
int neuron_manager_install_port(uint8_t port_id, uint8_t *p_rx_buffer, uint32_t rx_buffer_size, uint8_t *p_tx_buffer, uint32_t tx_buffer_size);

/**
 * @brief 为指定端口绑定协议支持
 * @param port 端口结构体指针
 * @param protocol_type 协议类型，可以是PROTOCOL_TYPE_F0F7/PROTOCOL_TYPE_F3F4等
 * @param rx_buffer 该协议的接收缓冲区指针, raw协议不需要接收缓冲区
 * @param rx_buffer_size 接收缓冲区大小, 取一帧数据的最大值, raw协议不需要接收缓冲区
 */
int neuron_manager_bind_protocol(uint8_t port_id, uint8_t protocol_type, uint8_t *rx_buffer, uint32_t rx_buffer_size);

/**
 * @brief 为指定端口解绑协议支持
 * @param port 端口结构体指针
 * @param protocol_type 协议类型，可以是PROTOCOL_TYPE_F0F7/PROTOCOL_TYPE_F3F4等 
 */
int neuron_manager_unbind_protocol(uint8_t port_id, uint8_t protocol_type);

/**
 * @brief 接收数据到指定端口
 * @param port_id 端口ID
 * @param data 数据指针
 * @param len 数据长度
 * @param direct_parse 是否直接解析，true为直接解析，false为缓存到接收缓冲区
 * @return int 接收结果
 */
int neuron_manager_receive(uint8_t port_id, uint8_t *data, uint16_t len, bool direct_parse);

/**
 * @brief 发送数据到指定端口, 直接调用发送回调函数，不经过协议处理
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return int 发送结果
 */
int neuron_manager_send_raw_data(uint8_t port_id, uint8_t *data, uint16_t len);

/**
 * @brief 处理所有端口的数据
 * 该函数需要定期调用，用于处理所有端口的数据收发
 * 1. 检查接收缓冲区，如果有数据则交给协议处理
 * 2. 检查发送缓冲区，如果有数据则交给协议打包
 */
void neuron_manager_process(void);

#endif /* __NEURON_MANAGER_H__ */
/**************************** End of File (H) ****************************/ 
