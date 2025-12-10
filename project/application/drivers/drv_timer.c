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
\******************************************************************************/
#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>
#include "drv_timer.h" 	
#include "common.h" 	
/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/
// 定时器配置句柄
typedef struct
{
    TIM_TypeDef*              	tim_periph;       		// TIM外设
	uint32_t					tim_clock;         		// TIM外设时钟
    IRQn_Type         			nvic_irq;           	// 定时器中断
	DRV_TIMER_CALLBACK   		irq_callback;       	// 中断回调函数
} TS_DRV_TIMER_CONFIG;
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
// 定时器配置数组
static TS_DRV_TIMER_CONFIG s_timer_config[DRV_TIMER_ID_MAX] = {
    {TIM1, RCC_APB2Periph_TIM1, TIM1_UP_TIM10_IRQn, NULL},
    {TIM2, RCC_APB1Periph_TIM2, TIM2_IRQn, NULL},
    {TIM3, RCC_APB1Periph_TIM3, TIM3_IRQn, NULL},
    {TIM4, RCC_APB1Periph_TIM4, TIM4_IRQn, NULL},
    {TIM5, RCC_APB1Periph_TIM5, TIM5_IRQn, NULL},
    {TIM6, RCC_APB1Periph_TIM6, TIM6_DAC_IRQn, NULL},
    {TIM7, RCC_APB1Periph_TIM7, TIM7_IRQn, NULL},
};

static bool s_is_timer_enable[DRV_TIMER_ID_MAX] = {false};
/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/*
* 函数名称 : drv_timer_init
* 功能描述 : 初始化定时器
* 参    数 : p_this - 定时器句柄
* 返回值   : 无
* 示    例 : drv_timer_init(p_this, irq_callback);
*/
void drv_timer_init(TS_TIMER_HANDLE *p_this, DRV_TIMER_CALLBACK irq_callback)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(s_timer_config[p_this->id].tim_clock, ENABLE);

	TIM_TimeBaseInitStructure.TIM_ClockDivision	= TIM_CKD_DIV1; 
	TIM_TimeBaseInitStructure.TIM_Prescaler		= p_this->prescaler;
	TIM_TimeBaseInitStructure.TIM_Period 		= p_this->Period; 
	TIM_TimeBaseInitStructure.TIM_CounterMode	= TIM_CounterMode_Up; 
	TIM_TimeBaseInit(s_timer_config[p_this->id].tim_periph, &TIM_TimeBaseInitStructure);

	TIM_ITConfig(s_timer_config[p_this->id].tim_periph, TIM_IT_Update, ENABLE); 
	TIM_Cmd(s_timer_config[p_this->id].tim_periph, ENABLE); 
	NVIC_InitStructure.NVIC_IRQChannel						= s_timer_config[p_this->id].nvic_irq; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	s_timer_config[p_this->id].irq_callback = irq_callback;
}

/*
* 函数名称 : drv_timer_enable
* 功能描述 : 启动定时器
* 参    数 : id - 定时器ID
* 返回值   : int：返回错误代码
* 示    例 : drv_timer_enable(id);	
*/
int drv_timer_enable(TE_DRV_TIMER_ID id)
{
    if (id >= DRV_TIMER_ID_MAX)
    {
        return ERROR_INVALID_PARAM;
    }

    TIM_Cmd(s_timer_config[id].tim_periph, ENABLE);
    s_is_timer_enable[id] = true;

    return SUCCESS;
}


/*
* 函数名称 : drv_timer_disable
* 功能描述 : 关闭定时器
* 参    数 : id - 定时器ID
* 返回值   : int：返回错误代码
* 示    例 : drv_timer_disable(id);	
*/
int drv_timer_disable(TE_DRV_TIMER_ID id)
{
    
    if (id >= DRV_TIMER_ID_MAX)
    {
        return ERROR_INVALID_PARAM;
    }
    
    TIM_Cmd(s_timer_config[id].tim_periph, DISABLE);
    s_is_timer_enable[id] = false;

    return SUCCESS;
}


/*
* 函数名称 : drv_timer_is_busy
* 功能描述 : 获取定时器忙状态
* 参    数 : id - 定时器ID
* 返回值   : 忙状态
* 示    例 : bool result = drv_timer_is_busy(id);
*/
bool drv_timer_is_busy(TE_DRV_TIMER_ID id)
{
    return s_is_timer_enable[id];
}


//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)==SET) //溢出中断
	{
		if(s_timer_config[DRV_TIMER_ID_4].irq_callback != NULL)
		{
			s_timer_config[DRV_TIMER_ID_4].irq_callback();
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志位
}











