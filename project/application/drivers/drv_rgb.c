/****************************************************************************\
**  文件名称 :  drv_rgb.c
**  功能描述 :  RGB 灯带驱动。TIM2+PWM+DMA 产生 WS2812B 时序，刷新灯珠颜色数据。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/****************************************************************************\
                            Includes
\****************************************************************************/
#include "stm32f10x.h"
#include "string.h"
#include "drv_rgb.h"
#include "system.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
// 灯珠数量配置
#define DRV_RGB_LED_NUM         8                  // 总灯珠数量
#define DRV_RGB_DATA_SIZE       24                 // 每个灯珠24位
#define DRV_RGB_0_CODE          30        // 0码：高电平≈0.416μs（30/90*1.25μs，符合WS2812B 0码时序）
#define DRV_RGB_1_CODE          60        // 1码：高电平≈0.833μs（60/90*1.25μs，符合WS2812B 1码时序）
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
uint16_t drv_rgb_value[DRV_RGB_DATA_SIZE * DRV_RGB_LED_NUM];    // 灯珠数据：drv_rgb_value
/****************************************************************************\
                            Functions definitions
\****************************************************************************/
/**
 * \brief RGB 驱动初始化：配置 TIM2 PWM、DMA 及 GPIO，用于 WS2812B 数据输出
 */
void drv_rgb_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_InternalClockConfig(TIM2);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 90 - 1;        
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;      
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    /*配置TIM2输出比较通道一*/
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 60;        
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_DMACmd(TIM2, TIM_DMA_CC2, ENABLE);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);

    /*配置DMA1*/
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)&(TIM2->CCR2);              //输出比较寄存器：TIM2->CCR2
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_HalfWord;      //外设数据宽度16bits
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;            //外设地址不自增
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)drv_rgb_value;              //灯珠数据：drv_rgb_value
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_HalfWord;          //存储器数据宽度16bits
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;                 //存储器地址自增
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralDST;                //传输方向：存储器→外设
    DMA_InitStructure.DMA_BufferSize            = DRV_RGB_DATA_SIZE;                    //初始值无意义，后续动态设置  
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;                      //非循环模式
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;                      //硬件触发（TIM2）
    DMA_InitStructure.DMA_Priority              = DMA_Priority_Medium;                  //优先级中等
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);                                         //初始化DMA1通道1

    DMA_Cmd(DMA1_Channel7, DISABLE);
    TIM_Cmd(TIM2, DISABLE);

    DMA_SetCurrDataCounter(DMA1_Channel7, (DRV_RGB_DATA_SIZE * DRV_RGB_LED_NUM));
    drv_rgb_clear();
    drv_rgb_rest();
    DMA_Cmd(DMA1_Channel7, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    while(DMA_GetFlagStatus(DMA1_FLAG_TC7) != SET);
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC7);
    TIM_Cmd(TIM2, DISABLE);
    drv_rgb_rest();

    DMA_SetCurrDataCounter(DMA1_Channel7, DRV_RGB_DATA_SIZE);
    drv_rgb_clear();
    drv_rgb_rest();
    DMA_Cmd(DMA1_Channel7,ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    while(DMA_GetFlagStatus(DMA1_FLAG_TC7) != SET);
    DMA_Cmd(DMA1_Channel7,DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC7);
    TIM_Cmd(TIM2, DISABLE);
    drv_rgb_rest();
}

void drv_rgb_rest(void)
{
    TIM_Cmd(TIM2,DISABLE);
    GPIO_ResetBits(GPIOA,GPIO_Pin_0);
    delay_ms(1);
}

void drv_rgb_clear(void)
{
    uint16_t i=0;
    for(i=0;i < (DRV_RGB_DATA_SIZE * DRV_RGB_LED_NUM); i++)
    {
        drv_rgb_value[i] = DRV_RGB_0_CODE;
    }
}

void drv_rgb_show(uint8_t num)
{
    drv_rgb_rest();
    DMA_SetCurrDataCounter(DMA1_Channel7, DRV_RGB_DATA_SIZE * (num+1));
    DMA_Cmd(DMA1_Channel7, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    while (DMA_GetFlagStatus(DMA1_FLAG_TC7) != SET);
    DMA_ClearFlag(DMA1_FLAG_TC7);
    DMA_Cmd(DMA1_Channel7, DISABLE);
    TIM_Cmd(TIM2, DISABLE);
    drv_rgb_rest();
}

