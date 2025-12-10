/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_link.c
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
#include "neuron_link_f0f7.h"
#include "router_dispatch_f0f7.h"
#include "byte_buffer.h"
#include "mk_common.h"
#include "neuron_stream_f0f7.h"

/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define NEURON_LINK_F0F7_FRAME_START            (0xF0)    // 帧起始符：F0
#define NEURON_LINK_F0F7_FRAME_END              (0xF7)    // 帧结束符：F7
        
#define NEURON_LINK_F0F7_CHECKSUM_MARK          (0x7F)    // 校验和掩码：7F（用于限制校验和在7位范围内）
#define NEURON_LINK_F0F7_FRAME_HEAD_SIZE        (4)       // 帧头部大小

#define NEURON_LINK_F0F7_HEAD_OFFSET            (0) // 头偏移
#define NEURON_LINK_F0F7_ADDRESS_OFFSET         (1) // 地址偏移
#define NEURON_LINK_F0F7_TYPE_OFFSET            (2) // 类型偏移
#define NEURON_LINK_F0F7_SUB_TYPE_OFFSET        (3) // 子类型偏移
#define NEURON_LINK_F0F7_DATA_OFFSET            (4) // 数据偏移
#define NEURON_LINK_F0F7_CMD_OFFSET             (4) // 命令偏移
#define NEURON_LINK_F0F7_SEQUENCE_NUMBER_OFFSET (5) // 包序偏移

#define CMD_RESPOND_STREAM_LINK_CHANNEL_STATUS 0x6F // 流链路通道状态回复命令

#define GENERAL_RESPONSE     0x15
#define PROCESS_SUC          0x0F

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
// 链路层状态机状态定义
typedef enum
{
    NEURON_LINK_F0F7_STATE_IDLE = 0x00,          // 空闲状态，等待帧起始符
    NEURON_LINK_F0F7_STATE_FRAME_DETECT,         // 帧格式检测
    NEURON_LINK_F0F7_STATE_RECEIVING,            // 正在接收数据状态
} TE_NEURON_LINK_F0F7_STATE;

// 链路层处理状态定义
#define NEURON_LINK_F0F7_STATUS_NULL 0x00 // 空状态
#define NEURON_LINK_F0F7_STATUS_SUCCESS 0x01 // 接收成功
#define NEURON_LINK_F0F7_STATUS_CRC_ERROR 0x02 // 校验和错误
#define NEURON_LINK_F0F7_STATUS_INVALID_CHARACTER 0x03 // 非法字符 
#define NEURON_LINK_F0F7_STATUS_TOO_LONG 0x04 // 数据帧过长  
#define NEURON_LINK_F0F7_STATUS_SAME_PACKAGE 0x05 // 重复数据包

typedef struct {
    uint8_t  port_id;
    uint8_t  state;    
    uint16_t recv_len;
    uint16_t data_size;
    uint8_t *data;
    uint8_t  sequence_number;   // 包序
    uint8_t  stream_status;     // 流链路状态
    TE_NEURON_LINK_F0F7_TYPE data_type;
    uint16_t error_package;     // 错误包数量
    uint32_t timeout_ms;  
} TS_NEURON_LINK_F0F7;

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
// 静态链路对象数组，支持多个端口同时工作
static TS_NEURON_LINK_F0F7 s_f0f7_link[NEURON_F0F7_MAX_NUM];
static f0f7_send_callback s_send_cb = NULL;
static f0f7_forward_callback s_forward_cb = NULL;
static uint8_t s_f0f7_link_num = 0;
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
static TS_NEURON_LINK_F0F7 *neuron_link_f0f7_port_to_link(uint8_t port_id)
{
    int _idx = 0;
    do {
        if (s_f0f7_link[_idx].port_id == port_id) {
            return &s_f0f7_link[_idx]; 
        }
    } while ((++_idx) < s_f0f7_link_num);
    return NULL;  
}

/**
 * @brief 计算数据帧的校验和
 * @param buf 数据缓冲区指针
 * @param size 数据长度
 * @return uint8_t 返回计算得到的校验和
 * 
 * 功能说明：
 * 对数据帧中除起始符和结束符以外的所有字节进行累加，
 * 然后与校验和掩码进行按位与操作，确保结果在7位范围内
 */
static uint8_t neuron_link_f0f7_check(uint8_t* buf, uint16_t size)
{
    uint8_t check_sum = 0;
    for (uint16_t i = 0; i < size; i++)
    {
        check_sum += buf[i];   
    }
    check_sum &= NEURON_LINK_F0F7_CHECKSUM_MARK;  
    return check_sum;
}

/**
 * @brief 发送F0F7协议回包
 * @param port_id 端口ID
 * @param status 状态码
 * @param pk_num 包数量
 */
static void neuron_link_f0f7_auto_reply(uint8_t port_id, TE_NEURON_LINK_F0F7_TYPE data_type, uint8_t seq, uint8_t status)
{ 

    if (NEURON_LINK_F0F7_COMMAND_TYPE == data_type)
    {
        #if (NEURON_F0F7_LINK_CMD_AUTO_REPLY)
        uint8_t frame_buf[32] = {0}; 
        frame_buf[0] = 0x02;
        frame_buf[1] = seq;
        frame_buf[2] = status;  
        neuron_link_f0f7_pack(port_id, frame_buf, 3);
        #endif
    }
    else 
    {
        #if NEURON_F0F7_STREAM_ENABLE
        extern void neuron_stream_f0f7_reply_0x6F(uint8_t port_id, uint8_t sub_type, uint8_t status);

        TS_NEURON_LINK_F0F7 *link = neuron_link_f0f7_port_to_link(port_id);
        if (link == NULL) {
            return;
        }

        link->stream_status = status; // 记录流链路状态
        neuron_stream_f0f7_reply_0x6F(port_id, F0F7_STREAM_SERVICE_SUB_TYPE, status); // 回复61 61流状态
        #endif
    }
}

/**
 * @brief 发送通用指令回复
 * @param port_id 端口ID
 * @param code 回复码
 */
static void neuron_link_f0f7_send_sysex_return_code(uint8_t port_id, uint8_t code)
{ 
    neuron_link_f0f7_pack_raw(port_id, GENERAL_RESPONSE, code, NULL, 0);    
}

/**
 * @brief 注册F0F7协议链路对象
 * @param port_id 端口ID，用于标识不同的通信端口
 * @param rx_buffer 接收数据缓冲区指针
 * @param rx_buffer_size 接收缓冲区大小
 * @param send_cb 发送回调函数
 * @param forward_cb 转发回调函数
 * @return int 初始化成功返回MK_SUCCESS，失败返回MK_ERROR_MAX
 * 
 * 功能说明：
 * 1. 查找空闲的链路对象或已存在的端口ID对应的链路对象
 * 2. 初始化链路对象的基本参数
 * 3. 设置接收缓冲区和初始状态
 */
int neuron_link_f0f7_register(uint8_t port_id, uint8_t *rx_buffer, uint32_t rx_buffer_size, f0f7_send_callback send_cb, f0f7_forward_callback forward_cb)
{ 
    if (s_f0f7_link_num >= NEURON_F0F7_MAX_NUM) {
        return MK_ERROR_OUT_OF_SCOPE;
    }
 
    s_f0f7_link[s_f0f7_link_num].port_id   = port_id;                          
    s_f0f7_link[s_f0f7_link_num].state     = NEURON_LINK_F0F7_STATE_IDLE;      
    s_f0f7_link[s_f0f7_link_num].recv_len  = 0;
    s_f0f7_link[s_f0f7_link_num].data      = rx_buffer;                        
    s_f0f7_link[s_f0f7_link_num].data_size = rx_buffer_size;
    s_f0f7_link[s_f0f7_link_num].error_package = 0;

    s_send_cb = send_cb;
    s_forward_cb = forward_cb;
    s_f0f7_link_num++;
    return MK_SUCCESS;  
}

/**
 * @brief 解析接收到的数据，提取有效的数据帧
 * @param port_id 端口ID，标识数据来源端口
 * @param data 接收到的原始数据缓冲区
 * @param len 接收数据的长度
 * @return int 处理成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 * 
 * 功能说明：
 * 使用状态机方式解析F0F7协议数据帧：
 * 1. 查找帧起始符F0
 * 2. 接收数据直到帧结束符F7
 * 3. 验证校验和的正确性
 * 4. 处理各种异常情况（超时、非法字符、数据过长等）
 */
int neuron_link_f0f7_unpack(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint8_t current_byte;

    TS_NEURON_LINK_F0F7 *link = neuron_link_f0f7_port_to_link(port_id);
    if (link == NULL) {
        return MK_ERROR_NULL_POINTER;   
    }

    for (uint16_t i = 0; i < len; i++)
    {
        current_byte = data[i];
        switch (link->state)
        {
            case NEURON_LINK_F0F7_STATE_IDLE:
                if (NEURON_LINK_F0F7_FRAME_START == current_byte)
                {
                    link->state = NEURON_LINK_F0F7_STATE_FRAME_DETECT;  
                    link->data[0] = current_byte;                         
                    link->recv_len = 1;                                   
                }
                break;

            case NEURON_LINK_F0F7_STATE_FRAME_DETECT:
                link->data[link->recv_len] = current_byte;   
                link->recv_len++;    

                // 获取到完整的头部数据
                if (link->recv_len == NEURON_LINK_F0F7_FRAME_HEAD_SIZE)
                {
                    // 接收到流数据包
                    if ((link->data[2] == F0F7_STREAM_SERVICE_TYPE) && (link->data[3] == F0F7_STREAM_SERVICE_SUB_TYPE))
                    {
                        link->data_type = NEURON_LINK_F0F7_STREAM_TYPE; 
                    }
                    else 
                    {
                        link->data_type = NEURON_LINK_F0F7_COMMAND_TYPE; 
                    }
                    link->state = NEURON_LINK_F0F7_STATE_RECEIVING;
                }
                break;

            case NEURON_LINK_F0F7_STATE_RECEIVING: 
                link->data[link->recv_len] = current_byte;   
                link->recv_len++;                       
                
                if (NEURON_LINK_F0F7_FRAME_END == current_byte)
                { 
                    if (neuron_link_f0f7_check(link->data+1, link->recv_len-3) == link->data[link->recv_len-2]) // 校验和正确
                    { 
                        link->sequence_number = link->data[NEURON_LINK_F0F7_SEQUENCE_NUMBER_OFFSET];

                        #ifdef BOOTLOADER   
                            //处于bootloader 需要先回复通用指令 
                            neuron_link_f0f7_send_sysex_return_code(port_id, PROCESS_SUC); 
                        #else       
                            //处于APP且兼容升级流程  
                            extern bool neuron_general_is_into_upgrade(void);
                            if(neuron_general_is_into_upgrade())
                            {     
                                neuron_link_f0f7_send_sysex_return_code(port_id, PROCESS_SUC); //假装处于bootloader，回复通用指令
                            }                            
                        #endif   

                        if (link->data_type == NEURON_LINK_F0F7_STREAM_TYPE)
                        {
                            link->stream_status = NEURON_LINK_F0F7_STATUS_SUCCESS;
                        }
                        
                        // 路由分发处理
                        if (NEURON_ROUTER_F0F7_FORWARD == neuron_router_f0f7_route(port_id, &link->data[NEURON_LINK_F0F7_ADDRESS_OFFSET], link->recv_len - 3)) {
                            if (s_forward_cb != NULL) {
                                s_forward_cb(port_id, 0x01, link->data, link->recv_len);
                            }
                        }
                        neuron_link_f0f7_auto_reply(port_id, link->data_type, link->sequence_number, NEURON_LINK_F0F7_STATUS_SUCCESS);
                        link->state = NEURON_LINK_F0F7_STATE_IDLE;  
                    }
                    else
                    {
                        // 校验和错误处理
                        link->error_package++;   
                        neuron_link_f0f7_auto_reply(port_id, link->data_type, link->sequence_number, NEURON_LINK_F0F7_STATUS_CRC_ERROR);
                        link->state = NEURON_LINK_F0F7_STATE_IDLE;
                    }
                }
                // 在接收数据过程中又检测到起始符F0，说明新帧开始，重新开始接收
                else if (current_byte == NEURON_LINK_F0F7_FRAME_START)
                { 
                    link->data[0] = current_byte;             
                    link->recv_len = 1;                  
                    link->error_package++;
                    link->state = NEURON_LINK_F0F7_STATE_FRAME_DETECT;
                }
                // 接收到非法数据（大于7F的字节）
                else if (current_byte > NEURON_LINK_F0F7_CHECKSUM_MARK)
                {
                    link->error_package++;  
                    neuron_link_f0f7_auto_reply(port_id, link->data_type, link->sequence_number, NEURON_LINK_F0F7_STATUS_INVALID_CHARACTER);
                    link->state = NEURON_LINK_F0F7_STATE_IDLE;  // 重置状态为空闲
                }
                // 检查接收到的数据长度是否超过定义的最大长度
                else if (link->recv_len >= link->data_size)
                {
                    link->error_package++;  
                    neuron_link_f0f7_auto_reply(port_id, link->data_type, link->sequence_number, NEURON_LINK_F0F7_STATUS_TOO_LONG);
                    link->state = NEURON_LINK_F0F7_STATE_IDLE;  // 重置状态为空闲
                }
                break;

            default:
                link->state = NEURON_LINK_F0F7_STATE_IDLE;
                break;
        }
    }

    return MK_SUCCESS;  
}   

/**
 * @brief 将上层数据封装成F0F7协议格式的数据帧并发送
 * @param port_id 端口ID，标识发送端口
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度
 * @return int 封装发送成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_f0f7_pack(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint8_t frame_buf[NEURON_F0F7_CMD_REPLY_LEN + 10] = {0};

    frame_buf[NEURON_LINK_F0F7_HEAD_OFFSET]     = NEURON_LINK_F0F7_FRAME_START;
    frame_buf[NEURON_LINK_F0F7_ADDRESS_OFFSET]  = NEURON_DEVICE_ADDRESS;
    frame_buf[NEURON_LINK_F0F7_TYPE_OFFSET]     = NEURON_DEVICE_TYPE;
    frame_buf[NEURON_LINK_F0F7_SUB_TYPE_OFFSET] = NEURON_DEVICE_SUB_TYPE;
    memcpy(&frame_buf[NEURON_LINK_F0F7_DATA_OFFSET], data, len);
    frame_buf[len + NEURON_LINK_F0F7_DATA_OFFSET] = neuron_link_f0f7_check(frame_buf+1, len+3);
    frame_buf[len + 5] = NEURON_LINK_F0F7_FRAME_END; 

    s_send_cb(port_id, frame_buf, len + 6);
    return MK_SUCCESS;
}  

/**
 * @brief 将上层数据封装成F0F7协议格式的数据帧并发送
 * @param port_id 端口ID，标识发送端口
 * @param address 地址
 * @param type 类型
 * @param sub_type 子类型
 * @param data 待发送的数据缓冲区
 * @param len 待发送数据的长度
 * @return int 封装发送成功返回MK_SUCCESS，失败返回MK_ERROR_NULL_POINTER
 */
int neuron_link_f0f7_pack_raw(uint8_t port_id, uint8_t type, uint8_t sub_type, uint8_t *data, uint16_t len)
{
    uint8_t frame_buf[NEURON_F0F7_CMD_REPLY_LEN + 10] = {0};

    frame_buf[NEURON_LINK_F0F7_HEAD_OFFSET]     = NEURON_LINK_F0F7_FRAME_START;
    frame_buf[NEURON_LINK_F0F7_ADDRESS_OFFSET]  = NEURON_DEVICE_ADDRESS;
    frame_buf[NEURON_LINK_F0F7_TYPE_OFFSET]     = type;
    frame_buf[NEURON_LINK_F0F7_SUB_TYPE_OFFSET] = sub_type;
    memcpy(&frame_buf[NEURON_LINK_F0F7_DATA_OFFSET], data, len);
    frame_buf[len + NEURON_LINK_F0F7_DATA_OFFSET] = neuron_link_f0f7_check(frame_buf+1, len+3);
    frame_buf[len + 5] = NEURON_LINK_F0F7_FRAME_END; 

    s_send_cb(port_id, frame_buf, len + 6); 
    return MK_SUCCESS;
}  

/**
 * @brief 将上层数据封装成F0F7协议格式的数据帧
 * @param data 待发送的数据缓冲区, 从type开始到数据部分
 * @param len 待发送数据的长度
 * @param pack_data 封装后的数据缓冲区
 * @return int 封装后的数据长度
 */
int neuron_link_f0f7_pack_data(uint8_t *data, uint16_t len, uint8_t *pack_data)
{
    uint16_t pack_len = 0;
    pack_data[pack_len++] = NEURON_LINK_F0F7_FRAME_START;
    pack_data[pack_len++] = NEURON_DEVICE_ADDRESS;
    memcpy(pack_data+pack_len, data, len);
    pack_len += len;
    pack_data[pack_len++] = neuron_link_f0f7_check(pack_data+1, len+3);
    pack_data[pack_len++] = NEURON_LINK_F0F7_FRAME_END; 
    return pack_len;
}  

/******************************* End of File (C）****************************/
