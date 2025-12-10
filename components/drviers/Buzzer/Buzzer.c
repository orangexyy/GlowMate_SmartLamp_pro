#include "stm32f4xx.h"                  // Device header
#include "buzzer.h" 

void Buzzer_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA时钟

  //GPIOF9,F10初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_SetBits(GPIOE,GPIO_Pin_0 );//设置高，灯灭
}

void Buzzer_ON(void)
{
	GPIO_ResetBits(GPIOE ,GPIO_Pin_0);
}

void Buzzer_OFF(void)
{
	GPIO_SetBits(GPIOE,GPIO_Pin_0);
}




