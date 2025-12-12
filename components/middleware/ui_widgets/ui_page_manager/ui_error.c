
#include "ui_error.h"
#include "../lvgl/lvgl.h"
#include "../ui_cont/ui_cont.h"
#include "../ui_label/ui_label.h"
#include "../ui_label/ui_text_font.h"
#include "../ui_label/ui_text_language.h"
#include "../ui_img/ui_img.h"
#include "../ui_anim/ui_anim.h"
#include "../ui_page_manager/ui_page_manager.h"  

LV_IMAGE_DECLARE(P100_FEED_DEVICE);
LV_IMAGE_DECLARE(P100_FEED2_GIF_00);
LV_IMAGE_DECLARE(P100_FEED2_GIF_02);
LV_IMAGE_DECLARE(P100_FEED2_GIF_04);
LV_IMAGE_DECLARE(P100_FEED2_GIF_06);
LV_IMAGE_DECLARE(P100_FEED2_GIF_08);
LV_IMAGE_DECLARE(P100_FEED2_GIF_10);
LV_IMAGE_DECLARE(P100_FEED2_GIF_12);
LV_IMAGE_DECLARE(P100_FEED2_GIF_14);
LV_IMAGE_DECLARE(P100_FEED2_GIF_16);
LV_IMAGE_DECLARE(P100_FEED2_GIF_18);
LV_IMAGE_DECLARE(P100_FEED2_GIF_20);
LV_IMAGE_DECLARE(P100_FEED2_GIF_22);
LV_IMAGE_DECLARE(P100_FEED2_GIF_24);
LV_IMAGE_DECLARE(P100_FEED2_GIF_26);
LV_IMAGE_DECLARE(P100_FEED2_GIF_28);
LV_IMAGE_DECLARE(P100_FEED2_GIF_30);
LV_IMAGE_DECLARE(P100_FEED2_GIF_32);
LV_IMAGE_DECLARE(P100_FEED2_GIF_34);        
LV_IMAGE_DECLARE(P100_FEED2_GIF_36);
LV_IMAGE_DECLARE(P100_FEED2_GIF_38);
LV_IMAGE_DECLARE(P100_FEED2_GIF_40);
LV_IMAGE_DECLARE(P100_FEED2_GIF_42);
LV_IMAGE_DECLARE(P100_FEED2_GIF_44);
LV_IMAGE_DECLARE(P100_FEED2_GIF_46);
LV_IMAGE_DECLARE(P100_FEED2_GIF_48);
LV_IMAGE_DECLARE(P100_FEED2_GIF_50);
LV_IMAGE_DECLARE(P100_FEED2_GIF_52);
LV_IMAGE_DECLARE(P100_FEED2_GIF_54);
LV_IMAGE_DECLARE(P100_FEED2_GIF_56);
LV_IMAGE_DECLARE(P100_FEED2_GIF_58);
LV_IMAGE_DECLARE(P100_FEED2_GIF_60);
LV_IMAGE_DECLARE(P100_FEED2_GIF_64);
LV_IMAGE_DECLARE(P100_FEED2_GIF_66);


extern const lv_img_dsc_t P100_FEED_DEVICE;
extern const lv_img_dsc_t P100_FEED2_GIF_00;
extern const lv_img_dsc_t P100_FEED2_GIF_02;
extern const lv_img_dsc_t P100_FEED2_GIF_04;
extern const lv_img_dsc_t P100_FEED2_GIF_06;
extern const lv_img_dsc_t P100_FEED2_GIF_08;
extern const lv_img_dsc_t P100_FEED2_GIF_10;
extern const lv_img_dsc_t P100_FEED2_GIF_12;
extern const lv_img_dsc_t P100_FEED2_GIF_14;
extern const lv_img_dsc_t P100_FEED2_GIF_16;
extern const lv_img_dsc_t P100_FEED2_GIF_18;
extern const lv_img_dsc_t P100_FEED2_GIF_20;
extern const lv_img_dsc_t P100_FEED2_GIF_22;
extern const lv_img_dsc_t P100_FEED2_GIF_24;
extern const lv_img_dsc_t P100_FEED2_GIF_26;
extern const lv_img_dsc_t P100_FEED2_GIF_28;
extern const lv_img_dsc_t P100_FEED2_GIF_30;
extern const lv_img_dsc_t P100_FEED2_GIF_32;
extern const lv_img_dsc_t P100_FEED2_GIF_34;
extern const lv_img_dsc_t P100_FEED2_GIF_36;
extern const lv_img_dsc_t P100_FEED2_GIF_38;
extern const lv_img_dsc_t P100_FEED2_GIF_40;
extern const lv_img_dsc_t P100_FEED2_GIF_42;
extern const lv_img_dsc_t P100_FEED2_GIF_44;
extern const lv_img_dsc_t P100_FEED2_GIF_46;
extern const lv_img_dsc_t P100_FEED2_GIF_48;
extern const lv_img_dsc_t P100_FEED2_GIF_50;
extern const lv_img_dsc_t P100_FEED2_GIF_52;
extern const lv_img_dsc_t P100_FEED2_GIF_54;
extern const lv_img_dsc_t P100_FEED2_GIF_56;
extern const lv_img_dsc_t P100_FEED2_GIF_58;
extern const lv_img_dsc_t P100_FEED2_GIF_60;
extern const lv_img_dsc_t P100_FEED2_GIF_64;
extern const lv_img_dsc_t P100_FEED2_GIF_66;


const lv_img_dsc_t *anim_error_frames[] = {
    &P100_FEED2_GIF_00, &P100_FEED2_GIF_02, &P100_FEED2_GIF_04, &P100_FEED2_GIF_06, &P100_FEED2_GIF_08,
    &P100_FEED2_GIF_10, &P100_FEED2_GIF_12, &P100_FEED2_GIF_14, &P100_FEED2_GIF_16, &P100_FEED2_GIF_18,
    &P100_FEED2_GIF_20, &P100_FEED2_GIF_22, &P100_FEED2_GIF_24, &P100_FEED2_GIF_26, &P100_FEED2_GIF_28,
    &P100_FEED2_GIF_30, &P100_FEED2_GIF_32, &P100_FEED2_GIF_34, &P100_FEED2_GIF_36, &P100_FEED2_GIF_38,
    &P100_FEED2_GIF_40, &P100_FEED2_GIF_42, &P100_FEED2_GIF_46, &P100_FEED2_GIF_52, &P100_FEED2_GIF_54,
    &P100_FEED2_GIF_56, &P100_FEED2_GIF_58, &P100_FEED2_GIF_60, &P100_FEED2_GIF_64, &P100_FEED2_GIF_66,   
}; // 帧数组

TS_UI_ERROR g_ui_error;

void ui_error_init(void) 
{
    // 创建红色背景容器
    ui_cont_create(&g_ui_error.cont, lv_scr_act(), 500, 500);
    ui_cont_set_pos(&g_ui_error.cont, 0, 0, LV_ALIGN_CENTER, 0);
    ui_cont_set_style(&g_ui_error.cont, 0x00000000, LV_OPA_COVER);

    // 创建错误图片
    ui_img_create(&g_ui_error.error_img, g_ui_error.cont, &P100_FEED_DEVICE, 200, 135);
    ui_img_set_pos(&g_ui_error.error_img, 0, 0, LV_ALIGN_CENTER, 1);

    // 启动错误动画（1000ms一次，无限重复）
    ui_anim_frame_init(&g_ui_error.error_anim, g_ui_error.cont, anim_error_frames, 26, 100, true);

    // 创建错误标签
    ui_label_create(&g_ui_error.error_label, g_ui_error.cont, 200, 30,
                   ui_text_get_string_for_language("error_mode"),UI_TEXT_FONT_REGULAR, 32);
    ui_label_set_absolute_pos(&g_ui_error.error_label, 130, 550, LV_TEXT_ALIGN_CENTER);
    ui_label_set_style(&g_ui_error.error_label, 0xFFFFFFFF, 255, 0, 0, LV_LABEL_LONG_WRAP);

    
}

void ui_error_exit(void) 
{
    ui_anim_frame_stop(&g_ui_error.error_anim);
    if (g_ui_error.cont) 
    {
        lv_obj_del(g_ui_error.cont);
        g_ui_error.cont = NULL;
    }
}

void ui_error_on_enter(void) 
{
    ui_anim_frame_start(&g_ui_error.error_anim);
    lv_label_set_text(g_ui_error.error_label, ui_text_get_string_for_language("error_mode"));
}   

void ui_error_on_exit(void) 
{
    ui_anim_frame_stop(&g_ui_error.error_anim);
}