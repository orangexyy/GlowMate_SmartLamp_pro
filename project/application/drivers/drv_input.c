/****************************************************************************\
**  文件名称 :  drv_input.c
**  功能描述 :  输入驱动。挥手检测、红外 1/2 通道 GPIO 配置与电平读取。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "stm32f10x.h"                  // Device header
#include "drv_input.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
#define MW_DETECT_CLOCK 		RCC_APB2Periph_GPIOB
#define MW_DETECT_PORT 		    GPIOB
#define MW_DETECT_PIN 		    GPIO_Pin_12

#define IR_DETECT_1_CLOCK 		RCC_APB2Periph_GPIOA
#define IR_DETECT_1_PORT 		GPIOA
#define IR_DETECT_1_PIN 		GPIO_Pin_6

#define IR_DETECT_2_CLOCK 		RCC_APB2Periph_GPIOA
#define IR_DETECT_2_PORT 		GPIOA
#define IR_DETECT_2_PIN 		GPIO_Pin_7
/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/

/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/**
 * \brief 输入驱动初始化：配置挥手检测与两路红外 GPIO 为浮空输入
 */
void drv_input_init(void)
{
    RCC_APB2PeriphClockCmd(MW_DETECT_CLOCK, ENABLE);
    RCC_APB2PeriphClockCmd(IR_DETECT_1_CLOCK, ENABLE);
    RCC_APB2PeriphClockCmd(IR_DETECT_2_CLOCK, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =   MW_DETECT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MW_DETECT_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =  IR_DETECT_1_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IR_DETECT_1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =  IR_DETECT_2_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IR_DETECT_2_PORT, &GPIO_InitStructure);


}

uint8_t drv_input_get_value(uint8_t id)
{
	uint8_t state = 0;
    switch (id)
    {
        case INPUT_ID_IR_DETECT_1:
            state = GPIO_ReadInputDataBit(IR_DETECT_1_PORT, IR_DETECT_1_PIN);
            break;
        case INPUT_ID_IR_DETECT_2:
            state = GPIO_ReadInputDataBit(IR_DETECT_2_PORT, IR_DETECT_2_PIN);
            break;
        case INPUT_ID_MW_DETECT:
            state = GPIO_ReadInputDataBit(MW_DETECT_PORT, MW_DETECT_PIN);
            break;
        default:
            break;  
    }
    return state;
}
