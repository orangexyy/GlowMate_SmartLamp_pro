#ifndef __UI_LABEL_H
#define __UI_LABEL_H

#include "../lvgl/lvgl.h"

#include "ui_text_language.h"
#include "ui_text_font.h"


/*
* 创建标签
* @param label 标签指针
* @param parent 父对象
* @param width 宽度
* @param height 高度
* @param text 文本
* @param font_type 字体类型
* @param font_size 字体大小
* @return 无返回值
*/
void ui_label_create(lv_obj_t **label, lv_obj_t * parent,uint16_t width, uint16_t height, 
                        const char *text, TE_UI_TEXT_FONT_TYPE font_type, uint8_t font_size);

/*
* 设置标签位置
* @param label 标签指针
* @param x_pos x坐标
* @param y_pos y坐标
* @param value 对齐方式
* @return 无返回值
*/
void ui_label_set_absolute_pos(lv_obj_t **label, uint16_t x_pos, uint16_t y_pos, lv_text_align_t value);

/*
* 设置标签位置
* @param label 标签指针
* @param align 对齐方式
* @param x_ofs x偏移量
* @param y_ofs y偏移量
* @param value 对齐方式
* @return 无返回值
*/
void ui_label_set_relative_pos(lv_obj_t **label, lv_align_t align, uint16_t x_ofs, uint16_t y_ofs, lv_text_align_t value);

/*
* 设置标签样式
* @param label 标签指针
* @param text_color 文本颜色
* @param text_opa 文本透明度
* @param bg_color 背景颜色
* @param bg_opa 背景透明度
* @param long_mode 长文本模式
* @return 无返回值
*/
void ui_label_set_style(lv_obj_t **label, uint32_t text_color, uint8_t text_opa, 
                            uint32_t bg_color, uint8_t bg_opa, lv_label_long_mode_t long_mode);


#endif
