#ifndef _PID_H_
#define _PID_H_

#include "sys.h"  

struct PID
{
	float kp;
	float ki;
	float kd;
};

/*定义5ms的编码器数值最大累计*/
#define _VELOCITY_MAX_ 100000

//int8_t Turn_PID(uint8_t err);
int16_t Velocity1_PID(int16_t current,int16_t Target);
int16_t Velocity2_PID(int16_t current,int16_t Target);
#endif /*PID算法*/
