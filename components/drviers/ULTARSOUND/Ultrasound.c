#include "stm32f4xx.h"                  // Device header
#include "delay.h"
#include "timer.h"

#define Trig_Pin GPIO_Pin_0
#define Echo_Pin GPIO_Pin_1

uint32_t Time;
uint32_t Time_end;

void Ultrasound_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = Trig_Pin;				//TRIG
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = Echo_Pin;				//ECHO
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,Trig_Pin);
	delay_us(15);
}

uint16_t Ultrasound_mm(void)
{
	uint32_t Distance = 0;
	uint32_t Distance_mm = 0;
	
	GPIO_SetBits(GPIOA,Trig_Pin);
	delay_us(15);
	GPIO_ResetBits(GPIOA,Trig_Pin);
	while(GPIO_ReadInputDataBit(GPIOA,Echo_Pin) == 0);
	Time = 0;
	while(GPIO_ReadInputDataBit(GPIOA,Echo_Pin) == 1);
	Time_end = Time;
	
	if(Time_end/100<38)
	{
		Distance = (Time_end*346)/2;
		Distance_mm  = Distance/100;
	}
	return Distance_mm;
}

float  Ultrasound_m(void)
{
	uint32_t Distance = 0;
	uint32_t Distance_mm = 0;
	float Distance_m = 0;
	
	GPIO_SetBits(GPIOA,Trig_Pin);
	delay_us(20);
	GPIO_ResetBits(GPIOA,Trig_Pin);
	while(GPIO_ReadInputDataBit(GPIOA,Echo_Pin) == 0);
	Time = 0;
	while(GPIO_ReadInputDataBit(GPIOA,Echo_Pin) == 1);
	Time_end = Time;
	
	if(Time_end/100<38)					//判断echo响应是否超时   若无判断的话会陷入死循环
	{
		Distance = (Time_end*346)/2;
		Distance_mm  = Distance/100;
		Distance_m = Distance_mm/1000;
	}
	return Distance_m;
}
/*
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == 1)
		{
			Time++;
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}
*/


