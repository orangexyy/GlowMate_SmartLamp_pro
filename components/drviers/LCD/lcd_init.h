#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "sys.h"

#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 280

#else
#define LCD_W 280
#define LCD_H 240
#endif


#define TCLK 		PAout(5)  	//PA5  SCLK
#define TDIN 		PAout(7)  	//PD5   MOSI
#define DOUT 		PAin(6)   	//PE8   MISO
#define TCS  		PAout(4)  	//PE12  CS2
#define PEN  		PBin(0)    //PE14  INT


//-----------------LCD端口定义---------------- 

#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_5)//SCL=SCLK
#define LCD_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7)//SDA=MOSI
#define LCD_MOSI_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_0)//RES
#define LCD_RES_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_0)

#define LCD_DC_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_1)//DC
#define LCD_DC_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_1)

#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_2)//BLK
#define LCD_BLK_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_2)
 		     
#define LCD_CS_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_3)//CS1
#define LCD_CS_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_3)




void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif




