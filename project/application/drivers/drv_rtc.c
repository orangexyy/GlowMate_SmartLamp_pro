/****************************************************************************\
**  文件名称 :  drv_rtc.c
**  功能描述 :  RTC 驱动。LSE 时钟、RTC 初始化、时间设置/读取、闹钟与专注时间接口。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "stm32f10x.h"                  // Device header
#include <time.h>
#include "app.h"

#include "drv_rtc.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
/** RTC 时间数组：年、月、日、星期、时、分、秒 */
uint16_t rtc_time[7] = {2026, 3, 3, 2, 0, 17, 0};	// 2026-03-03 02:00:00
/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/**
 * \brief RTC 驱动初始化：首次上电配置 LSE 与 RTC 并写入初始时间，否则仅等待同步
 */
void drv_rtc_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A6)
	{
		RCC_LSEConfig(RCC_LSE_ON);
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		
		RTC_SetPrescaler(32768 - 1);
		RTC_WaitForLastTask();
		
		drv_rtc_set_time();
		
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A6);
	}
	else
	{
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}
}

//如果LSE无法起振导致程序卡死在初始化函数中
//可将初始化函数替换为下述代码，使用LSI当作RTCCLK
//LSI无法由备用电源供电，故主电源掉电时，RTC走时会暂停
/* 
void MyRTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		RCC_LSICmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		
		RTC_SetPrescaler(40000 - 1);
		RTC_WaitForLastTask();
		
		MyRTC_SetTime();
		
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		RCC_LSICmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}
}*/

void drv_rtc_set_time(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_date.tm_year 	= rtc_time[0] - 1900;
	time_date.tm_mon 	= rtc_time[1] - 1;
	time_date.tm_mday 	= rtc_time[2];
	time_date.tm_wday 	= rtc_time[3];
	time_date.tm_hour 	= rtc_time[4];
	time_date.tm_min 	= rtc_time[5];
	time_date.tm_sec 	= rtc_time[6];
	
	time_cnt = mktime(&time_date) - 8 * 60 * 60;
	
	RTC_SetCounter(time_cnt);
	RTC_WaitForLastTask();
}

void drv_rtc_read_time(uint16_t* time_data)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter() + 8 * 60 * 60;
	
	time_date = *localtime(&time_cnt);
	
	time_data[0] = time_date.tm_year + 1900;
	time_data[1] = time_date.tm_mon + 1;
	time_data[2] = time_date.tm_mday;
	time_data[3] = time_date.tm_wday;
	time_data[4] = time_date.tm_hour;
	time_data[5] = time_date.tm_min;
	time_data[6] = time_date.tm_sec;
}

void drv_rtc_set_timestamp(uint32_t timestamp)
{
	RTC_SetCounter(timestamp);
	RTC_WaitForLastTask();
}

