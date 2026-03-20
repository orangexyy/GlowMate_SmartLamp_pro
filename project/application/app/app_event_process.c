/****************************************************************************\
**  文件名称 :  app_event_process.c
**  功能描述 :  事件处理应用。初始化事件管理器，创建事件消费协程，将按键/灯光等事件转为灯光与 MP3 控制。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "drv_key.h"
#include "plugin_mp3.h"
#include "app_mp3.h"
#include "svc_key.h"
#include "app_key.h"
#include "app_light.h"
#include "app_event_process.h"
#include "event_manager.h"
#include "driver.h"
#include "thread.h"
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
static char app_event_process_task(thread_t* pt);
static void app_event_process_handler(TS_EVENT_DATA* event);



/**
 * \brief 事件处理初始化：初始化事件管理器并创建事件消费协程
 */
void app_event_process_init(void)
{
    event_manager_init();
	
    thread_create(app_event_process_task);
}

/**
 * \brief 事件处理协程任务：周期读取事件并分发到处理函数
 */
static char app_event_process_task(thread_t* pt)
{       
    thread_begin
    {
        while (1)
        {
            TS_EVENT_DATA event;
            if (event_manager_read(&event))
            {
                app_event_process_handler(&event);
            }
			thread_sleep(10);
        }
    }
    thread_end
}


/**
 * \brief 单条事件处理：根据模块与类型更新灯光模式/档位/亮度或 MP3 等
 * \param event 事件数据
 */
static void app_event_process_handler(TS_EVENT_DATA* event)
{
    uint8_t mode = app_light_get_mode();
    uint8_t level = app_light_get_level();
    uint8_t brightness = app_light_get_brightness();
    uint32_t color = app_light_get_color();
    

    if (event->module == EVENT_MODULE_KEY)
    {
		/* 仅响应短按：按键短按下时才执行对应操作 */
        if (event->type != KEY_PRESS_SHORT_DOWN)
        {
            return;
        }
        switch (event->param)
        {
            case MODE_KEY:
                mode++;
                if (mode == APP_LIGHT_MODE_MAX)
                {
                    mode = APP_LIGHT_MODE_COOL_WHITE;
                }
                /* 关灯模式仅挥手进入，按键不选到 OFF */
                if (mode == APP_LIGHT_MODE_OFF)
                {
                    mode = APP_LIGHT_MODE_COOL_WHITE;
                }
                app_light_set_mode(mode);
                break;
            case SETTING_KEY:
                app_light_set_adjust_brightness_status(0);    //按键调整亮度
                level++;
                if (level == APP_LIGHT_LEVEL_MAX)
                {
                    level = APP_LIGHT_LEVEL_NONE;
                }
                app_light_set_level(level);
                printf("[SETTING_KEY] level=%d brightness=%d\r\n", app_light_get_level(), app_light_get_brightness());
				break;
            case BACK_KEY:
                app_light_set_adjust_brightness_status(0);    //按键调整亮度
                level--;
                if (level == APP_LIGHT_LEVEL_NONE)
                {
                    level = APP_LIGHT_LEVEL_MAX;
                }
                app_light_set_level(level);
                printf("[BACK_KEY] level=%d brightness=%d\r\n", app_light_get_level(), app_light_get_brightness());
				break;
			case PLAY_KEY:
                plugin_mp3_get_state() == MP3_STATE_PLAY ? plugin_mp3_pause() : plugin_mp3_play();
                break;
            case UP_KEY:
                plugin_mp3_next();
                break;
            case DOWN_KEY:
                plugin_mp3_previous();
                break;
        }
    }

    if (event->module == EVENT_MODULE_ASR)
    {
		
    }
	
	if (event->module == EVENT_MODULE_MP3)
    {
        app_mp3_set_volume_from_adc(event->param);
    }

    if (event->module == EVENT_MODULE_LIGHT)
    {
        app_light_set_brightness(event->param);
        app_light_set_adjust_brightness_status(1);        //滚轮调整亮度
    }
}

/******************************* End of File (C) ******************************/
