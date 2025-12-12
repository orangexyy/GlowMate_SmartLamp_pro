#ifndef __UI_ANIM_H
#define __UI_ANIM_H

#include "lvgl.h"


// 单图动画类型
typedef enum {
    UI_ANIM_FADE,           // 透明度动画
    UI_ANIM_MOVE,           // 平行移动动画
    UI_ANIM_SCALE,          // 缩放动画
} TE_UI_ANIM_TYPE;

// 单图动画控制结构体
typedef struct {
    lv_obj_t *target;       // 目标图片对象
    lv_anim_t anim;         // LVGL动画对象
    TE_UI_ANIM_TYPE type;   // 动画类型
    bool is_running;        // 动画运行状态
} TS_UI_ANIM_SINGLE_CONTROL;

// 播放方向枚举（新增）
typedef enum {
    UI_ANIM_DIR_FORWARD,    // 顺序播放（帧索引递增）
    UI_ANIM_DIR_REVERSE     // 逆序播放（帧索引递减）
} TS_UI_ANIM_DIR;

// 帧动画控制结构体
typedef struct {
    lv_obj_t *img_obj;               // 图片对象
    const lv_img_dsc_t **frames;     // 帧数据数组
    uint16_t frame_count;            // 总帧数
    uint16_t current_frame;          // 当前帧索引
    uint32_t frame_delay;            // 帧间隔（ms）
    bool loop;                       // 是否循环播放
    bool is_running;                 // 是否正在播放

    // 区间播放参数
    bool in_range_mode;              // 是否处于区间播放模式
    uint16_t range_start;            // 区间起始帧x
    uint16_t range_end;              // 区间目标帧y
    TS_UI_ANIM_DIR range_dir;        // 区间播放方向

    lv_timer_t *timer;               // 定时器
} TS_UI_ANIM_FRAME_CONTROL;

// ---------------------- 单图动画API ----------------------
/*
* 透明度动画
* @param anim 动画控制结构体
* @param target 目标对象
* @param start_opa 起始透明度
* @param end_opa 结束透明度
* @param duration 动画时长
* @param delay 开始播放动画的延迟时间
* @param repeat 动画重复次数，-1表示无限循环
* @return 无返回值
*/
void ui_anim_fade(TS_UI_ANIM_SINGLE_CONTROL *anim, lv_obj_t *target,
                     uint8_t start_opa, uint8_t end_opa,
                     uint32_t duration, uint32_t delay, int32_t repeat);

/*
* 平行移动动画
* @param anim 动画控制结构体
* @param target 目标对象
* @param start_x 起始x坐标
* @param start_y 起始y坐标
* @param end_x 结束x坐标
* @param end_y 结束y坐标
* @param duration 动画时长
* @param delay 动画延迟
* @param repeat 动画重复次数
* @return 无返回值
*/
void ui_anim_move(TS_UI_ANIM_SINGLE_CONTROL *anim, lv_obj_t *target,
                     int32_t start_x, int32_t start_y,
                     int32_t end_x, int32_t end_y,
                     uint32_t duration, uint32_t delay, int32_t repeat);

/*
* 缩放动画
* @param anim 动画控制结构体
* @param target 目标对象
* @param start_w 起始宽度
* @param start_h 起始高度
* @param end_w 结束宽度
* @param end_h 结束高度
* @param duration 动画时长
* @param delay 动画延迟
* @param repeat 动画重复次数
* @return 无返回值
*/
void ui_anim_scale(TS_UI_ANIM_SINGLE_CONTROL *anim, lv_obj_t *target,
                      uint32_t start_w, uint32_t start_h,
                      uint32_t end_w, uint32_t end_h,
                      uint32_t duration, uint32_t delay, int32_t repeat);

/*
* 停止单图动画
* @param anim 动画控制结构体
* @return 无返回值
*/
void ui_anim_single_stop(TS_UI_ANIM_SINGLE_CONTROL *anim);

// ---------------------- 帧动画API ----------------------
/*
* 初始化帧动画
* @param anim 动画控制结构体
* @param parent 父对象
* @param frames 帧数据数组
* @param frame_count 帧数
* @param frame_delay 帧间隔
* @param loop 是否循环播放
* @return 成功返回true，失败返回false
*/
bool ui_anim_frame_init(TS_UI_ANIM_FRAME_CONTROL *anim, lv_obj_t *parent,
                       const lv_img_dsc_t **frames, uint16_t frame_count,
                       uint32_t frame_delay, bool loop);

/*
* 设置区间播放：从x帧按方向播放到y帧
* @param anim 动画控制结构体
* @param x 起始帧（0 <= x < frame_count）
* @param y 目标帧（0 <= y < frame_count，x != y）
* @param dir 播放方向（顺序/逆序）
* @return 成功返回true，失败返回false
*/
bool ui_anim_frame_play_range(TS_UI_ANIM_FRAME_CONTROL *anim,uint16_t x, uint16_t y, TS_UI_ANIM_DIR dir);

/*
 * 退出区间播放模式，恢复全局循环模式
 * @param anim  动画控制结构体
 * @return 无返回值
*/
void ui_anim_frame_exit_range_mode(TS_UI_ANIM_FRAME_CONTROL *anim); 

/*
* 启动/继续帧动画
* @param anim 动画控制结构体
* @return 无返回值
*/
void ui_anim_frame_start(TS_UI_ANIM_FRAME_CONTROL *anim);

/*
* 暂停帧动画
* @param anim 动画控制结构体
* @return 无返回值
*/
void ui_anim_frame_stop(TS_UI_ANIM_FRAME_CONTROL *anim);

/*
* 调整帧动画延迟（帧率）
* @param anim 动画控制结构体
* @param new_delay 新的延迟时间
* @return 无返回值
*/
void ui_anim_frame_set_delay(TS_UI_ANIM_FRAME_CONTROL *anim, uint32_t new_delay);

/*
* 隐藏/显示帧动画
* @param anim 动画控制结构体
* @param hide 是否隐藏
* @return 无返回值
*/
void ui_anim_frame_set_hide(TS_UI_ANIM_FRAME_CONTROL *anim, bool hide);

/*
* 销毁帧动画（释放资源）
* @param anim 动画控制结构体
* @return 无返回值
*/
void ui_anim_frame_del(TS_UI_ANIM_FRAME_CONTROL *anim);

#endif // UI_ANIM_H

