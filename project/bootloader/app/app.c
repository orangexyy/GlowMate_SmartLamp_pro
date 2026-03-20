/****************************************************************************\
**  文件名称 :  app.c
**  功能描述 :  Bootloader 应用入口。初始化驱动与升级管理，创建应用协程（可选）。
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
#include "driver.h"
#include "plugin_led.h"
#include "thread.h"
#include "upgrade_manager.h"
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
            if (flag)
            {
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_BLACK);
            }
            else
            {
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_BLUE);
            }
			drv_output_set_value(OUTPUT_ID_LED, flag);
            // printf("app_task\r\n");
            thread_sleep(1000);
        }
    }
    thread_end
}
/******************************* End of File (C) ******************************/
