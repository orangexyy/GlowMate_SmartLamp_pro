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

/****************************************************************************\
                               Includes
\****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ring_buf.h"
#include "mk_common.h"
#include "neuron_manager.h"
#include "neuron_config_def.h"
#include "neuron_link_f0f7.h"
#include "neuron_link_f3f4.h"
#include "neuron_raw.h"

/****************************************************************************\
                            Macro definitions
\****************************************************************************/

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef struct {
    uint8_t         port_id;        /* 端口ID */
    uint8_t         protocol_type;  /* 当前使用的协议类型 */
    TS_RING_BUF_DEF rx_buffer;      /* 接收环形缓冲区 */
    TS_RING_BUF_DEF tx_buffer;      /* 发送环形缓冲区 */
} TS_NEURON_PROTOCOL;

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
// 存储所有Neuron端口的指针数组
static TS_NEURON_PROTOCOL s_neuron_list[NEURON_MAX_PORT_NUM] = {0};
static volatile uint8_t s_neuron_num = 0;
static send_callback_t s_send_callback = NULL;
static forward_callback_t s_forward_callback = NULL;
/****************************************************************************\
                            Functions definitions
\****************************************************************************/
extern void neuron_stream_f3f4_process_data(void);

/**
 * @brief 根据端口ID获取对应的协议结构体指针
 * @param port_id 端口ID
 * @return TS_NEURON_PROTOCOL* 返回对应的协议结构体指针，未找到则返回NULL
 */
static inline TS_NEURON_PROTOCOL *neuron_manager_port_to_protocol(uint8_t port_id)
{
    int _idx = 0;
    do {
        if (s_neuron_list[_idx].port_id == port_id) {
            return &s_neuron_list[_idx]; 
        }       
    } while ((++_idx) < s_neuron_num);
    return NULL;  
}

/**
 * @brief 从指定端口发送数据
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return uint32_t 实际发送的数据长度
 */
static uint32_t neuron_manager_send(uint8_t port_id, uint8_t *data, uint16_t len)
{
    TS_NEURON_PROTOCOL *protocol = neuron_manager_port_to_protocol(port_id);
    if (protocol == NULL) {
        return 0;
    }
    
    if (protocol->tx_buffer.max_size > 0) {
        return ring_buf_write(&protocol->tx_buffer, data, len);
    }
    return 0;
}

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
 */
void neuron_manager_init(send_callback_t send_callback, forward_callback_t forward_callback)
{
    s_send_callback = send_callback;
    s_forward_callback = forward_callback;
    s_neuron_num = 0;
    memset(s_neuron_list, 0, sizeof(s_neuron_list));
}

/**
 * @brief 初始化并注册一个新的Neuron端口
 * @param port_id 端口ID, 自定义枚举
 * @param p_rx_buffer 接收缓冲区指针
 * @param rx_buffer_size 接收缓冲区大小
 * @param p_tx_buffer 发送缓冲区指针
 * @param tx_buffer_size 发送缓冲区大小
 * @return int 初始化结果
 */
int neuron_manager_install_port(uint8_t port_id, uint8_t *p_rx_buffer, uint32_t rx_buffer_size, 
                              uint8_t *p_tx_buffer, uint32_t tx_buffer_size)
{
    if (s_neuron_num >= NEURON_MAX_PORT_NUM) {
        return MK_ERROR_OUT_OF_SCOPE;
    }

    // 检查端口ID是否已存在
    if (neuron_manager_port_to_protocol(port_id) != NULL) {
        return MK_ERROR_ID;
    }

    TS_NEURON_PROTOCOL *protocol = &s_neuron_list[s_neuron_num];
    
    if (p_rx_buffer != NULL && rx_buffer_size > 0) {
        ring_buf_init(&protocol->rx_buffer, p_rx_buffer, rx_buffer_size);
    }
    if (p_tx_buffer != NULL && tx_buffer_size > 0) {
        ring_buf_init(&protocol->tx_buffer, p_tx_buffer, tx_buffer_size);
    }

    protocol->port_id = port_id;
    protocol->protocol_type = 0;
    s_neuron_num++;
    
    return MK_SUCCESS;
}

/**
 * @brief 为指定端口绑定协议支持
 * @param port_id 端口ID
 * @param protocol_type 协议类型
 * @param rx_buffer 接收缓冲区指针, raw协议不需要接收缓冲区
 * @param rx_buffer_size 接收缓冲区大小, 取一帧数据的最大值, raw协议不需要接收缓冲区
 * @return int 绑定结果
 */
int neuron_manager_bind_protocol(uint8_t port_id, uint8_t protocol_type, 
                               uint8_t *rx_buffer, uint32_t rx_buffer_size)
{
    TS_NEURON_PROTOCOL *protocol = neuron_manager_port_to_protocol(port_id);
    if (protocol == NULL) {
        return MK_ERROR_NULL_POINTER;
    }

    int ret = MK_SUCCESS;
    if (protocol_type == PROTOCOL_TYPE_F0F7) {
        // 初始化F0F7协议
        #if (NEURON_F0F7_MAX_NUM)
        ret = neuron_link_f0f7_register(port_id, rx_buffer, rx_buffer_size, neuron_manager_send, s_forward_callback);
        #endif
    } else if (protocol_type == PROTOCOL_TYPE_F3F4) {
        // 初始化F3F4协议
        #if (NEURON_F3F4_MAX_NUM)
        ret = neuron_link_f3f4_register(port_id, rx_buffer, rx_buffer_size, neuron_manager_send);
        #endif
    } else if (protocol_type == PROTOCOL_TYPE_RAW) {
        // 初始化RAW协议
        #if (NEURON_RAW_MAX_NUM)
        ret = neuron_link_raw_register(port_id, rx_buffer, rx_buffer_size, neuron_manager_send);
        #endif
    } 
    
    if (ret == MK_SUCCESS) {
        protocol->protocol_type |= protocol_type;
    }
    
    return ret;
}

/**
 * @brief 为指定端口解绑协议支持
 * @param port 端口结构体指针
 * @param protocol_type 协议类型，可以是PROTOCOL_TYPE_F0F7/PROTOCOL_TYPE_F3F4等 
 */
int neuron_manager_unbind_protocol(uint8_t port_id, uint8_t protocol_type)
{
    TS_NEURON_PROTOCOL *protocol = neuron_manager_port_to_protocol(port_id);    
    if (protocol == NULL) {
        return MK_ERROR_NULL_POINTER;
    }

    protocol->protocol_type &= ~protocol_type;
    return MK_SUCCESS;
}

/**
 * @brief 接收数据到指定端口
 * @param port_id 端口ID
 * @param data 数据指针
 * @param len 数据长度
 * @return int 接收结果
 */
int neuron_manager_receive(uint8_t port_id, uint8_t *data, uint16_t len, bool direct_parse)
{
    TS_NEURON_PROTOCOL *protocol = neuron_manager_port_to_protocol(port_id);
    if (protocol == NULL) {
        return MK_ERROR_NULL_POINTER;
    }

    // 如果直接解析，则立即处理数据
    if (direct_parse) { 
        #if (NEURON_F0F7_MAX_NUM)
        if (protocol->protocol_type & PROTOCOL_TYPE_F0F7) {
            return neuron_link_f0f7_unpack(protocol->port_id, data, len);
        }
        #endif
        #if (NEURON_F3F4_MAX_NUM)
        if (protocol->protocol_type & PROTOCOL_TYPE_F3F4) {
            return neuron_link_f3f4_unpack(protocol->port_id, data, len);
        }
        #endif
        return MK_SUCCESS;
    }
    
    // 否则将数据写入接收缓冲区
    if (protocol->rx_buffer.max_size > 0) {
        return ring_buf_write(&protocol->rx_buffer, data, len);
    }
    return MK_ERROR_OUT_OF_SCOPE;
}

/**
 * @brief 发送数据到指定端口, 直接调用发送回调函数，不经过协议处理
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return int 发送结果
 */
int neuron_manager_send_raw_data(uint8_t port_id, uint8_t *data, uint16_t len)
{
    if (s_send_callback != NULL) {
        s_send_callback(port_id, data, len);
    }
    return MK_SUCCESS;
}

/**
 * @brief 处理所有端口的数据
 * 该函数需要定期调用，用于处理所有端口的数据收发
 * 1. 检查接收缓冲区，如果有数据则交给协议处理
 * 2. 检查发送缓冲区，如果有数据则交给协议打包
 */
void neuron_manager_process(void)
{
    TS_NEURON_PROTOCOL *protocol;
    uint8_t data[NEURON_ONCE_PROCESS_LEN];
    uint16_t len;

    for (int i = 0; i < s_neuron_num; i++) {
        protocol = &s_neuron_list[i];

        // 处理接收缓冲区数据
        while ((len = ring_buf_read(&protocol->rx_buffer, sizeof(data), data)) > 0) {
            #if (NEURON_F0F7_MAX_NUM)
            if (protocol->protocol_type & PROTOCOL_TYPE_F0F7) {
                neuron_link_f0f7_unpack(protocol->port_id, data, len);
            }
            #endif
            #if (NEURON_F3F4_MAX_NUM)
            if (protocol->protocol_type & PROTOCOL_TYPE_F3F4) {
                neuron_link_f3f4_unpack(protocol->port_id, data, len);
            }
            #endif
            #if (NEURON_RAW_MAX_NUM)
            if (protocol->protocol_type & PROTOCOL_TYPE_RAW) {
                neuron_link_raw_unpack(protocol->port_id, data, len);
            }
            #endif
        }

        // 处理发送缓冲区数据
        if (s_send_callback != NULL) {
            while ((len = ring_buf_read(&protocol->tx_buffer, sizeof(data), data)) > 0) {
                s_send_callback(protocol->port_id, data, len);
            }
        }
    }

#if NEURON_F3F4_MAX_NUM
    neuron_stream_f3f4_process_data();
#endif
}

/******************************* End of File (C）****************************/
