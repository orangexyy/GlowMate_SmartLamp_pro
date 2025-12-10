/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_router.h
**  功能描述 :  路由层负责根据消息的地址、类型和子类型决定消息的处理方式，
                包括本地处理或转发到其他设备。
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

#ifndef __NEURON_SYSTEM_F0F7_H__
#define __NEURON_SYSTEM_F0F7_H__

#include <stdint.h>
#include <stdbool.h> 

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

typedef enum
{
    NEURON_SYSTEM_CMD_NULL,
    NEURON_SYSTEM_CMD_REBOOT,
    NEURON_SYSTEM_CMD_QUERY_VERSION,
} TS_NEURON_SYSTEM_CMD;

typedef int (*neuron_system_callback_t)(TS_NEURON_SYSTEM_CMD cmd, uint8_t *data, uint16_t len, uint8_t *response, uint16_t *response_len);

/****************************************************************************\
                           Global functions definitions
\****************************************************************************/

/**
 * @brief 设置系统回调
 * @param callback 回调函数
 */
void neuron_system_set_callback(neuron_system_callback_t callback);

#endif /* __NEURON_SYSTEM_F0F7_H__ */
/**************************** End of File (H) ****************************/ 
