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
#include "neuron_link_f3f4.h"
#include "neuron_stream_f3f4.h" 
#include "byte_buffer.h"
#include "ring_buf.h"
#include "mk_common.h"

#if NEURON_F3F4_MAX_NUM
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
// 流控相关命令
#define NEURON_STREAM_CMD_START_TRANSFER         0x01 // 开始流数据传输
#define NEURON_STREAM_CMD_END_TRANSFER           0x02 // 结束流数据传输
#define NEURON_STREAM_CMD_RECEIVE_DATA           0x03 // 接收数据
#define NEURON_STREAM_CMD_GET_STATUS             0x04 // 获取流链路状态 

#define NEURON_STREAM_F3F4_WAIT_REPORTING_TRUE (1) // 等待上报状态
#define NEURON_STREAM_F3F4_WAIT_REPORTING_FALSE (0) // 不等待上报状态

#define NEURON_STREAM_F3F4_BYTES_TO_UINT32(b0, b1, b2, b3) ((uint32_t)(b0) | ((uint32_t)(b1) << 8) | ((uint32_t)(b2) << 16) | ((uint32_t)(b3) << 24))
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef struct
{
    uint8_t port_id;
    uint8_t data_type; // 数据类型
    uint16_t package_index; // 包序号
    uint32_t total_len; // 总长度
    uint32_t checksum; // 校验和
    TS_RING_BUF_DEF rx_buffer;
    uint8_t wait_reporting; // 等待上报状态
    neuron_stream_f3f4_cb_t data_cb;
} TS_NEURON_STREAM;

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static TS_NEURON_STREAM s_f3f4_stream = {0}; 

/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/**
 * @brief 开始流数据传输
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_start_transfer(uint8_t port_id, uint8_t *data, uint16_t len)
{
    s_f3f4_stream.data_type = data[0];
    s_f3f4_stream.total_len = NEURON_STREAM_F3F4_BYTES_TO_UINT32(data[1], data[2], data[3], data[4]);
    s_f3f4_stream.checksum = NEURON_STREAM_F3F4_BYTES_TO_UINT32(data[5], data[6], data[7], data[8]); 
    s_f3f4_stream.package_index = 0;
    ring_buf_flush(&s_f3f4_stream.rx_buffer);

    int ret = MK_SUCCESS;
    uint8_t buff[2] = {0x00};

    if (s_f3f4_stream.data_cb != NULL) {
        ret = s_f3f4_stream.data_cb(NEURON_STREAM_F3F4_STATUS_START);
    }

    buff[0] = s_f3f4_stream.data_type;
    if (ret == MK_SUCCESS) {
        buff[1] = 0x00; 
    } else { 
        buff[1] = 0x01; 
    }
    neuron_link_f3f4_pack_reply(port_id, NEURON_STREAM_CMD_START_TRANSFER, buff, 2);
}

/**
 * @brief 结束流数据传输
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_end_transfer(uint8_t port_id, uint8_t *data, uint16_t len)
{
    int ret = MK_SUCCESS;
    uint8_t buff[2] = {0x00};

    if (s_f3f4_stream.data_cb != NULL) {
        ret = s_f3f4_stream.data_cb(NEURON_STREAM_F3F4_STATUS_END);
    }

    buff[0] = s_f3f4_stream.data_type;
    if (ret == MK_SUCCESS) {
        buff[1] = 0x00;
    } else { 
        buff[1] = 0x01;
    } 
    neuron_link_f3f4_pack_reply(port_id, NEURON_STREAM_CMD_START_TRANSFER, buff, 2);
}

/**
 * @brief 获取流链路通道状态
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_get_channel_status(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint8_t buff[7] = {0x00};

    uint32_t unused_count = ring_buf_unused_count(&s_f3f4_stream.rx_buffer);

    buff[0] = s_f3f4_stream.data_type;
    buff[1] = (uint8_t)(s_f3f4_stream.package_index);
    buff[2] = (uint8_t)(s_f3f4_stream.package_index >> 8);
    buff[3] = (uint8_t)(unused_count);
    buff[4] = (uint8_t)(unused_count >> 8);
    buff[5] = (uint8_t)(unused_count >> 16);
    buff[6] = (uint8_t)(unused_count >> 24);
    neuron_link_f3f4_pack_reply(port_id, NEURON_STREAM_CMD_GET_STATUS, buff, 7);
}

/**
 * @brief 接收数据
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_receive_data(uint8_t port_id, uint8_t *data, uint16_t len)
{
    if (s_f3f4_stream.data_type == data[0]) { // 数据类型匹配
        s_f3f4_stream.package_index = data[1] + (data[2] << 8);
        ring_buf_write(&s_f3f4_stream.rx_buffer, data+3, len-3);

        if (ring_buf_unused_count(&s_f3f4_stream.rx_buffer) < NEURON_F3F4_FLOW_LEFT_COUNT_THRES) {
            s_f3f4_stream.wait_reporting = NEURON_STREAM_F3F4_WAIT_REPORTING_TRUE;
        }

        neuron_stream_get_channel_status(port_id, NULL, 0);
    }
}

/**
 * @brief 流控命令服务订阅者
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_stream_subscriber(uint8_t port_id, uint8_t *data, uint16_t len)
{      
    uint8_t *payload = data + 1;
    uint8_t payload_len = len - 1;  

    switch (data[0]) {
        case NEURON_STREAM_CMD_START_TRANSFER:    
            neuron_stream_start_transfer(port_id, payload, payload_len);       
            break;

        case NEURON_STREAM_CMD_END_TRANSFER:   
            neuron_stream_end_transfer(port_id, payload, payload_len);
            break;

        case NEURON_STREAM_CMD_RECEIVE_DATA: 
            neuron_stream_receive_data(port_id, payload, payload_len);
            break;
        
        case NEURON_STREAM_CMD_GET_STATUS:     
            neuron_stream_get_channel_status(port_id, payload, payload_len);
            break;

        default:
            break;
    }
}

/**
 * @brief 流链路数据服务订阅者
 * @param port_id 端口ID
 * @param subtype 子类型
 * @param data 数据缓冲区
 * @param len 数据长度
 */
void neuron_stream_f3f4_subscriber(uint8_t port_id, uint8_t *data, uint16_t len)
{
    neuron_stream_subscriber(port_id, data, len);
}

/**
 * @brief 注册流链路
 * @param port_id 端口ID
 * @param rx_buffer 接收缓冲区
 * @param rx_buffer_size 接收缓冲区大小
 * @param data_cb 数据回调函数
 * @return int 注册结果
 */
int neuron_stream_f3f4_register(uint8_t port_id, uint8_t *rx_buffer, uint32_t rx_buffer_size, neuron_stream_f3f4_cb_t data_cb)
{
    memset(&s_f3f4_stream, 0, sizeof(TS_NEURON_STREAM));
    ring_buf_init(&s_f3f4_stream.rx_buffer, rx_buffer, rx_buffer_size); 

    s_f3f4_stream.port_id = port_id;
    s_f3f4_stream.data_cb = data_cb;
    return MK_SUCCESS;
}

/**
 * @brief 获取流链路环形缓冲区句柄，如果自己处理流数据时可以调用这个函数获取环形缓冲区句柄
 * @return TS_RING_BUF_DEF* 环形缓冲区结构体指针
 */
TS_RING_BUF_DEF *neuron_stream_f3f4_get_ringbuf_handle(void)
{
    return &s_f3f4_stream.rx_buffer;
}

/**
 * @brief 获取流链路环形缓冲区剩余空间
 * @return uint32_t 剩余空间大小
 */
uint32_t neuron_stream_f3f4_get_available_space(void)
{
    return ring_buf_unused_count(&s_f3f4_stream.rx_buffer);
}

/**
 * @brief 获取包序号
 * @return char 包序号
 */
char neuron_stream_f3f4_get_package_index(void)
{
    return s_f3f4_stream.package_index;
}

/**
 * @brief 判断流链路环形缓冲区是否为空
 * @return bool 是否为空
 */
bool neuron_stream_f3f4_is_empty(void)
{
    return (ring_buf_count(&s_f3f4_stream.rx_buffer) == 0);
}

/**
 * @brief 清空流链路环形缓冲区
 */
void neuron_stream_f3f4_clear_ringbuf(void)
{
    ring_buf_flush(&s_f3f4_stream.rx_buffer);
}

/**
 * @brief 等待上报数据, 当流链路环形缓冲区剩余空间小于NEURON_F3F4_FLOW_LEFT_COUNT_THRES时，会自动上报流链路状态
 */
void neuron_stream_f3f4_process_data(void)
{
    if (s_f3f4_stream.wait_reporting == NEURON_STREAM_F3F4_WAIT_REPORTING_TRUE) {
        if (ring_buf_count(&s_f3f4_stream.rx_buffer) > NEURON_F3F4_FLOW_LEFT_COUNT_THRES) {
            neuron_stream_get_channel_status(s_f3f4_stream.port_id, NULL, 0); 
            s_f3f4_stream.wait_reporting = NEURON_STREAM_F3F4_WAIT_REPORTING_FALSE;
        }
    }
}

#endif
/******************************* End of File (C）****************************/
