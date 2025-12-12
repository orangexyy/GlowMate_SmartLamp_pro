#ifndef __UI_WORK_H
#define __UI_WORK_H

#include "lvgl.h"
#include "../ui_anim/ui_anim.h"

// 工作页面结构体
typedef struct {
    lv_obj_t *cont;                     // 根容器
    lv_obj_t *work_label;               // 任务标签   
    TS_UI_ANIM_FRAME_CONTROL work_anim; // 工作动画
} TS_UI_WORK;

// 页面生命周期函数声明
void ui_work_init(void);
void ui_work_exit(void);
void ui_work_on_enter(void);
void ui_work_on_exit(void);

extern TS_UI_WORK g_ui_work;

#endif