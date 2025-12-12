#ifndef __UI_AUTO_H
#define __UI_AUTO_H

#include "lvgl.h"
#include "../ui_anim/ui_anim.h"

// 自动模式页面结构体（存储UI组件和动画状态）
typedef struct {
    lv_obj_t *cont;                     // 根容器
    lv_obj_t *auto_label;               // 自动标签
    TS_UI_ANIM_FRAME_CONTROL auto_anim; // 自动动画
} TS_UI_AUTO;

// 页面生命周期函数声明
void ui_auto_init(void);
void ui_auto_exit(void);
void ui_auto_on_enter(void);
void ui_auto_on_exit(void);

// 外部全局变量声明（供页面管理器调用）
extern TS_UI_AUTO g_ui_auto;

#endif