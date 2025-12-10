#include "stm32f4xx.h"                  // Device header
#include "led.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程模板-库函数版本
//genbotter.com							  
////////////////////////////////////////////////////////////////////////////////// 	 

//初始化PA6和PA7为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

  //GPIOF9,F10初始化设置
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//LED0和LED1对应IO口
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
//	
//	GPIO_ResetBits(GPIOA,GPIO_Pin_6 | GPIO_Pin_7);//
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;//LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//
}

void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA ,GPIO_Pin_4);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

void LED1_Turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_4) == 0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	}
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOA ,GPIO_Pin_7);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
}

void LED2_Turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_7) == 0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	}
}


