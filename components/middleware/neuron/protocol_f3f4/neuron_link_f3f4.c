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
#include "neuron_link_f3f4.h" 
#include "router_dispatch_f3f4.h"
#include "byte_buffer.h"
#include "mk_common.h"

#if NEURON_F3F4_MAX_NUM
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define NEURON_LINK_F3F4_FRAME_START            (0xF3)    // 帧起始符：F3
#define NEURON_LINK_F3F4_FRAME_END              (0xF4)    // 帧结束符：F4

#define NEURON_LINK_F3F4_HEAD_OFFSET            (0) // 头偏移
#define NEURON_LINK_F3F4_HEADER_CHECK_OFFSET    (1) // 头校验偏移
#define NEURON_LINK_F3F4_LENGTH_OFFSET          (2) // 长度偏移
#define NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET     (4) // 协议ID偏移
#define NEURON_LINK_F3F4_SRC_ADDR_OFFSET        (5) // 源地址偏移
#define NEURON_LINK_F3F4_DST_ADDR_OFFSET        (6) // 目的地址偏移
#define NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET (7) // 包序偏移
#define NEURON_LINK_F3F4_CMD_OFFSET             (11) // 命令偏移
#define NEURON_LINK_F3F4_DATA_OFFSET            (12) // 数据偏移

#define NEURON_LINK_F3F4_FRAME_HEAD_SIZE        (4)       // 帧头部大小

#define NEURON_LINK_F3F4_PACK_CMD_LEN_SIZE      (8)  // 封装命令长度   协议ID+源地址+目的地址+序列号+命令ID


#define NEURON_LINK_F3F4_BYTES_TO_UINT16(b0, b1)        ((uint16_t)(b0) | ((uint16_t)(b1) << 8))
#define NEURON_LINK_F3F4_BYTES_TO_UINT32(b0, b1, b2, b3) ((uint32_t)(b0) | ((uint32_t)(b1) << 8) | ((uint32_t)(b2) << 16) | ((uint32_t)(b3) << 24))


/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
// 链路层状态机状态定义
typedef enum
{
    NEURON_LINK_F3F4_STATE_IDLE = 0x00,          // 空闲状态，等待帧起始符 
    NEURON_LINK_F3F4_STATE_FRAME_HEADER,         // 帧格式检测
    NEURON_LINK_F3F4_STATE_RECEIVING,            // 正在接收数据状态  
} TE_NEURON_LINK_F3F4_STATE;

// 链路层处理状态定义
#define NEURON_LINK_F3F4_STATUS_NULL 0x00 // 空状态
#define NEURON_LINK_F3F4_STATUS_SUCCESS 0x01 // 接收成功
#define NEURON_LINK_F3F4_STATUS_CRC_ERROR 0x02 // 校验和错误
#define NEURON_LINK_F3F4_STATUS_INVALID_CHARACTER 0x03 // 非法字符 
#define NEURON_LINK_F3F4_STATUS_TOO_LONG 0x04 // 数据帧过长  
#define NEURON_LINK_F3F4_STATUS_SAME_PACKAGE 0x05 // 重复数据包

typedef struct {
    uint8_t  port_id;
    uint8_t  state;   
    uint16_t data_size; 
    uint16_t recv_len;
    uint8_t  protocol_id;
    uint8_t  src_addr;
    uint8_t  dst_addr;
    uint16_t frame_len;
    uint8_t *data;
    uint32_t recv_seq;   // 接收包序  
    uint32_t send_seq;   // 发送包序  
} TS_NEURON_LINK_F3F4;

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
// 静态链路对象数组，支持多个端口同时工作
static TS_NEURON_LINK_F3F4 s_f3f4_link[NEURON_F3F4_MAX_NUM];
static f3f4_send_callback s_send_cb = NULL; 
static uint8_t s_f3f4_link_num = 0;
/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/**
 * @brief 根据端口ID获取对应的链路对象
 * @param port_id 端口ID
 * @return TS_NEURON_LINK_F3F4* 返回对应的链路对象指针，未找到则返回NULL
 * 
 * 功能说明：
 * 遍历静态链路数组，查找与指定端口ID匹配的链路对象
 */
static TS_NEURON_LINK_F3F4 *neuron_link_f3f4_port_to_link(uint8_t port_id)
{
    int _idx = 0;
    do {
        if (s_f3f4_link[_idx].port_id == port_id) {
            return &s_f3f4_link[_idx]; 
        }
    } while ((++_idx) < s_f3f4_link_num);
    return NULL;  
}

/**
 * @brief 计算数据帧的异或校验
 * @param buf 数据缓冲区指针
 * @param size 数据长度
 * @return uint8_t 返回计算得到的校验和
 */
static uint8_t neuron_link_f3f4_xor(uint8_t* buf, uint16_t size)
{
    uint8_t xor_result = 0;
    for (uint16_t i = 0; i < size; i++)
    {
        xor_result ^= buf[i];   
    }  
    return xor_result;
}

/**
 * @brief 注册F3F4协议链路对象
 * @param port_id 端口ID，用于标识不同的通信端口
 * @param rx_buffer 接收数据缓冲区指针
 * @param rx_buffer_size 接收缓冲区大小
 * @param send_cb 发送回调函数
 * @return int 初始化成功返回MK_SUCCESS，失败返回MK_ERROR_MAX
 */
int neuron_link_f3f4_register(uint8_t port_id, uint8_t *rx_buffer, uint32_t rx_buffer_size, f3f4_send_callback send_cb)
{ 
    if (s_f3f4_link_num >= NEURON_F3F4_MAX_NUM) {
        return MK_ERROR_OUT_OF_SCOPE;
    }
 
    s_f3f4_link[s_f3f4_link_num].port_id   = port_id;                          
    s_f3f4_link[s_f3f4_link_num].state     = NEURON_LINK_F3F4_STATE_IDLE;      
    s_f3f4_link[s_f3f4_link_num].recv_len  = 0;
    s_f3f4_link[s_f3f4_link_num].data      = rx_buffer;                        
    s_f3f4_link[s_f3f4_link_num].data_size = rx_buffer_size; 

    s_send_cb = send_cb;
    s_f3f4_link_num++;
    return MK_SUCCESS;  
}

/**
 * @brief 解析接收到的数据，提取有效的数据帧
 * @param port_id 端口ID，标识数据来源端口
 * @param data 接收到的原始数据缓冲区
 * @param len 接收数据的长度
 * @return int 处理成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_f3f4_unpack(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint8_t current_byte;

    TS_NEURON_LINK_F3F4 *link = neuron_link_f3f4_port_to_link(port_id);
    if (link == NULL) {
        return MK_ERROR_NULL_POINTER;   
    }

    for (uint16_t i = 0; i < len; i++)
    {
        current_byte = data[i];
        switch (link->state)
        {
            case NEURON_LINK_F3F4_STATE_IDLE:
                if (NEURON_LINK_F3F4_FRAME_START == current_byte)
                {
                    link->state = NEURON_LINK_F3F4_STATE_FRAME_HEADER;  
                    link->data[0] = current_byte;                         
                    link->recv_len = 1;                                   
                }
                break;

            case NEURON_LINK_F3F4_STATE_FRAME_HEADER:
                link->data[link->recv_len] = current_byte;   
                link->recv_len++;    

                // 获取到完整的头部数据
                if (link->recv_len == NEURON_LINK_F3F4_FRAME_HEAD_SIZE)
                {
                    if (((link->data[0] + link->data[2] + link->data[3]) & 0xFF) == link->data[NEURON_LINK_F3F4_HEADER_CHECK_OFFSET])
                    {
                        link->frame_len = NEURON_LINK_F3F4_BYTES_TO_UINT16(link->data[2], link->data[3]);
                        if (link->frame_len > link->data_size) {
                            link->state = NEURON_LINK_F3F4_STATE_IDLE; 
                        } else {
                            link->state = NEURON_LINK_F3F4_STATE_RECEIVING;
                        }
                    }
                    else
                    { 
                        link->state = NEURON_LINK_F3F4_STATE_IDLE;
                    }
                }
                break;

            case NEURON_LINK_F3F4_STATE_RECEIVING: 
                link->data[link->recv_len] = current_byte;   
                link->recv_len++;                       
                
                if (link->recv_len == (link->frame_len + 6))
                { 
                    if (NEURON_LINK_F3F4_FRAME_END == current_byte)
                    {
                        if (neuron_link_f3f4_xor(link->data+NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET, link->frame_len) == link->data[link->recv_len-2]) // 校验正确
                        { 
                            link->protocol_id = link->data[NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET];
                            link->src_addr = link->data[NEURON_LINK_F3F4_SRC_ADDR_OFFSET];
                            link->dst_addr = link->data[NEURON_LINK_F3F4_DST_ADDR_OFFSET];
                            link->recv_seq = NEURON_LINK_F3F4_BYTES_TO_UINT32(link->data[7], link->data[8], link->data[9], link->data[10]);
            
                            // 路由分发处理
                            neuron_router_f3f4_route(port_id, &link->data[NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET], link->frame_len - 1);
                        }
                    }
                    link->state = NEURON_LINK_F3F4_STATE_IDLE;
                }
                // 检查接收到的数据长度是否超过定义的最大长度
                else if (link->recv_len >= link->data_size)
                {  
                    link->state = NEURON_LINK_F3F4_STATE_IDLE;  // 重置状态为空闲
                }
                break;

            default:
                link->state = NEURON_LINK_F3F4_STATE_IDLE;
                break;
        }
    }

    return MK_SUCCESS;  
}   

/**
 * @brief 将上层数据封装成F3F4协议格式的数据帧并回复
 * @param port_id 端口ID，标识发送端口
 * @param cmd 命令
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度
 * @return int 封装发送成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_f3f4_pack_reply(uint8_t port_id, uint8_t cmd, uint8_t *data, uint16_t len)
{
    uint8_t frame_buf[NEURON_F3F4_CMD_REPLY_LEN + 16];
    uint16_t message_len = len + NEURON_LINK_F3F4_PACK_CMD_LEN_SIZE;
    TS_NEURON_LINK_F3F4 *link = neuron_link_f3f4_port_to_link(port_id);
    if (link == NULL) {
        return MK_ERROR_NULL_POINTER;   
    }

    frame_buf[NEURON_LINK_F3F4_HEAD_OFFSET]              = NEURON_LINK_F3F4_FRAME_START;
    frame_buf[NEURON_LINK_F3F4_HEADER_CHECK_OFFSET]      = NEURON_LINK_F3F4_FRAME_START + (uint8_t)message_len + (uint8_t)(message_len >> 8);
    frame_buf[NEURON_LINK_F3F4_LENGTH_OFFSET]            = (uint8_t)message_len;
    frame_buf[NEURON_LINK_F3F4_LENGTH_OFFSET+1]          = (uint8_t)(message_len >> 8);
    
    frame_buf[NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET]       = link->protocol_id;
    frame_buf[NEURON_LINK_F3F4_SRC_ADDR_OFFSET]          = link->dst_addr;
    frame_buf[NEURON_LINK_F3F4_DST_ADDR_OFFSET]          = link->src_addr;
    frame_buf[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET]   = (uint8_t)(link->recv_seq);
    frame_buf[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET+1] = (uint8_t)(link->recv_seq >> 8);
    frame_buf[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET+2] = (uint8_t)(link->recv_seq >> 16);
    frame_buf[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET+3] = (uint8_t)(link->recv_seq >> 24);
    frame_buf[NEURON_LINK_F3F4_CMD_OFFSET]               = cmd;
    memcpy(&frame_buf[NEURON_LINK_F3F4_DATA_OFFSET], data, len);

    uint16_t frame_offset = len + NEURON_LINK_F3F4_DATA_OFFSET;
    frame_buf[frame_offset] = neuron_link_f3f4_xor(frame_buf+NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET, len + NEURON_LINK_F3F4_PACK_CMD_LEN_SIZE);
    frame_buf[frame_offset + 1] = NEURON_LINK_F3F4_FRAME_END; 

    s_send_cb(port_id, frame_buf, len + 14);
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
int neuron_link_f3f4_pack_raw(uint8_t port_id, uint8_t protocol_id, uint8_t dst_addr, uint8_t cmd, uint8_t *data, uint16_t len)
{
    uint8_t frame_buf[NEURON_F3F4_CMD_REPLY_LEN + 16];
    uint16_t message_len = len + NEURON_LINK_F3F4_PACK_CMD_LEN_SIZE;
    TS_NEURON_LINK_F3F4 *link = neuron_link_f3f4_port_to_link(port_id);
    if (link == NULL) {
        return MK_ERROR_NULL_POINTER;   
    }

    link->send_seq++;

    frame_buf[NEURON_LINK_F3F4_HEAD_OFFSET]              = NEURON_LINK_F3F4_FRAME_START;
    frame_buf[NEURON_LINK_F3F4_HEADER_CHECK_OFFSET]      = NEURON_LINK_F3F4_FRAME_START + (uint8_t)message_len + (uint8_t)(message_len >> 8);
    frame_buf[NEURON_LINK_F3F4_LENGTH_OFFSET]            = (uint8_t)message_len;
    frame_buf[NEURON_LINK_F3F4_LENGTH_OFFSET+1]          = (uint8_t)(message_len >> 8);
    frame_buf[NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET]       = protocol_id;
    frame_buf[NEURON_LINK_F3F4_SRC_ADDR_OFFSET]          = NEURON_DEVICE_ADDRESS;
    frame_buf[NEURON_LINK_F3F4_DST_ADDR_OFFSET]          = dst_addr;
    frame_buf[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET]   = (uint8_t)(link->send_seq);
    frame_buf[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET+1] = (uint8_t)(link->send_seq >> 8);
    frame_buf[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET+2] = (uint8_t)(link->send_seq >> 16);
    frame_buf[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET+3] = (uint8_t)(link->send_seq >> 24);
    frame_buf[NEURON_LINK_F3F4_CMD_OFFSET]               = cmd;
    memcpy(&frame_buf[NEURON_LINK_F3F4_DATA_OFFSET], data, len);

    uint16_t frame_offset = len + NEURON_LINK_F3F4_DATA_OFFSET;
    frame_buf[frame_offset] = neuron_link_f3f4_xor(frame_buf+NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET, len + NEURON_LINK_F3F4_PACK_CMD_LEN_SIZE);
    frame_buf[frame_offset + 1] = NEURON_LINK_F3F4_FRAME_END; 

    s_send_cb(port_id, frame_buf, len + 14);
    return MK_SUCCESS;
}  


/**
 * @brief 将上层数据封装成F3F4协议格式的数据帧
 * @param protocol_id 协议ID
 * @param src_addr 源地址
 * @param dst_addr 目的地址
 * @param cmd 命令
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度
 * 
 * @return int 封装发送成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_f3f4_pack_data(uint8_t protocol_id, uint8_t dst_addr, uint8_t cmd, uint8_t *data, uint16_t len, uint8_t *pack_data)
{
    static uint32_t send_seq = 0;
    uint16_t message_len = len + NEURON_LINK_F3F4_PACK_CMD_LEN_SIZE;
    
    send_seq++;

    pack_data[NEURON_LINK_F3F4_HEAD_OFFSET]              = NEURON_LINK_F3F4_FRAME_START;
    pack_data[NEURON_LINK_F3F4_HEADER_CHECK_OFFSET]      = NEURON_LINK_F3F4_FRAME_START + (uint8_t)message_len + (uint8_t)(message_len >> 8);
    pack_data[NEURON_LINK_F3F4_LENGTH_OFFSET]            = (uint8_t)message_len;
    pack_data[NEURON_LINK_F3F4_LENGTH_OFFSET+1]          = (uint8_t)(message_len >> 8);
    pack_data[NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET]       = protocol_id;
    pack_data[NEURON_LINK_F3F4_SRC_ADDR_OFFSET]          = NEURON_DEVICE_ADDRESS;
    pack_data[NEURON_LINK_F3F4_DST_ADDR_OFFSET]          = dst_addr;
    pack_data[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET]   = (uint8_t)(send_seq);
    pack_data[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET+1] = (uint8_t)(send_seq >> 8);
    pack_data[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET+2] = (uint8_t)(send_seq >> 16);
    pack_data[NEURON_LINK_F3F4_SEQUENCE_NUMBER_OFFSET+3] = (uint8_t)(send_seq >> 24);
    pack_data[NEURON_LINK_F3F4_CMD_OFFSET]               = cmd;
    memcpy(&pack_data[NEURON_LINK_F3F4_DATA_OFFSET], data, len);

    uint16_t frame_offset = len + NEURON_LINK_F3F4_DATA_OFFSET;
    pack_data[frame_offset++] = neuron_link_f3f4_xor(pack_data+NEURON_LINK_F3F4_PROTOCOL_ID_OFFSET, len + NEURON_LINK_F3F4_PACK_CMD_LEN_SIZE);
    pack_data[frame_offset++] = NEURON_LINK_F3F4_FRAME_END; 

    return frame_offset;
}  

#endif
/******************************* End of File (C）****************************/
