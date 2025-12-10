#include "stm32f4xx.h"                  // Device header

/*
	PWM频率： 	Freq = CK_PSC/(PSC+1)/(ARR+1)
	PWM占空比：	Duty = CCR/(ARR+1)
	PWM分辨率：	Reso = 1/(ARR+1)
*/



void PWM_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1); //复用GPIOA_Pin9为TIM1_Ch1, 
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);//复用GPIOA_Pin11为TIM1_Ch2,
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);//复用GPIOA_Pin13为TIM1_Ch3,
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);//复用GPIOA_Pin14为TIM1_Ch4,

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);	//复用GPIOA_Pin6为TIM8_Ch1, 
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8); //复用GPIOA_Pin7为TIM8_Ch2,
//	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8); //复用GPIOB_Pin0为TIM8_Ch3,
//	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8); //复用GPIOB_Pin1为TIM8_Ch4,
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);

	TIM_InternalClockConfig(TIM8);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 16800 - 1;         //ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;        //PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM8, TIM_FLAG_Update);
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;                     //CCR 
	
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable); 
	TIM_OC1Init(TIM8, &TIM_OCInitStructure); //通道1
		
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable); 
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);//通道2
		
//	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable); 
//	TIM_OC3Init(TIM8, &TIM_OCInitStructure);//通道3
//	
//	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable); 
//	TIM_OC4Init(TIM8, &TIM_OCInitStructure);//通道4

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM1在CCR1上的预装载寄存器,CCR自动装载默认也是打开的
	
	TIM_ARRPreloadConfig(TIM8,ENABLE);//ARPE使能 
	TIM_Cmd(TIM8, ENABLE);  //使能TIM1
	
	TIM_CtrlPWMOutputs(TIM8, ENABLE);//使能TIM1的PWM输出，TIM1与TIM8有效,如果没有这行会问题

	

}

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM8,Compare);
}

void PWM_SetCompare2(uint16_t Compare)
{
	TIM_SetCompare2(TIM8,Compare);
}

void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM8,Compare);
}

void PWM_SetCompare4(uint16_t Compare)
{
	TIM_SetCompare4(TIM8,Compare);
}

