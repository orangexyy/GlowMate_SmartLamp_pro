#ifndef _Ultrasound_H
#define _Ultrasound_H
#include "sys.h" 

extern uint32_t Time;
extern uint32_t Time_end;

void Ultrasound_Init(void);
uint16_t Ultrasound_mm(void);
float Ultrasound_m(void);


#endif
