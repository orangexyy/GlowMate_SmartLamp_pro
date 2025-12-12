#ifndef __UI_WEATHER_H
#define __UI_WEATHER_H

#include "lvgl.h"
#include "ui_anim.h"

// 主页页面结构体（存储UI组件和动画状态）
typedef struct {
    lv_obj_t *cont;                     // 根容器
    lv_obj_t *weather_label;               // 主页标签
    lv_obj_t *weather_img;                 // 主页图片
    TS_UI_ANIM_FRAME_CONTROL weather_anim; // 主页动画
} TS_UI_WEATHER;

// 页面生命周期函数声明
void ui_weather_init(void);
void ui_weather_exit(void);
void ui_weather_on_enter(void);
void ui_weather_on_exit(void);

// 外部全局变量声明（供页面管理器调用）
extern TS_UI_WEATHER g_ui_weather;

#endif



 

