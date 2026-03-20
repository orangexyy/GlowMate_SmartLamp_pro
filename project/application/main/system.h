/****************************************************************************\
**  文件名称 :  system.h
**  功能描述 :  系统时钟与延时头文件。声明 SysTick 初始化、延时、系统 tick。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

#ifndef __SYSTEM_H
#define __SYSTEM_H 			   
#include "sys.h"  

/** 外部变量声明（定义在 system.c / 中断中） */
extern uint32_t g_system_tick;

// 函数声明
void SysTick_Init(u8 SYSCLK);  // 初始化SysTick为1ms周期性中断
void delay_ms(u16 nms);        // 延时n毫秒
void delay_us(u32 nus);        // 延时n微秒
uint32_t get_system_tick(void);

#endif





























