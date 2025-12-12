#ifndef __UI_ERROR_H
#define __UI_ERROR_H

#include "lvgl.h"
#include "../ui_anim/ui_anim.h"

// 错误页面结构体
typedef struct {
    lv_obj_t *cont;                         // 根容器
    lv_obj_t *error_label;                  // 错误标签
    lv_obj_t *error_img;                    // 错误图片
    TS_UI_ANIM_FRAME_CONTROL error_anim;    // 错误动画
} TS_UI_ERROR;

// 页面生命周期函数声明
void ui_error_init(void);
void ui_error_exit(void);
void ui_error_on_enter(void);
void ui_error_on_exit(void);

extern TS_UI_ERROR g_ui_error;

#endif