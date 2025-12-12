#include "ui_home.h"
#include "lvgl.h"
#include "ui_cont.h"
#include "ui_label.h"
#include "ui_text_font.h"
#include "ui_text_language.h"
#include "ui_img.h"
#include "ui_anim.h"
#include "ui_page_manager.h"  


// 全局页面对象
TS_UI_HOME g_ui_home;

void ui_home_init(void) 
{
    // 创建根容器
    ui_cont_create(&g_ui_home.cont, lv_scr_act(), 128, 160);
    ui_cont_set_pos(&g_ui_home.cont, 0, 0, LV_ALIGN_CENTER, 0);
    ui_cont_set_style(&g_ui_home.cont, 0x00000000, LV_OPA_COVER);

    // 创建任务标签
    ui_label_create(&g_ui_home.home_label, g_ui_home.cont, 50, 50,
                   ui_text_get_string_for_language("error_mode"),UI_TEXT_FONT_REGULAR, 14);
    ui_label_set_absolute_pos(&g_ui_home.home_label, 0, 0, LV_TEXT_ALIGN_CENTER);
    ui_label_set_style(&g_ui_home.home_label, 0xFFFFFFFF, 255, 0, 0, LV_LABEL_LONG_WRAP);

    // 启动主页动画（1000ms一次，无限重复）
    // ui_anim_frame_init(&g_ui_home.home_anim, g_ui_home.cont, anim_home_frames, 1, 100, true);
}   

void ui_home_exit(void) 
{
    // ui_anim_frame_stop(&g_ui_home.home_anim);
    if (g_ui_home.cont) {
        lv_obj_del(g_ui_home.cont);
        g_ui_home.cont = NULL;
    }
}

void ui_home_on_enter(void) 
{
    // ui_anim_frame_start(&g_ui_home.home_anim);
    lv_label_set_text(g_ui_home.home_label, ui_text_get_string_for_language("error_mode"));  
}   

void ui_home_on_exit(void) 
{
    // ui_anim_frame_stop(&g_ui_home.home_anim);
}

