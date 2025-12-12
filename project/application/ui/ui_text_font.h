#ifndef __UI_TEXT_FONT_H
#define __UI_TEXT_FONT_H

#include "lvgl.h"

// 字体定义
typedef enum
{
    UI_TEXT_FONT_REGULAR = 0, 
    UI_TEXT_FONT_MEDIUM,
    UI_TEXT_FONT_BOLD

}TE_UI_TEXT_FONT_TYPE;

/*
* 获取字体类型
* @param font_type 字体类型
* @param font_size 字体大小
* @return 字体类型
*/
lv_font_t *ui_text_get_font_type(TE_UI_TEXT_FONT_TYPE font_type, uint8_t font_size);


#endif

