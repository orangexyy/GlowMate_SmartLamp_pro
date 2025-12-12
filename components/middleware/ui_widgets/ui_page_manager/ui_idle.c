#include "ui_idle.h"
#include "../lvgl/lvgl.h"
#include "../ui_cont/ui_cont.h"
#include "../ui_label/ui_label.h"
#include "../ui_label/ui_text_font.h"
#include "../ui_label/ui_text_language.h"
#include "../ui_img/ui_img.h"
#include "../ui_anim/ui_anim.h"
#include "../ui_page_manager/ui_page_manager.h"  

LV_IMAGE_DECLARE(P100_START_GIF_012);
LV_IMAGE_DECLARE(P100_START_GIF_014);
LV_IMAGE_DECLARE(P100_START_GIF_016);
LV_IMAGE_DECLARE(P100_START_GIF_018);
LV_IMAGE_DECLARE(P100_START_GIF_020);
LV_IMAGE_DECLARE(P100_START_GIF_022);
LV_IMAGE_DECLARE(P100_START_GIF_024);
LV_IMAGE_DECLARE(P100_START_GIF_026);
LV_IMAGE_DECLARE(P100_START_GIF_028);
LV_IMAGE_DECLARE(P100_START_GIF_030);
LV_IMAGE_DECLARE(P100_START_GIF_032);
LV_IMAGE_DECLARE(P100_START_GIF_034);
LV_IMAGE_DECLARE(P100_START_GIF_036);
LV_IMAGE_DECLARE(P100_START_GIF_038);
LV_IMAGE_DECLARE(P100_START_GIF_040);
LV_IMAGE_DECLARE(P100_START_GIF_042);
LV_IMAGE_DECLARE(P100_START_GIF_044);
LV_IMAGE_DECLARE(P100_START_GIF_046);
LV_IMAGE_DECLARE(P100_START_GIF_048);
LV_IMAGE_DECLARE(P100_START_GIF_050);
LV_IMAGE_DECLARE(P100_START_GIF_052);
LV_IMAGE_DECLARE(P100_START_GIF_054);
LV_IMAGE_DECLARE(P100_START_GIF_056);
LV_IMAGE_DECLARE(P100_START_GIF_058);
LV_IMAGE_DECLARE(P100_START_GIF_060);
LV_IMAGE_DECLARE(P100_START_GIF_062);
LV_IMAGE_DECLARE(P100_START_GIF_064);
LV_IMAGE_DECLARE(P100_START_GIF_066);
LV_IMAGE_DECLARE(P100_START_GIF_068);
LV_IMAGE_DECLARE(P100_START_GIF_070);
LV_IMAGE_DECLARE(P100_START_GIF_072);
LV_IMAGE_DECLARE(P100_START_GIF_074);
LV_IMAGE_DECLARE(P100_START_GIF_076);
LV_IMAGE_DECLARE(P100_START_GIF_078);
LV_IMAGE_DECLARE(P100_START_GIF_080);
LV_IMAGE_DECLARE(P100_START_GIF_082);
LV_IMAGE_DECLARE(P100_START_GIF_084);
LV_IMAGE_DECLARE(P100_START_GIF_086);
LV_IMAGE_DECLARE(P100_START_GIF_088);
LV_IMAGE_DECLARE(P100_START_GIF_090);
LV_IMAGE_DECLARE(P100_START_GIF_092);
LV_IMAGE_DECLARE(P100_START_GIF_094);
LV_IMAGE_DECLARE(P100_START_GIF_096);
LV_IMAGE_DECLARE(P100_START_GIF_098);
LV_IMAGE_DECLARE(P100_START_GIF_100);
LV_IMAGE_DECLARE(P100_START_GIF_102);
LV_IMAGE_DECLARE(P100_START_GIF_104);
LV_IMAGE_DECLARE(P100_START_GIF_106);
LV_IMAGE_DECLARE(P100_START_GIF_108);
LV_IMAGE_DECLARE(P100_START_GIF_110);
LV_IMAGE_DECLARE(P100_START_GIF_112);
LV_IMAGE_DECLARE(P100_START_GIF_114);
LV_IMAGE_DECLARE(P100_START_GIF_116);
LV_IMAGE_DECLARE(P100_START_GIF_118);
LV_IMAGE_DECLARE(P100_START_GIF_120);
LV_IMAGE_DECLARE(P100_START_GIF_122);
LV_IMAGE_DECLARE(P100_START_GIF_124);
LV_IMAGE_DECLARE(P100_START_GIF_126);
LV_IMAGE_DECLARE(P100_START_GIF_128);
LV_IMAGE_DECLARE(P100_START_GIF_130);
LV_IMAGE_DECLARE(P100_START_GIF_132);
LV_IMAGE_DECLARE(P100_START_GIF_134);
LV_IMAGE_DECLARE(P100_START_GIF_136);
LV_IMAGE_DECLARE(P100_START_GIF_138);
LV_IMAGE_DECLARE(P100_START_GIF_140);
LV_IMAGE_DECLARE(P100_START_GIF_142);
LV_IMAGE_DECLARE(P100_START_GIF_144);
LV_IMAGE_DECLARE(P100_START_GIF_146);


extern const lv_img_dsc_t P100_START_GIF_012;
extern const lv_img_dsc_t P100_START_GIF_014;
extern const lv_img_dsc_t P100_START_GIF_016;
extern const lv_img_dsc_t P100_START_GIF_018;
extern const lv_img_dsc_t P100_START_GIF_020;
extern const lv_img_dsc_t P100_START_GIF_022;
extern const lv_img_dsc_t P100_START_GIF_024;
extern const lv_img_dsc_t P100_START_GIF_026;
extern const lv_img_dsc_t P100_START_GIF_028;
extern const lv_img_dsc_t P100_START_GIF_030;
extern const lv_img_dsc_t P100_START_GIF_032;
extern const lv_img_dsc_t P100_START_GIF_034;
extern const lv_img_dsc_t P100_START_GIF_036;
extern const lv_img_dsc_t P100_START_GIF_038;
extern const lv_img_dsc_t P100_START_GIF_040;
extern const lv_img_dsc_t P100_START_GIF_042;
extern const lv_img_dsc_t P100_START_GIF_044;
extern const lv_img_dsc_t P100_START_GIF_046;
extern const lv_img_dsc_t P100_START_GIF_048;
extern const lv_img_dsc_t P100_START_GIF_050;
extern const lv_img_dsc_t P100_START_GIF_052;
extern const lv_img_dsc_t P100_START_GIF_054;
extern const lv_img_dsc_t P100_START_GIF_056;
extern const lv_img_dsc_t P100_START_GIF_058;
extern const lv_img_dsc_t P100_START_GIF_060;
extern const lv_img_dsc_t P100_START_GIF_062;
extern const lv_img_dsc_t P100_START_GIF_064;
extern const lv_img_dsc_t P100_START_GIF_066;
extern const lv_img_dsc_t P100_START_GIF_068;
extern const lv_img_dsc_t P100_START_GIF_070;
extern const lv_img_dsc_t P100_START_GIF_072;
extern const lv_img_dsc_t P100_START_GIF_074;
extern const lv_img_dsc_t P100_START_GIF_076;
extern const lv_img_dsc_t P100_START_GIF_078;
extern const lv_img_dsc_t P100_START_GIF_080;
extern const lv_img_dsc_t P100_START_GIF_082;
extern const lv_img_dsc_t P100_START_GIF_084;
extern const lv_img_dsc_t P100_START_GIF_086;
extern const lv_img_dsc_t P100_START_GIF_088;
extern const lv_img_dsc_t P100_START_GIF_090;
extern const lv_img_dsc_t P100_START_GIF_092;
extern const lv_img_dsc_t P100_START_GIF_094;
extern const lv_img_dsc_t P100_START_GIF_096;
extern const lv_img_dsc_t P100_START_GIF_098;
extern const lv_img_dsc_t P100_START_GIF_100;
extern const lv_img_dsc_t P100_START_GIF_102;
extern const lv_img_dsc_t P100_START_GIF_104;
extern const lv_img_dsc_t P100_START_GIF_106;
extern const lv_img_dsc_t P100_START_GIF_108;
extern const lv_img_dsc_t P100_START_GIF_110;
extern const lv_img_dsc_t P100_START_GIF_112;
extern const lv_img_dsc_t P100_START_GIF_114;
extern const lv_img_dsc_t P100_START_GIF_116;
extern const lv_img_dsc_t P100_START_GIF_118;
extern const lv_img_dsc_t P100_START_GIF_120;
extern const lv_img_dsc_t P100_START_GIF_122;
extern const lv_img_dsc_t P100_START_GIF_124;
extern const lv_img_dsc_t P100_START_GIF_126;
extern const lv_img_dsc_t P100_START_GIF_128;
extern const lv_img_dsc_t P100_START_GIF_130;
extern const lv_img_dsc_t P100_START_GIF_132;
extern const lv_img_dsc_t P100_START_GIF_134;
extern const lv_img_dsc_t P100_START_GIF_136;
extern const lv_img_dsc_t P100_START_GIF_138;
extern const lv_img_dsc_t P100_START_GIF_140;
extern const lv_img_dsc_t P100_START_GIF_142;
extern const lv_img_dsc_t P100_START_GIF_144;
extern const lv_img_dsc_t P100_START_GIF_146;

const lv_img_dsc_t *anim_idle_frames[] = {
    &P100_START_GIF_012, &P100_START_GIF_014, &P100_START_GIF_016, &P100_START_GIF_018, &P100_START_GIF_020,
    &P100_START_GIF_022, &P100_START_GIF_024, &P100_START_GIF_026, &P100_START_GIF_028, &P100_START_GIF_030,
    &P100_START_GIF_032, &P100_START_GIF_034, &P100_START_GIF_036, &P100_START_GIF_038, &P100_START_GIF_040,
    &P100_START_GIF_042, &P100_START_GIF_044, &P100_START_GIF_046, &P100_START_GIF_048, &P100_START_GIF_050,
    &P100_START_GIF_052, &P100_START_GIF_054, &P100_START_GIF_056, &P100_START_GIF_058, &P100_START_GIF_060,
    &P100_START_GIF_062, &P100_START_GIF_064, &P100_START_GIF_066, &P100_START_GIF_068, &P100_START_GIF_070,
    &P100_START_GIF_072, &P100_START_GIF_074, &P100_START_GIF_076, &P100_START_GIF_078, &P100_START_GIF_080,
    &P100_START_GIF_082, &P100_START_GIF_084, &P100_START_GIF_086, &P100_START_GIF_088, &P100_START_GIF_090,
    &P100_START_GIF_092, &P100_START_GIF_094, &P100_START_GIF_096, &P100_START_GIF_098, &P100_START_GIF_100,
    &P100_START_GIF_102, &P100_START_GIF_104, &P100_START_GIF_106, &P100_START_GIF_108, &P100_START_GIF_110,
    &P100_START_GIF_112, &P100_START_GIF_114, &P100_START_GIF_116, &P100_START_GIF_118, &P100_START_GIF_120,
    &P100_START_GIF_122, &P100_START_GIF_124, &P100_START_GIF_126, &P100_START_GIF_128, &P100_START_GIF_130,
    &P100_START_GIF_132, &P100_START_GIF_134, &P100_START_GIF_136, &P100_START_GIF_138, &P100_START_GIF_140,
    &P100_START_GIF_142, &P100_START_GIF_146, 
}; // 帧数组

TS_UI_IDLE g_ui_idle;

void ui_idle_init(void) 
{
    // 创建黑色背景容器
    ui_cont_create(&g_ui_idle.cont, lv_scr_act(), 500, 500);
    ui_cont_set_pos(&g_ui_idle.cont, 0, 0, LV_ALIGN_CENTER, 0);
    ui_cont_set_style(&g_ui_idle.cont, 0x00000000, LV_OPA_COVER);

    // 创建闲置提示标签
    ui_label_create(&g_ui_idle.idle_label, g_ui_idle.cont, 200, 30,
                   ui_text_get_string_for_language("idle_mode"),UI_TEXT_FONT_REGULAR, 32);
    ui_label_set_absolute_pos(&g_ui_idle.idle_label, 130, 550, LV_TEXT_ALIGN_CENTER);
    ui_label_set_style(&g_ui_idle.idle_label, 0xFFFFFFFF, 255, 0, 0, LV_LABEL_LONG_WRAP);

    // 启动闲置动画（1000ms一次，无限重复）
    ui_anim_frame_init(&g_ui_idle.idle_anim, g_ui_idle.cont, anim_idle_frames, 67, 100, true);
}

void ui_idle_exit(void) 
{
    ui_anim_frame_stop(&g_ui_idle.idle_anim);
    if (g_ui_idle.cont) {
        lv_obj_del(g_ui_idle.cont);
        g_ui_idle.cont = NULL;
    }
}

void ui_idle_on_enter(void) 
{
    ui_anim_frame_start(&g_ui_idle.idle_anim);
    lv_label_set_text(g_ui_idle.idle_label, ui_text_get_string_for_language("idle_mode"));
}

void ui_idle_on_exit(void) {
    ui_anim_frame_stop(&g_ui_idle.idle_anim);
}