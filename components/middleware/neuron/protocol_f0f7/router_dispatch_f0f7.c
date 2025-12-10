/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_router.c
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
#include "router_dispatch_f0f7.h" 
#include "neuron_stream_f0f7.h"
#include "neuron_user_f0f7.h"

/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define NEURON_ROUTER_F0F7_BROADCAST_ADDRESS (0xFF) // 广播地址

/* 命令类型定义 */
#define SYSTEM_CMD_MAX                 0x20    // 系统命令最大值
#define ALL_DEVICE                     0xFF    // 广播命令   
#define CTL_ASSIGN_DEV_ID              0x10    // 分配地址命令
#define NO_SUBTYPE_BOUND               0x60    // 无子类型命令边界

// 定义流命令类型和子类型
#define NEURON_STREAM_TYPE             0x61 // 流命令类型
#define NEURON_STREAM_COMMAND_SUB_TYPE 0x60 // 流命令子类型
#define NEURON_STREAM_DATA_SUB_TYPE    0x61 // 流数据子类型
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
 
/****************************************************************************\
                            Functions definitions
\****************************************************************************/
extern void neuron_general_subscriber(uint8_t port_id, uint8_t type, uint8_t subtype, uint8_t *data, uint16_t len);
extern void neuron_user_subscriber(uint8_t port_id, uint8_t *data, uint16_t len);
extern void neuron_stream_f0f7_subscriber(uint8_t port_id, uint8_t subtype, uint8_t *data, uint16_t len);
extern void neuron_system_subscriber(uint8_t port_id, uint8_t type, uint8_t subtype, uint8_t *data, uint16_t len);

/**
 * @brief F0F7协议分发处理函数
 * @param port_id 端口ID
 * @param data 数据缓冲区指针
 * @param len 数据长度
 */
static void neuron_dispatcher_f0f7_dispatch(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint8_t type = data[0];
    uint8_t subtype = data[1];
    uint8_t *payload = data + 2; // 去掉类型和子类型后的有效数据
    uint16_t payload_len = len - 2; // 去掉类型和子类型后的有效数据长度

    // 根据类型字段进行分发处理
    if (type <= SYSTEM_CMD_MAX) // 0x00-0x0F、0x10-0x20 统一系统命令处理函数
    {
       neuron_system_subscriber(port_id, type, subtype, payload, payload_len);
    }
    else if (type <= NO_SUBTYPE_BOUND) // 0x21-0x60 没有子类型的命令类型
    {
        // neuron_no_subtype_subscriber(port_id, payload, payload_len); // 暂时没有子类型的命令
    }
    else if (type <= 0x7F) // 0x61-0x7F 包含子类型的命令类型
    {
        if (type == NEURON_STREAM_TYPE) // 流数据服务类型
        {
            if ((subtype == NEURON_STREAM_COMMAND_SUB_TYPE) || (subtype == NEURON_STREAM_DATA_SUB_TYPE))
            {
            #if NEURON_F0F7_STREAM_ENABLE
                neuron_stream_f0f7_subscriber(port_id, subtype, payload, payload_len);
            #endif
            }
            else 
            {
                neuron_general_subscriber(port_id, type, subtype, payload, payload_len);
            }
        }
        else if ((type == NEURON_DEVICE_TYPE) && (subtype == NEURON_DEVICE_SUB_TYPE)) // 本设备类型
        {
            neuron_user_subscriber(port_id, payload, payload_len);
        }
    }
}

/**
 * @brief F0F7协议路由处理函数
 * @param port_id 端口ID，标识数据来源端口
 * @param address 目标地址，用于判断数据的路由方向
 * @param data 数据缓冲区指针
 * @param len 数据长度
 * 
 * 功能说明：
 * 根据目标地址决定数据的处理方式：
 * 1. 如果地址是本机地址，则交给本地分发器处理
 * 2. 如果地址是广播地址(0xFF)，则交给本地分发器处理
 * 3. 如果地址是其他设备地址，则转发到对应设备
 */
uint8_t neuron_router_f0f7_route(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint8_t address = data[0];

    // 接收到本机的数据或广播数据
    if (address == NEURON_DEVICE_ADDRESS || (address == NEURON_ROUTER_F0F7_BROADCAST_ADDRESS))
    {
        neuron_dispatcher_f0f7_dispatch(port_id, data+1, len-1); // 减去地址
        return NEURON_ROUTER_F0F7_SUCCESS;
    }
    else
    {
        // 转发到其他设备
        return NEURON_ROUTER_F0F7_FORWARD;
    }
}
/******************************* End of File (C）****************************/
