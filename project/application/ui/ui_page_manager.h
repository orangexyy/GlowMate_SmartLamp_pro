#ifndef __UI_PAGE_MANAGER_H
#define __UI_PAGE_MANAGER_H

#include "lvgl.h"

// 页面生命周期函数原型
typedef void (*ui_page_init_func)(lv_obj_t *root);      // 初始化页面组件
typedef void (*ui_page_exit_func)(void);                // 释放页面资源
typedef void (*ui_page_on_enter_callback_func)(void);   // 进入页面时回调
typedef void (*ui_page_on_exit_callback_func)(void);    // 退出页面时回调

// 页面ID枚举（根据项目实际页面扩展）
typedef enum {
    UI_PAGE_ID_NONE = 0,
    UI_PAGE_ID_HOME,
    UI_PAGE_ID_WEATHER,
    UI_PAGE_ID_MAX  // 页面总数标记
} TE_UI_PAGE_ID;


// 页面结构体定义
typedef struct {
    TE_UI_PAGE_ID id;                              // 页面ID
    lv_obj_t *root;                                // 页面根容器
    ui_page_init_func init;                        // 初始化函数
    ui_page_exit_func exit;                        // 退出函数
    ui_page_on_enter_callback_func on_enter;       // 进入回调
    ui_page_on_exit_callback_func on_exit;         // 退出回调
    bool is_inited;                                // 是否已初始化
} TS_UI_PAGE_CONTROL;

// 页面管理器结构体
typedef struct {
    TS_UI_PAGE_CONTROL pages[UI_PAGE_ID_MAX];      // 页面数组
    TE_UI_PAGE_ID cur_page;                        // 当前激活页面
    TE_UI_PAGE_ID pre_page;                        // 上一个页面
} TS_UI_PAGE_MANAGER_CONTROL;

/*
* 初始化页面管理器
* @param 无参数
* @return 无返回值
*/
void ui_page_manager_init(void);

/*
* 注册页面（需在初始化后、切换前调用）
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
                  ui_page_on_exit_callback_func on_exit);

/*
* 切换页面（销毁当前页面，初始化并显示目标页面）
* @param target_id 目标页面ID
* @return 是否成功
*/
bool ui_page_switch(TE_UI_PAGE_ID target_id);

/*
* 获取当前页面ID
* @param 无参数
* @return 当前页面ID
*/
TE_UI_PAGE_ID ui_page_get_current(void);

/*
* 获取上一个页面ID
* @param 无参数
* @return 上一个页面ID
*/
TE_UI_PAGE_ID ui_page_get_prev(void);

/*
* 销毁页面所有组件（内部使用）
* @param root 页面根容器
* @return 无返回值
*/
void ui_page_destroy_components(lv_obj_t *root);

#endif // __UI_PAGE_MANAGER_H

