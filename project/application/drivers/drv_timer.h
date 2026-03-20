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

/****************************************************************************\
                               Includes
\****************************************************************************/
#ifndef __DRV_TIMER_H
#define __DRV_TIMER_H

#include "stdint.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
typedef enum
{
	TIMER_ID_3 = 0,
	TIMER_ID_4 = 1,
    TIMER_ID_MAX = 2,
} TE_TIMER_ID;
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/

/****************************************************************************\
                            Functions definitions
\****************************************************************************/
/**
 * \brief 定时器初始化
 * \return 无
 */
void drv_timer3_init(void);

void drv_timer4_init(void);

/**
 * \brief 启动定时器
 * \return 无
 */
void drv_timer_start(uint8_t timer_id);

/**
 * \brief 暂停定时器
 * \return 无
 */
void drv_timer_pause(uint8_t timer_id); 

/**
 * \brief 重置定时器
 * \return 无
 */
void drv_timer_reset(uint8_t timer_id);

/**s
 * \brief 设置闹钟
 * \param hour: 小时
 * \param minute: 分钟
 * \param second: 秒数
 * \return 无
 */
void drv_timer_set_alarm_data(uint8_t hour, uint8_t minute, uint8_t second);

/**
 * \brief 获取闹钟
 * \param hour: 小时指针（可为NULL）
 * \param minute: 分钟指针（可为NULL）
 * \param second: 秒数指针（可为NULL）
 * \return 无
 */
void drv_timer_get_alarm_data(uint8_t* hour, uint8_t* minute, uint8_t* second);

/**
 * \brief 闹钟到时轮询，需在主任务中每 1 秒调用一次；负责每 2 秒向串口2 发送一次 M8 S1，共 3 次
 */
void drv_timer_alarm_poll(void);

/**
 * \brief 获取专注时间
 * \param hour: 小时指针（可为NULL）
 * \param minute: 分钟指针（可为NULL）
 * \param second: 秒数指针（可为NULL）
 * \return 无
 */
void drv_timer_get_focus_time(uint8_t *hour, uint8_t *minute, uint8_t *second);

#endif
/****************************************************************************\
                            End of File
\****************************************************************************/


