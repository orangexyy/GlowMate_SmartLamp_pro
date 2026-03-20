/****************************************************************************\
**  文件名称 :  main.c
**  功能描述 :  应用主入口。初始化 SysTick、NVIC 分组、驱动与应用，然后启动协程调度。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

#include "stm32f10x.h"                  // Device header
#include "sys.h"
#include "system.h"
#include "driver.h"
#include "app.h"
#include "thread.h"

/** 通用计数（可选调试用） */
uint16_t cnt = 0;

/**
 * \brief 主函数：完成系统与业务初始化后进入协程调度，不再返回
 */
int main(void)
{
	SysTick_Init(72);
	/* 设置中断优先级分组为组2：2 位抢占优先级，2 位响应优先级 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	driver_init();
	app_init();
	/** 系统开始：协程调度主循环 */
	thread_run();
}




