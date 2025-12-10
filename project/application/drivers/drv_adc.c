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
#include <stdio.h>
#include "stm32f4xx.h"
#include "drv_adc.h"
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

void drv_adc_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	 

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; // APB2=84MHz，ADC时钟=21MHz（≤36MHz，合规）
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_Cmd(ADC1, ENABLE);
}

uint16_t drv_adc_get_value(uint8_t id)
{
	uint8_t adc_channel = 0;
	uint16_t adc_value = 0;
	switch (id) {
		case DRV_ADC_ID_0:
			adc_channel = ADC_Channel_0;
			break;
		case DRV_ADC_ID_1:
			adc_channel = ADC_Channel_1;
			break;
		default:
			return 0;
	}
	
	ADC_RegularChannelConfig(ADC1, adc_channel, 1, ADC_SampleTime_480Cycles);			    
	ADC_SoftwareStartConv(ADC1);
	
	uint32_t timeout = 0xFFFF;
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) && timeout--);
	if(timeout == 0) return 0;
	
	adc_value = ADC_GetConversionValue(ADC1) & 0x0FFF; 
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	return adc_value;
}
