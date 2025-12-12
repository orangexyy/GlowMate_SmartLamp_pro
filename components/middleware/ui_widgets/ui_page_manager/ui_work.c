#include "ui_work.h"
#include "../lvgl/lvgl.h"
#include "../ui_cont/ui_cont.h"
#include "../ui_label/ui_label.h"
#include "../ui_label/ui_text_font.h"
#include "../ui_label/ui_text_language.h"
#include "../ui_img/ui_img.h"
#include "../ui_anim/ui_anim.h"
#include "../ui_page_manager/ui_page_manager.h"  


LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_000);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_002);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_004);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_006);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_008);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_010);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_012);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_014);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_016);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_018);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_020);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_022);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_024);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_026);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_028);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_030);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_032);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_034);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_036);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_038);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_040);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_042);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_044);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_046);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_048);
LV_IMAGE_DECLARE(P100_PROCESSINGREADY_GIF_050);



extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_000;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_002;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_004;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_006;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_008;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_010;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_012;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_014;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_016;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_018;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_020;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_022;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_024;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_026;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_028;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_030;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_032;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_034;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_036;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_038;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_040;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_042;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_044;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_046;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_048;
extern const lv_img_dsc_t P100_PROCESSINGREADY_GIF_050;


const lv_img_dsc_t *anim_work_frames[] = {
    &P100_PROCESSINGREADY_GIF_000, &P100_PROCESSINGREADY_GIF_002, &P100_PROCESSINGREADY_GIF_004, &P100_PROCESSINGREADY_GIF_006, &P100_PROCESSINGREADY_GIF_008,
    &P100_PROCESSINGREADY_GIF_010, &P100_PROCESSINGREADY_GIF_012, &P100_PROCESSINGREADY_GIF_014, &P100_PROCESSINGREADY_GIF_016, &P100_PROCESSINGREADY_GIF_018,
    &P100_PROCESSINGREADY_GIF_020, &P100_PROCESSINGREADY_GIF_022, &P100_PROCESSINGREADY_GIF_024, &P100_PROCESSINGREADY_GIF_026, &P100_PROCESSINGREADY_GIF_028,
    &P100_PROCESSINGREADY_GIF_030, &P100_PROCESSINGREADY_GIF_032, &P100_PROCESSINGREADY_GIF_034, &P100_PROCESSINGREADY_GIF_036, &P100_PROCESSINGREADY_GIF_038,
    &P100_PROCESSINGREADY_GIF_040, &P100_PROCESSINGREADY_GIF_042, &P100_PROCESSINGREADY_GIF_044, &P100_PROCESSINGREADY_GIF_046, &P100_PROCESSINGREADY_GIF_048,
    &P100_PROCESSINGREADY_GIF_050, 
}; // 帧数组

TS_UI_WORK g_ui_work;

void ui_work_init(void) 
{
    // 创建根容器
    ui_cont_create(&g_ui_work.cont, lv_scr_act(), 500, 500);
    ui_cont_set_pos(&g_ui_work.cont, 0, 0, LV_ALIGN_CENTER, 0);
    ui_cont_set_style(&g_ui_work.cont, 0x00000000, LV_OPA_COVER);

    // 创建任务标签
    ui_label_create(&g_ui_work.work_label, g_ui_work.cont, 200, 30,
                   ui_text_get_string_for_language("work_mode"),UI_TEXT_FONT_REGULAR, 32);
    ui_label_set_absolute_pos(&g_ui_work.work_label, 130, 550, LV_TEXT_ALIGN_CENTER);
    ui_label_set_style(&g_ui_work.work_label, 0xFFFFFFFF, 255, 0, 0, LV_LABEL_LONG_WRAP);

    // 启动工作动画（1000ms一次，无限重复）
    ui_anim_frame_init(&g_ui_work.work_anim, g_ui_work.cont, anim_work_frames, 26, 100, true);
}   

void ui_work_exit(void) 
{
    ui_anim_frame_stop(&g_ui_work.work_anim);
    if (g_ui_work.cont) {
        lv_obj_del(g_ui_work.cont);
        g_ui_work.cont = NULL;
    }
}

void ui_work_on_enter(void) 
{
    ui_anim_frame_start(&g_ui_work.work_anim);
    lv_label_set_text(g_ui_work.work_label, ui_text_get_string_for_language("work_mode"));  
}   

void ui_work_on_exit(void) 
{
    ui_anim_frame_stop(&g_ui_work.work_anim);
}