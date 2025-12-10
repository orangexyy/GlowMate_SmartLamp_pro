#ifndef __AD_H
#define __AD_H

//void AD_Init(void);
//uint16_t Get_Adc(uint8_t ch);
//uint16_t Get_AdcAverage(uint8_t ch,uint8_t times);

extern uint16_t AdcValue[];
void bsp_InitAdcDMA(void);
void bsp_InitAdc(void);


#endif
