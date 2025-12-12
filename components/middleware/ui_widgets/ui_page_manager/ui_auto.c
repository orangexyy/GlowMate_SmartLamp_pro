#include "ui_auto.h"
#include "../lvgl/lvgl.h"
#include "../ui_cont/ui_cont.h"
#include "../ui_label/ui_label.h"
#include "../ui_label/ui_text_font.h"
#include "../ui_label/ui_text_language.h"
#include "../ui_img/ui_img.h"
#include "../ui_anim/ui_anim.h"
#include "../ui_page_manager/ui_page_manager.h"  

LV_IMAGE_DECLARE(P100_AUTO2_GIF_000);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_002);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_004);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_006);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_008);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_010);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_012);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_014);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_016);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_018);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_020);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_022);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_024);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_026);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_028);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_030);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_032);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_034);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_036);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_038);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_040);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_042);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_046);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_052);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_054);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_056);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_058);
LV_IMAGE_DECLARE(P100_AUTO2_GIF_060);


extern const lv_img_dsc_t P100_AUTO2_GIF_000;
extern const lv_img_dsc_t P100_AUTO2_GIF_002;
extern const lv_img_dsc_t P100_AUTO2_GIF_004;
extern const lv_img_dsc_t P100_AUTO2_GIF_006;
extern const lv_img_dsc_t P100_AUTO2_GIF_008;
extern const lv_img_dsc_t P100_AUTO2_GIF_010;
extern const lv_img_dsc_t P100_AUTO2_GIF_012;
extern const lv_img_dsc_t P100_AUTO2_GIF_014;
extern const lv_img_dsc_t P100_AUTO2_GIF_016;
extern const lv_img_dsc_t P100_AUTO2_GIF_018;
extern const lv_img_dsc_t P100_AUTO2_GIF_020;
extern const lv_img_dsc_t P100_AUTO2_GIF_022;
extern const lv_img_dsc_t P100_AUTO2_GIF_024;
extern const lv_img_dsc_t P100_AUTO2_GIF_026;
extern const lv_img_dsc_t P100_AUTO2_GIF_028;
extern const lv_img_dsc_t P100_AUTO2_GIF_030;
extern const lv_img_dsc_t P100_AUTO2_GIF_032;
extern const lv_img_dsc_t P100_AUTO2_GIF_034;
extern const lv_img_dsc_t P100_AUTO2_GIF_036;
extern const lv_img_dsc_t P100_AUTO2_GIF_038;
extern const lv_img_dsc_t P100_AUTO2_GIF_040;
extern const lv_img_dsc_t P100_AUTO2_GIF_042;
extern const lv_img_dsc_t P100_AUTO2_GIF_046;
extern const lv_img_dsc_t P100_AUTO2_GIF_052;
extern const lv_img_dsc_t P100_AUTO2_GIF_054;
extern const lv_img_dsc_t P100_AUTO2_GIF_056;
extern const lv_img_dsc_t P100_AUTO2_GIF_058;
extern const lv_img_dsc_t P100_AUTO2_GIF_060;


const lv_img_dsc_t *anim_auto_frames[] = {
    &P100_AUTO2_GIF_000, &P100_AUTO2_GIF_002, &P100_AUTO2_GIF_004, &P100_AUTO2_GIF_006, &P100_AUTO2_GIF_008,
    &P100_AUTO2_GIF_010, &P100_AUTO2_GIF_012, &P100_AUTO2_GIF_014, &P100_AUTO2_GIF_016, &P100_AUTO2_GIF_018,
    &P100_AUTO2_GIF_020, &P100_AUTO2_GIF_022, &P100_AUTO2_GIF_024, &P100_AUTO2_GIF_026, &P100_AUTO2_GIF_028,
    &P100_AUTO2_GIF_030, &P100_AUTO2_GIF_032, &P100_AUTO2_GIF_034, &P100_AUTO2_GIF_036, &P100_AUTO2_GIF_038,
    &P100_AUTO2_GIF_040, &P100_AUTO2_GIF_042, &P100_AUTO2_GIF_046, &P100_AUTO2_GIF_052, &P100_AUTO2_GIF_054,
    &P100_AUTO2_GIF_056, &P100_AUTO2_GIF_058, &P100_AUTO2_GIF_060, 
}; // 帧数组

// 全局页面对象
TS_UI_AUTO g_ui_auto;

// 初始化页面（创建UI组件，仅调用一次）
void ui_auto_init(void) 
{
    // 创建根容器
    ui_cont_create(&g_ui_auto.cont, lv_scr_act(), 500, 500);
    ui_cont_set_pos(&g_ui_auto.cont, 0, 0, LV_ALIGN_CENTER, 0);
    ui_cont_set_style(&g_ui_auto.cont, 0x00000000, LV_OPA_COVER);

    // 创建标题标签（多语言文本）
    ui_label_create(&g_ui_auto.auto_label, g_ui_auto.cont, 200, 30,
                   ui_text_get_string_for_language("auto_mode"),UI_TEXT_FONT_REGULAR, 32);
    ui_label_set_absolute_pos(&g_ui_auto.auto_label, 130, 550, LV_TEXT_ALIGN_CENTER);
    ui_label_set_style(&g_ui_auto.auto_label, 0xFFFFFFFF, 255, 0, 0, LV_LABEL_LONG_WRAP);

    // 启动闲置动画（1000ms一次，无限重复）
    ui_anim_frame_init(&g_ui_auto.auto_anim, g_ui_auto.cont, anim_auto_frames, 28, 100, true);
}

// 退出页面（释放资源，仅销毁时调用）
void ui_auto_exit(void) 
{
    // 停止动画
    ui_anim_frame_stop(&g_ui_auto.auto_anim);
    // 销毁UI组件（容器会递归删除子对象）
    if (g_ui_auto.cont) {
        lv_obj_del(g_ui_auto.cont);
        g_ui_auto.cont = NULL;
    }
}

// 进入页面回调（每次显示时调用）
void ui_auto_on_enter(void) 
{
    // 启动标题淡入动画
    ui_anim_frame_start(&g_ui_auto.auto_anim);
    // 更新自动文本（读取最新语言配置）
    lv_label_set_text(g_ui_auto.auto_label, ui_text_get_string_for_language("auto_mode"));
}   

// 退出页面回调（每次隐藏时调用）
void ui_auto_on_exit(void) 
{
    // 停止动画（避免后台运行）
    ui_anim_frame_stop(&g_ui_auto.auto_anim);
}