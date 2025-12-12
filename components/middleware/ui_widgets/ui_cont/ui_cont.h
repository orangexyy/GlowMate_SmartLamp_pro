#ifndef __UI_CONT_H
#define __UI_CONT_H

#include "../lvgl/lvgl.h"


/*
* 创建容器
* @param cont 容器指针
* @param parent 父对象
* @param width 宽度
* @param height 高度
* @return 无返回值
*/
void ui_cont_create(lv_obj_t **cont, lv_obj_t * parent, uint16_t width, uint16_t height);

/*
* 设置容器位置
* @param cont 容器指针
* @param x_pos x坐标
* @param y_pos y坐标
* @param align 对齐方式
* @param flag 标志位
* @return 无返回值
*/
void ui_cont_set_pos(lv_obj_t **cont, uint16_t x_pos, uint16_t y_pos, lv_align_t align, uint8_t flag);

/*
* 设置容器样式
* @param cont 容器指针
* @param bg_color 背景颜色
* @param bg_opa 背景透明度
* @return 无返回值
*/
void ui_cont_set_style(lv_obj_t **cont, uint32_t bg_color, uint8_t bg_opa);


#endif

