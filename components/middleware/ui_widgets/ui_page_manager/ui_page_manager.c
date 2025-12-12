#include "lvgl.h"
#include "ui_page_manager.h"
#include "ui_cont.h"  // 复用容器创建接口
#include <string.h>

static TS_UI_PAGE_MANAGER_CONTROL ui_page_manager_control;  // 全局页面管理器实例

/*
* 初始化页面管理器
* @param 无参数
* @return 无返回值
*/
void ui_page_manager_init(void) 
{
    memset(&ui_page_manager_control, 0, sizeof(TS_UI_PAGE_MANAGER_CONTROL));
    ui_page_manager_control.cur_page = UI_PAGE_ID_NONE;
    ui_page_manager_control.pre_page = UI_PAGE_ID_NONE;

    // 初始化所有页面根容器
    for (TE_UI_PAGE_ID i = 0; i < UI_PAGE_ID_MAX; i++) 
    {
        ui_page_manager_control.pages[i].id         = i;
        ui_page_manager_control.pages[i].root       = NULL;
        ui_page_manager_control.pages[i].is_inited  = false;
        ui_page_manager_control.pages[i].init       = NULL;
        ui_page_manager_control.pages[i].exit       = NULL;
        ui_page_manager_control.pages[i].on_enter   = NULL;
        ui_page_manager_control.pages[i].on_exit    = NULL;
    }
}

/*
* 注册页面
* @param id 页面ID
* @param init 初始化函数
* @param exit 退出函数
* @param on_enter 进入回调
* @param on_exit 退出回调
* @return 是否成功
*/
bool ui_page_register(TE_UI_PAGE_ID id, 
                  ui_page_init_func init, 
                  ui_page_exit_func exit, 
                  ui_page_on_enter_callback_func on_enter, 
                  ui_page_on_exit_callback_func on_exit) 
{
    if (id <= UI_PAGE_ID_NONE || id >= UI_PAGE_ID_MAX) 
    {
        return false;
    }

    TS_UI_PAGE_CONTROL *ui_page_control = &ui_page_manager_control.pages[id];
    ui_page_control->init       = init;
    ui_page_control->exit       = exit;
    ui_page_control->on_enter   = on_enter;
    ui_page_control->on_exit    = on_exit;

    // 创建页面根容器（默认全屏）
    ui_cont_create(&ui_page_control->root, lv_scr_act(), LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(ui_page_control->root, LV_OBJ_FLAG_HIDDEN);  // 初始隐藏
    lv_obj_clear_flag(ui_page_control->root, LV_OBJ_FLAG_SCROLLABLE);  // 禁止滚动

    return true;
}

/*
* 销毁页面所有组件
* @param root 页面根容器
* @return 无返回值
*/
void ui_page_destroy_components(lv_obj_t *root) 
{
    if (root == NULL) 
    {
        return;
    }
    lv_obj_t *child = lv_obj_get_child(root, 0);
    while (child != NULL) 
    {
        lv_obj_t *next_child = lv_obj_get_child(root, (int32_t)(lv_obj_get_index(child) + 1));
        lv_obj_del(child);  // 递归删除所有子组件
        child = next_child;
    }
}

/*
* 切换页面
* @param target_page_id 目标页面ID
* @return 是否成功
*/
bool ui_page_switch(TE_UI_PAGE_ID target_page_id) 
{
    if (target_page_id <= UI_PAGE_ID_NONE || target_page_id >= UI_PAGE_ID_MAX) return false;
    if (target_page_id == ui_page_manager_control.cur_page) return true;  // 避免重复切换

    TS_UI_PAGE_CONTROL *target_page_control = &ui_page_manager_control.pages[target_page_id];
    if (target_page_control->init == NULL) return false;  // 未注册的页面

    // 处理当前页面退出
    if (ui_page_manager_control.cur_page != UI_PAGE_ID_NONE) 
    {
        TS_UI_PAGE_CONTROL *curr_page_control = &ui_page_manager_control.pages[ui_page_manager_control.cur_page];
        
        // 调用退出回调
        if (curr_page_control->on_exit != NULL) 
        {
            curr_page_control->on_exit();
        }

        // 销毁当前页面组件
        ui_page_destroy_components(curr_page_control->root);
        
        // 调用页面退出函数（释放额外资源）
        if (curr_page_control->exit != NULL) 
        {
            curr_page_control->exit();
        }

        // 隐藏当前页面
        lv_obj_add_flag(curr_page_control->root, LV_OBJ_FLAG_HIDDEN);
        curr_page_control->is_inited = false;

        // 记录上一页
        ui_page_manager_control.pre_page = ui_page_manager_control.cur_page;
    }

    // 初始化并显示目标页面
    // 调用页面初始化函数（创建组件）
    target_page_control->init(target_page_control->root);
    // 显示页面根容器
    lv_obj_clear_flag(target_page_control->root, LV_OBJ_FLAG_HIDDEN);
    // 标记为已初始化
    target_page_control->is_inited = true;
    // 调用进入回调
    if (target_page_control->on_enter != NULL) 
    {
        target_page_control->on_enter();
    }

    // 更新当前页面
    ui_page_manager_control.cur_page = target_page_id;
    return true;
}

/*
* 获取当前页面ID
* @param 无参数
* @return 当前页面ID
*/
TE_UI_PAGE_ID ui_page_get_current(void) 
{
    return ui_page_manager_control.cur_page;
}

/*
* 获取上一个页面ID
* @param 无参数
* @return 上一个页面ID
*/
TE_UI_PAGE_ID ui_page_get_prev(void) 
{
    return ui_page_manager_control.pre_page;
}

