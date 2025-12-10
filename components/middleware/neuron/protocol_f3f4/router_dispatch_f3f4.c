/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_router_f3f4.c
**  功能描述 :  路由层负责根据消息的地址、类型和子类型决定消息的处理方式，
                包括本地处理或转发到其他设备。
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
#include "neuron_config_def.h"
#include "router_dispatch_f3f4.h"  

#if NEURON_F3F4_MAX_NUM
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define NEURON_F3F4_PROTOCOL_ID_COMMON 0x00 // 通用协议ID
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static neuron_router_f3f4_callback s_f3f4_callback = NULL;
/****************************************************************************\
                            Functions definitions
\****************************************************************************/
extern void neuron_stream_f3f4_subscriber(uint8_t port_id, uint8_t *data, uint16_t len);

/**
 * @brief F0F7协议分发处理函数
 * @param port_id 端口ID
 * @param data 数据缓冲区指针
 * @param len 数据长度
 */
static void neuron_dispatcher_f3f4_dispatch(uint8_t port_id, uint8_t *data, uint16_t len)
{
    // 根据协议ID进行分发处理
    if (data[0] < 0x10) // 流控相关
    {
        neuron_stream_f3f4_subscriber(port_id, data, len);
    }
    else if (data[0] < 0x20) // 升级相关
    {

    }
}

/**
 * @brief F0F7协议路由处理函数
 * @param port_id 端口ID，标识数据来源端口
 * @param data 数据缓冲区指针
 * @param len 数据长度
 */
void neuron_router_f3f4_route(uint8_t port_id, uint8_t *data, uint16_t len)
{
    if (data[2] == NEURON_DEVICE_ADDRESS)
    {
        if (data[0] == NEURON_F3F4_PROTOCOL_ID_COMMON)
        {
            neuron_dispatcher_f3f4_dispatch(port_id, data+7, len-7); // 减去源地址、目标地址、序列号
        }
        else
        {
            // 产品线协议相关
            if (s_f3f4_callback != NULL)
            {
                s_f3f4_callback(port_id, data+7, len-7);
            }
        }
    }
}

void neuron_router_f3f4_register_callback(neuron_router_f3f4_callback callback)
{
    s_f3f4_callback = callback;
}

#endif
/******************************* End of File (C）****************************/
