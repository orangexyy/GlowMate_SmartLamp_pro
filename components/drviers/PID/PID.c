#include "PID.h"

//struct PID pid_x={10,0,0};//摄像头转向pid

struct PID pid_x={0,0,0};//灰度传感器转向pid
struct PID pid_y={-25,-0.7,0};//速度pid
struct PID pid_z={-25,-0.5,0};
float error_last1,error_ki1;//上一次的误差和误差累积
float error_last2,error_ki2;//上一次的误差和误差累积
////-20,-0.15
///*
// *功能：转向PID实现
// *参数：current 当前角度   Target 目标角度  （采用传入的adc误差表示）
// *返回值：输出结果
//*/
//int8_t Turn_PID(uint8_t err)
//{
//	int8_t result;
//	static int8_t error_last;//上一次的误差和误差累积
//	err=err-80;
//	result=err*20;//+error_Integral*pid_x.ki+(error-error_last)*pid_x.kd;//计算结果
//	error_last=err;//更新误差
//	return result;
//}

/*
 *功能：右速度PID实现   
 *参数：current 当前速度   Target 目标速度
 *返回值：输出结果
*/
int16_t Velocity1_PID(int16_t current,int16_t Target)
{
	int16_t error,result;//当前误差,和返回的结果

	int16_t error_kp,error_kd;
	
	error=current-Target;//得到本次误差
	
	error_kp=error*pid_y.kp;				//P项
	error_ki1+=error*pid_y.ki;				//I项
	if(error_ki1>1400)error_ki1=1400;		//I项限幅
	if(error_ki1<-1400)error_ki1=-1400;
	
	//OLED_ShowSignedNum(4, 8,error_ki, 5);
	error_kd=(error-error_last1)*pid_y.kd;	//D项
	
	result=error_kp+error_ki1+error_kd;		//PID输出

	error_last1=error;//更新误差
	result=(result>1400)?1400:result;
	result=(result<-1400)?-1400:result;
	return result;
}

/*
 *功能：左速度PID实现   
 *参数：current 当前速度   Target 目标速度
 *返回值：输出结果
*/
int16_t Velocity2_PID(int16_t current,int16_t Target)
{
	int16_t error,result;//当前误差,和返回的结果

	int16_t error_kp,error_kd;
	error=current-Target;//得到本次误差
	error_kp=error*pid_z.kp;                //P项
	error_ki2+=error*pid_z.ki;              //I项
	if(error_ki2>1500)error_ki2=1500;       //I项限幅
	if(error_ki2<-1500)error_ki2=-1500;      
	                                         
	//OLED_ShowSignedNum(4, 8,error_ki, 5);  
	error_kd=(error-error_last2)*pid_z.kd;  //D项
	                                         
	result=error_kp+error_ki2+error_kd;     //PID输出
                                             
	error_last2=error;//更新误差
	result=(result>1500)?1500:result;
	result=(result<-1500)?-1500:result;
	return result;
}


