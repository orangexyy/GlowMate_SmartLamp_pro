#include "../lvgl/lvgl.h"
#include "ui_img.h"

/*
* 创建图片
* @param img 图片指针
* @param parent 父对象
* @param src 图片源
* @param width 宽度
* @param height 高度
* @return 无返回值
*/
void ui_img_create(lv_obj_t **img, lv_obj_t * parent, const lv_img_dsc_t * src, uint16_t width, uint16_t height)
{
    if (parent == NULL) {
        parent = lv_scr_act();  
    }

    *img = lv_img_create(parent);
    if (*img == NULL) {
        return;  
    }
    lv_img_set_src(*img, src);
    lv_obj_set_size(*img, width, height);
}

/*
* 设置图片位置
* @param img 图片指针
* @param x_pos x坐标
* @param y_pos y坐标
* @param align 对齐方式
* @param flag 标志位
* @return 无返回值
*/
void ui_img_set_pos(lv_obj_t **img, uint16_t x_pos, uint16_t y_pos, lv_align_t align, uint8_t flag)
{
    if (*img == NULL) {
        return;  
    }
    if (flag == 0)
    {
        lv_obj_set_pos(*img, x_pos, y_pos);
    }
    else
    {
        lv_obj_align(*img, align, x_pos, x_pos);
    }
}
