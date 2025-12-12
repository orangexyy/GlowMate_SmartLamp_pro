#ifndef __UI_IMG_H
#define __UI_IMG_H

#include "../lvgl/lvgl.h"

/*
* 创建图片
* @param img 图片指针
* @param parent 父对象
* @param src 图片源
* @param width 宽度
* @param height 高度
* @return 无返回值
*/
void ui_img_create(lv_obj_t **img, lv_obj_t * parent, const lv_img_dsc_t * src, uint16_t width, uint16_t height);

/*
* 设置图片位置
* @param img 图片指针
* @param x_pos x坐标
* @param y_pos y坐标
* @param align 对齐方式
* @param flag 标志位
* @return 无返回值
*/
void ui_img_set_pos(lv_obj_t **img, uint16_t x_pos, uint16_t y_pos, lv_align_t align, uint8_t flag);



#endif
