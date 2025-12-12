#include "../lvgl/lvgl.h"
#include "ui_cont/ui_cont.h"
#include "ui_label/ui_label.h"
#include "ui_label/ui_text_font.h"
#include "ui_label/ui_text_language.h"
#include "ui_img/ui_img.h"
#include "ui_button/ui_button.h"
#include "ui_anim/ui_anim.h"
#include "ui_page_manager/ui_page_manager.h"   
#include "ui_page_manager/ui_auto.h"
#include "ui_page_manager/ui_idle.h"
#include "ui_page_manager/ui_error.h"
#include "ui_page_manager/ui_work.h"


LV_IMAGE_DECLARE(P100_FEED2_GIF_00);
extern const lv_img_dsc_t P100_FEED2_GIF_00;


static lv_obj_t * cont = NULL;
static lv_obj_t * cont1 = NULL;
static lv_obj_t * label1 = NULL;
static lv_obj_t * img = NULL;
lv_timer_t *timer = NULL;

static lv_obj_t * button_to_auto = NULL;
static lv_obj_t * button_to_error = NULL;
static lv_obj_t * button_to_idle = NULL;
static lv_obj_t * button_to_work = NULL;

static lv_obj_t * button_to_auto_label = NULL;
static lv_obj_t * button_to_error_label = NULL;
static lv_obj_t * button_to_idle_label = NULL;
static lv_obj_t * button_to_work_label = NULL;

static TS_UI_ANIM_FRAME_CONTROL my_anim;

const char *btn_texts[UI_PAGE_ID_MAX] = {
    "None", "To Auto", "To Error", "To Idle", "To Work"
};



// 按钮回调函数：切换到目标页面
static void btn_switch_page_handler(lv_event_t *e) 
{
    if (e == NULL) {
        return;  
    }
    TE_UI_PAGE_ID target_page = (TE_UI_PAGE_ID)lv_event_get_user_data(e);
    if (target_page == UI_PAGE_ID_NONE) {
        return;  
    }
    ui_page_switch(target_page);  // 调用页面管理器切换页面
}


void test(void)
{
    ui_text_language_init();

    // ui_cont_create(&cont, lv_screen_active(), 100, 200);
    // ui_cont_set_pos(&cont, 0, 0, LV_ALIGN_CENTER, 1);
    // ui_cont_set_style(&cont, 0, 0);

    // ui_img_create(&img, cont1, &P100_FEED2_GIF_00, P100_FEED2_GIF_00.header.w, P100_FEED2_GIF_00.header.h);
    // ui_img_set_pos(&img, 0, 0, LV_ALIGN_CENTER, 1);

    // // 动画背景完全透明（即使图片资源有瑕疵，也能兜底）
    // lv_obj_set_style_bg_opa(img, 0, LV_PART_MAIN);
    // lv_obj_set_style_bg_color(img, lv_color_hex(0x00000000), LV_PART_MAIN); // 透明色
    // lv_obj_set_style_opa(img, 100, LV_PART_MAIN);

    // ui_label_create(&label1, cont, 100, 100, "string1", UI_TEXT_FONT_REGULAR, 38);
    // ui_label_set_relative_pos(&label1, LV_ALIGN_CENTER, 0, 0, LV_TEXT_ALIGN_CENTER);
    // ui_label_set_style(&label1, 0, 255, 0, 100, LV_LABEL_LONG_WRAP);

    // timer = lv_timer_create(img_refresh_cb, 1000, NULL);
    // lv_timer_set_repeat_count(timer, -1);  // -1 表示无限循环


    // TS_UI_ANIM_SINGLE_CONTROL fade_anim;
    // ui_anim_fade(&fade_anim, img, 0, 255, 1000, 0, 5);

    // TS_UI_ANIM_SINGLE_CONTROL move_anim;
    // ui_anim_move(&move_anim, img, 0, 0, 200, 200, 1000, 0, 5);

    // TS_UI_ANIM_SINGLE_CONTROL scale_anim;
    // ui_anim_scale(&scale_anim, img, 0, 0, 200, 200, 1000, 0, 5);

    // ui_cont_create(&cont1, lv_screen_active(), 500, 500);
    // ui_cont_set_pos(&cont1, 0, 0, LV_ALIGN_CENTER, 0);
    // ui_cont_set_style(&cont1, 0, 255);

    // TS_UI_ANIM_FRAME_CONTROL my_anim; // 定义动画控制结构体
    // 初始化：父对象为当前屏幕，帧数组为anim_frames，3帧，每帧100ms，循环播放
    // ui_anim_frame_init(&my_anim, cont1, anim_frames, 68, 100, true);
    // ui_anim_frame_start(&my_anim);

    // 场景1：从第1帧顺序播放到第3帧（1→2→3）
    // ui_anim_frame_play_range(&my_anim, 50, 1, UI_ANIM_DIR_REVERSE);
    // ui_anim_frame_exit_range_mode(&my_anim);
    // ui_anim_frame_start(&my_anim);  // 启动播放

    
    ui_button_create(&button_to_auto, lv_scr_act(), 80, 50);  // 创建按钮
    ui_button_set_pos(&button_to_auto, 0, 600, LV_ALIGN_DEFAULT, 0);  // 横向排列
    ui_button_set_style(&button_to_auto, 0xFF0000FF, 255);  // 设置按钮样式
    ui_button_set_callback(&button_to_auto, btn_switch_page_handler, (void *)UI_PAGE_ID_AUTO);  // 绑定回调
    ui_label_create(&button_to_auto_label, button_to_auto, 100, 50, "auto_mode", UI_TEXT_FONT_REGULAR, 32);
    ui_label_set_relative_pos(&button_to_auto_label, LV_ALIGN_CENTER, 0, 0, LV_TEXT_ALIGN_CENTER);    


    ui_button_create(&button_to_error, lv_scr_act(), 80, 50);  // 创建按钮
    ui_button_set_pos(&button_to_error, 150, 600, LV_ALIGN_DEFAULT, 0);  // 横向排列
    ui_button_set_style(&button_to_error, 0xFF0000FF, 255);  // 设置按钮样式
    ui_button_set_callback(&button_to_error, btn_switch_page_handler, (void *)UI_PAGE_ID_ERROR);  // 绑定回调
    ui_label_create(&button_to_error_label, button_to_error, 100, 50, "error_mode", UI_TEXT_FONT_REGULAR, 32);
    ui_label_set_relative_pos(&button_to_error_label, LV_ALIGN_CENTER, 0, 0, LV_TEXT_ALIGN_CENTER);    

    ui_button_create(&button_to_idle, lv_scr_act(), 80, 50);  // 创建按钮
    ui_button_set_pos(&button_to_idle, 300, 600, LV_ALIGN_DEFAULT, 0);  // 横向排列
    ui_button_set_style(&button_to_idle, 0xFF0000FF, 255);  // 设置按钮样式
    ui_button_set_callback(&button_to_idle, btn_switch_page_handler, (void *)UI_PAGE_ID_IDLE);  // 绑定回调
    ui_label_create(&button_to_idle_label, button_to_idle, 100, 50, "idle_mode", UI_TEXT_FONT_REGULAR, 32);
    ui_label_set_relative_pos(&button_to_idle_label, LV_ALIGN_CENTER, 0, 0, LV_TEXT_ALIGN_CENTER);    

    ui_button_create(&button_to_work, lv_scr_act(), 80, 50);  // 创建按钮
    ui_button_set_pos(&button_to_work, 450, 600, LV_ALIGN_DEFAULT, 0);  // 横向排列
    ui_button_set_style(&button_to_work, 0xFF0000FF, 255);  // 设置按钮样式
    ui_button_set_callback(&button_to_work, btn_switch_page_handler, (void *)UI_PAGE_ID_WORK);  // 绑定回调
    ui_label_create(&button_to_work_label, button_to_work, 100, 50, "work_mode", UI_TEXT_FONT_REGULAR, 32);
    ui_label_set_relative_pos(&button_to_work_label, LV_ALIGN_CENTER, 0, 0, LV_TEXT_ALIGN_CENTER);    

    ui_page_manager_init();
    ui_page_register(UI_PAGE_ID_AUTO, (ui_page_init_func)ui_auto_init, (ui_page_exit_func)ui_auto_exit, (ui_page_on_enter_callback_func)ui_auto_on_enter, (ui_page_on_exit_callback_func)ui_auto_on_exit);
    ui_page_register(UI_PAGE_ID_ERROR, (ui_page_init_func)ui_error_init, (ui_page_exit_func)ui_error_exit, (ui_page_on_enter_callback_func)ui_error_on_enter, (ui_page_on_exit_callback_func)ui_error_on_exit);
    ui_page_register(UI_PAGE_ID_IDLE, (ui_page_init_func)ui_idle_init, (ui_page_exit_func)ui_idle_exit, (ui_page_on_enter_callback_func)ui_idle_on_enter, (ui_page_on_exit_callback_func)ui_idle_on_exit);
    ui_page_register(UI_PAGE_ID_WORK, (ui_page_init_func)ui_work_init, (ui_page_exit_func)ui_work_exit, (ui_page_on_enter_callback_func)ui_work_on_enter, (ui_page_on_exit_callback_func)ui_work_on_exit);
    ui_page_switch(UI_PAGE_ID_IDLE);
    // ui_page_switch(UI_PAGE_ID_AUTO);
    // ui_page_switch(UI_PAGE_ID_ERROR);
    // ui_page_switch(UI_PAGE_ID_WORK);

}
