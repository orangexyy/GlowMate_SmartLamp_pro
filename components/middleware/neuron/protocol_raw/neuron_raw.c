/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_link_f3f4.c
**  功能描述 :  链路层负责将上层数据封装为符合协议格式的帧，并通过硬件发送；
                接收硬件数据后解析帧结构，验证起始符、结束符和校验和，将有效数据传递给路由层。
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
#include <stdlib.h>
#include "ring_buf.h"
#include "neuron_config_def.h" 
#include "byte_buffer.h"
#include "mk_common.h" 
#include "neuron_raw.h"
#include "mk_log.h"

#if (NEURON_RAW_MAX_NUM)
/****************************************************************************\
                            Macro definitions
\****************************************************************************/

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef struct
{
    uint8_t port_id;
    uint8_t *rx_buffer;
    uint32_t rx_buffer_size; 
    uint16_t cmd_index; 
    raw_unpack_callback unpack_cb;
} TS_NEURON_RAW;

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static raw_send_callback s_send_cb = NULL;
static TS_NEURON_RAW s_raw_stream[NEURON_MAX_PORT_NUM] = {0};
static uint8_t s_raw_link_num = 0;
/****************************************************************************\
                            Functions definitions
\****************************************************************************/
/**
 * @brief 根据端口ID获取对应的链路对象
 * @param port_id 端口ID
 * @return TS_NEURON_LINK_F0F7* 返回对应的链路对象指针，未找到则返回NULL
 * 
 * 功能说明：
 * 遍历静态链路数组，查找与指定端口ID匹配的链路对象
 */
static TS_NEURON_RAW *neuron_link_raw_port_to_link(uint8_t port_id)
{
    int _idx = 0;
    do {
        if (s_raw_stream[_idx].port_id == port_id) {
            return &s_raw_stream[_idx]; 
        }
    } while ((++_idx) < s_raw_link_num);
    return NULL;  
}

/**
 * @brief 分发命令
 * @param port_id 端口ID
 * @param cmd_data 命令数据
 * @param cmd_len 命令长度
 * @param reply_data 回复数据
 */
static void neuron_link_raw_cmd_dispatch(uint8_t port_id, char *cmd_data, uint16_t cmd_len)
{
    extern void neuron_user_cmd_process(uint8_t port_id, char *cmd_data, uint16_t cmd_len, char *reply_data);
 
    char reply_data[NEURON_RAW_CMD_REPLY_LEN] = {0};
    neuron_user_cmd_process(port_id, cmd_data, cmd_len, reply_data);
    uint16_t reply_len = strlen(reply_data);
    if (reply_len > NEURON_RAW_CMD_REPLY_LEN) // 回复数据长度超过最大长度, 卡死, 需要修改
    {
        LOG_E("reply len too long, %d. max is %d", reply_len, NEURON_RAW_CMD_REPLY_LEN);
        while (1);
    }

    if (reply_len > 0)  
    {
        s_send_cb(port_id, (uint8_t *)reply_data, reply_len);
    }
}

/**
 * @brief 注册F3F4协议链路对象
 * @param port_id 端口ID，用于标识不同的通信端口
 * @param rx_buffer 接收数据缓冲区指针
 * @param rx_buffer_size 接收缓冲区大小
 * @param send_cb 发送回调函数
 * @return int 初始化成功返回MK_SUCCESS，失败返回MK_ERROR_MAX
 */
int neuron_link_raw_register(uint8_t port_id, uint8_t *rx_buffer, uint32_t rx_buffer_size, raw_send_callback send_cb)
{ 
    if (s_raw_link_num >= NEURON_RAW_MAX_NUM) {
        return MK_ERROR_OUT_OF_SCOPE;
    }

    s_raw_stream[s_raw_link_num].port_id = port_id;
    s_raw_stream[s_raw_link_num].cmd_index = 0; 
    s_raw_stream[s_raw_link_num].rx_buffer = rx_buffer;
    s_raw_stream[s_raw_link_num].rx_buffer_size = rx_buffer_size - 1; // 预留一个字节用于结束符
    s_send_cb = send_cb;
    s_raw_link_num++;
    return MK_SUCCESS;  
}

/**
 * @brief 解析接收到的数据，提取有效的数据帧
 * @param port_id 端口ID，标识数据来源端口
 * @param data 接收到的原始数据缓冲区
 * @param len 接收数据的长度
 * @return int 处理成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_raw_unpack(uint8_t port_id, uint8_t *data, uint16_t len)
{
    TS_NEURON_RAW *raw_stream = neuron_link_raw_port_to_link(port_id);
    if (raw_stream == NULL) {
        return MK_ERROR_NULL_POINTER;
    }

    // 如果未注册自定义解析数据回调函数，则直接调用gcode的解析
    if (raw_stream->unpack_cb == NULL)  
    {
        for (uint16_t i = 0; i < len; i++)
        {
            raw_stream->rx_buffer[raw_stream->cmd_index] = data[i];
            raw_stream->cmd_index++;

            if (data[i] == '\n') {
                // 命令结束，处理命令
                raw_stream->rx_buffer[raw_stream->cmd_index] = '\0'; 
                neuron_link_raw_cmd_dispatch(port_id, (char *)raw_stream->rx_buffer, raw_stream->cmd_index);
                raw_stream->cmd_index = 0;
                break;
            }
            else if (raw_stream->cmd_index >= raw_stream->rx_buffer_size) {
                // 接收到的数据超过协议帧最大长度，丢弃数据帧
                raw_stream->cmd_index = 0;
                break;
            }
        }
    }
    else 
    {
        raw_stream->unpack_cb(port_id, data, len); // 自定义解析数据
    }
    return MK_SUCCESS;  
}   

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
int neuron_link_raw_pack(uint8_t port_id, uint8_t *data, uint16_t len)
{
    s_send_cb(port_id, data, len);
    return MK_SUCCESS;
}  

/**
 * @brief 注册自定义解析数据回调函数
 * @param unpack_cb 数据解析回调函数
 * @return int 注册成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_raw_register_callback(uint8_t port_id, raw_unpack_callback unpack_cb)
{
    TS_NEURON_RAW *raw_stream = neuron_link_raw_port_to_link(port_id);
    if (raw_stream == NULL) {
        return MK_ERROR_NULL_POINTER;
    }

    raw_stream->unpack_cb = unpack_cb;
    return MK_SUCCESS;
}

#endif
/******************************* End of File (C）****************************/
