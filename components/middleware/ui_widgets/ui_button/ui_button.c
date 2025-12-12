#include "ui_button.h"
#include "../ui_label/ui_label.h"
#include "../lvgl/lvgl.h"

/*
* 创建按钮
* @param button 按钮指针
* @param parent 父对象
* @param width 宽度
* @param height 高度
* @return 无返回值
*/
void ui_button_create(lv_obj_t **button, lv_obj_t *parent, uint16_t width, uint16_t height) 
{
    if (parent == NULL) {
        parent = lv_scr_act();  
    }
    if (*button == NULL) {
        *button = lv_button_create(parent);
        return;  
    }
    lv_obj_set_size(*button, width, height);
}

/*
* 设置按钮位置
* @param button 按钮指针
* @param x_pos x坐标
* @param y_pos y坐标
* @param align 对齐方式
* @param flag 标志位
* @return 无返回值
*/
void ui_button_set_pos(lv_obj_t **button, uint16_t x_pos, uint16_t y_pos, lv_align_t align, uint8_t flag)
{
    if (*button == NULL) {
        return;  
    }
    if (flag == 0)
    {
        lv_obj_set_pos(*button, x_pos, y_pos);
    }
    else
    {
        lv_obj_align(*button, align, x_pos, y_pos);
    }
}

/*
* 设置按钮样式
* @param button 按钮指针
* @param bg_color 背景颜色
* @param bg_opa 背景透明度
* @return 无返回值
*/
void ui_button_set_style(lv_obj_t **button, uint32_t bg_color, uint8_t bg_opa)
{
    if (*button == NULL) {
        return;  
    }

    //设置背景颜色
    lv_obj_set_style_bg_color(*button, lv_color_hex(bg_color), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(*button, bg_opa, LV_PART_MAIN); 
}

    
/*
* 绑定按钮回调函数
* @param button 按钮指针
* @param cb 回调函数
* @param user_data 用户数据
* @return 无返回值
*/
void ui_button_set_callback(lv_obj_t **button, lv_event_cb_t cb, void *user_data) 
{
    if (*button == NULL) {
        return;  
    }
    lv_obj_add_event_cb(*button, cb, LV_EVENT_CLICKED, user_data);
}