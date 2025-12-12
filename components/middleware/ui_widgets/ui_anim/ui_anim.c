#include "ui_anim.h"

// ---------------------- 单图动画实现 ----------------------
/*
* 统一的单图动画结束回调（更新运行状态）
* @param anim 动画控制结构体指针
* @return 无返回值
*/
static void ui_anim_single_ready_cb(lv_anim_t *anim) 
{
    TS_UI_ANIM_SINGLE_CONTROL *anim_lib = (TS_UI_ANIM_SINGLE_CONTROL *)anim->user_data;
    if(anim_lib == NULL) return;
    // 非无限循环时，动画结束后标记为未运行
    if(anim->repeat_cnt != LV_ANIM_REPEAT_INFINITE)  
    {
        anim_lib->is_running = false;
    }
}

/*
* 淡入淡出动画回调函数
* @param var 目标对象
* @param opa 目标对象的透明度
* @return 无返回值
*/
static void ui_anim_fade_cb(void *var, int32_t opa) 
{
    lv_obj_t *img = (lv_obj_t *)var;
    // 关键：指定作用于图片的 MAIN 部件
    lv_obj_set_style_opa(img, opa, LV_PART_MAIN);
}

/*
* 淡入淡出动画
* @param anim 动画控制结构体指针
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
                     uint32_t duration, uint32_t delay, int32_t repeat) 
{
    if(anim == NULL || target == NULL) return;

    anim->target = target;
    anim->type = UI_ANIM_FADE;
    anim->is_running = false;

    lv_anim_init(&anim->anim);
    lv_anim_set_var(&anim->anim, target);
    lv_anim_set_exec_cb(&anim->anim, (lv_anim_exec_xcb_t)ui_anim_fade_cb);
    lv_anim_set_values(&anim->anim, start_opa, end_opa);
    lv_anim_set_time(&anim->anim, duration);
    lv_anim_set_delay(&anim->anim, delay);
    lv_anim_set_repeat_count(&anim->anim, repeat);
    lv_anim_set_user_data(&anim->anim, anim); // 传递动画结构体
    lv_anim_set_ready_cb(&anim->anim, ui_anim_single_ready_cb);

    lv_anim_start(&anim->anim);
    anim->is_running = true;
}

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
                     uint32_t duration, uint32_t delay, int32_t repeat) 
{
    if(anim == NULL || target == NULL) return;

    anim->target = target;
    anim->type = UI_ANIM_MOVE;
    anim->is_running = false;

    // 初始化位置
    lv_obj_set_pos(target, start_x, start_y);

    // X轴动画配置
    lv_anim_init(&anim->anim);
    lv_anim_set_var(&anim->anim, target);
    lv_anim_set_exec_cb(&anim->anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&anim->anim, start_x, end_x);
    lv_anim_set_time(&anim->anim, duration);
    lv_anim_set_delay(&anim->anim, delay);
    lv_anim_set_repeat_count(&anim->anim, repeat);
    lv_anim_set_user_data(&anim->anim, anim);
    lv_anim_set_ready_cb(&anim->anim, ui_anim_single_ready_cb);

    // Y轴动画（手动复制X轴参数，替代 lv_anim_copy）
    lv_anim_t anim_y;
    lv_anim_init(&anim_y);
    // 复制X轴动画的核心参数
    lv_anim_set_var(&anim_y, target);  // 目标对象
    lv_anim_set_time(&anim_y, duration);  // 时长
    lv_anim_set_delay(&anim_y, delay);  // 延迟
    lv_anim_set_repeat_count(&anim_y, repeat);  // 重复次数
    lv_anim_set_user_data(&anim_y, anim);  // 用户数据
    lv_anim_set_ready_cb(&anim_y, ui_anim_single_ready_cb);  // 结束回调
    // Y轴特有参数（执行函数和值）
    lv_anim_set_exec_cb(&anim_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&anim_y, start_y, end_y);

    // 启动动画
    lv_anim_start(&anim_y);
    lv_anim_start(&anim->anim);
    anim->is_running = true;
}

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
                  uint32_t duration, uint32_t delay, int32_t repeat) 
{
    if(anim == NULL || target == NULL) return;

    anim->target = target;
    anim->type = UI_ANIM_SCALE;
    anim->is_running = false;

    // 1. 初始化图片初始尺寸
    lv_obj_set_size(target, start_w, start_h);

    // 2. 配置宽度动画（原逻辑不变）
    lv_anim_init(&anim->anim);
    lv_anim_set_var(&anim->anim, target);  // 目标对象：当前图片
    lv_anim_set_exec_cb(&anim->anim, (lv_anim_exec_xcb_t)lv_obj_set_width);  // 执行函数：修改宽度
    lv_anim_set_values(&anim->anim, start_w, end_w);  // 宽度变化：起始→结束
    lv_anim_set_time(&anim->anim, duration);  // 动画时长
    lv_anim_set_delay(&anim->anim, delay);  // 延迟启动时间
    lv_anim_set_repeat_count(&anim->anim, repeat);  // 重复次数（-1=无限）
    lv_anim_set_user_data(&anim->anim, anim);  // 传递动画控制结构体（用于回调）
    lv_anim_set_ready_cb(&anim->anim, ui_anim_single_ready_cb);  // 结束回调

    // 3. 配置高度动画（手动复制宽度动画参数，替代 lv_anim_copy）
    lv_anim_t anim_h;
    lv_anim_init(&anim_h);  // 初始化高度动画

    // 手动复制宽度动画的核心参数（确保与宽度动画同步）
    lv_anim_set_var(&anim_h, target);  // 目标对象：和宽度动画一致
    lv_anim_set_time(&anim_h, duration);  // 时长：和宽度动画一致
    lv_anim_set_delay(&anim_h, delay);  // 延迟：和宽度动画一致
    lv_anim_set_repeat_count(&anim_h, repeat);  // 重复次数：和宽度动画一致
    lv_anim_set_user_data(&anim_h, anim);  // 用户数据：和宽度动画一致
    lv_anim_set_ready_cb(&anim_h, ui_anim_single_ready_cb);  // 结束回调：和宽度动画一致

    // 高度动画特有参数（覆盖复制的参数）
    lv_anim_set_exec_cb(&anim_h, (lv_anim_exec_xcb_t)lv_obj_set_height);  // 执行函数：修改高度
    lv_anim_set_values(&anim_h, start_h, end_h);  // 高度变化：起始→结束

    // 4. 启动宽度和高度动画
    lv_anim_start(&anim_h);  // 启动高度动画
    lv_anim_start(&anim->anim);  // 启动宽度动画
    anim->is_running = true;  // 标记动画为运行中
}

/*
* 停止单图动画
* @param anim 动画控制结构体
* @return 无返回值
*/
void ui_anim_single_stop(TS_UI_ANIM_SINGLE_CONTROL *anim) 
{
    if(anim == NULL || !anim->is_running) return;

    lv_anim_del(anim->target, NULL); // 停止目标所有动画
    anim->is_running = false;
}


// ---------------------- 帧动画实现 ----------------------
/*
* 帧动画回调函数
* @param timer 定时器指针
* @return 无返回值
*/
static void ui_anim_frame_cb(lv_timer_t *timer) 
{
    TS_UI_ANIM_FRAME_CONTROL *anim = (TS_UI_ANIM_FRAME_CONTROL *)timer->user_data;
    if (anim == NULL || anim->frame_count <= 1) return;

    uint16_t next_frame = anim->current_frame;

    // 1. 处理“区间播放模式”
    if (anim->in_range_mode) {
        // 根据方向计算下一帧
        if (anim->range_dir == UI_ANIM_DIR_FORWARD) {
            next_frame = anim->current_frame + 1;  // 顺序：递增
        } else {
            next_frame = anim->current_frame - 1;  // 逆序：递减
        }

        // 判断是否到达目标帧（区间结束）
        bool reach_end = false;
        if (anim->range_dir == UI_ANIM_DIR_FORWARD) {
            reach_end = (next_frame > anim->range_end);  // 顺序：超过目标帧
        } else {
            reach_end = (next_frame < anim->range_end);  // 逆序：小于目标帧
        }

        if (reach_end) {
            // 到达目标帧：停在目标帧，退出区间模式
            anim->current_frame = anim->range_end;
            lv_img_set_src(anim->img_obj, anim->frames[anim->current_frame]);
            ui_anim_frame_stop(anim);
            anim->in_range_mode = false;  // 退出区间模式
            return;
        }
    }
    // 2. 处理“原全局循环模式”（保持原有逻辑）
    else {
        next_frame = anim->current_frame + 1;
        if (next_frame >= anim->frame_count) {
            if (anim->loop) {
                next_frame = 0;  // 全局循环：回到起点
            } else {
                ui_anim_frame_stop(anim);  // 非循环：停止
                return;
            }
        }
    }

    // 更新当前帧并显示
    anim->current_frame = next_frame;
    lv_img_set_src(anim->img_obj, anim->frames[anim->current_frame]);
    lv_obj_set_style_bg_color(anim->img_obj, lv_color_hex(0x00000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(anim->img_obj, 0, LV_PART_MAIN);
}

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
                       uint32_t frame_delay, bool loop) 
{

    if (anim == NULL || parent == NULL || frames == NULL || frame_count == 0) return false;

    // 初始化图片对象
    anim->img_obj = lv_img_create(parent);
    if (anim->img_obj == NULL) return false;

    // 居中显示
    lv_obj_set_align(anim->img_obj, LV_ALIGN_CENTER);
    lv_obj_set_pos(anim->img_obj, 0, 0);
    lv_obj_set_style_bg_opa(anim->img_obj, 0, LV_PART_MAIN);

    // 帧信息初始化（含新增区间参数）
    anim->frames = frames;
    anim->frame_count = frame_count;
    anim->current_frame = 0;
    anim->frame_delay = frame_delay;
    anim->loop = loop;
    anim->is_running = false;
    anim->in_range_mode = false;  // 默认不启用区间模式
    anim->range_start = 0;
    anim->range_end = 0;
    anim->range_dir = UI_ANIM_DIR_FORWARD;

    // 显示第一帧
    lv_img_set_src(anim->img_obj, anim->frames[0]);

    // 创建定时器
    anim->timer = lv_timer_create(anim->frame_count > 1 ? ui_anim_frame_cb : NULL, frame_delay, anim);
    lv_timer_pause(anim->timer);  // 初始暂停

    return true;
}

/*
 * 设置区间播放：从x帧按方向播放到y帧
 * @param anim      动画控制结构体
 * @param x         起始帧（0 <= x < frame_count）
 * @param y         目标帧（0 <= y < frame_count，x != y）
 * @param dir       播放方向（顺序/逆序）
 * @return          成功返回true（参数合法），失败返回false
 */
bool ui_anim_frame_play_range(TS_UI_ANIM_FRAME_CONTROL *anim, 
                             uint16_t x, uint16_t y, 
                             TS_UI_ANIM_DIR dir) 
{
    // 参数合法性检查
    if (anim == NULL || anim->frame_count <= 1) return false;
    if (x >= anim->frame_count || y >= anim->frame_count) return false;  // x/y超出范围
    if (x == y) return false;  // 起始帧与目标帧相同，无需播放

    // 停止当前播放（避免冲突）
    ui_anim_frame_stop(anim);

    // 设置区间参数
    anim->in_range_mode = true;       // 启用区间播放模式
    anim->range_start = x;            // 起始帧
    anim->range_end = y;              // 目标帧
    anim->range_dir = dir;            // 播放方向
    anim->current_frame = x;          // 从x帧开始

    // 立即显示起始帧
    lv_img_set_src(anim->img_obj, anim->frames[x]);

    return true;
}

/*
 * 退出区间播放模式，恢复全局循环模式
 * @param anim  动画控制结构体
 * @return 无返回值
 */
void ui_anim_frame_exit_range_mode(TS_UI_ANIM_FRAME_CONTROL *anim) 
{
    if (anim == NULL) return;
    anim->in_range_mode = false;
}

/*
* 启动帧动画
* @param anim 动画控制结构体
* @return 无返回值
*/
void ui_anim_frame_start(TS_UI_ANIM_FRAME_CONTROL *anim) 
{
    if(anim == NULL || anim->is_running) return;
    lv_timer_resume(anim->timer);
    anim->is_running = true;
}

/*
* 停止帧动画
* @param anim 动画控制结构体
* @return 无返回值
*/
void ui_anim_frame_stop(TS_UI_ANIM_FRAME_CONTROL *anim) 
{
    if(anim == NULL || !anim->is_running) return;
    lv_timer_pause(anim->timer);
    anim->is_running = false;
}

/*
* 设置帧动画延迟
* @param anim 动画控制结构体
* @param new_delay 新的延迟时间
* @return 无返回值
*/
void ui_anim_frame_set_delay(TS_UI_ANIM_FRAME_CONTROL *anim, uint32_t new_delay) 
{
    if(anim == NULL || anim->timer == NULL) return;
    anim->frame_delay = new_delay;
    lv_timer_set_period(anim->timer, new_delay);
}

/*
* 设置帧动画隐藏
* @param anim 动画控制结构体
* @param hide 是否隐藏
* @return 无返回值
*/
void ui_anim_frame_set_hide(TS_UI_ANIM_FRAME_CONTROL *anim, bool hide) 
{
    if(anim == NULL) return;
    if (hide)
    {
        lv_obj_add_flag(anim->img_obj, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_clear_flag(anim->img_obj, LV_OBJ_FLAG_HIDDEN);
    }
}
    
/*
* 销毁帧动画
* @param anim 动画控制结构体
* @return 无返回值
*/
void ui_anim_frame_del(TS_UI_ANIM_FRAME_CONTROL *anim) 
{
    if(anim == NULL) return;
    ui_anim_frame_stop(anim);
    lv_obj_del(anim->img_obj); // 删除图片对象
    anim->timer = NULL; // 定时器会随对象自动销毁
}