/****************************************************************************\
**  文件名称 :  drv_timer.c
**  功能描述 :  定时器驱动。TIM3/TIM4 时基与闹钟/专注计时、闹钟到时串口 M8 上报及轮询接口。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "stm32f10x.h"                  // Device header
#include "drv_timer.h"
#include "drv_comm.h"
#include "event_manager.h"
#include "app_event_process.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
static uint8_t s_alarm_hour = 0;
static uint8_t s_alarm_minute = 0;
static uint8_t s_alarm_second = 0;

static uint8_t s_focus_hour = 0;
static uint8_t s_focus_minute = 0;
static uint8_t s_focus_second = 0;

/** 闹钟到时串口2 发 M8 S1 共 3 次、间隔 2 秒，由 drv_timer_alarm_poll() 在任务中执行 */
#define ALARM_RING_SEND_COUNT   (3u)
#define ALARM_RING_INTERVAL_TICKS (2u)   /* 间隔 2 秒，需主任务每 1 秒调用一次 alarm_poll */
static uint8_t s_alarm_ring_pending = 0;
static uint8_t s_alarm_ring_send_count = 0;
static uint8_t s_alarm_ring_wait_ticks = 0;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/** TIM3 初始化：时基与更新中断，用于计时 */
void drv_timer3_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_InternalClockConfig(TIM3);	
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;				
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;				
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);				
	
						
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);		
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);				
	NVIC_InitTypeDef NVIC_InitStructure;						
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);								
	TIM_Cmd(TIM3, DISABLE);			
}

/** TIM4 初始化：时基与更新中断 */
void drv_timer4_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_InternalClockConfig(TIM4);	
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;				
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;				
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);				
	
						
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);		
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);				
	NVIC_InitTypeDef NVIC_InitStructure;						
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);								
	TIM_Cmd(TIM4, DISABLE);			
}

void drv_timer_start(uint8_t timer_id)
{
	switch (timer_id)
	{
		case TIMER_ID_3:
			TIM_Cmd(TIM3, ENABLE);
			break;
		case TIMER_ID_4:
			TIM_Cmd(TIM4, ENABLE);
			break;
	}
}

void drv_timer_pause(uint8_t timer_id)
{
	switch (timer_id)
	{
		case TIMER_ID_3:
			TIM_Cmd(TIM3, DISABLE);
			break;
		case TIMER_ID_4:
			TIM_Cmd(TIM4, DISABLE);
			break;
	}
}

void drv_timer_reset(uint8_t timer_id)
{
	switch (timer_id)
	{
		case TIMER_ID_3:
			s_alarm_hour = 0;
			s_alarm_minute = 0;
			s_alarm_second = 0;
			TIM_Cmd(TIM3, DISABLE);
			break;
		case TIMER_ID_4:
			s_focus_hour = 0;
			s_focus_minute = 0;
			s_focus_second = 0;
			TIM_Cmd(TIM4, DISABLE);
			break;
	}
}

//闹钟功能
void drv_timer_set_alarm_data(uint8_t hour, uint8_t minute, uint8_t second)
{
	s_alarm_hour = hour;
	s_alarm_minute = minute;
	s_alarm_second = second;
    TIM_Cmd(TIM3, ENABLE);	
}

void drv_timer_get_alarm_data(uint8_t* hour, uint8_t* minute, uint8_t* second)
{
	if (hour != NULL) {
		*hour = s_alarm_hour;
	}
	if (minute != NULL) {
		*minute = s_alarm_minute;
	}
	if (second != NULL) {
		*second = s_alarm_second;
	}
}

/**
 * \brief 闹钟到时轮询：在主任务中每 1 秒调用一次，按 2 秒间隔向串口2 发送 3 次 M8 S1
 */
void drv_timer_alarm_poll(void)
{
	if (!s_alarm_ring_pending)
		return;
	if (s_alarm_ring_wait_ticks > 0)
	{
		s_alarm_ring_wait_ticks--;
		return;
	}
	drv_usart_send_string(DRV_USART_ID_2, (char *)"M8 S1\r\n");
	s_alarm_ring_send_count++;
	if (s_alarm_ring_send_count >= ALARM_RING_SEND_COUNT)
		s_alarm_ring_pending = 0;
	else
		s_alarm_ring_wait_ticks = ALARM_RING_INTERVAL_TICKS;
}

//专注时间
void drv_timer_get_focus_time(uint8_t *hour, uint8_t *minute, uint8_t *second)
{	
	if (hour != NULL)
		*hour = s_focus_hour;
	if (minute != NULL)
		*minute = s_focus_minute;
	if (second != NULL)
		*second = s_focus_second;
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		// 检查倒计时是否结束（时:分:秒都为0）
		if (s_alarm_hour == 0 && s_alarm_minute == 0 && s_alarm_second == 0)
		{
			printf("TIME OUT\r\n");
			TIM_Cmd(TIM3, DISABLE);
			s_alarm_ring_pending = 1;
			s_alarm_ring_send_count = 0;
			s_alarm_ring_wait_ticks = 0;
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
			return;
		}
		
		// 倒计时递减逻辑：从秒开始，逐级借位
		if (s_alarm_second > 0)
		{
			s_alarm_second--;
		}
		else
		{
			s_alarm_second = 59;
			if (s_alarm_minute > 0)
			{
				s_alarm_minute--;
			}
			else if (s_alarm_hour > 0)
			{
				s_alarm_minute = 59;
				s_alarm_hour--;
			}
			// 如果小时和分钟都为0，秒也为0，会在下次中断时被上面的检查捕获并停止定时器
		}
		
		printf("[alarm] %02d:%02d:%02d\r\n", (int)s_alarm_hour, (int)s_alarm_minute, (int)s_alarm_second);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		/* 学习时间递增：秒 → 分 → 时 */
		s_focus_second++;
		if (s_focus_second >= 60)
		{
			s_focus_second = 0;
			s_focus_minute++;
			if (s_focus_minute >= 60)
			{
				s_focus_minute = 0;
				if (s_focus_hour < 255)
					s_focus_hour++;
			}
		}
		printf("[focus] %02d:%02d:%02d\r\n", (int)s_focus_hour, (int)s_focus_minute, (int)s_focus_second);
	}
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}
