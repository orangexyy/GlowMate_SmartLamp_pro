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
#include <stm32f4xx.h>
#include "drv_rgb.h"

/******************************************************************************\
                            Macro definitions
\******************************************************************************/
#define RGB_CODE_1           130          // 1码的PWM比较值
#define RGB_CODE_0           60          // 0码的PWM比较值
#define RGB_DATA_BITS        32          // RGB数据位数
#define RGB_REPEAT_COUNT     32          // RGB数据重复发送次数
#define RGB_BUFFER_SIZE      1024        // LED缓冲区大小 (32*32=1024)
/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
uint16_t rgb_data_buffer[RGB_BUFFER_SIZE] = {0};  // LED数据缓冲区
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
void drv_rgb_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);      /*使能GPIOE时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);       /*使能TIM1时钟*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);                        /*使能DMA2时钟*/
  
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);   /*将GPIOA8重映射到TIM1的输出通道上*/
      
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;               /*将引脚功能配置为复用*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;               /*推挽输出*/
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;                 /*上拉*/
    GPIO_Init(GPIOA, &GPIO_InitStructure);
     
    /* 定时器周期 : T =(arr + 1) * (PSC + 1) / Tck.   arr:周期值 PSC:预分频值  Tck: 系统时钟频率 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 210 - 1;                     /* T = (TIM_Period + 1)*(0+1)/168M  = 800kHz*/
    TIM_TimeBaseStructure.TIM_Prescaler = 0;                        /* 0：不预分频 */
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;         /* 0：时钟分频因子设为1，不分频*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;     /*向上计数*/
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                /*重复计数值为0，不使用重复计数*/
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                /*PWM1模式，计数值小于比较值输出高电平，大于输出低电平*/
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    /*使能输出通道*/
    TIM_OCInitStructure.TIM_Pulse = 0;                               /*设置占空比为0， 1 ~ TIM_TimeBaseStructure.TIM_Period */
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;        /*设置输出极性高电平有效*/
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; /*禁用互补输出通道*/
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;      /*设置互补输出通道极性高电平有效*/
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCNIdleState_Set;      /*设置空闲时输出高电平*/
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;   /*设置互补通道空闲时输出低电平*/ 
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);                         /*将上述配置应用到定时器一的通道三上*/
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);                /*使能通道三的输出比较寄存器的预装载功能*/
  
    TIM_Cmd(TIM1, ENABLE);                                           /*使能TIM1定时器*/
    TIM_CtrlPWMOutputs(TIM1,ENABLE);                                 /*使能TIM1的PWM输出*/
    
    DMA_DeInit(DMA2_Stream6);                                                   /*对DMA2_Stream6进行默认值初始化*/
    /* DMA2 Stream6 Config for PWM1 by TIM1_CH1*/
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;                              /*配置DMA通道，通道0*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(TIM1->CCR1);         /*配置外设基地址：TIM1的CCR3寄存器*/
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rgb_data_buffer;          /*配置源数据的基地址*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                     /*设置搬运方向：存储器到外设*/
    DMA_InitStructure.DMA_BufferSize = RGB_BUFFER_SIZE;                                      /*设置传输数据的大小*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /*外设基地址不自增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /*源数据基地址自增*/
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; /*设置外设数据大小为半字(16bit)*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         /*设置存储器数据大小为半字(16bit)*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               /*设置DMA为普通模式*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         /*DMA优先级为高*/
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                      /*禁用DMA的FIFO模式*/
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;               /*设置DMA的FIFO阈值为满*/
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                 /*设置内存突发传输模式为单次传输*/
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         /*设置外设突发传输模式为单次传输*/
  
    DMA_Init(DMA2_Stream6, &DMA_InitStructure);                                 /*将上述配置应用到 DMA2_Stream6*/

    TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);                                      /*将TIM1的DMA请求映射到通道三并使能*/
}

/* @brief 将RGB数据编码到LED缓冲区
 * @param rgb_data: RGB颜色数据（32位）
 * @note 将32位RGB数据重复编码32次，每次编码32位，总共1024位
 */
 static void rgb_data_encode(uint32_t rgb_data)
 {
     uint16_t buffer_idx = 0;
     uint16_t repeat_idx, bit_idx;
     uint32_t bit_mask;
 
     // 重复发送32次相同的RGB数据
     for(repeat_idx = 0; repeat_idx < RGB_REPEAT_COUNT; repeat_idx++)
     {
         // 从最高位到最低位依次检查每一位
         bit_mask = 0x80000000;
         for(bit_idx = 0; bit_idx < RGB_DATA_BITS; bit_idx++)
         {
             // 检查当前位是否为1
             if(rgb_data & bit_mask)
             {
                 rgb_data_buffer[buffer_idx] = RGB_CODE_1;
             }
             else
             {
                 rgb_data_buffer[buffer_idx] = RGB_CODE_0;
             }
             buffer_idx++;
             bit_mask >>= 1;  // 右移检查下一位
         }
     }
 }
 
/* @brief 通过DMA发送LED缓冲区数据
* @note 启动DMA传输并等待传输完成
*/
static void rgb_dma_send(void)
{
    DMA_SetCurrDataCounter(DMA2_Stream6, RGB_BUFFER_SIZE);         /*指定要传输的数据量*/
    TIM_Cmd(TIM1, ENABLE);                                    /*启用TIM1定时器*/
    TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);                    /*将TIM1的DMA请求映射到通道三并使能*/
    DMA_Cmd(DMA2_Stream6, ENABLE);                            /*启用DMA2_Stream6，开始数据传输*/
    while(!DMA_GetFlagStatus(DMA2_Stream6, DMA_FLAG_TCIF6));  /*等待DMA传输完成，使用循环检查DMA传输完成标志*/
    DMA_Cmd(DMA2_Stream6, DISABLE);                           /*禁用DMA2流6，停止数据传输*/
    DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6);              /*清除DMA传输完成标志*/
    TIM_Cmd(TIM1, DISABLE); 
}
    
/* @brief 发送RGB颜色数据
* @param rgb_data: RGB颜色数据（32位，格式：0xRRGGBBWW等）
* @note 将RGB数据编码为PWM波形并通过DMA发送
*/
void drv_rgb_send(uint32_t rgb_data)
{
    // 将RGB数据编码到缓冲区
    rgb_data_encode(rgb_data);
    
    // 通过DMA发送数据
    rgb_dma_send();
}












