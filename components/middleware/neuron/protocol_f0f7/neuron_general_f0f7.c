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
#include "neuron_general_f0f7.h" 
#include "router_dispatch_f0f7.h"
#include "neuron_link_f0f7.h"
#include "byte_buffer.h"

/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define CTL_SET_FEEDBACK           0x01 // set feedback.
#define CTL_SET_RGB_LED            0x02
#define CTL_FIND_BLOCK             0x03
#define CTL_UPDATE_FIRMWARE        0x05 // 开始升级固件
#define CTL_READ_HARDWARE_ID       0x06 // 读取硬件ID
#define CTL_TRANSFER_UPGRADE_FILE  0x07  // 传输升级
#define CTL_QUERY_UPGRADE_RESULT   0x08  // 升级结果
#define CTL_SET_AUTO_SWITCH        0x0B
#define CTL_QUERY_BOOT_VERSION     0x11 // 查询固件版本

#define GENERAL_RESPONSE           0x15 // 通用响应

#define CTL_GENERAL                0x61 // 通用命令

#define WRONG_TYPE                 0x12

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static neuron_general_upgrade_callback_t s_upgrade_callback;
static bool s_into_upgrade = false;
/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/**
 * @brief 设置反馈
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_general_set_feedback(uint8_t port_id, uint8_t *data, uint16_t len)
{
     
}

/**
 * @brief 设置RGB LED
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_general_set_rgb_led(uint8_t port_id, uint8_t *data, uint16_t len)
{
    
}

/**
 * @brief 查找块
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_general_find_block(uint8_t port_id, uint8_t *data, uint16_t len)
{
    
}

/**
 * @brief 更新固件
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_general_update_firmware(uint8_t port_id, uint8_t *data, uint16_t len)
{
    BYTE_BUFFER byte_buffer;
    uint8_t     block_type;
    uint8_t     block_sub_type; 

    byte_buffer_init(&byte_buffer, data, len, 0);
    byte_buffer_read_byte_8(&byte_buffer, &block_type);
    byte_buffer_read_byte_8(&byte_buffer, &block_sub_type);

    if (s_upgrade_callback != NULL) 
    {
        s_upgrade_callback(port_id, NEURON_GENERAL_CMD_INTO_BOOTLOADER, data, len, NULL, 0);
    }
    
    s_into_upgrade = true;
}

/**
 * @brief 读取硬件ID
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_general_read_hardware_id(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint32_t ad1_value;
    uint32_t ad2_value;

    BYTE_BUFFER byte_buffer;
    uint8_t     response_buf[32] = {0};

    ad1_value = BOARD_HARDWAREID_HIGH;
    ad2_value = BOARD_HARDWAREID_LOW;

    byte_buffer_init(&byte_buffer, response_buf, 32, 0);
    byte_buffer_write_short_24(&byte_buffer, ad1_value);
    byte_buffer_write_short_24(&byte_buffer, ad2_value);
    neuron_link_f0f7_pack_raw(port_id, CTL_GENERAL, CTL_READ_HARDWARE_ID, response_buf, byte_buffer.index);
}

/**
 * @brief 设置自动切换
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_general_set_auto_switch(uint8_t port_id, uint8_t *data, uint16_t len)
{
    
}

/**
 * @brief 传输升级文件
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_general_transfer_upgrade_file(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint8_t ret = REPLY_ERROR;
    short package_no = 0;
    BYTE_BUFFER byte_buffer;
    uint8_t response_buf[32];

    byte_buffer_init(&byte_buffer, data, len, 0); 
    byte_buffer_read_short_16(&byte_buffer, &package_no);

    if (s_upgrade_callback != NULL) 
    {
        if (package_no == 0)
        {
            ret = (uint8_t)s_upgrade_callback(port_id, NEURON_GENERAL_CMD_UPGRADE_START, data+byte_buffer.index, len-byte_buffer.index, NULL, 0);
        }
        else 
        {
            ret = (uint8_t)s_upgrade_callback(port_id, NEURON_GENERAL_CMD_UPGRADE_TRANSFER, data, len, NULL, 0);
        }
    }

    byte_buffer_init(&byte_buffer, response_buf, 32, 0);   
    byte_buffer_write_short_16(&byte_buffer, package_no);
    byte_buffer_write_byte_8(&byte_buffer, ret);
    neuron_link_f0f7_pack_raw(port_id, CTL_GENERAL, CTL_TRANSFER_UPGRADE_FILE, response_buf, byte_buffer.index);
}

/**
 * @brief 查询升级结果
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_general_query_upgrade_result(uint8_t port_id, uint8_t *data, uint16_t len)
{
    BYTE_BUFFER byte_buffer;
    uint8_t response_buf[32];
    uint8_t upgrade_status = 0;

    if (s_upgrade_callback != NULL) 
    {
        upgrade_status = (uint8_t)s_upgrade_callback(port_id, NEURON_GENERAL_CMD_QUERY_UPGRADE_STATUS, NULL, 0, NULL, 0);
    }

    byte_buffer_init(&byte_buffer, response_buf, 32, 0);   
    byte_buffer_write_byte_8(&byte_buffer, upgrade_status);
    neuron_link_f0f7_pack_raw(port_id, CTL_GENERAL, CTL_QUERY_UPGRADE_RESULT, response_buf, byte_buffer.index); 
}

/**
 * @brief 查询固件版本
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
static void neuron_general_query_boot_version(uint8_t port_id, uint8_t *data, uint16_t len)
{
    uint8_t response_buf[64] = {0};
    uint16_t response_len = 0;
    if (s_upgrade_callback != NULL) 
    {
        s_upgrade_callback(port_id, NEURON_GENERAL_CMD_QUERY_VERSION, NULL, 0, response_buf, &response_len);
    }
    neuron_link_f0f7_pack_raw(port_id, CTL_GENERAL, CTL_QUERY_BOOT_VERSION, response_buf, response_len); 
}

/**
 * @brief 通用命令订阅者
 * @param port_id 端口ID
 * @param data 数据缓冲区
 * @param len 数据长度
 */
void neuron_general_subscriber(uint8_t port_id, uint8_t type, uint8_t subtype, uint8_t *data, uint16_t len)
{
    switch (subtype)   
    {
        case CTL_SET_FEEDBACK: 
            neuron_general_set_feedback(port_id, data, len);
            break;

        case CTL_SET_RGB_LED:
            neuron_general_set_rgb_led(port_id, data, len);
            break;

        case CTL_FIND_BLOCK:
            neuron_general_find_block(port_id, data, len);
            break;

        case CTL_UPDATE_FIRMWARE:
            neuron_general_update_firmware(port_id, data, len);
            break;

        case CTL_READ_HARDWARE_ID:
            neuron_general_read_hardware_id(port_id, data, len);
            break;

        case CTL_TRANSFER_UPGRADE_FILE:
            neuron_general_transfer_upgrade_file(port_id, data, len);
            break;

        case CTL_QUERY_UPGRADE_RESULT:
            neuron_general_query_upgrade_result(port_id, data, len);
            break;

        case CTL_QUERY_BOOT_VERSION:
            neuron_general_query_boot_version(port_id, data, len);
            break;

        case CTL_SET_AUTO_SWITCH:
            neuron_general_set_auto_switch(port_id, data, len);
            break;

        default:
            break;
    }
}

/**
 * @brief 设置升级回调函数
 * @param callback 升级回调函数
 */
void neuron_general_set_upgrade_callback(neuron_general_upgrade_callback_t callback)
{
    s_upgrade_callback = callback;
}

/**
 * @brief 判断是否进入升级状态
 * @return 是否进入升级状态
 */
bool neuron_general_is_into_upgrade(void)
{
    return s_into_upgrade;
}

/******************************* End of File (C）****************************/
