/****************************************************************************\
**  文件名称 :  drv_adc.c
**  功能描述 :  ADC 驱动。电源检测、音量调节、亮度调节、光感 1/2 通道的初始化与采样读取。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include "stm32f10x.h"                  // Device header
#include "drv_adc.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
#define ADC1_CLOCK 						RCC_APB2Periph_ADC1

#define ADC_POWER_DETECT_CLOCK 			RCC_APB2Periph_GPIOA
#define ADC_POWER_DETECT_PORT 			GPIOA
#define ADC_POWER_DETECT_PIN 			GPIO_Pin_0
#define ADC_POWER_DETECT_CHANNEL 		ADC_Channel_0

#define ADC_VOLUME_ADJUST_CLOCK 		RCC_APB2Periph_GPIOA
#define ADC_VOLUME_ADJUST_PORT 			GPIOA
#define ADC_VOLUME_ADJUST_PIN 			GPIO_Pin_4
#define ADC_VOLUME_ADJUST_CHANNEL 		ADC_Channel_4

#define ADC_LIGHT_ADJUST_CLOCK 			RCC_APB2Periph_GPIOA
#define ADC_LIGHT_ADJUST_PORT 			GPIOA
#define ADC_LIGHT_ADJUST_PIN 			GPIO_Pin_5
#define ADC_LIGHT_ADJUST_CHANNEL 		ADC_Channel_5

#define ADC_LIGHT_SENSOR_1_CLOCK 		RCC_APB2Periph_GPIOB
#define ADC_LIGHT_SENSOR_1_PORT 		GPIOB
#define ADC_LIGHT_SENSOR_1_PIN 			GPIO_Pin_0
#define ADC_LIGHT_SENSOR_1_CHANNEL 		ADC_Channel_8

#define ADC_LIGHT_SENSOR_2_CLOCK 		RCC_APB2Periph_GPIOB
#define ADC_LIGHT_SENSOR_2_PORT 		GPIOB
#define ADC_LIGHT_SENSOR_2_PIN 			GPIO_Pin_1
#define ADC_LIGHT_SENSOR_2_CHANNEL 		ADC_Channel_9
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
 * \brief ADC 驱动初始化：使能 ADC1 与各通道 GPIO，配置通道并启动转换
 */
void drv_adc_init(void)
{
	RCC_APB2PeriphClockCmd(ADC1_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(ADC_POWER_DETECT_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(ADC_VOLUME_ADJUST_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(ADC_LIGHT_ADJUST_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(ADC_LIGHT_SENSOR_1_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(ADC_LIGHT_SENSOR_2_CLOCK, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = ADC_POWER_DETECT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC_POWER_DETECT_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = ADC_VOLUME_ADJUST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC_VOLUME_ADJUST_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = ADC_LIGHT_ADJUST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC_LIGHT_ADJUST_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = ADC_LIGHT_SENSOR_1_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC_LIGHT_SENSOR_1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = ADC_LIGHT_SENSOR_2_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC_LIGHT_SENSOR_2_PORT, &GPIO_InitStructure);
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
}

uint16_t drv_adc_get_value(uint8_t id)
{
	switch (id)
	{
		case ADC_ID_POWER_DETECT:
			ADC_RegularChannelConfig(ADC1, ADC_POWER_DETECT_CHANNEL, 1, ADC_SampleTime_55Cycles5);
			break;
		case ADC_ID_VOLUME_ADJUST:
			ADC_RegularChannelConfig(ADC1, ADC_VOLUME_ADJUST_CHANNEL, 1, ADC_SampleTime_55Cycles5);
			break;
		case ADC_ID_LIGHT_ADJUST:
			ADC_RegularChannelConfig(ADC1, ADC_LIGHT_ADJUST_CHANNEL, 1, ADC_SampleTime_55Cycles5);
			break;
		case ADC_ID_LIGHT_SENSOR_1:
			ADC_RegularChannelConfig(ADC1, ADC_LIGHT_SENSOR_1_CHANNEL, 1, ADC_SampleTime_55Cycles5);
			break;
		case ADC_ID_LIGHT_SENSOR_2:
			ADC_RegularChannelConfig(ADC1, ADC_LIGHT_SENSOR_2_CHANNEL, 1, ADC_SampleTime_55Cycles5);
			break;
		default:
			break;
	}
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC1);
}

