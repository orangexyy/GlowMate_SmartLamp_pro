#ifndef __UI_IDLE_H
#define __UI_IDLE_H

#include "lvgl.h"
#include "../ui_anim/ui_anim.h"

// 闲置页面结构体
typedef struct {
    lv_obj_t *cont;               // 根容器
    lv_obj_t *idle_label;         // 闲置提示标签
    TS_UI_ANIM_FRAME_CONTROL idle_anim; // 闲置动画
} TS_UI_IDLE;

// 页面生命周期函数声明
void ui_idle_init(void);
void ui_idle_exit(void);
void ui_idle_on_enter(void);
void ui_idle_on_exit(void);

extern TS_UI_IDLE g_ui_idle;

#endif