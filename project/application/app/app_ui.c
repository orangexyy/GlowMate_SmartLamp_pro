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
#include <stdio.h>
#include <stdint.h>
#include "app_ui.h"
#include "driver.h"
#include "thread.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "ui_cont.h"
#include "ui_label.h"
#include "ui_anim.h"
#include "ui_text_font.h"
#include "ui_text_language.h"
#include "ui_page_manager.h" 
#include "ui_home.h" 
#include "ui_weather.h" 
#include "plugin_lcd.h"
#include "drv_lcd.h"
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

static char app_ui_task (thread_t* pt);

LV_IMG_DECLARE(TEST);

extern const lv_img_dsc_t TEST;

lv_obj_t * img = NULL;

/**
 * \brief app初始化
 */
void app_ui_init(void)
{
    lv_init();
    lv_port_disp_init();
    plugin_lcd_fill_dma(0, 0, LCD_W, LCD_H, BLUE);  // 红色RGB565编码

    ui_text_language_init();
    // ui_page_manager_init();
    // ui_page_register(UI_PAGE_ID_HOME, (ui_page_init_func)ui_home_init, (ui_page_exit_func)ui_home_exit, (ui_page_on_enter_callback_func)ui_home_on_enter, (ui_page_on_exit_callback_func)ui_home_on_exit);
    // ui_page_register(UI_PAGE_ID_WEATHER, (ui_page_init_func)ui_weather_init, (ui_page_exit_func)ui_weather_exit, (ui_page_on_enter_callback_func)ui_weather_on_enter, (ui_page_on_exit_callback_func)ui_weather_on_exit);
    // ui_page_switch(UI_PAGE_ID_HOME);

    img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &TEST);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    TS_UI_ANIM_SINGLE_CONTROL fade_anim;
    ui_anim_fade(&fade_anim, img, 0, 255, 1000, 0, -1);


    thread_create(app_ui_task);
}

/**
 * \brief 任务
 */
static char app_ui_task(thread_t* pt)
{
    thread_begin
    {
        while (1)
        {
            lv_task_handler();
            thread_sleep(5);
        }
    }
    thread_end
}

/******************************* End of File (C) ******************************/
