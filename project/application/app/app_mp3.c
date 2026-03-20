/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  app_mp3.c
**  功能描述 :  MP3 应用。音量 ADC 调节、M 码音量设置/查询，以及协程内周期调节音量。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/****************************************************************************\
                               Includes
\****************************************************************************/

#include <cstdint>
#include <stdio.h>
#include <string.h>
#include "drv_adc.h"
#include "plugin_mp3.h"
#include "app_mp3.h"
#include "app_event_process.h"
#include "event_manager.h"
#include "thread.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define VOLUME_LEVEL_MAX    (30u)
#define ADC_VALUE_MAX       (4095u)

/** 音量来源：M 指令查询时返回哪一个 */
#define VOLUME_SOURCE_ADC   (0u)
#define VOLUME_SOURCE_MCMD  (1u)
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static uint16_t s_adc_volume_value = 0;
static uint8_t s_cur_adjust_volume = 0;
static uint8_t s_last_adjust_volume = 0;

/** ADC 调节后的音量（事件处理中设置） */
static uint8_t s_volume_by_adc = 10;
/** M 指令设置后的音量 */
static uint8_t s_volume_by_mcmd = 10;
/** 当前生效来源：0=ADC，1=M 指令；M 指令查询音量时按此返回对应变量 */
static uint8_t s_volume_source = VOLUME_SOURCE_ADC;
/****************************************************************************\
                            Functions definitions
\****************************************************************************/
static char app_mp3_task (thread_t* pt);

/**
 * \brief MP3 应用初始化：创建 MP3 协程任务
 */
void app_mp3_init(void)
{       
    thread_create(app_mp3_task);
}

/**
 * \brief MP3 协程任务：周期执行音量调节
 */
static char app_mp3_task(thread_t* pt)
{
    thread_begin
    {
        while (1)
        {
			app_mp3_adjust_volume();
		    thread_sleep(100);
        }
    }
    thread_end
}

void app_mp3_adjust_volume(void)
{       
    s_adc_volume_value = drv_adc_get_value(ADC_ID_VOLUME_ADJUST);
    /* ADC 0~4095 映射为音量等级 0~30 */
    s_cur_adjust_volume = (uint8_t)(((uint32_t)s_adc_volume_value * VOLUME_LEVEL_MAX) / ADC_VALUE_MAX);
    
    if (s_cur_adjust_volume != s_last_adjust_volume)
    {
        s_last_adjust_volume = s_cur_adjust_volume;
        printf("[ADC volume] adc=%u level=%d\r\n", (unsigned)s_adc_volume_value, s_cur_adjust_volume);
        event_manager_write(EVENT_MODULE_MP3, 0, s_cur_adjust_volume, NULL, 0);
    }
}

/**
 * @brief 由 M 指令设置音量（M12）；查询时若来源为 M 指令则返回此值
 */
void app_mp3_set_volume(uint8_t volume)
{
    s_volume_by_mcmd = volume;
    s_volume_source = VOLUME_SOURCE_MCMD;
    plugin_mp3_set_volume(volume);
    printf("[app_mp3] volume by MCMD=%d\r\n", volume);
}

/**
 * @brief 由 ADC/事件处理设置音量；查询时若来源为 ADC 则返回此值
 */
void app_mp3_set_volume_from_adc(uint8_t volume)
{
    s_volume_by_adc = volume;
    s_volume_source = VOLUME_SOURCE_ADC;
    plugin_mp3_set_volume(volume);
    printf("[app_mp3] volume by ADC=%d\r\n", volume);
}

/**
 * @brief 获取音量（供 M 指令查询等）：按最近一次设置来源返回 ADC 或 M 指令对应的音量
 */
uint8_t app_mp3_get_volume(void)
{
    return (s_volume_source == VOLUME_SOURCE_MCMD) ? s_volume_by_mcmd : s_volume_by_adc;
}


/******************************* End of File (C）****************************/
