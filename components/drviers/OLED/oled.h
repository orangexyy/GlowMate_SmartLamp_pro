#ifndef __OLED_H
#define __OLED_H

#include "sys.h"
#include "stdlib.h"



/*--------------------引脚定义--------------------------*/
#define 		OLED_SCLK_Set()			GPIO_SetBits(GPIOB,GPIO_Pin_8)	//PB8（SCL）输出高
#define			OLED_SCLK_Clr()			GPIO_ResetBits(GPIOB,GPIO_Pin_8)	//PB8（SCL）输出低
#define			OLED_SDIN_Set()			GPIO_SetBits(GPIOB,GPIO_Pin_9) //PB9（SDA）输出高
#define			OLED_SDIN_Clr()			GPIO_ResetBits(GPIOB,GPIO_Pin_9)	//PB9（SDA）输出高
#define 		OLED_READ_SDIN()		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)	//读取PB9（SDA）电平
		

/*definition--------------------------------------------*/
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#define SIZE 		6		//显示字符的大小
#define Max_Column	128		//最大列数 
#define Max_Row		64		//最大行数
#define X_WIDTH 	128		//X轴的宽度
#define Y_WIDTH 	64	    //Y轴的宽度					  				   
#define	IIC_ACK		0		//应答
#define	IIC_NO_ACK	1		//不应答




/*-----------------声明函数-----------------------------*/
void OLED_WR_Byte(unsigned char dat,unsigned char cmd);	  //OLED写字节函数
void OLED_Display_On(void);	//开显示函数
void OLED_Display_Off(void);//关显示函数
void OLED_Init(void);	//OLED初始化函数
void OLED_Clear(void);  //清屏函数
void OLED_Clear_Part(uint8_t x, uint8_t start, uint8_t end);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr); //显示字符函数
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2); //在指定的位置，显示一个指定数的长度大小函数
void OLED_Float(unsigned char Y,unsigned char X,double real,unsigned char N);
void OLED_ShowString(unsigned char x,unsigned char y, unsigned char *p);	 //在指定位置开始显示字符串函数
void OLED_Set_Pos(unsigned char x, unsigned char y);		//画点函数
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no,unsigned char m,unsigned char Chn[][m]); //声明在指定位置显示汉字函数
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]); //显示图片函数
void OLED_Scroll(void);	//滚动函数	 
void OLED_DrawGIF(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1, unsigned char k, int m, unsigned char GIF[][m]); //显示动图（未测试）


#endif





