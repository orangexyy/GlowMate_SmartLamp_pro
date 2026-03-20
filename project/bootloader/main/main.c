/****************************************************************************\
**  文件名称 :  main.c
**  功能描述 :  Bootloader 主入口。初始化 SysTick、驱动、应用后进入协程调度，支持升级与跳转。
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
#include "drv_flash.h"
#include "drv_comm.h"
#include "system.h"
#include "upgrade_manager.h"

/**
 * \brief Bootloader 主函数：初始化后进入协程调度
 */
int main(void)
{
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	driver_init();
	app_init();
	/** 系统开始：协程调度 */
	thread_run();
}




