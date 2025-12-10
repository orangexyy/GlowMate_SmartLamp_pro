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

/******************************************************************************\
                               Includes
\******************************************************************************/
#include "stm32f4xx.h"
#include "drv_comm.h"
#include "stdbool.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

// 串口配置结构体
typedef struct
{
    USART_TypeDef*              usart_periph;       // USART外设
    drv_usart_rx_callback_t     rx_callback;        // 接收回调函数
}TS_DRV_USART_CONFIG;

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
// 串口配置数组
static TS_DRV_USART_CONFIG s_usart_config[DRV_USART_ID_MAX] = {
    {USART1, NULL},
    {USART2, NULL},
    {USART3, NULL},
};

// printf输出串口ID（默认USART1）
static TE_DRV_USART_ID s_printf_usart_id = DRV_USART_ID_1;

uint8_t usart1_rx_flag = 0;    //串口是否已接收完毕
uint8_t usart1_tx_busy = 0;
uint16_t usart1_rx_len = 0;
uint16_t usart1_tx_len = 0;
uint8_t usart1_rx_buffer[USART1_RX_BUF_SIZE] = {0};
uint8_t usart1_tx_buffer[USART1_TX_BUF_SIZE] = {0};

uint8_t usart2_rx_flag = 0;    //串口是否已接收完毕
uint8_t usart2_tx_busy = 0;
uint16_t usart2_rx_len = 0;
uint16_t usart2_tx_len = 0;
uint8_t usart2_rx_buffer[USART2_RX_BUF_SIZE] = {0};
uint8_t usart2_tx_buffer[USART2_TX_BUF_SIZE] = {0};

// volatile bool usart3_receive_flag = false;    //串口是否已接收完毕
// uint16_t usart3_rx_len;
// uint8_t usart3_rx_buffer[USART3_RX_SIZE];
// uint8_t usart3_tx_buffer[USART3_TX_SIZE];

/******************************************************************************\
                             Functions definitions
\******************************************************************************/
static void usart1_init(unsigned int bound);
static void usart2_init(unsigned int bound);

/* @brief printf重映射函数，将printf输出重定向到串口
 * @param ch: 要输出的字符
 * @param f: 文件指针（标准库使用）
 * @return 输出的字符
 * @note 此函数会被标准库的printf调用，输出到s_printf_usart_id指定的串口
 */
int fputc(int ch, FILE *f)
{
    if(s_printf_usart_id < DRV_USART_ID_MAX)
    {
        USART_TypeDef* usart = s_usart_config[s_printf_usart_id].usart_periph;
        while(USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);  // 等待发送完成
        USART_SendData(usart, (uint8_t)ch);
    }
    return ch;
}

/* @brief 设置printf输出串口
 * @param id: 串口ID
 */
void drv_usart_set_printf_port(TE_DRV_USART_ID id)
{
    if(id < DRV_USART_ID_MAX)
    {
        s_printf_usart_id = id;
    }
}

/* @brief 获取当前printf输出串口
 * @return 当前printf输出的串口ID
 */
TE_DRV_USART_ID drv_usart_get_printf_port(void)
{
    return s_printf_usart_id;
}

/* @brief 初始化串口（通用接口）
 * @param id: 串口ID
 * @param bound: 波特率
 * @param callback: 接收回调函数
 */
void drv_usart_init(TE_DRV_USART_ID id, uint32_t bound, drv_usart_rx_callback_t callback)
{
    if(id >= DRV_USART_ID_MAX)
    {
        return;
    }

    switch(id)
    {
        case DRV_USART_ID_1:
            usart1_init(bound);
            break;
        case DRV_USART_ID_2:
            usart2_init(bound);
            break;
        default:
            break;
    }

    s_usart_config[id].rx_callback = callback;
}

/* @brief 注册串口接收回调函数（通用接口）
* @param id: 串口ID
* @param callback: 回调函数指针，NULL表示取消回调
*/
void drv_usart_set_rx_callback(TE_DRV_USART_ID id, drv_usart_rx_callback_t callback)
{
    if(id < DRV_USART_ID_MAX)
    {
        s_usart_config[id].rx_callback = callback;
    }
}

/* @brief 发送单个字符（通用接口）
* @param id: 串口ID
* @param data: 要发送的数据
*/
void drv_usart_send_byte(TE_DRV_USART_ID id, uint8_t data)
{
    if(id >= DRV_USART_ID_MAX)
    {
        return;
    }
    
    USART_TypeDef* usart = s_usart_config[id].usart_periph;
    while(USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
    USART_SendData(usart, data);
}

/* @brief 发送字符串（通用接口）
* @param id: 串口ID
* @param String: 要发送的字符串
*/
void drv_usart_send_string(TE_DRV_USART_ID id, char *String)
{
    if(String == NULL || id >= DRV_USART_ID_MAX)
    {
        return;
    }
    
    uint8_t i;
    for(i = 0; String[i] != '\0'; i++)
    {
        drv_usart_send_byte(id, String[i]);
    }
}

/* @brief 发送指定长度的数据（通用接口）
* @param id: 串口ID
* @param data: 要发送的数据指针
* @param size: 要发送的数据长度
*/
void drv_usart_send_data(TE_DRV_USART_ID id, uint8_t *data, uint16_t size)
{
    if(data == NULL || id >= DRV_USART_ID_MAX)
    {
        return;
    }
    
    uint16_t i;
    for(i = 0; i < size; i++)
    {
        drv_usart_send_byte(id, data[i]);
    }
}

/* @brief 发送指定长度的数据（通用接口）
* @param id: 串口ID
* @param data: 要发送的数据指针
* @param len: 要发送的数据长度（字节数）
* @note 注意：如果data是字符串，len应该不包含末尾的\0
*/
void drv_usart_dma_send_data(TE_DRV_USART_ID id, uint8_t *data, uint16_t len)
{
	uint16_t timeout_1 = 0;
	uint16_t timeout_2 = 0;
    switch (id) {
        case DRV_USART_ID_1:
            
            if(usart1_tx_busy != 0 || len > USART1_TX_BUF_SIZE) /**上一次配置DMA数据发送未完成不进行数据发送**/
            {
                return;
            }
            usart1_tx_busy = 1;
            usart1_tx_len = len; 
            memcpy(usart1_tx_buffer,data, len);
            DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
            DMA_Cmd(DMA2_Stream7, DISABLE);
            while(DMA_GetCmdStatus(DMA2_Stream7) != DISABLE)
            {
                if (timeout_1 < 0xffff)  // 超时退出
                {
                    timeout_1++;
                }
                else
                {
                    break; 
                }
            }
            DMA_SetCurrDataCounter(DMA2_Stream7, usart1_tx_len);
            DMA_Cmd(DMA2_Stream7, ENABLE);
            break;
        case DRV_USART_ID_2:
            if(usart2_tx_busy != 0 || len > USART2_TX_BUF_SIZE) /**上一次配置DMA数据发送未完成不进行数据发送**/
            {
                return;
            }
            usart2_tx_busy = 1;
            usart2_tx_len = len; 
            memcpy(usart2_tx_buffer,data, len);
            DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
            DMA_Cmd(DMA1_Stream6, DISABLE);
            while(DMA_GetCmdStatus(DMA1_Stream6) != DISABLE)
            {
                if (timeout_2 < 0xffff)  // 超时退出
                {
                    timeout_2++;
                }
                else
                {
                    break; 
                }
            }
            DMA_SetCurrDataCounter(DMA1_Stream6, usart2_tx_len);
            DMA_Cmd(DMA1_Stream6, ENABLE);
            break;
        default:
            break;
    }
    
}

static void usart1_periph_init(unsigned int bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOA,&GPIO_InitStructure); 

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_Init(USART1, &USART_InitStructure); 

    USART_Cmd(USART1, ENABLE);  
}

static void usart1_dma_init(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA2_Stream2);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_InitStructure.DMA_Channel = DMA_Channel_4; 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;           /**规定了DMA的外设基地址*/
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)usart1_rx_buffer;         /**规定了DMA的内存基地址*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     /**外设作为数据传输的来源*/
    DMA_InitStructure.DMA_BufferSize = USART1_RX_BUF_SIZE;                      /**接收最大长度*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /**外设地址寄存器不递增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /**内存地址寄存器递增*/
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     /**外设数据宽度8位*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             /**内存数据宽度8位*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             /**工作在循环模式*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         /**DMA通道拥有高优先级*/
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream2, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream2, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

    DMA_DeInit(DMA2_Stream7);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
    DMA_InitStructure.DMA_Channel = DMA_Channel_4; 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;           /**DMA的外设基地址*/
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)usart1_tx_buffer;         /**DMA的内存基地址*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                     /**内存作为数据传输的来源*/
    DMA_InitStructure.DMA_BufferSize = USART1_TX_BUF_SIZE;                      /**发送最大长度*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /**外设地址寄存器不递增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /**内存地址寄存器递增*/
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     /**外设数据宽度8位*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             /**内存数据宽度8位*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               /**工作在正常模式*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         /**DMA通道拥有高优先级*/
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream7, DISABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

static void usart1_interrupt_init(void)
{
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_IDLE);
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_TC);
    
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);   
}

/* @brief 总初始化*/
static void usart1_init(unsigned int bound)
{
    usart1_periph_init(bound);                  //初始化串口外设
    usart1_dma_init();           //初始化接收寄存器的DMA通道
    usart1_interrupt_init();   //初始化串口的空闲中断 
}

void USART1_IRQHandler(void)
{
    if( USART_GetITStatus(USART1, USART_IT_IDLE) != RESET )
    {
        USART_ReceiveData(USART1);
        USART_ClearITPendingBit(USART1,USART_IT_IDLE);
        DMA_Cmd(DMA2_Stream2, DISABLE);
        while(DMA_GetCmdStatus(DMA2_Stream2) != DISABLE);  

        usart1_rx_len = USART1_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2); 

        DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_DMEIF2);
        DMA_SetCurrDataCounter(DMA2_Stream2, USART1_RX_BUF_SIZE); 
        DMA_Cmd(DMA2_Stream2, ENABLE);  
        
        if(usart1_rx_len < USART1_RX_BUF_SIZE)  
        {
            usart1_rx_buffer[usart1_rx_len] = '\0';
        }
        else  
        {
            usart1_rx_buffer[USART1_RX_BUF_SIZE - 1] = '\0';
        }
        
        if(s_usart_config[DRV_USART_ID_1].rx_callback != NULL && usart1_rx_len > 0)
        {
            s_usart_config[DRV_USART_ID_1].rx_callback(usart1_rx_buffer, usart1_rx_len);
        }
        usart1_rx_flag = true;
    }
    else if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        usart1_tx_busy = 0;
        USART_ClearITPendingBit(USART1, USART_IT_TC);
    }
}

static void usart2_periph_init(unsigned int bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOA,&GPIO_InitStructure); 

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_Init(USART2, &USART_InitStructure); 

    USART_Cmd(USART2, ENABLE);  
}

static void usart2_dma_init(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Stream5);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    DMA_InitStructure.DMA_Channel = DMA_Channel_4; 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;           /**规定了DMA的外设基地址*/
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)usart2_rx_buffer;         /**规定了DMA的内存基地址*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     /**外设作为数据传输的来源*/
    DMA_InitStructure.DMA_BufferSize = USART2_RX_BUF_SIZE;                      /**接收最大长度*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /**外设地址寄存器不递增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /**内存地址寄存器递增*/
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     /**外设数据宽度8位*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             /**内存数据宽度8位*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             /**工作在循环模式*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         /**DMA通道拥有高优先级*/
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream5, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    DMA_DeInit(DMA1_Stream6);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
    DMA_InitStructure.DMA_Channel = DMA_Channel_4; 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;           /**DMA的外设基地址*/
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)usart2_tx_buffer;         /**DMA的内存基地址*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                     /**内存作为数据传输的来源*/
    DMA_InitStructure.DMA_BufferSize = USART2_TX_BUF_SIZE;                      /**发送最大长度*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /**外设地址寄存器不递增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /**内存地址寄存器递增*/
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     /**外设数据宽度8位*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             /**内存数据宽度8位*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               /**工作在正常模式*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         /**DMA通道拥有高优先级*/
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream6, DISABLE);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
}

static void usart2_interrupt_init(void)
{
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  
    USART_ClearFlag(USART2, USART_FLAG_IDLE);
    USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    USART_ClearFlag(USART2, USART_FLAG_TC);
    
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);   
}

/* @brief 总初始化*/
static void usart2_init(unsigned int bound)
{
    usart2_periph_init(bound);                  //初始化串口外设
    usart2_dma_init();           //初始化接收寄存器的DMA通道
    usart2_interrupt_init();   //初始化串口的空闲中断 
}

void USART2_IRQHandler(void)
{
    if( USART_GetITStatus(USART2, USART_IT_IDLE) != RESET )
    {
        USART_ReceiveData(USART2);
        USART_ClearITPendingBit(USART2,USART_IT_IDLE);
        DMA_Cmd(DMA1_Stream5, DISABLE);
        while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);  

        usart2_rx_len = USART2_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Stream5); 

        DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_DMEIF5);
        DMA_SetCurrDataCounter(DMA1_Stream5, USART2_RX_BUF_SIZE); 
        DMA_Cmd(DMA1_Stream5, ENABLE);  
        
        if(usart2_rx_len < USART2_RX_BUF_SIZE)  
        {
            usart2_rx_buffer[usart2_rx_len] = '\0';
        }
        else  
        {
            usart2_rx_buffer[USART2_RX_BUF_SIZE - 1] = '\0';
        }
        
        if(s_usart_config[DRV_USART_ID_2].rx_callback != NULL && usart2_rx_len > 0)
        {
            s_usart_config[DRV_USART_ID_2].rx_callback(usart2_rx_buffer, usart2_rx_len);
        }
        usart2_rx_flag = true;
    }
    else if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
    {
        usart2_tx_busy = 0;
        USART_ClearITPendingBit(USART2, USART_IT_TC);
    }
}

