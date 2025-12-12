#ifndef __UI_HOME_H
#define __UI_HOME_H

#include "lvgl.h"
#include "ui_anim.h"

// 主页页面结构体（存储UI组件和动画状态）
typedef struct {
    lv_obj_t *cont;                     // 根容器
    lv_obj_t *home_label;               // 主页标签
    TS_UI_ANIM_FRAME_CONTROL home_anim; // 主页动画
} TS_UI_HOME;

// 页面生命周期函数声明
void ui_home_init(void);
void ui_home_exit(void);
void ui_home_on_enter(void);
void ui_home_on_exit(void);

// 外部全局变量声明（供页面管理器调用）
extern TS_UI_HOME g_ui_home;

#endif



 

