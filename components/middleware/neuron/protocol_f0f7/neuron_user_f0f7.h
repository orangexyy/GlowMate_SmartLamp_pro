/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  neuron_user_f0f7.h
**  功能描述 :  用户层负责处理用户请求，包括MCODE和GCODE命令的处理。
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/
#ifndef __NEURON_USER_F0F7_H__
#define __NEURON_USER_F0F7_H__

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef enum
{
    NEURON_BLOCK_START,
    NEURON_BLOCK_ING,
    NEURON_BLOCK_END,
} TE_NEURON_BLOCK_STATE;

typedef void (*neuron_user_mcode_handler_t)(uint8_t port_id, char *data, uint16_t len, char *reply_data);
typedef void (*neuron_user_gcode_handler_t)(uint8_t port_id, char *data, uint16_t len, char *reply_data);

/**
 * @brief 块数据传输处理函数
 * @param state 块数据传输状态
 * @param filename 文件名
 * @param data 数据
 * @param len 数据长度
 * @return 处理后的已接收的数据长度
 */
typedef uint32_t (*neuron_user_block_handler_t)(TE_NEURON_BLOCK_STATE state, char *filename, uint8_t *data, uint16_t len);

/****************************************************************************\
                           Global functions definitions
\****************************************************************************/
/**
 * @brief 设置MCODE命令处理函数
 * @param handler 处理函数指针
 */
void neuron_user_set_mcode_handler(neuron_user_mcode_handler_t handler);

/**
 * @brief 设置GCODE命令处理函数
 * @param handler 处理函数指针
 */
void neuron_user_set_gcode_handler(neuron_user_gcode_handler_t handler);

/**
 * @brief 设置块数据传输处理函数
 * @param handler 处理函数指针
 */
void neuron_user_set_block_handler(neuron_user_block_handler_t handler);

#endif /* __NEURON_USER_F0F7_H__ */
/**************************** End of File (H) ****************************/ 
