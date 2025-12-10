/******************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  mk_hal_uart.h
**  功能描述 :  串口驱动
**  作    者 :  王滨泉
**  日    期 :  2022.11.14
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2022.11.14
                1 首次创建
\******************************************************************************/
#ifndef MK_HAL_UART_H
#define MK_HAL_UART_H

#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------------------------------

/******************************************************************************\
                             Includes
\******************************************************************************/
//#include "device_config.h"
#include "mk_hal_gpio.h"
#include <stdint.h>
#include <stdbool.h>
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
//------------------------------------------------------------------------------
// 普通串口模式默认配置
#define MK_USART_DEFAULT_CONFIG  {                              \
    .id                         = MK_USART_ID_MAX,              \
    .Mode                       = (MK_USART_MODE_TX | MK_USART_MODE_RX),          \
    .io.TxPin                   = MK_GPIOA_00,              \
    .io.RxPin                   = MK_GPIOA_01,              \
    .BaudRate 	                = 115200,                       \
		.Init.WordLength            = MK_USART_WORDLENGTH_8B,       \
    .Init.StopBits 	            = MK_USART_STOPBITS_1,          \
    .Init.Parity                = MK_USART_PARITY_NO,           \
    .Init.HwFlowCtl             = MK_USART_HWCONTROL_NONE,      \
    .Init.OverSample            = MK_USART_OVERSAMPLE_16,       \
}   

//------------------------------------------------------------------------------
// DMA模式默认配置
#define MK_USART_DMA_DEFAULT_CONFIG  {                                      \
    .Direction                  = MK_USART_DMA_MEMORY_TO_PERIPH,        \
    .PeriphInc                  = MK_USART_DMA_PERIPH_INCREASE_DISABLE, \
    .MemInc 	                = MK_USART_DMA_MEMORY_INCREASE_ENABLE,  \
    .Periph_Memory_Width        = MK_USART_DMA_PERIPH_WIDTH_BYTE,       \
    .Mode                       = MK_USART_DMA_CIRCULAR,                \
    .Priority 	                = DMA_PRIORITY_MEDIUM,                  \
}
  
//------------------------------------------------------------------------------
/******************************************************************************\
                             Typedef definitions
\******************************************************************************/
//------------------------------------------------------------------------------
// 串口ID
typedef enum    
{   
    MK_USART_ID_0,          // USART0   
    MK_USART_ID_1,          // USART1      
    MK_UART_ID_3 ,  				// UART3
    MK_UART_ID_4 ,					// UART4
    MK_USART_ID_MAX
}TE_MK_USART_ID;

// 串口IO管脚
typedef struct                              
{                                       
    uint8_t	        TxPin;   // 发送管脚 
    uint8_t     	RxPin;   // 接收管脚
}TS_MK_USART_IO; 

//------------------------------------------------------------------------------
// DMA方向    
typedef enum    
{   
    MK_USART_DMA_PERIPH_TO_MEMORY   = DMA_PERIPHERAL_TO_MEMORY,      // 外设到内存方向
    MK_USART_DMA_MEMORY_TO_PERIPH   = DMA_MEMORY_TO_PERIPHERAL,      // 内存到外设方向
}TE_MK_USART_DMA_DIRECTION; 

//------------------------------------------------------------------------------
// DMA 外设自增模式   
typedef enum    
{   
    MK_USART_DMA_PERIPH_INCREASE_DISABLE    = DMA_PERIPH_INCREASE_DISABLE,  // 外设自增模式不使能
    MK_USART_DMA_PERIPH_INCREASE_ENABLE     = DMA_PERIPH_INCREASE_ENABLE,   // 外设自增模式使能
}TE_MK_USART_DMA_PERIPHINC; 

//------------------------------------------------------------------------------
// DMA 内存自增模式           
typedef enum            
{           
    MK_USART_DMA_MEMORY_INCREASE_DISABLE    = DMA_MEMORY_INCREASE_DISABLE,  // 内存自增模式不使能 
    MK_USART_DMA_MEMORY_INCREASE_ENABLE     = DMA_MEMORY_INCREASE_ENABLE,   // 内存自增模式使能
}TE_MK_USART_DMA_MEMINC;

//------------------------------------------------------------------------------
// DMA外设数据宽度
typedef enum 
{
    MK_USART_DMA_PERIPH_WIDTH_BYTE          = DMA_PERIPHERAL_WIDTH_8BIT,        // 1字节对齐 
    MK_USART_DMA_PERIPH_WIDTH_HALFWORD      = DMA_PERIPHERAL_WIDTH_16BIT,       // 2字节对齐 
    MK_USART_DMA_PERIPH_WIDTH_WORD          = DMA_PERIPHERAL_WIDTH_32BIT,       // 4字节对齐 
}TE_MK_USART_DMA_PERIPH_WIDTH;

//------------------------------------------------------------------------------
// DMA优先级
typedef enum 
{
    MK_USART_DMA_PRIORITY_LOW               = DMA_PRIORITY_LOW,             // 低   
    MK_USART_DMA_PRIORITY_MEDIUM            = DMA_PRIORITY_MEDIUM,          // 中   
    MK_USART_DMA_PRIORITY_HIGH              = DMA_PRIORITY_HIGH,            // 高   
    MK_USART_DMA_PRIORITY_ULTRA_HIGH        = DMA_PRIORITY_ULTRA_HIGH,      // 最高 
}TE_MK_USART_DMA_PRIORITY;

//------------------------------------------------------------------------------
// DMA结构体                                
typedef struct                              
{                                           
    TE_MK_USART_DMA_DIRECTION           Direction;              // 方向                 
    TE_MK_USART_DMA_PERIPHINC           PeriphInc;              // 外设自增模式 
    TE_MK_USART_DMA_MEMINC              MemInc;                 // 内存自增模式  
    TE_MK_USART_DMA_PERIPH_WIDTH        Periph_Memory_Width;    // 外设数据对齐长度                   
    TE_MK_USART_DMA_PRIORITY            Priority;               // 优先级                  
} TS_MK_USART_DMA_INIT; 


// 字节长度    
typedef enum        
{     
    MK_USART_WORDLENGTH_8B      = USART_WL_8BIT,        // 8bit
    MK_USART_WORDLENGTH_9B      = USART_WL_9BIT,        // 9bit
}TE_MK_USART_WORDLENGTH;

//------------------------------------------------------------------------------
// 停止位
typedef enum
{
    MK_USART_STOPBITS_1         = USART_STB_1BIT,       // 停止位1
    MK_USART_STOPBITS_0_5       = USART_STB_0_5BIT,     // 停止位0.5
    MK_USART_STOPBITS_2         = USART_STB_2BIT,       // 停止位2
    MK_USART_STOPBITS_1_5       = USART_STB_1_5BIT,     // 停止位1.5
}TE_MK_USART_STOPBIT;

//------------------------------------------------------------------------------
// 校验方式   
typedef enum    
{   
    MK_USART_PARITY_NO          = USART_PM_NONE,        // 无校验
    MK_USART_PARITY_EVEN        = USART_PM_EVEN,        // 偶校验
    MK_USART_PARITY_ODD         = USART_PM_ODD,         // 奇校验
}TE_MK_USART_PARITY; 

//------------------------------------------------------------------------------
// 串口模式
typedef enum 
{
    MK_USART_MODE_TX            = 0x01,                 // TX模式     
    MK_USART_MODE_RX            = 0x02,                 // RX模式      
    MK_USART_MODE_TX_DMA        = 0x04,                 // DMA TX模式	
    MK_USART_MODE_RX_DMA        = 0x08,                 // DMA RX模式
    MK_USART_MODE_HALFDUPLEX    = 0x10,                 // 半双工模式
}TE_MK_USART_MODE; 

// 硬件流控制
typedef enum 
{
    MK_USART_HWCONTROL_NONE,                            // 无
    MK_USART_HWCONTROL_RTS,                             // RTS流控
    MK_USART_HWCONTROL_CTS,                             // CTS流控
    MK_USART_HWCONTROL_RTS_CTS,                         // RTS和CTS流控
}TE_MK_USART_HWCONTROL; 

//------------------------------------------------------------------------------
// 过采样模式         
typedef enum    
{   
    MK_USART_OVERSAMPLE_16      = USART_OVSMOD_16,      // 16位
    MK_USART_OVERSAMPLE_8       = USART_OVSMOD_8,       // 8位
}TE_MK_USART_OVERSAMPLE; 
//------------------------------------------------------------------------------
// 串口结构体                               
typedef struct                              
{ 	                                        
    uint32_t                    BaudRate;               // 波特率            
    TE_MK_USART_WORDLENGTH      WordLength;             // 数据长度    
    TE_MK_USART_STOPBIT         StopBits;               // 停止位              
    TE_MK_USART_PARITY 	        Parity;                 // 校验位                              
    TE_MK_USART_HWCONTROL       HwFlowCtl;              // 硬件流控制 
    TE_MK_USART_OVERSAMPLE      OverSample;             // 过采样模式                
}TS_MK_USART_INIT; 

// 串口结构体
typedef struct 
{
    TE_MK_USART_ID              id;                     // 串口ID
    uint8_t                     Mode;                   // 模式
    TS_MK_USART_IO              io;	                    // IO管脚
		uint32_t                    BaudRate;               // 波特率
		TS_MK_USART_INIT            Init;	                // 设置初始化
}TS_MK_USART_HANDLE;

//------------------------------------------------------------------------------
// 回调的接收数据结构体                                               
typedef struct                                                          
{                                                                       
    uint8_t                     *Data;                  // 数据指针
    uint16_t                     Size;                  // 数据长度
}TS_MK_USART_CB_RX; 

//------------------------------------------------------------------------------
// 串口回调事件    
typedef enum    
{   
    MK_USART_EVENT_TX,      // 发送事件
    MK_USART_EVENT_RX,      // 接收事件
}TE_MK_USART_EVENT;

//------------------------------------------------------------------------------
// 回调结构体                                                        
typedef struct                                                          
{                                                                       
    TE_MK_USART_ID              id;                     // 串口ID
    TE_MK_USART_EVENT           Event;                  // 事件
    TS_MK_USART_CB_RX           Rx;                     // 接收的数据
}TS_MK_USART_CB;

//------------------------------------------------------------------------------
// 回调函数
typedef int (*MK_HAL_USART_CALLBACK)(uint8_t *data, uint32_t len);
/******************************************************************************\
                         Global variables definitions
\******************************************************************************/

/******************************************************************************\
                         Global functions definitions
\******************************************************************************/

/*
* 函数名称 : mk_hal_usart_open
* 功能描述 : 串口初始化函数
* 参    数 : p_this     - 串口结构体
             FunctionCB - 接收回调函数
* 返回值   : int：返回错误代码
* 示    例 : mk_hal_usart_open(&p_this, FunctionCB);	 
*/
/******************************************************************************/
int mk_hal_usart_open(TS_MK_USART_HANDLE *p_this, MK_HAL_USART_CALLBACK FunctionCB);
/******************************************************************************/

/*
* 函数名称 : mk_hal_usart_dma_open
* 功能描述 : 串口DMA初始化函数
* 参    数 : p_this     - 串口结构体
             FunctionCB - 接收回调函数
* 返回值   : int：返回错误代码
* 示    例 : mk_hal_usart_open(&p_this, FunctionCB);	 
*/
/******************************************************************************/
int mk_hal_usart_dma_open(TS_MK_USART_HANDLE *p_this, MK_HAL_USART_CALLBACK FunctionCB);
/******************************************************************************/

/*
* 函数名称 : mk_hal_uart_write
* 功能描述 : 串口输出发送(多字节)
* 参    数 : id   - id
             buf  - 数据指针
             size - 数据长度
* 返回值   : MK_SUCCESS   - 数据写入成功
             MK_FAIL - 数据写入失败
*/
/******************************************************************************/
int mk_hal_uart_write(TE_MK_USART_ID id, uint8_t* buf, uint32_t size);
/******************************************************************************/

/*
* 函数名称 : mk_hal_uart_write_sync
* 功能描述 : 串口输出发送(多字节)，不入缓冲区
* 参    数 : id   - id
             buf  - 数据指针
             size - 数据长度
* 返回值   : MK_SUCCESS   - 数据写入成功
             MK_FAIL - 数据写入失败
*/
/******************************************************************************/
int mk_hal_uart_write_sync(TE_MK_USART_ID id, uint8_t* buf, uint32_t size);
/******************************************************************************/

/*
* 函数名称 : mk_hal_uart_dma_write
* 功能描述 : 串口输出发送(多字节)
* 参    数 : channel - 通道
             buf - 数据指针
             size - 数据长度
* 返回值   : 无
*/
/******************************************************************************/
int mk_hal_uart_dma_write(TE_MK_USART_ID id, uint8_t* buf, uint32_t size);
/******************************************************************************/

/*
* 函数名称 : mk_hal_uart_get_tx_busy
* 功能描述 : 获取发送忙状态
* 参    数 : id - usart id
* 返回值   : 无
*/
/******************************************************************************/
bool mk_hal_uart_get_tx_busy(TE_MK_USART_ID id);
/******************************************************************************/


/*
* 函数名称 : mk_hal_uart_read
* 功能描述 : 串口接收(多字节)
* 参    数 : channel - 通道
* 返回值   : 数据
*/
/******************************************************************************/
uint8_t mk_hal_uart_read(uint8_t channel);
/******************************************************************************/

/*
* 函数名称 : mk_hal_uart_set_baudrate
* 功能描述 : 串口波特率设置
* 参    数 : id - id
             baudrate - 波特率
* 返回值   : 无
*/
/******************************************************************************/
void mk_hal_uart_set_baudrate(uint8_t id, uint32_t baudrate);
/******************************************************************************/

/*
* 函数名称 : mk_hal_uart_set_usart_cb
* 功能描述 : 数据接收回调设置
* 参    数 : id - id
             FunctionCB - 接收回调函数
* 返回值   : 无
*/
/******************************************************************************/
void mk_hal_uart_set_usart_cb(uint8_t id, MK_HAL_USART_CALLBACK FunctionCB);
/******************************************************************************/

void dma_reload_init();


//--------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif

/****************************** End of File (H) *******************************/ 

