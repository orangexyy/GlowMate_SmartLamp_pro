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
#include "neuron_system_f0f7.h" 
#include "router_dispatch_f0f7.h"
#include "neuron_link_f0f7.h"

/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define CTL_ASSIGN_DEV_ID          0x10 // Assignment device ID
#define CTL_SYSTEM_RESET           0x11 // reset from host
#define CTL_QUERY_FIRMWARE_VERSION 0x12 // query version of block frimware.
#define CTL_SET_BAUDRATE           0x13
#define CTL_COMMUNICATION_TEST     0x14 // test communication quality by large data package

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static neuron_system_callback_t s_system_callback = NULL;
/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/**
 * @brief 分配设备ID
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_system_assign_dev_id(uint8_t port_id, uint8_t *data, uint16_t len)
{
    extern bool neuron_general_is_into_upgrade(void);

    uint8_t dev_id[2] = {0};
    if (neuron_general_is_into_upgrade())
    {
        dev_id[0] = 0x00;
        dev_id[1] = 0x00;
    }
    else
    {
        dev_id[0] = NEURON_DEVICE_TYPE;
        dev_id[1] = NEURON_DEVICE_SUB_TYPE;
    }
    neuron_link_f0f7_pack_raw(port_id, CTL_ASSIGN_DEV_ID, dev_id[0], &dev_id[1], 1);
}

/**
 * @brief 重启
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_system_reboot(uint8_t port_id, uint8_t *data, uint16_t len)
{
    if (s_system_callback != NULL) 
    {
        s_system_callback(NEURON_SYSTEM_CMD_REBOOT, NULL, 0, NULL, 0);
    }
}

/**
 * @brief 通用命令订阅者
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
void neuron_system_subscriber(uint8_t port_id, uint8_t type, uint8_t subtype, uint8_t *data, uint16_t len)
{
    switch (type)   
    {
        case CTL_ASSIGN_DEV_ID:
            neuron_system_assign_dev_id(port_id, data, len);
            break;

        case CTL_SYSTEM_RESET:
            neuron_system_reboot(port_id, data, len);
            break;

        case CTL_QUERY_FIRMWARE_VERSION:
            break;

        case CTL_SET_BAUDRATE:
            break;

        case CTL_COMMUNICATION_TEST:
            break;

        default:
            break;
    }
}

/**
 * @brief 设置系统回调
 * @param callback 回调函数
 */
void neuron_system_set_callback(neuron_system_callback_t callback)
{
    s_system_callback = callback;
}

/******************************* End of File (C）****************************/
