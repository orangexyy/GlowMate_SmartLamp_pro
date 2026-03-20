/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  drv_key.c
**  功能描述 :  按键驱动。模式/设定/播放/上/下键 GPIO 配置与电平读取。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "stm32f10x.h"                  // Device header
#include "drv_key.h"
#include "common.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
#define KEY_MODE_CLOCK          RCC_APB2Periph_GPIOA 
#define KEY_MODE_PORT           GPIOA
#define KEY_MODE_PIN            GPIO_Pin_11

#define KEY_SETTING_CLOCK       RCC_APB2Periph_GPIOA 
#define KEY_SETTING_PORT        GPIOA
#define KEY_SETTING_PIN         GPIO_Pin_12

#define KEY_PLAY_CLOCK          RCC_APB2Periph_GPIOB 
#define KEY_PLAY_PORT           GPIOB
#define KEY_PLAY_PIN            GPIO_Pin_13

#define KEY_UP_CLOCK            RCC_APB2Periph_GPIOB 
#define KEY_UP_PORT             GPIOB
#define KEY_UP_PIN              GPIO_Pin_14

#define KEY_DOWN_CLOCK          RCC_APB2Periph_GPIOB 
#define KEY_DOWN_PORT           GPIOB
#define KEY_DOWN_PIN            GPIO_Pin_15
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
 * \brief 按键初始化 
 * \return 错误代码 \ref mk_common.h    
 */
int drv_key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(KEY_MODE_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin =  KEY_MODE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_MODE_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(KEY_SETTING_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_SETTING_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_SETTING_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(KEY_PLAY_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_PLAY_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PLAY_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(KEY_UP_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_UP_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_UP_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(KEY_DOWN_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_DOWN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_DOWN_PORT, &GPIO_InitStructure);
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
            state = !GPIO_ReadInputDataBit(KEY_MODE_PORT, KEY_MODE_PIN);
            break; 
        case SETTING_KEY:
            state = !GPIO_ReadInputDataBit(KEY_SETTING_PORT, KEY_SETTING_PIN);
            break;
        case PLAY_KEY:
            state = !GPIO_ReadInputDataBit(KEY_PLAY_PORT, KEY_PLAY_PIN);
            break;
        case UP_KEY:
            state = !GPIO_ReadInputDataBit(KEY_UP_PORT, KEY_UP_PIN);
            break;
        case DOWN_KEY:
            state = !GPIO_ReadInputDataBit(KEY_DOWN_PORT, KEY_DOWN_PIN);
            break;
        default:
            return 0;
    }
    return (state == 0); // 按下为低电平
}

/******************************* End of File (C) ******************************/
