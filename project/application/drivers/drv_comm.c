/****************************************************************************\
**  文件名称 :  drv_comm.c
**  功能描述 :  通信驱动。USART1/2/3 初始化、收发、printf 重定向、接收回调注册与 DMA/中断处理。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                               Includes
\******************************************************************************/
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

volatile bool usart1_rx_flag = false;    //串口是否已接收完毕
uint16_t usart1_rx_len = 0;
uint8_t usart1_rx_buffer[USART1_RX_BUF_SIZE] = {0};

volatile bool usart2_rx_flag = false;    //串口是否已接收完毕
uint16_t usart2_rx_len = 0;
uint8_t usart2_rx_buffer[USART2_RX_BUF_SIZE] = {0};

volatile bool usart3_rx_flag = false;    //串口是否已接收完毕
uint16_t usart3_rx_len = 0;
uint8_t usart3_rx_buffer[USART3_RX_BUF_SIZE] = {0};

/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/**
 * \brief printf 重定向：将 printf 输出到 s_printf_usart_id 指定的串口
 * \param ch 要输出的字符
 * \param f 文件指针（标准库使用）
 * \return 输出的字符
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

static void usart1_periph_init(unsigned int bound)
{
    /*初始化发送接收引脚，PA9(USART1_TX)，PA10(USART1_RX)。*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*初始化USART1外设。*/
    USART_DeInit(USART1);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

static void usart1_idle_interrupt_init(void)
{
    USART_ClearFlag(USART1, USART_IT_IDLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    /*已在主函数中设置优先级分组。*/
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void usart1_rx_dma_init(void)
{
    DMA_DeInit(DMA1_Channel5);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    DMA_InitTypeDef DMA_InitStructure = {
        .DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR,
        .DMA_MemoryBaseAddr = (uint32_t)usart1_rx_buffer, 
        .DMA_DIR = DMA_DIR_PeripheralSRC,
        .DMA_BufferSize = sizeof(usart1_rx_buffer), 
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
        .DMA_MemoryInc = DMA_MemoryInc_Enable,
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
        .DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,
        .DMA_Mode = DMA_Mode_Normal,
        .DMA_Priority = DMA_Priority_High,
        .DMA_M2M = DMA_M2M_Disable,
    };
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);   //初始化DMA通道5
    DMA_Cmd(DMA1_Channel5, ENABLE);                //使能DMA通道5
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); //使能USART1的DMA接收请求
}
/* @brief 总初始化*/
static void usart1_init(unsigned int bound)
{
    usart1_periph_init(bound);                  //初始化串口外设
    usart1_rx_dma_init();           //初始化接收寄存器的DMA通道
    usart1_idle_interrupt_init();   //初始化串口的空闲中断 
}


static void usart2_periph_init(unsigned int bound)
{
    /*初始化发送接收引脚，PA2(USART1_TX)，PA3(USART1_RX)。*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*初始化USART2外设。*/
    USART_DeInit(USART2);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
}

static void usart2_idle_interrupt_init(void)
{
    USART_ClearFlag(USART2, USART_IT_IDLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    /*已在主函数中设置优先级分组。*/
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void usart2_rx_dma_init(void)
{
    DMA_DeInit(DMA1_Channel6);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    DMA_InitTypeDef DMA_InitStructure = {
        .DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR,
        .DMA_MemoryBaseAddr = (uint32_t)usart2_rx_buffer, 
        .DMA_DIR = DMA_DIR_PeripheralSRC,
        .DMA_BufferSize = sizeof(usart2_rx_buffer), 
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
        .DMA_MemoryInc = DMA_MemoryInc_Enable,
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
        .DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,
        .DMA_Mode = DMA_Mode_Normal,
        .DMA_Priority = DMA_Priority_High,
        .DMA_M2M = DMA_M2M_Disable,
    };
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);   //初始化DMA通道6
    DMA_Cmd(DMA1_Channel6, ENABLE);                //使能DMA通道6
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); //使能USART2的DMA接收请求
}
/* @brief 总初始化*/
static void usart2_init(unsigned int bound)
{
    usart2_periph_init(bound);                  //初始化串口外设
    usart2_rx_dma_init();           //初始化接收寄存器的DMA通道
    usart2_idle_interrupt_init();   //初始化串口的空闲中断 
}

static void usart3_periph_init(unsigned int bound)
{
    /*初始化发送接收引脚，PB10(USART3_TX)，PB11(USART3_RX)。*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*初始化USART3外设。*/
    USART_DeInit(USART3);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART3, ENABLE);
}

static void usart3_idle_interrupt_init(void)
{
    USART_ClearFlag(USART3, USART_IT_IDLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    /*已在主函数中设置优先级分组。*/
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void usart3_rx_dma_init(void)
{
    DMA_DeInit(DMA1_Channel3);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    DMA_InitTypeDef DMA_InitStructure = {
        .DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR,
        .DMA_MemoryBaseAddr = (uint32_t)usart3_rx_buffer, 
        .DMA_DIR = DMA_DIR_PeripheralSRC,
        .DMA_BufferSize = sizeof(usart3_rx_buffer), 
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
        .DMA_MemoryInc = DMA_MemoryInc_Enable,
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
        .DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,
        .DMA_Mode = DMA_Mode_Normal,
        .DMA_Priority = DMA_Priority_High,
        .DMA_M2M = DMA_M2M_Disable,
    };
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);   //初始化DMA通道3
    DMA_Cmd(DMA1_Channel3, ENABLE);                //使能DMA通道3
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); //使能USART3的DMA接收请求
}
/* @brief 总初始化*/
static void usart3_init(unsigned int bound)
{
    usart3_periph_init(bound);                  //初始化串口外设
    usart3_rx_dma_init();           //初始化接收寄存器的DMA通道
    usart3_idle_interrupt_init();   //初始化串口的空闲中断 
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
        case DRV_USART_ID_3:
            usart3_init(bound);
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
/* @brief 等待串口传输完成（等待最后一个字节真正发送完成）
 * @param id: 串口ID
 * @note 带超时机制，避免死循环
 * @note 超时时间：约200ms（基于72MHz系统时钟，每次循环约2个CPU周期）
 */
void drv_usart_wait_transmit_complete(TE_DRV_USART_ID id)
{
    if(id >= DRV_USART_ID_MAX)
    {
        return;
    }
    
    USART_TypeDef* usart = s_usart_config[id].usart_periph;
    if(usart != NULL)
    {
        uint32_t timeout = 0;
        
        // 等待发送缓冲区空（带超时，最多等待约200ms）
        // 72MHz系统时钟，每次循环约2个CPU周期，7200000次循环约200ms
        timeout = 0;
        while(USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET)
        {
            timeout++;
            if(timeout > 7200000)  // 超时约200ms，退出循环
            {
                break;
            }
        }
        
        // 等待传输完成（最后一个字节真正发送完成，带超时，最多等待约200ms）
        // 在115200波特率下，一个字节传输时间约87us，加上一些余量，200ms足够
        timeout = 0;
        while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET)
        {
            timeout++;
            if(timeout > 7200000)  // 超时约200ms，退出循环
            {
                break;
            }
        }
    }
}

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

void USART1_IRQHandler(void)
{
    if( USART_GetITStatus(USART1, USART_IT_IDLE) != RESET )
    {
        USART1 -> SR;  //访问一下SR寄存器
        USART1 -> DR;  //访问一下DR寄存器

        DMA_Cmd(DMA1_Channel5, DISABLE);    //禁用DMA通道5
        usart1_rx_len = USART1_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5); //获取接收到的数据长度
        DMA_SetCurrDataCounter(DMA1_Channel5, USART1_RX_BUF_SIZE); //设置DMA通道5的当前数据计数器
        DMA_Cmd(DMA1_Channel5, ENABLE);    //使能DMA通道5

        usart1_rx_buffer[usart1_rx_len] = '\0';          //给最后一位补上结束符，方便字符串处理
        
        // 调用回调函数处理接收到的数据（在中断中快速处理）
        if(s_usart_config[DRV_USART_ID_1].rx_callback != NULL)
        {
            s_usart_config[DRV_USART_ID_1].rx_callback(usart1_rx_buffer, usart1_rx_len);
        }
	
        usart1_rx_flag = true;                             //标记一帧数据已接收完成
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);         //清除IDLE中断标志位
    }
}

void USART2_IRQHandler(void)
{
    if( USART_GetITStatus(USART2, USART_IT_IDLE) != RESET )
    {
        USART2 -> SR;  //访问一下SR寄存器
        USART2 -> DR;  //访问一下DR寄存器

        DMA_Cmd(DMA1_Channel6, DISABLE);    //禁用DMA通道6
        usart2_rx_len = USART2_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6); //获取接收到的数据长度
        DMA_SetCurrDataCounter(DMA1_Channel6, USART2_RX_BUF_SIZE); //设置DMA通道6的当前数据计数器
        DMA_Cmd(DMA1_Channel6, ENABLE);    //使能DMA通道6

        usart2_rx_buffer[usart2_rx_len] = '\0';          //给最后一位补上结束符，方便字符串处理
        
        // 调用回调函数处理接收到的数据（在中断中快速处理）
        if(s_usart_config[DRV_USART_ID_2].rx_callback != NULL)
        {
            s_usart_config[DRV_USART_ID_2].rx_callback(usart2_rx_buffer, usart2_rx_len);
        }
	
        usart2_rx_flag = true;                             //标记一帧数据已接收完成
        USART_ClearITPendingBit(USART2, USART_IT_IDLE);         //清除IDLE中断标志位
    }
}

void USART3_IRQHandler(void)
{
    if( USART_GetITStatus(USART3, USART_IT_IDLE) != RESET )
    {
        USART3 -> SR;  //访问一下SR寄存器
        USART3 -> DR;  //访问一下DR寄存器

        DMA_Cmd(DMA1_Channel3, DISABLE);    //禁用DMA通道3
        usart3_rx_len = USART3_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3); //获取接收到的数据长度
        DMA_SetCurrDataCounter(DMA1_Channel3, USART3_RX_BUF_SIZE); //设置DMA通道3的当前数据计数器
        DMA_Cmd(DMA1_Channel3, ENABLE);    //使能DMA通道3

        usart3_rx_buffer[usart3_rx_len] = '\0';          //给最后一位补上结束符，方便字符串处理
        
        // 调用回调函数处理接收到的数据（在中断中快速处理）
        if(s_usart_config[DRV_USART_ID_3].rx_callback != NULL)
        {
            s_usart_config[DRV_USART_ID_3].rx_callback(usart3_rx_buffer, usart3_rx_len);
        }
	
        usart3_rx_flag = true;                             //标记一帧数据已接收完成
        USART_ClearITPendingBit(USART3, USART_IT_IDLE);         //清除IDLE中断标志位
    }
}
