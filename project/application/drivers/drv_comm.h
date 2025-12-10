/****************************************************************************\
**  版    权 :  
**  文件名称 :  
**  功能描述 :  
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

/****************************************************************************\
                               Includes
\****************************************************************************/
#ifndef __DRV_COMM_H
#define __DRV_COMM_H

#include <stdio.h>	
#include <stdbool.h>	
#include <stdint.h>
#include "stm32f4xx.h"                  // Device header
#include "sys.h" 
/****************************************************************************\
                             Macro definitions
\****************************************************************************/
//串口驱动定义
#define USART1_RX_BUF_SIZE   256
#define USART1_TX_BUF_SIZE   256
#define USART2_RX_BUF_SIZE   256
#define USART2_TX_BUF_SIZE   256
/****************************************************************************\
                             Typedef definitions
\****************************************************************************/
// 串口ID枚举
typedef enum
{
    DRV_USART_ID_1 = 0,      // USART1
    DRV_USART_ID_2,          // USART2
    DRV_USART_ID_3,          // USART3
    DRV_USART_ID_MAX
}TE_DRV_USART_ID;

// 串口接收回调函数类型定义
typedef void (*drv_usart_rx_callback_t)(uint8_t *data, uint16_t len);
/****************************************************************************\
                             Variables definitions
\****************************************************************************/

/****************************************************************************\
                             Functions definitions
\****************************************************************************/
// 通用接口函数（支持多串口）
void drv_usart_init(TE_DRV_USART_ID id, uint32_t bound, drv_usart_rx_callback_t callback);  // 初始化串口
void drv_usart_set_rx_callback(TE_DRV_USART_ID id, drv_usart_rx_callback_t callback);       // 注册接收回调函数
void drv_usart_send_byte(TE_DRV_USART_ID id, uint8_t data);                                 // 发送单个字符
void drv_usart_send_string(TE_DRV_USART_ID id, char *String);                               // 发送字符串
void drv_usart_send_data(TE_DRV_USART_ID id, uint8_t *data, uint16_t size);                 // 发送指定长度的数据
void drv_usart_dma_send_data(TE_DRV_USART_ID id, uint8_t *data, uint16_t data_len);         // 使用DMA发送数据

#endif
/****************************************************************************\
                             End of File
\****************************************************************************/


