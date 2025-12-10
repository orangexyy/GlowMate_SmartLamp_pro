/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_stream.c
**  功能描述 :  https://makeblock.feishu.cn/wiki/VndhwvAsfipt2mkGGsGcIxGlnlh
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
#include "neuron_config_def.h"
#include "neuron_link_f0f7.h"
#include "neuron_stream_f0f7.h"
#include "neuron_user_f0f7.h"
#include "byte_buffer.h"
#include "ring_buf.h"
#include "mk_common.h"

#if NEURON_F0F7_STREAM_ENABLE
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
// 流控相关命令
#define STREAM_CMD_GET_LINK_CHANNEL_NUM               0x50 // 获取链路通道数量 
#define STREAM_CMD_GET_COMMUNICATION_STATISTICS       0x56 // 获取链路通道通信状态统计
#define STREAM_CMD_CLEAR_COMMUNICATION_STATISTICS     0x57 // 清除链路通道通信状态统计
#define STREAM_CMD_START_STREAM_LINK_TRANSFER         0x60 // 开始流数据传输
#define STREAM_CMD_END_STREAM_LINK_TRANSFER           0x61 // 结束流数据传输
#define STREAM_CMD_GET_STREAM_LINK_CHANNEL_STATUS     0x6F // 获取流链路通道状态 

#define NEURON_STREAM_SERVICE_TYPE     0x61 // 流服务类型
#define NEURON_STREAM_COMMAND_SUB_TYPE 0x60 // 流命令子类型
#define NEURON_STREAM_DATA_SUB_TYPE    0x61 // 流数据子类型

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef struct
{
    uint8_t port_id;
    uint8_t package_index; // 包序号
    uint32_t total_len;
    uint32_t checksum;
    TS_RING_BUF_DEF rx_buffer;
    uint8_t cmd_buf[NEURON_F0F7_STREAM_ONCE_PROCESS_LEN];
    uint16_t cmd_index;
    uint32_t line_count; // 解析的gcode行数, 每解析一行gcode, 加1
    neuron_stream_f0f7_cb_t data_cb;
} TS_NEURON_STREAM;

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static TS_NEURON_STREAM s_f0f7_stream = {0}; 

/****************************************************************************\
                            Functions definitions
\****************************************************************************/
void neuron_stream_f0f7_reply_0x6F(uint8_t port_id, uint8_t sub_type, uint8_t status)
{
    BYTE_BUFFER byte_buffer; 
    uint8_t     data_buf[32];

    byte_buffer_init(&byte_buffer, data_buf, sizeof(data_buf), 0);
    
    byte_buffer_write_byte_8(&byte_buffer, STREAM_CMD_GET_STREAM_LINK_CHANNEL_STATUS);
    byte_buffer_write_long(&byte_buffer, neuron_stream_f0f7_get_available_space());
    byte_buffer_write_byte_8(&byte_buffer, status);
    byte_buffer_write_long(&byte_buffer, s_f0f7_stream.package_index);
    byte_buffer_write_byte_8(&byte_buffer, NEURON_LINK_F0F7_STREAM_TYPE);  
    neuron_link_f0f7_pack_raw(port_id, F0F7_STREAM_SERVICE_TYPE, sub_type, data_buf, byte_buffer.index);
}

/**
 * @brief 获取链路通道数量，非必须实现，暂不处理
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_get_link_channel_num(uint8_t port_id, uint8_t *data, uint16_t len)
{
    // 暂不实现
}

/**
 * @brief 获取流链路通道状态，非必须实现，暂不处理
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_get_statistics(uint8_t port_id, uint8_t *data, uint16_t len)
{
    BYTE_BUFFER     byte_buffer;
    uint8_t         data_buf[32];

    // 回复通信统计数据
    byte_buffer_init(&byte_buffer, data_buf, 32, 0);
    byte_buffer_write_byte_8(&byte_buffer, STREAM_CMD_GET_COMMUNICATION_STATISTICS);
    byte_buffer_write_byte_8(&byte_buffer, 0);
    byte_buffer_write_long(&byte_buffer, 0);
    byte_buffer_write_long(&byte_buffer, 0);

    neuron_link_f0f7_pack_raw(port_id, NEURON_STREAM_SERVICE_TYPE, NEURON_STREAM_COMMAND_SUB_TYPE, data_buf, byte_buffer.index);
}

/**
 * @brief 清除链路通道通信状态统计，非必须实现，暂不处理
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_clear_statistics(uint8_t port_id, uint8_t *data, uint16_t len)
{
    // 暂不实现
}

/**
 * @brief 开始流数据传输
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_start_transfer(uint8_t port_id, uint8_t *data, uint16_t len)
{
    BYTE_BUFFER byte_buffer; 
    uint8_t data_buf[32];
    long total_len = 0;
    long checksum = 0;

    // 解析传输参数
    byte_buffer_init(&byte_buffer, data, len, 1); // 去掉channel_id
    byte_buffer_read_long(&byte_buffer, &total_len);
    byte_buffer_read_long(&byte_buffer, &checksum); 
    
    s_f0f7_stream.total_len = total_len;
    s_f0f7_stream.checksum = checksum;
    s_f0f7_stream.line_count = 0;
    s_f0f7_stream.cmd_index = 0;
    s_f0f7_stream.package_index = 0;
    ring_buf_flush(&s_f0f7_stream.rx_buffer);

    if (s_f0f7_stream.data_cb != NULL) {
        s_f0f7_stream.data_cb(NEURON_STREAM_F0F7_STATUS_START);
    }

    // 回复确认
    byte_buffer_init(&byte_buffer, data_buf, sizeof(data_buf), 0);
    byte_buffer_write_byte_8(&byte_buffer, STREAM_CMD_START_STREAM_LINK_TRANSFER);
    neuron_link_f0f7_pack_raw(port_id, NEURON_STREAM_SERVICE_TYPE, NEURON_STREAM_COMMAND_SUB_TYPE, data_buf, byte_buffer.index);
}

/**
 * @brief 结束流数据传输
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_end_transfer(uint8_t port_id, uint8_t *data, uint16_t len)
{
    // 暂不实现
    if (s_f0f7_stream.data_cb != NULL) {
        s_f0f7_stream.data_cb(NEURON_STREAM_F0F7_STATUS_END);
    }
}

/**
 * @brief 获取流链路通道状态
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_get_channel_status(uint8_t port_id, uint8_t *data, uint16_t len)
{
    neuron_stream_f0f7_reply_0x6F(port_id, NEURON_STREAM_COMMAND_SUB_TYPE, 0x01);
}

/**
 * @brief 流控命令服务订阅者
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_cmd_subscriber(uint8_t port_id, uint8_t *data, uint16_t len)
{      
    uint8_t *payload = data + 1;
    uint8_t payload_len = len - 1;  

    switch (data[0]) {
        case STREAM_CMD_GET_LINK_CHANNEL_NUM:          
            neuron_stream_get_link_channel_num(port_id, payload, payload_len);
            break;

        case STREAM_CMD_GET_COMMUNICATION_STATISTICS:  
            neuron_stream_get_statistics(port_id, payload, payload_len);
            break;

        case STREAM_CMD_CLEAR_COMMUNICATION_STATISTICS:
            neuron_stream_clear_statistics(port_id, payload, payload_len);
            break;
        
        case STREAM_CMD_START_STREAM_LINK_TRANSFER:    
            neuron_stream_start_transfer(port_id, payload, payload_len);
            break;

        case STREAM_CMD_END_STREAM_LINK_TRANSFER:      
            neuron_stream_end_transfer(port_id, payload, payload_len);
            break;

        case STREAM_CMD_GET_STREAM_LINK_CHANNEL_STATUS:
            neuron_stream_get_channel_status(port_id, payload, payload_len);
            break;

        default:
            break;
    }
}

/**
 * @brief 流控数据服务订阅者
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_data_subscriber(uint8_t port_id, uint8_t *data, uint16_t len)
{
    s_f0f7_stream.package_index = data[0];
    ring_buf_write(&s_f0f7_stream.rx_buffer, data+1, len-1);
    // neuron_stream_f0f7_reply_0x6F(port_id, NEURON_STREAM_DATA_SUB_TYPE, 0x01); // 在neuron_link_f0f7.c中回复,否则会回复两包
}

/**
 * @brief 流链路数据服务订阅者
 * @param port_id 端口ID
 * @param subtype 子类型
 * @param data 数据缓冲区
 * @param len 数据长度
 */
void neuron_stream_f0f7_subscriber(uint8_t port_id, uint8_t subtype, uint8_t *data, uint16_t len)
{
    if (subtype == NEURON_STREAM_COMMAND_SUB_TYPE) {
        neuron_stream_cmd_subscriber(port_id, data, len);
    } else if (subtype == NEURON_STREAM_DATA_SUB_TYPE) {
        neuron_stream_data_subscriber(port_id, data, len);
    } 
}

/**
 * @brief 注册流链路
 * @param port_id 端口ID
 * @param rx_buffer 接收缓冲区
 * @param rx_buffer_size 接收缓冲区大小
 * @param data_cb 数据回调函数
 * @return int 注册结果
 */
int neuron_stream_f0f7_register(uint8_t port_id, uint8_t *rx_buffer, uint32_t rx_buffer_size, neuron_stream_f0f7_cb_t data_cb)
{
    memset(&s_f0f7_stream, 0, sizeof(TS_NEURON_STREAM));
    ring_buf_init(&s_f0f7_stream.rx_buffer, rx_buffer, rx_buffer_size); 

    s_f0f7_stream.port_id = port_id;
    s_f0f7_stream.data_cb = data_cb;
    return MK_SUCCESS;
}

/**
 * @brief 获取流链路环形缓冲区句柄，如果自己处理流数据时可以调用这个函数获取环形缓冲区句柄
 * @return TS_RING_BUF_DEF* 环形缓冲区结构体指针
 */
TS_RING_BUF_DEF *neuron_stream_f0f7_get_ringbuf_handle(void)
{
    return &s_f0f7_stream.rx_buffer;
}

/**
 * @brief 获取流链路环形缓冲区剩余空间
 * @return uint32_t 剩余空间大小
 */
uint32_t neuron_stream_f0f7_get_available_space(void)
{
    return ring_buf_unused_count(&s_f0f7_stream.rx_buffer);
}

/**
 * @brief 获取包序号
 * @return char 包序号
 */
char neuron_stream_f0f7_get_package_index(void)
{
    return s_f0f7_stream.package_index;
}

/**
 * @brief 判断流链路环形缓冲区是否为空
 * @return bool 是否为空
 */
bool neuron_stream_f0f7_is_empty(void)
{
    return (ring_buf_count(&s_f0f7_stream.rx_buffer) == 0);
}

/**
 * @brief 清空流链路环形缓冲区
 */
void neuron_stream_f0f7_clear_ringbuf(void)
{
    ring_buf_flush(&s_f0f7_stream.rx_buffer);
}

/**
 * @brief 获取解析的gcode行数
 * @return uint32_t 行数
 */
uint32_t neuron_stream_f0f7_get_line_count(void)
{
    return s_f0f7_stream.line_count;
}

/**
 * @brief 设置解析的gcode行数
 * @param line_count 行数
 */
void neuron_stream_f0f7_set_line_count(uint32_t line_count)
{
    s_f0f7_stream.line_count = line_count;
}

/**
 * @brief 处理流链路环形缓冲区数据
 * @param port_id 端口ID
 */
void neuron_stream_f0f7_process_gcode(uint8_t port_id)
{
    extern void neuron_user_dispatch_gcode(uint8_t port_id, char *cmd_data, uint16_t cmd_len);

    uint8_t byte_data;
    while (ring_buf_read(&s_f0f7_stream.rx_buffer, 1, &byte_data) > 0) {
        s_f0f7_stream.cmd_buf[s_f0f7_stream.cmd_index] = byte_data;
        s_f0f7_stream.cmd_index++;

        if (byte_data == '\n') {
            // 命令结束，处理命令
            s_f0f7_stream.cmd_buf[s_f0f7_stream.cmd_index] = '\0';
            s_f0f7_stream.line_count++;
            neuron_user_dispatch_gcode(port_id, (char *)s_f0f7_stream.cmd_buf, s_f0f7_stream.cmd_index);
            s_f0f7_stream.cmd_index = 0;
            break;
        } else if (s_f0f7_stream.cmd_index >= NEURON_F0F7_STREAM_ONCE_PROCESS_LEN) {
            // 接收到的数据超过协议帧最大长度，丢弃数据帧
            s_f0f7_stream.cmd_index = 0;
            break;
        }
    }
}
#endif
/******************************* End of File (C）****************************/
