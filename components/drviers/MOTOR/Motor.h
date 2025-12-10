#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h" 

#define Ain1  PEout(1)
#define Ain2  PEout(2)
               
#define Bin1  PEout(3)
#define Bin2  PEout(4)

void Motor_Init(void);
void Motor_L(int8_t Speed);
void Motor_R(int8_t Speed);



void Limit(int *motoA,int *motoB);
int GFP_abs(int p);
void Load(int moto1,int moto2);

void Car_go(uint8_t Speed);
void Car_back(uint8_t Speed);
void Car_left(uint8_t Speed);	//可改成两个输入量 分别控制左右两个电机
void Car_right(uint8_t Speed);


#endif
