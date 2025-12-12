#include "../lvgl/lvgl.h"
#include "ui_cont.h"

/*
* 创建容器
* @param cont 容器指针
* @param parent 父对象
* @param width 宽度
* @param height 高度
* @return 无返回值
*/
void ui_cont_create(lv_obj_t **cont, lv_obj_t * parent, uint16_t width, uint16_t height)
{
    if (parent == NULL) {
        parent = lv_scr_act();  
    }

    *cont = lv_obj_create(parent);
    if (*cont == NULL) {
        return;  
    }

    lv_obj_set_size(*cont, width, height);
}

/*
* 设置容器位置
* @param cont 容器指针
* @param x_pos x坐标
* @param y_pos y坐标
* @param align 对齐方式
* @param flag 标志位
* @return 无返回值
*/
void ui_cont_set_pos(lv_obj_t **cont, uint16_t x_pos, uint16_t y_pos, lv_align_t align, uint8_t flag)
{
    if (*cont == NULL) {
        return;  
    }
    if (flag == 0)
    {
        lv_obj_set_pos(*cont, x_pos, y_pos);
    }
    else
    {
        lv_obj_align(*cont, align, x_pos, x_pos);
    }
}


/*
* 设置容器样式
* @param cont 容器指针
* @param bg_color 背景颜色
* @param bg_opa 背景透明度
* @return 无返回值
*/
void ui_cont_set_style(lv_obj_t **cont, uint32_t bg_color, uint8_t bg_opa)
{
    if (*cont == NULL) {
        return;  
    }

    //设置背景颜色
    lv_obj_set_style_bg_color(*cont, lv_color_hex(bg_color), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(*cont, bg_opa, LV_PART_MAIN);  
}
