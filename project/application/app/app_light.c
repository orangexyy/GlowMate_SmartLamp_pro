/****************************************************************************\
**  文件名称 :  app_light.c
**  功能描述 :  灯光应用模块。负责灯光模式/档位/亮度/颜色设置、光感与滚轮调节、
**             红外挥手开关灯检测及与 RGB/指示灯 插件协同。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "app_light.h"
#include "drv_adc.h"
#include "drv_rgb.h"
#include "drv_input.h"
#include "plugin_rgb.h"
#include "plugin_led.h"
#include "app_event_process.h"
#include "event_manager.h"
#include "thread.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
#define LIGHT_BRIGHTNESS_MAX        (100u)
#define ADC_VALUE_MAX               (4095u)
/* ADC 变化超过该阈值才上报亮度事件，避免小幅抖动频繁上报（0~4095） */
#define ADC_LIGHT_CHANGE_THRESHOLD  (100u)
/* 挥手检测：两路触发时间差须在 [MIN, TIMEOUT] 内才视为挥手，避免同时触发误关灯（单位：10ms） */
#define WAVE_DETECT_MIN_TICKS      (10u)    /* 最小间隔约 50ms，早于此次视为同时触发，不响应 */
#define WAVE_DETECT_TIMEOUT_TICKS  (80u)   /* 最大间隔约 800ms，超时放弃 */
/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
uint16_t adc_val_light = 0;
uint8_t cur_light = 0;
uint8_t last_light = 0;
uint8_t adjust_brightness_status = 0;   //0:按键调整亮度 1:滚轮调整亮度
uint8_t color_state = 1;  //1:渐变模式 0:静态模式

TS_APP_LIGHT_DATA s_light_data = {
    APP_LIGHT_MODE_COOL_WHITE, 
    APP_LIGHT_LEVEL_HIGH, 
    100, 
    PLUGIN_RGB_COLOR_GREEN,
};
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
static char app_light_task(thread_t* pt);

/**
 * \brief 灯光应用初始化：初始化 RGB 插件、设置初始颜色并创建灯光协程任务
 */
void app_light_init(void)
{
    plugin_rgb_init();
    plugin_rgb_set_all_color(s_light_data.color);
    thread_create(app_light_task);
}


/**
 * \brief 灯光协程任务：周期执行设置/调节/挥手开关灯逻辑，并可按需打印调试信息
 */
#define LIGHT_PRINT_INTERVAL_MS   (1000)
#define LIGHT_TASK_SLEEP_MS       (10)
#define LIGHT_PRINT_TICKS        (LIGHT_PRINT_INTERVAL_MS / LIGHT_TASK_SLEEP_MS)

static char app_light_task(thread_t* pt)
{
    static uint32_t s_print_tick = 0;

    thread_begin
    {
        while (1)
        {
            app_light_set();
            app_light_adjust();
            app_light_switch();   /* 红外挥手开关灯 */

            if (++s_print_tick >= LIGHT_PRINT_TICKS)
            {
                s_print_tick = 0;
                // printf("s_light_data.brightness: %d\r\n", s_light_data.brightness);
                // printf("s_light_data.mode: %d\r\n", s_light_data.mode);
                // printf("s_light_data.level: %d\r\n", s_light_data.level);
                // printf("s_light_data.color: %d\r\n", s_light_data.color);
            }
            thread_sleep(LIGHT_TASK_SLEEP_MS);
        }
    }
    thread_end
}



/**
 * \brief 根据当前模式与档位设置灯光：包括冷/暖/自然白、自动光感、彩色渐变、夜灯、学习、RGB、关灯等
 */
void app_light_set(void)
{
    if (adjust_brightness_status == 0)
    {
        switch (s_light_data.level) 
        {
            case APP_LIGHT_LEVEL_NONE:
                s_light_data.brightness = 0;
                break;
            case APP_LIGHT_LEVEL_LOW:
                s_light_data.brightness = 30;
                break;
            case APP_LIGHT_LEVEL_MID:
                s_light_data.brightness = 60;
                break;
            case APP_LIGHT_LEVEL_HIGH:
                s_light_data.brightness = 100;
                break;
            default:
                break;
        }
    }

    switch (s_light_data.mode)
    {
        case APP_LIGHT_MODE_COOL_WHITE:
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_GREEN);
            s_light_data.color = PLUGIN_RGB_COLOR_COOL_WHITE;
            plugin_rgb_set_all_brightness(s_light_data.color, s_light_data.brightness);
            break;
        case APP_LIGHT_MODE_NORMAL_WHITE:
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_GREEN);
            s_light_data.color = PLUGIN_RGB_COLOR_WHITE;
            plugin_rgb_set_all_brightness(s_light_data.color, s_light_data.brightness);
            break;
        case APP_LIGHT_MODE_WARM_WHITE:
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_GREEN);
            s_light_data.color = PLUGIN_RGB_COLOR_WARM_WHITE;
            plugin_rgb_set_all_brightness(s_light_data.color, s_light_data.brightness);
            break;
        case APP_LIGHT_MODE_AUTO: {
            /* 光感 ADC 越小表示外界越亮，灯调暗；ADC 越大外界越暗，灯调亮。ADC 0~4095 映射为亮度 0~100 */
            uint16_t adc_env = drv_adc_get_value(ADC_ID_LIGHT_SENSOR_1);
            s_light_data.brightness = (uint8_t)(((uint32_t)adc_env * LIGHT_BRIGHTNESS_MAX) / ADC_VALUE_MAX);
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_BLUE);
            s_light_data.color = PLUGIN_RGB_COLOR_WARM_WHITE;
            plugin_rgb_set_all_brightness(s_light_data.color, s_light_data.brightness);
            break;
        }
        case APP_LIGHT_MODE_COLOR:  
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_RED);
            if (color_state == 1)
            {
                plugin_rgb_gradient_all_color_node_nonblocking(200, 10);
            }
            break;
        case APP_LIGHT_MODE_NIGHT:
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_YELLOW);
            s_light_data.color = PLUGIN_RGB_COLOR_WARM_WHITE;
            plugin_rgb_set_all_brightness(s_light_data.color, s_light_data.brightness);
            break;
        case APP_LIGHT_MODE_STUDY:
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_CYAN);
            s_light_data.color = PLUGIN_RGB_COLOR_WARM_WHITE;
            plugin_rgb_set_all_brightness(s_light_data.color, s_light_data.brightness);
            break;
        case APP_LIGHT_MODE_RGB:
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_PURPLE);
            // s_light_data.color = PLUGIN_RGB_COLOR_RGB;
            plugin_rgb_set_all_brightness(s_light_data.color, s_light_data.brightness);
            break;
        case APP_LIGHT_MODE_OFF:
            /* 关灯模式：仅挥手关灯进入，不跑渐变/不依赖挡位，直接灭灯 */
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_WHITE);
            s_light_data.brightness = 0;
            plugin_rgb_set_all_brightness(s_light_data.color, 0);
            break;
        default:
            plugin_led_set_color(LED_ID_MODE, LED_COLOR_WHITE);
            s_light_data.brightness = 0;
            s_light_data.color = PLUGIN_RGB_COLOR_WHITE;
            plugin_rgb_set_all_brightness(s_light_data.color, s_light_data.brightness);
            break;
    }
} 

/**
 * \brief 根据滚轮 ADC 调节亮度并超过阈值时上报亮度事件
 */
void app_light_adjust(void)
{
    static uint16_t last_reported_adc = 0xFFFFu;  /* 上次上报时的 ADC 值，0xFFFF 表示尚未上报过 */
    uint16_t adc_delta;

    adc_val_light = drv_adc_get_value(ADC_ID_LIGHT_ADJUST);
    /* ADC 0~4095 映射为亮度等级 0~100 */
    cur_light = (uint8_t)(((uint32_t)adc_val_light * LIGHT_BRIGHTNESS_MAX) / ADC_VALUE_MAX);

    if (last_reported_adc == 0xFFFFu)
    {
        adc_delta = ADC_LIGHT_CHANGE_THRESHOLD + 1;  /* 首次必上报 */
    }
    else
    {
        adc_delta = (adc_val_light >= last_reported_adc) ?
                    (adc_val_light - last_reported_adc) : (last_reported_adc - adc_val_light);
    }

    if (adc_delta > ADC_LIGHT_CHANGE_THRESHOLD)
    {
        last_reported_adc = adc_val_light;
        last_light = cur_light;
        event_manager_write(EVENT_MODULE_LIGHT, 0, cur_light, NULL, 0);
    }
} 

/**
 * \brief 挥手开关灯：一路红外触发后，另一路须在 [最小间隔, 超时] 内触发才视为挥手。
 *        时间差过小视为同时触发（误触），过大视为超时；关灯时保存模式/挡位，开灯时恢复。
 */
void app_light_switch(void)
{
    enum { WAVE_IDLE = 0, WAVE_WAIT_FOR_2, WAVE_WAIT_FOR_1 };
    static uint8_t last_v1 = 0xFF, last_v2 = 0xFF;
    static uint8_t wave_state = WAVE_IDLE;
    static uint32_t wave_start_tick = 0;
    static uint32_t tick_count = 0;
    static uint8_t saved_level = APP_LIGHT_LEVEL_MID;
    static uint8_t saved_mode = APP_LIGHT_MODE_COLOR;

    uint8_t v1 = drv_input_get_value(INPUT_ID_IR_DETECT_1);
    uint8_t v2 = drv_input_get_value(INPUT_ID_IR_DETECT_2);
    uint8_t edge_1 = (last_v1 != 0xFF && v1 != last_v1);
    uint8_t edge_2 = (last_v2 != 0xFF && v2 != last_v2);
    uint8_t other_triggered;
    uint32_t elapsed;
    last_v1 = v1;
    last_v2 = v2;
    tick_count++;

    switch (wave_state)
    {
    case WAVE_IDLE:
        if (edge_1)
        {
            wave_state = WAVE_WAIT_FOR_2;
            wave_start_tick = tick_count;
        }
        else if (edge_2)
        {
            wave_state = WAVE_WAIT_FOR_1;
            wave_start_tick = tick_count;
        }
        break;

    case WAVE_WAIT_FOR_2:
    case WAVE_WAIT_FOR_1:
        other_triggered = (wave_state == WAVE_WAIT_FOR_2 && edge_2) ||
                         (wave_state == WAVE_WAIT_FOR_1 && edge_1);
        if (other_triggered)
        {
            elapsed = tick_count - wave_start_tick;
            /* 仅当时间差在 [MIN, TIMEOUT] 内才视为挥手，过小=同时触发误触，过大=超时 */
            if (elapsed >= WAVE_DETECT_MIN_TICKS && elapsed <= WAVE_DETECT_TIMEOUT_TICKS)
            {
                if (s_light_data.mode == APP_LIGHT_MODE_OFF)
                {
                    s_light_data.mode = saved_mode;
                    s_light_data.level = saved_level;
                }
                else
                {
                    saved_mode = s_light_data.mode;
                    saved_level = s_light_data.level;
                    s_light_data.mode = APP_LIGHT_MODE_OFF;
                }
            }
            wave_state = WAVE_IDLE;
        }
        else if ((tick_count - wave_start_tick) > WAVE_DETECT_TIMEOUT_TICKS)
        {
            wave_state = WAVE_IDLE;
        }
        break;

    default:
        break;
    }
}

/**
 * \brief 设置灯光模式，超出范围时取模到有效模式
 * \param mode 模式枚举值
 */
void app_light_set_mode(uint8_t mode)
{
    /* 模式仅允许 0~(APP_LIGHT_MODE_MAX-1)，超出时取模到有效范围 */
    if (mode >= APP_LIGHT_MODE_MAX)
    {
        mode = mode % APP_LIGHT_MODE_MAX;
    }
    s_light_data.mode = mode;
}   

/**
 * \brief 获取当前灯光模式
 * \return 当前模式
 */
uint8_t app_light_get_mode(void)
{
    return s_light_data.mode;
}

/**
 * \brief 设置灯光档位，超出时取模避免循环错乱
 * \param level 档位 (0~3)
 */
void app_light_set_level(uint8_t level)
{
    /* 档位仅允许 0~3，超出范围时限制到有效值，避免 SETTING_KEY/BACK_KEY 循环错乱 */
    if (level >= APP_LIGHT_LEVEL_MAX)
    {
        level = level % APP_LIGHT_LEVEL_MAX;
    }
    s_light_data.level = level;
}

/**
 * \brief 获取当前灯光档位
 * \return 档位值
 */
uint8_t app_light_get_level(void)
{
    return s_light_data.level;
}

/**
 * \brief 设置亮度 (0~100)
 * \param brightness 亮度值
 */
void app_light_set_brightness(uint8_t brightness)
{
    s_light_data.brightness = brightness;
}

/**
 * \brief 获取当前亮度
 * \return 亮度值
 */
uint8_t app_light_get_brightness(void)
{
    return s_light_data.brightness;
}

/**
 * \brief 设置灯光颜色 (RGB 值)
 * \param color 颜色值
 */
void app_light_set_color(uint32_t color)
{
    s_light_data.color = color;
} 

/**
 * \brief 获取当前灯光颜色
 * \return 颜色值
 */
uint32_t app_light_get_color(void)
{
    return s_light_data.color;
}

/**
 * \brief 设置颜色状态：1 渐变模式，0 静态模式
 * \param state 状态值
 */
void app_light_set_color_state(uint8_t state)
{
    color_state = state;
}

/**
 * \brief 获取颜色状态
 * \return 1 渐变 / 0 静态
 */
uint8_t app_light_get_color_state(void)
{
    return color_state;
}

/**
 * \brief 设置亮度调节来源状态：0 按键调整，1 滚轮调整
 * \param status 状态值
 */
void app_light_set_adjust_brightness_status(uint8_t status)
{
    adjust_brightness_status = status;
}

/**
 * \brief 获取亮度调节来源状态
 * \return 0 按键 / 1 滚轮
 */
uint8_t app_light_get_adjust_brightness_status(void)
{
    return adjust_brightness_status;
}

/******************************* End of File (C) ******************************/
