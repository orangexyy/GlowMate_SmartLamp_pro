/****************************************************************************\
**  文件名称 :  drv_output.c
**  功能描述 :  Bootloader 输出驱动。GPIO 控制 LED/状态灯等，与 application 实现类似。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include "stm32f10x.h"                  // Device header
#include "drv_output.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
#define OUTPUT_LED_PORT_CLOCK 	            RCC_APB2Periph_GPIOC
#define OUTPUT_LED_PORT 		            GPIOC
#define OUTPUT_LED_PIN 			            GPIO_Pin_13

#define OUTPUT_STATE_LED_R_PORT_CLOCK 	    RCC_APB2Periph_GPIOA
#define OUTPUT_STATE_LED_R_PORT 		    GPIOA
#define OUTPUT_STATE_LED_R_PIN 			    GPIO_Pin_15

#define OUTPUT_STATE_LED_G_PORT_CLOCK 	    RCC_APB2Periph_GPIOB
#define OUTPUT_STATE_LED_G_PORT 		    GPIOB
#define OUTPUT_STATE_LED_G_PIN 			    GPIO_Pin_3

#define OUTPUT_STATE_LED_B_PORT_CLOCK 	    RCC_APB2Periph_GPIOB
#define OUTPUT_STATE_LED_B_PORT 		    GPIOB
#define OUTPUT_STATE_LED_B_PIN 			    GPIO_Pin_4

#define OUTPUT_MODE_LED_R_PORT_CLOCK 	    RCC_APB2Periph_GPIOB
#define OUTPUT_MODE_LED_R_PORT 		        GPIOB
#define OUTPUT_MODE_LED_R_PIN 			    GPIO_Pin_5

#define OUTPUT_MODE_LED_G_PORT_CLOCK 	    RCC_APB2Periph_GPIOB
#define OUTPUT_MODE_LED_G_PORT 		        GPIOB
#define OUTPUT_MODE_LED_G_PIN 			    GPIO_Pin_6

#define OUTPUT_MODE_LED_B_PORT_CLOCK 	    RCC_APB2Periph_GPIOB
#define OUTPUT_MODE_LED_B_PORT 		        GPIOB
#define OUTPUT_MODE_LED_B_PIN 			    GPIO_Pin_7
/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/

/******************************************************************************\
                             Functions definitions
\******************************************************************************/

void drv_output_init(void)
{
    // 释放JTAG引脚PB3/PB4
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	RCC_APB2PeriphClockCmd (OUTPUT_LED_PORT_CLOCK,ENABLE);
	RCC_APB2PeriphClockCmd (OUTPUT_STATE_LED_R_PORT_CLOCK,ENABLE);
	RCC_APB2PeriphClockCmd (OUTPUT_STATE_LED_G_PORT_CLOCK,ENABLE);
	RCC_APB2PeriphClockCmd (OUTPUT_STATE_LED_B_PORT_CLOCK,ENABLE);
	RCC_APB2PeriphClockCmd (OUTPUT_MODE_LED_R_PORT_CLOCK,ENABLE);
	RCC_APB2PeriphClockCmd (OUTPUT_MODE_LED_G_PORT_CLOCK,ENABLE);
	RCC_APB2PeriphClockCmd (OUTPUT_MODE_LED_B_PORT_CLOCK,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Pin  = OUTPUT_STATE_LED_R_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OUTPUT_STATE_LED_R_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = OUTPUT_STATE_LED_G_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OUTPUT_STATE_LED_G_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = OUTPUT_STATE_LED_B_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OUTPUT_STATE_LED_B_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = OUTPUT_MODE_LED_R_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OUTPUT_MODE_LED_R_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = OUTPUT_MODE_LED_G_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OUTPUT_MODE_LED_G_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = OUTPUT_MODE_LED_B_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OUTPUT_MODE_LED_B_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = OUTPUT_LED_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OUTPUT_LED_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(OUTPUT_STATE_LED_R_PORT,OUTPUT_STATE_LED_R_PIN);
	GPIO_ResetBits(OUTPUT_STATE_LED_G_PORT,OUTPUT_STATE_LED_G_PIN);
	GPIO_ResetBits(OUTPUT_STATE_LED_B_PORT,OUTPUT_STATE_LED_B_PIN);
	GPIO_ResetBits(OUTPUT_MODE_LED_R_PORT,OUTPUT_MODE_LED_R_PIN);
	GPIO_ResetBits(OUTPUT_MODE_LED_G_PORT,OUTPUT_MODE_LED_G_PIN);
	GPIO_ResetBits(OUTPUT_MODE_LED_B_PORT,OUTPUT_MODE_LED_B_PIN);
	GPIO_ResetBits(OUTPUT_LED_PORT,OUTPUT_LED_PIN);
}

void drv_output_set_value(uint8_t id, uint8_t value)
{
    switch (id)
    {
        case OUTPUT_ID_LED:
            GPIO_WriteBit(OUTPUT_LED_PORT, OUTPUT_LED_PIN, value ? Bit_SET : Bit_RESET);
            break;
        case OUTPUT_ID_STATE_LED_R:
            GPIO_WriteBit(OUTPUT_STATE_LED_R_PORT, OUTPUT_STATE_LED_R_PIN, value ? Bit_SET : Bit_RESET);
            break;
        case OUTPUT_ID_STATE_LED_G:
            GPIO_WriteBit(OUTPUT_STATE_LED_G_PORT, OUTPUT_STATE_LED_G_PIN, value ? Bit_SET : Bit_RESET);
            break;
        case OUTPUT_ID_STATE_LED_B:
            GPIO_WriteBit(OUTPUT_STATE_LED_B_PORT, OUTPUT_STATE_LED_B_PIN, value ? Bit_SET : Bit_RESET);
            break;
        case OUTPUT_ID_MODE_LED_R:
            GPIO_WriteBit(OUTPUT_MODE_LED_R_PORT, OUTPUT_MODE_LED_R_PIN, value ? Bit_SET : Bit_RESET);
            break;
        case OUTPUT_ID_MODE_LED_G:
            GPIO_WriteBit(OUTPUT_MODE_LED_G_PORT, OUTPUT_MODE_LED_G_PIN, value ? Bit_SET : Bit_RESET);
            break;
        case OUTPUT_ID_MODE_LED_B:
            GPIO_WriteBit(OUTPUT_MODE_LED_B_PORT, OUTPUT_MODE_LED_B_PIN, value ? Bit_SET : Bit_RESET);
            break;
        default:
            break;
    }
}

