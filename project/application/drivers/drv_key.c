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
#include "system.h"
#include "drv_key.h"
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
int drv_key_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	return 0;
}

/**
 * \brief 按键初始化 
 * \param id 按键id
 * \return 按键状态
 */
 uint8_t drv_key_read(uint8_t id)
 {
	 uint8_t state = 0;
	 switch (id)
	 {
		 case MODE_KEY:
			 state = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3);
			 break; 
		 case POWER_KEY:
			 state = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4);
			 break;
		 default:
			 return 0;
	 }
	 return (state == 0); // 按下为低电平
 }





