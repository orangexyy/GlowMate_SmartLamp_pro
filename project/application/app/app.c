/****************************************************************************\
**  文件名称 :  app.c
**  功能描述 :  应用层总入口。初始化按键/灯光/MP3/外设/事件处理/升级，并创建
**             主应用协程（心跳 LED、定时闹钟轮询、调试打印等）。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "app.h"
#include "app_key.h"
#include "app_light.h"
#include "app_mp3.h"
#include "app_periph.h"
#include "app_event_process.h"
#include "upgrade_manager.h"
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
static char app_task (thread_t* pt);

/**
 * \brief app初始化
 */
void app_init(void)
{
	app_key_init();
    app_light_init();
	app_mp3_init();
	app_periph_init();
    app_event_process_init();
	upgrade_init();
	
    thread_create(app_task);
}

/**
 * \brief 任务
 */
static char app_task(thread_t* pt)
{
	static uint8_t flag;
    thread_begin
    {
        while (1)
        {
			flag = !flag;
			drv_output_set_value(OUTPUT_ID_LED, flag);
			drv_timer_alarm_poll();
            printf("app_task\r\n");
		    thread_sleep(1000);
        }
    }
    thread_end
}

/******************************* End of File (C) ******************************/
