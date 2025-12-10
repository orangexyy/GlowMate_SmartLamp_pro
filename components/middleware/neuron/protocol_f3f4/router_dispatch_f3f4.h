/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  router_dispatch_f3f4.h
**  功能描述 :  路由层负责根据消息的地址、类型和子类型决定消息的处理方式，
                包括本地处理或转发到其他设备。
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

#ifndef __NEURON_ROUTER_DISPATCH_F3F4_H__
#define __NEURON_ROUTER_DISPATCH_F3F4_H__

#include <stdint.h>
#include <stdbool.h> 

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef void (*neuron_router_f3f4_callback)(uint8_t port_id, uint8_t *data, uint16_t len);

/****************************************************************************\
                           Global functions definitions
\****************************************************************************/
/**
 * @brief F3F4协议路由处理函数
 * @param port_id 端口ID，标识数据来源端口
 * @param data 数据缓冲区指针
 * @param len 数据长度
 */
void neuron_router_f3f4_route(uint8_t port_id, uint8_t *data, uint16_t len);

void neuron_router_f3f4_register_callback(neuron_router_f3f4_callback callback);

#endif /* __NEURON_ROUTER_DISPATCH_F3F4_H__ */
/**************************** End of File (H) ****************************/ 
