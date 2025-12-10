/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  
**  功能描述 :  
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

/****************************************************************************\
                               Includes
\****************************************************************************/
#include "mk_common.h"  
#include "svc_led.h" 
#include "drv_led.h"
#include "thread.h"

/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define LED_MAX(a, b, c)   (((a) > (b) ? (a) : (b)) > (c) ? ((a) > (b) ? (a) : (b)) : (c))  
#define LED_MIN(a, b, c)   (((a) < (b) ? (a) : (b)) < (c) ? ((a) < (b) ? (a) : (b)) : (c))

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
typedef struct 
{
    uint16_t duty;      /**< 占空比 */
    uint8_t led_id;     /**< led id */
    uint8_t active;     /**< 使能状态, 0停止/1运行 */
    uint8_t time_cnt;   /**< 时间计数 */
    uint8_t repeat_cnt; /**< 重复计数 */
    uint8_t direction;  /**< 呼吸方向 */
    uint8_t row_index;  /**< 行索引 */
    uint8_t col_index;  /**< 列索引 */
} TS_LED_PRIV;

typedef struct
{
    const TS_LED_TABLE *table;    /**< 样式表 */    
    uint8_t             type_num; /**< 样式表个数 */
} TS_LED_TYPE;

typedef struct
{  
    TS_LED_PRIV     priv;       /**< 私有数据 */
    LED_FINISH_CB   finish_cb;  /**< 结束回调函数 */
    LED_COSTOM_CB   custom_cb;  /**< 自定义回调函数 */
    TS_LED_TYPE     type;       /**< 样式表 */
} TS_LED;

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static TS_LED  s_led_list[LED_MAX_NUM];
static uint8_t s_led_num = 0;

/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/**
 * \brief RGB 转 HSV（H：色调、S：饱和度、V：亮度）
 */
static void _led_rgb2hsv(uint8_t r, uint8_t g, uint8_t b, float *h, float *s, float *v)
{
    // 使用整数运算找出最大值和最小值，避免浮点运算
    uint8_t cmax = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
    uint8_t cmin = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
    uint8_t delta = cmax - cmin;

    // V (亮度)
    *v = (float)cmax / 255.0f;
 
    if (cmax == 0) {
        *s = 0.0f;
        *h = 0.0f;
        return;
    }

    // S (饱和度)
    *s = (float)delta / (float)cmax;

    // 快速处理灰色情况
    if (delta == 0) {
        *h = 0.0f;
        return;
    }

    // H (色调)
    float h_temp;
    if (cmax == r) {
        h_temp = (float)(g - b) / (float)delta;
        *h = (g >= b) ? h_temp * 60.0f : (h_temp + 6.0f) * 60.0f;
    } else if (cmax == g) {
        h_temp = (float)(b - r) / (float)delta + 2.0f;
        *h = h_temp * 60.0f;
    } else {  // cmax == b
        h_temp = (float)(r - g) / (float)delta + 4.0f;
        *h = h_temp * 60.0f;
    }
}
  
/**
 * \brief HSV 转 RGB
 */
static void _led_hsv2rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    // 边界检查和标准化，避免后续计算错误
    if (h < 0.0f) h += 360.0f;
    if (h >= 360.0f) h -= 360.0f;
    if (s < 0.0f) s = 0.0f;
    if (s > 1.0f) s = 1.0f;
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;

    float h_div_60 = h / 60.0f;
    int hi = (int)h_div_60;
    float f = h_div_60 - hi;
    
    // 确保hi在0-5范围内
    if (hi < 0) hi += 6;
    if (hi >= 6) hi -= 6;
    
    // 预计算p, q, t值
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
 
    float rf, gf, bf;
     
    switch (hi) {
        case 0: rf = v; gf = t; bf = p; break;
        case 1: rf = q; gf = v; bf = p; break;
        case 2: rf = p; gf = v; bf = t; break;
        case 3: rf = p; gf = q; bf = v; break;
        case 4: rf = t; gf = p; bf = v; break;
        case 5: rf = v; gf = p; bf = q; break;
        default: rf = p; gf = p; bf = p; break; // 灰色
    }
    
    // 快速转换为uint8_t，使用四舍五入提高精度
    *r = (uint8_t)(rf * 255.0f + 0.5f);
    *g = (uint8_t)(gf * 255.0f + 0.5f);
    *b = (uint8_t)(bf * 255.0f + 0.5f);
}

/**
 * \brief 应用亮度到RGB颜色
 */
static void _led_apply_brightness(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t brightness)
{
    if (!(*r | *g | *b)) {
        return; // 单色灯，不需要处理RGB
    }
    
    float h, s, v;
    _led_rgb2hsv(*r, *g, *b, &h, &s, &v);
    _led_hsv2rgb(h, s, brightness / 255.0f, r, g, b);
}

/**
 * \brief 写入LED数据
 */
static void _led_write_data(uint8_t id, uint8_t brightness, uint8_t r, uint8_t g, uint8_t b)
{
    if (!(r | g | b)) { 
        drv_led_write_value(s_led_list[id].priv.led_id, brightness);
    } else {
        drv_led_write_rgb(s_led_list[id].priv.led_id, r, g, b);
    }
}

/**
 * \brief 重置LED状态
 */
static void _led_reset_state(TS_LED_PRIV *priv, uint8_t brightness)
{
    priv->col_index = 0;
    priv->time_cnt = 0;
    priv->repeat_cnt = 0;
    priv->direction = 0;
    priv->duty = LED_MIN_BRIGHTNESS;
}

/**
 * \brief 处理LED样式
 */
static void _led_process_pattern(uint8_t id, TS_LED *p_led, char pattern, uint8_t brightness, 
                                uint8_t r, uint8_t g, uint8_t b)
{
    switch(pattern) {
        case '-': // 亮
            if (!(r | g | b)) {
                _led_write_data(id, brightness, 0, 0, 0);
            } else {
                _led_apply_brightness(&r, &g, &b, brightness);
                _led_write_data(id, 0, r, g, b);
            }
            break;

        case ' ': // 灭 
            _led_write_data(id, 0, 0, 0, 0);
            break;

        case 'v': // 呼吸灯
            if (p_led->priv.direction == 0) {
                if (++p_led->priv.duty > brightness) {
                    p_led->priv.duty = brightness;
                    p_led->priv.direction = 1;
                }
            } else {
                if (--p_led->priv.duty <= LED_MIN_BRIGHTNESS) {
                    p_led->priv.duty = LED_MIN_BRIGHTNESS;
                    p_led->priv.direction = 0;
                }
            }
            
            if (!(r | g | b)) {
                _led_write_data(id, p_led->priv.duty, 0, 0, 0);
            } else {
                _led_apply_brightness(&r, &g, &b, p_led->priv.duty);
                _led_write_data(id, 0, r, g, b);
            }
            p_led->priv.col_index--; 
            break;
   
        case '/': // 渐亮
            if (++p_led->priv.duty < brightness) { 
                if (!(r | g | b)) {
                    _led_write_data(id, p_led->priv.duty, 0, 0, 0);
                } else {
                    _led_apply_brightness(&r, &g, &b, p_led->priv.duty);
                    _led_write_data(id, 0, r, g, b);
                }
                p_led->priv.col_index--;
            } else {
                if (!(r | g | b)) {
                    _led_write_data(id, brightness, 0, 0, 0);
                } else {
                    _led_apply_brightness(&r, &g, &b, brightness);
                    _led_write_data(id, 0, r, g, b);
                }
                p_led->priv.duty = LED_MIN_BRIGHTNESS;
            }
            break;

        case '\\': // 渐灭
            if (p_led->priv.duty > LED_MIN_BRIGHTNESS) {
                p_led->priv.duty--;
                if (!(r | g | b)) {
                    _led_write_data(id, p_led->priv.duty, 0, 0, 0);
                } else {
                    _led_apply_brightness(&r, &g, &b, p_led->priv.duty);
                    _led_write_data(id, 0, r, g, b);
                }
                p_led->priv.col_index--;
            } else {
                _led_write_data(id, LED_MIN_BRIGHTNESS, 0, 0, 0);
                p_led->priv.duty = brightness;
            }
            break;
        
        default: 
            if (p_led->custom_cb != NULL) { 
                TS_COLOR color = {r, g, b}; 
                p_led->custom_cb(id, pattern, brightness, (r | g | b) ? &color : NULL);
            }
            break;
    }
}

/**
 * \brief LED样式解析
 */
static void _svc_led_analysis(uint8_t id, TS_LED *p_led)
{
    const TS_LED_TABLE *table = &p_led->type.table[p_led->priv.row_index];
    char pattern = table->type[p_led->priv.col_index++];
                
    if (pattern == '\0') {   // 是否结束
        if ((table->repeat_tms == 0) || (++p_led->priv.repeat_cnt < table->repeat_tms)) {
            p_led->priv.col_index = 0;  
            pattern = table->type[p_led->priv.col_index++];
        } else {
            p_led->priv.active = 0;
            if (p_led->finish_cb != NULL) {
                p_led->finish_cb(id, p_led->priv.row_index);
            }
            return;
        }
    }
    
    _led_process_pattern(id, p_led, pattern, table->brightness, 
                        table->color.red, table->color.green, table->color.blue);
}

/**
 * \brief LED任务
 */
static char svc_led_run(thread_t* pt)
{
    thread_begin
    {
        while (1) {        
            for (uint8_t i = 0; i < s_led_num; i++) {
                TS_LED *p_led = &s_led_list[i];
                if (p_led->priv.active) {
                    if (++p_led->priv.time_cnt >= p_led->type.table[p_led->priv.row_index].interval) {
                        p_led->priv.time_cnt = 0;
                        _svc_led_analysis(i, p_led);
                    }
                }
            }
            thread_sleep(LED_SCAN_INTERVAL);
        }
    }
    thread_end 
}
 
/**
 * \brief 注册LED, 注册成功则返回id值
 */
int svc_led_init(uint8_t id, const TS_LED_TABLE *type_table, uint8_t type_num)
{
    if (type_table == NULL) {
        return MK_ERROR_NULL_POINTER;
    }

    uint8_t i;
    for (i = 0; i < LED_MAX_NUM; i++) {
        if (s_led_list[i].type.table == NULL) {
            break;
        }
    }
    
    if (i >= LED_MAX_NUM) {
        return MK_ERROR_NO_MEM;
    }

    memset(s_led_list+i, 0, sizeof(TS_LED));
    
    s_led_list[i].priv.led_id   = id;
    s_led_list[i].type.table    = type_table;
    s_led_list[i].type.type_num = type_num;
    
    s_led_num++;
    thread_create(svc_led_run);
    return s_led_num;
}

/**
 * \brief 根据设备个数
 */
uint8_t svc_led_get_num(void)
{
    return s_led_num;
}

/**
 * \brief 获取led模式总个数 
 */
int svc_led_get_mode_num(uint8_t id)
{  
    return s_led_list[id].type.type_num;
}

/**
 * \brief 注册显示结束回调函数 
 */
int svc_led_reg_finish_cb(uint8_t id, LED_FINISH_CB cb)
{ 
    s_led_list[id].finish_cb = cb;
    return MK_SUCCESS;
}

/**
 * \brief 注册自定义接口
 */
int svc_led_reg_costom_cb(uint8_t id, LED_COSTOM_CB cb)
{ 
    s_led_list[id].custom_cb = cb;
    return MK_SUCCESS;
}

/**
 * \brief 停止LED
 */
int svc_led_stop(uint8_t id)
{      
    _led_write_data(id, 0, 0, 0, 0);
    s_led_list[id].priv.active = 0;
    return MK_SUCCESS;
}

/**
 * \brief 停止所有LED设备
 */
int svc_led_stop_all(void)
{     
    for (uint8_t i = 0; i < s_led_num; i++) {
        _led_write_data(i, 0, 0, 0, 0);
        s_led_list[i].priv.active = 0; 
    }
    return MK_SUCCESS;
}

/**
 * \brief 设定LED的工作模式
 */
int svc_led_set_mode(uint8_t id, uint8_t mode)
{ 
    TS_LED *p_led = &s_led_list[id];    
    if (mode >= p_led->type.type_num) {
        return MK_ERROR_INVALID_PARAM;
    }
    
    p_led->priv.row_index = mode;
    _led_reset_state(&p_led->priv, p_led->type.table[mode].brightness);
    p_led->priv.active = 1;
    return MK_SUCCESS; 
}

/**
 * \brief 获取LED的工作模式
 */
uint8_t svc_led_get_mode(uint8_t id)
{  
    return s_led_list[id].priv.row_index;
}

/**
 * \brief 获取设备当前是否忙
 */
int svc_led_is_busy(uint8_t id)
{ 
    return s_led_list[id].priv.active;
}
 
/******************************* End of File (C）****************************/
