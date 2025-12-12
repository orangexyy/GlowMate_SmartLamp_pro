#include "lvgl.h"
#include "ui_text_font.h"


/*
* 获取字体类型
* @param font_type 字体类型
* @param font_size 字体大小
* @return 字体类型
*/
lv_font_t *ui_text_get_font_type(TE_UI_TEXT_FONT_TYPE font_type, uint8_t font_size)
{
    lv_font_t *p_font_type = NULL;
    switch (font_type)
    {
    case UI_TEXT_FONT_REGULAR:
        switch (font_size)
        {
        case 14:
            p_font_type = (lv_font_t *)(&lv_font_montserrat_14);
            break;
        case 32:
            p_font_type = (lv_font_t *)(&lv_font_montserrat_32);
            break;
        case 36:
            p_font_type = (lv_font_t *)(&lv_font_montserrat_36);
            break;  
        case 40:
            p_font_type = (lv_font_t *)(&lv_font_montserrat_40);
            break;      
        default:
            break;
        }
        break;
    case UI_TEXT_FONT_BOLD:
        switch (font_size)
        {
        case 38:
            p_font_type = (lv_font_t *)(&lv_font_montserrat_38);
            break;
        case 48:
            p_font_type = (lv_font_t *)(&lv_font_montserrat_48);
            break;      
        default:
            break;
        }
        break;
    default:
        break;
    }    
    return p_font_type;
}

