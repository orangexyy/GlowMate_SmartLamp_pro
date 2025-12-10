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

#ifndef __NEURON_GENERAL_F0F7_H__
#define __NEURON_GENERAL_F0F7_H__

#include <stdint.h>
#include <stdbool.h> 

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
//神经元升级相关接口
#define  REPLY_ERROR            (0x00)          //旧版本通用错误码
#define  REPLY_SUCC             (0x01)          
#define  REPLY_PACK_ERR         (0x02)          //包序错误
#define  REPLY_WRITE_ERR        (0x03)          //写入错误
#define  REPLY_FILE_RANGE_ERR   (0x04)          //文件超范围错误

typedef enum
{
    NEURON_GENERAL_CMD_NULL,
    NEURON_GENERAL_CMD_INTO_BOOTLOADER,
    NEURON_GENERAL_CMD_UPGRADE_START, 
    NEURON_GENERAL_CMD_UPGRADE_TRANSFER,  
    NEURON_GENERAL_CMD_QUERY_UPGRADE_STATUS,
    NEURON_GENERAL_CMD_QUERY_VERSION, 
} TS_NEURON_GENERAL_CMD;

typedef int (*neuron_general_upgrade_callback_t)(uint8_t port_id, TS_NEURON_GENERAL_CMD cmd, uint8_t *data, uint16_t len, uint8_t *response, uint16_t *response_len);

/****************************************************************************\
                           Global functions definitions
\****************************************************************************/

void neuron_general_set_upgrade_callback(neuron_general_upgrade_callback_t callback);

#endif /* __NEURON_GENERAL_F0F7_H__ */
/**************************** End of File (H) ****************************/ 
