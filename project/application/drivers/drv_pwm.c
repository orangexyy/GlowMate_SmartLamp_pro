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
#include <cstdint>
#include "drv_pwm.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/

/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
	PWM频率： 	Freq = CK_PSC/(PSC+1)/(ARR+1)
	PWM占空比：	Duty = CCR/(ARR+1)
	PWM分辨率：	Reso = 1/(ARR+1)
*/

// void drv_pwm_init(void)
// {
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
// 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

// 	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);	//复用GPIOA_Pin6为TIM8_Ch1, 
// 	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8); //复用GPIOA_Pin7为TIM8_Ch2,

// 	GPIO_InitTypeDef GPIO_InitStructure;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
// 	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
// 	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
// 	GPIO_Init(GPIOC, &GPIO_InitStructure);

// 	TIM_InternalClockConfig(TIM8);
	
// 	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
// 	TIM_TimeBaseInitStructure.TIM_ClockDivision 	= TIM_CKD_DIV1;       
// 	TIM_TimeBaseInitStructure.TIM_CounterMode 		= TIM_CounterMode_Up;
// 	TIM_TimeBaseInitStructure.TIM_Period 			= 16800 - 1;         	//ARR
// 	TIM_TimeBaseInitStructure.TIM_Prescaler 		= 1 - 1;        	//PSC
// 	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
// 	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStructure);
	
// 	TIM_ClearFlag(TIM8, TIM_FLAG_Update);
// 	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE);
	
// 	TIM_OCInitTypeDef TIM_OCInitStructure;
// 	TIM_OCStructInit(&TIM_OCInitStructure);
// 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
// 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
// 	TIM_OCInitStructure.TIM_Pulse = 0;
	
// 	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	
// 	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable); 
// 	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
		
// 	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable); 
// 	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	
// 	TIM_ARRPreloadConfig(TIM8,ENABLE);
// 	TIM_Cmd(TIM8, ENABLE);
	
// 	TIM_CtrlPWMOutputs(TIM8, ENABLE);	//使能TIM1的PWM输出，TIM1与TIM8有效,如果没有这行会问题
// }

void drv_pwm_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  // 使能TIM3时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能GPIOA时钟

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         // 复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       // 推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         // 上拉（增加稳定性）
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   // 高速
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3); // PA6 → TIM3_CH1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3); // PA7 → TIM3_CH2

	TIM_InternalClockConfig(TIM3); // 使用内部时钟
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;       // 时钟分频系数=1
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // 向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 16799;                     // AR=16799（关键参数）
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;                      // PSC=0（关键参数）
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;              // 重复计数器禁用
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

	// 禁用更新中断（生成PWM无需更新中断，避免占用资源）
	// TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	// TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure); 						
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                  // PWM1模式（计数器< Pulse时输出高电平）
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;          // 输出极性：高电平有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;      // 使能输出
	TIM_OCInitStructure.TIM_Pulse = 0;                                 // 初始占空比0%（Pulse=0）
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;       // 空闲状态复位

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); 
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); 					
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	
}

void drv_pwm_set_duty(uint8_t id, uint16_t duty)
{
	if (duty <= 0) duty = 0;
	if (duty > 100) duty = 100;

	switch (id)
	{
		case 0:
			TIM_SetCompare1(TIM3, (duty * (16799 + 1) / 100));
			break;
		case 1:
			TIM_SetCompare2(TIM3, (duty * (16799 + 1) / 100));
			break;
		default:
			break;	
	}

}



