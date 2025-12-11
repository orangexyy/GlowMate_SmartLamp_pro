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
#include "plugin_lcd.h"

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

/**
 * \brief app初始化
 */
void app_ui_init(void)
{
    lv_init();
    lv_port_disp_init();

    plugin_lcd_fill(0, 0, 128, 160, WHITE);

    // 1. 创建按钮（父对象为屏幕根对象）
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn, 20, 20);        // 位置：X=20, Y=20
    lv_obj_set_size(btn, 80, 40);       // 大小：宽80，高40
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF0000), LV_STATE_DEFAULT);  // 红色背景

    // 2. 在按钮上创建标签
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "LVGL v9");// 标签文本
    lv_obj_center(label);               // 标签在按钮中居中

    // 3. 创建一个文本标签（显示在屏幕下方）
    lv_obj_t *label2 = lv_label_create(lv_scr_act());
    lv_label_set_text(label2, "Test OK!");
    lv_obj_set_pos(label2, 40, 120);
    lv_obj_set_style_text_color(label2, lv_color_hex(0x00FF00), LV_STATE_DEFAULT);  // 绿色文本

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
