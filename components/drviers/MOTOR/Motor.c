#include "stm32f4xx.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA时钟

  //GPIOF9,F10初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO
	
//	PWM_Init();
	
}

void Motor_L(int8_t Speed)
{
	if(Speed >= 0)   //正转
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_1);
		GPIO_ResetBits(GPIOE,GPIO_Pin_2);
		PWM_SetCompare1(Speed);
	}
	else             //反转
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_1);
		GPIO_SetBits(GPIOE,GPIO_Pin_2);
		PWM_SetCompare1(-Speed);
	}
}

void Motor_R(int8_t Speed)
{
	if(Speed >= 0)   //正转
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_3);
		GPIO_ResetBits(GPIOE,GPIO_Pin_4);
		PWM_SetCompare2(Speed);
	}
	else             //反转
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_3);
		GPIO_SetBits(GPIOE,GPIO_Pin_4);
		PWM_SetCompare2(-Speed);
	}
}


/*限幅函数*/
void Limit(int *motoA,int *motoB)
{
	if(*motoA>PWM_MAX)*motoA=PWM_MAX;
	if(*motoA<PWM_MIN)*motoA=PWM_MIN;
	
	if(*motoB>PWM_MAX)*motoB=PWM_MAX;
	if(*motoB<PWM_MIN)*motoB=PWM_MIN;
}

/*绝对值函数*/
int GFP_abs(int p)
{
	int q;
	q=p>0?p:(-p);
	return q;
}

/*赋值函数*/
/*入口参数：PID运算完成后的最终PWM值*/
void Load(int moto1,int moto2)//moto1=-200：反转200个脉冲
{
	//1.研究正负号，对应正反转
	if(moto1<0)	Ain1=1,Ain2=0;//正转
	else 				Ain1=0,Ain2=1;//反转
	//2.研究PWM值
	TIM_SetCompare1(TIM8,GFP_abs(moto1));
	
	if(moto2>0)	Bin1=1,Bin2=0;
	else 				Bin1=0,Bin2=1;	
	TIM_SetCompare2(TIM8,GFP_abs(moto2));
}


void Car_go(uint8_t Speed)
{
	Motor_L(-Speed);
	Motor_R(Speed);
}

void Car_back(uint8_t Speed)
{
	Motor_L(Speed);
	Motor_R(-Speed);
}

void Car_left(uint8_t Speed)
{
	Motor_L(-(Speed-20));
	Motor_R(Speed);
}

void Car_right(uint8_t Speed)
{
	Motor_L(-Speed);
	Motor_R(Speed-20);
}

