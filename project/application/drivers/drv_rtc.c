/****************************************************************************\
**  版    权 : 
**  文件名称 :  
**  功能描述 :  
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "stm32f4xx.h"
#include <time.h>
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

/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/**
 * \brief RTC初始化
 * \return 无
 */
void drv_rtc_init(void)
{
	RTC_InitTypeDef rtc_init_structure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0xABAB)
	{
		RCC_LSEConfig(RCC_LSE_ON);
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		rtc_init_structure.RTC_AsynchPrediv = 0x7F;
		rtc_init_structure.RTC_SynchPrediv  = 0xFF;
		rtc_init_structure.RTC_HourFormat   = RTC_HourFormat_24;
		RTC_Init(&rtc_init_structure);
 
		drv_rtc_set_time(00, 48, 30, RTC_H12_AM);
		drv_rtc_set_date(24, 9, 28, 6);
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0xABAB);
	} 
}

/**
 * \brief 设置时间
 * \param hour 小时
 * \param min 分钟
 * \param sec 秒
 * \param ampm 上午下午
 * \return 无
 */
ErrorStatus drv_rtc_set_time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	RTC_TimeTypeDef rtc_time_type_init_structure;
	
	rtc_time_type_init_structure.RTC_Hours		= hour;
	rtc_time_type_init_structure.RTC_Minutes	= min;
	rtc_time_type_init_structure.RTC_Seconds	= sec;
	rtc_time_type_init_structure.RTC_H12		= ampm;
	
	return RTC_SetTime(RTC_Format_BIN, &rtc_time_type_init_structure);
	
}

/**
 * \brief 设置日期
 * \param year 年
 * \param month 月
 * \param date 日
 * \param week 周
 * \return 无
 */
ErrorStatus drv_rtc_set_date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	
	RTC_DateTypeDef rtc_date_type_init_structure;
	rtc_date_type_init_structure.RTC_Date 		= date;
	rtc_date_type_init_structure.RTC_Month 		= month;
	rtc_date_type_init_structure.RTC_WeekDay 	= week;
	rtc_date_type_init_structure.RTC_Year 		= year;
	return RTC_SetDate(RTC_Format_BIN,&rtc_date_type_init_structure);
}

/**
* \brief 单次读取RTC当前时间和日期（主动获取）
* \param rtc_time 时间
* \param rtc_date 日期
* \return 无（数据存入全局变量g_rtc_time/g_rtc_date）
* \note 必须遵循「先读时间、后读日期」的顺序，否则数据可能不一致
*/
void drv_rtc_get_time_date(RTC_TimeTypeDef *rtc_time, RTC_DateTypeDef *rtc_date)
{
	
	RTC_GetTime(RTC_Format_BIN, rtc_time);
	RTC_GetDate(RTC_Format_BIN, rtc_date);
}


/**
 * \brief 设置闹钟
 * \param hour 小时
 * \param min 分钟
 * \param sec 秒
 * \return 无
 */
void drv_rtc_set_alarm(uint8_t hour,uint8_t min,uint8_t sec)
{ 
	EXTI_InitTypeDef   	exti_init_structure;
	RTC_AlarmTypeDef 	rtc_alarm_type_init_structure;
	RTC_TimeTypeDef 	rtc_time_type_init_structure;
	NVIC_InitTypeDef   	nvic_init_structure;
	
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE); // 先禁用闹钟A
	
	// 1. 配置闹钟时间（时分秒）
	rtc_time_type_init_structure.RTC_Hours		= hour;
	rtc_time_type_init_structure.RTC_Minutes	= min;
	rtc_time_type_init_structure.RTC_Seconds	= sec;
	rtc_time_type_init_structure.RTC_H12		= RTC_H12_AM; // 24小时制下此参数无效（你的RTC是24小时制）

	// 2. 关键修改：按日期触发，且屏蔽日期匹配（只匹配时分秒）
	rtc_alarm_type_init_structure.RTC_AlarmDateWeekDay 		= 1; // 日期随意填1~31（因后续屏蔽日期匹配）
	rtc_alarm_type_init_structure.RTC_AlarmDateWeekDaySel	= RTC_AlarmDateWeekDaySel_Date; // 按日期触发
	rtc_alarm_type_init_structure.RTC_AlarmMask				= RTC_AlarmMask_DateWeekDay; // 屏蔽日期匹配（只校验时分秒）
	rtc_alarm_type_init_structure.RTC_AlarmTime				= rtc_time_type_init_structure;
	
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &rtc_alarm_type_init_structure);

	// 3. 清除残留中断标志（必须彻底）
	RTC_ClearITPendingBit(RTC_IT_ALRA);
	EXTI_ClearITPendingBit(EXTI_Line17);
	
	// 4. 使能闹钟中断和闹钟A
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	
	// 5. 配置EXTI和NVIC（原代码正确，保留）
	exti_init_structure.EXTI_Line = EXTI_Line17;
	exti_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init_structure.EXTI_Trigger = EXTI_Trigger_Rising;
	exti_init_structure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init_structure);

	nvic_init_structure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
	nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_structure.NVIC_IRQChannelSubPriority = 2;
	nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_structure);
}

/**
 * \brief 闹钟中断处理
 * \return 无
 */
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);
		printf("ALARM A!\r\n");
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);
}

