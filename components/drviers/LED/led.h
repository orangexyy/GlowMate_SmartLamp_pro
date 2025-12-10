#ifndef __LED_H
#define __LED_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程模板-库函数版本
//genbotter.com									  
////////////////////////////////////////////////////////////////////////////////// 	
//LED端口定义
#define LED0 PAout(4)	// D2
#define LED1 PAout(7)	// D3	 

void LED_Init(void);//初始化	

void LED1_ON(void);
void LED1_OFF(void);
void LED1_Turn(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Turn(void);


#endif
