#ifndef __UI_BUTTON_H
#define __UI_BUTTON_H

#include "../lvgl/lvgl.h"

/*
* 创建按钮
* @param button 按钮指针
* @param parent 父对象
* @param width 宽度
* @param height 高度
* @return 无返回值
*/
void ui_button_create(lv_obj_t **button, lv_obj_t *parent, uint16_t width, uint16_t height);

/*
* 设置按钮位置
* @param button 按钮指针
* @param x_pos x坐标
* @param y_pos y坐标
* @param align 对齐方式
* @param flag 标志位
* @return 无返回值
*/
void ui_button_set_pos(lv_obj_t **button, uint16_t x_pos, uint16_t y_pos, lv_align_t align, uint8_t flag);

/*
* 设置按钮样式
* @param button 按钮指针
* @param bg_color 背景颜色
* @param bg_opa 背景透明度
* @return 无返回值
*/
void ui_button_set_style(lv_obj_t **button, uint32_t bg_color, uint8_t bg_opa);

/*
* 绑定按钮回调函数
* @param button 按钮指针
* @param cb 回调函数
* @param user_data 用户数据
* @return 无返回值
*/
void ui_button_set_callback(lv_obj_t **button, lv_event_cb_t cb, void *user_data);

#endif