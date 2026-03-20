/****************************************************************************\
**  文件名称 :  mcode_command.c
**  功能描述 :  M 码串口指令解析与执行。处理 M1 版本、M2 灯光、M3 音量等指令，并向指定串口回复。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <cstdint>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "project_config.h"
#include "upgrade_manager.h"
#include "mk_str.h"
#include "plugin_rgb.h"
#include "plugin_mp3.h"
#include "plugin_led.h"
#include "driver.h"
#include "drv_comm.h"
#include "app_light.h"
#include "app_key.h"
#include "app_mp3.h"
#include "app_periph.h"
#include "mcode_command.h"
#include "app_event_process.h"
#include "event_manager.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define MCODE_REPLY_BUF_SIZE   (256)
#define MCODE_CASE(n)         case n: process_m##n(port_id, (char *)data, (uint32_t)size); break;

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/

/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/**
 * \brief 向指定串口发送格式化回复（接收自哪一端口就回哪一端口）
 * \param port_id 串口 ID
 * \param fmt 格式化字符串
 */
static void mcode_reply(TE_DRV_USART_ID port_id, const char *fmt, ...)
{
    char buf[MCODE_REPLY_BUF_SIZE];
    va_list args;

    if (port_id >= DRV_USART_ID_MAX)
        return;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    drv_usart_send_string(port_id, buf);
}

/** 获取版本号（M1） */
static void process_m1(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    mcode_reply(port_id, "M1 A%s B%s\r\n", APP_FIRMWARE_VERSION_STR, upgrade_get_boot_version_str());
}

/** 设置灯光模式/档位/颜色并回复当前值（M2） */
static void process_m2(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float mode = 0;
    float level = 0;
    float color = 0;
    uint32_t color_value = 0;
    if (mk_str_get_value_by_tag(data, size, 0, 'A', &mode))
    {
        app_light_set_mode((uint8_t)mode);
    }
    if (mk_str_get_value_by_tag(data, size, 0, 'B', &level))
    {
        app_light_set_level((uint8_t)level);
    }
    if (mk_str_get_value_by_tag(data, size, 0, 'C', &color))
    {
        color_value = (uint32_t)color;
        app_light_set_color(color_value);
    }
    /* 使 M2 立即生效：切回挡位控制亮度，并立刻应用到灯（避免上电后滚轮事件占优导致指令不生效） */
    app_light_set_adjust_brightness_status(0);
    mode = app_light_get_mode();
    level = app_light_get_level();
    color_value = app_light_get_color();
    mcode_reply(port_id, "M2 A%d B%d C%d\r\n", (uint8_t)mode, (uint8_t)level, (uint32_t)color_value);
}

//设置MP3
static void process_m3(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float cmd;
	uint8_t command;
	if(mk_str_get_value_by_tag(data, size, 0, 'A', &cmd))
    {
		command = (uint8_t)cmd;
        switch (command) {
            case 1:
                plugin_mp3_play();
                break;
            case 2:
                plugin_mp3_pause();
                break;
            case 3:
                plugin_mp3_previous();
                break;
            case 4:
                plugin_mp3_next();
                break;
            case 5:
                plugin_mp3_set_loop(1);
                break;
            case 6:
                plugin_mp3_set_loop(0);
                break;
            case 7:
                plugin_mp3_set_volume(5);
                break;
            case 8:
                plugin_mp3_set_volume(15);
                break;
            case 9:
                plugin_mp3_set_volume(25);
                break;
            case 10:
                plugin_mp3_set_volume(30);
                break;
            default:
                break;
        }
    }
    mcode_reply(port_id, "M3 A%d\r\n", (uint8_t)cmd);
}

//获取环境温湿度
static void process_m4(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    uint8_t temp = 0;
    uint8_t humi = 0;
    app_periph_get_dht11_data(&temp, &humi);
    mcode_reply(port_id, "M4 A%d B%d\r\n", temp, humi);
}

//获取RTC日期
static void process_m5(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    uint16_t rtc_time_test[7] = {0};
    drv_rtc_read_time(rtc_time_test);
    mcode_reply(port_id, "M5 A%d B%d\r\n", (uint16_t)rtc_time_test[1], (uint16_t)rtc_time_test[2]);
}

//获取RTC时间
static void process_m6(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    uint16_t rtc_time_test[7] = {0};
    drv_rtc_read_time(rtc_time_test);
    mcode_reply(port_id, "M6 A%d B%d\r\n", (uint16_t)rtc_time_test[4], (uint16_t)rtc_time_test[5]);
}

//设置闹钟状态
static void process_m7(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float cmd = 0;
    if (mk_str_get_value_by_tag(data, size, 0, 'A', &cmd))
    {
        if (cmd == 1)
        {
            drv_timer_start(TIMER_ID_3);
        }
        else if (cmd == 2)
        {
            drv_timer_pause(TIMER_ID_3);
        }
        else if (cmd == 3)
        {
            drv_timer_reset(TIMER_ID_3);
        }
    }
    mcode_reply(port_id, "M7 A%d\r\n", (uint8_t)cmd);
}

//设置闹钟时间
static void process_m8(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float hour = 0;
    float minute = 0;
    float second = 0;
    uint8_t h = 0, m = 0, s = 0;
    if (mk_str_get_value_by_tag(data, size, 0, 'A', &hour) && mk_str_get_value_by_tag(data, size, 0, 'B', &minute) && mk_str_get_value_by_tag(data, size, 0, 'C', &second))
    {
        drv_timer_set_alarm_data((uint8_t)hour, (uint8_t)minute, (uint8_t)second);
    }
    drv_timer_get_alarm_data(&h, &m, &s);
    mcode_reply(port_id, "M8 A%d B%d C%d\r\n", (int)h, (int)m, (int)s);
}   

//设置氛围灯
static void process_m9(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float cmd = 0;
    if (mk_str_get_value_by_tag(data, size, 0, 'A', &cmd))
    {
        app_light_set_color_state((uint8_t)cmd);
    }
    mcode_reply(port_id, "M9 A%d\r\n", (uint8_t)app_light_get_color_state());
}

//上位机按钮
static void process_m10(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float cmd = 0;
    uint8_t command = 0;
    if (mk_str_get_value_by_tag(data, size, 0, 'A', &cmd))
    {
        command = (uint8_t)cmd;
        switch (command)
        {
            case 1:
                event_manager_write(EVENT_MODULE_KEY, KEY_PRESS_SHORT_DOWN, MODE_KEY, NULL, 0);
                break;
            case 2: {
                app_light_set_adjust_brightness_status(0);        //按键调整亮度
                /* 当前挡位++，到达最亮(3)就保持最亮 */
                uint8_t level = app_light_get_level();
                if (level < APP_LIGHT_LEVEL_HIGH)
                    level++;
                app_light_set_level(level);
                break;
            }
            case 3:
                event_manager_write(EVENT_MODULE_KEY, KEY_PRESS_SHORT_DOWN, PLAY_KEY, NULL, 0);
                break;
            case 4:
                event_manager_write(EVENT_MODULE_KEY, KEY_PRESS_SHORT_DOWN, UP_KEY, NULL, 0);
                break;
            case 5:
                event_manager_write(EVENT_MODULE_KEY, KEY_PRESS_SHORT_DOWN, DOWN_KEY, NULL, 0);
                break;
            case 6: {
                app_light_set_adjust_brightness_status(0);        //按键调整亮度
                /* 当前挡位--，到达最暗(0)就保持最暗 */
                uint8_t level = app_light_get_level();
                if (level > APP_LIGHT_LEVEL_NONE)
                    level--;
                app_light_set_level(level);
                break;
            }
        }
    }
    mcode_reply(port_id, "M10 A%d\r\n", (uint8_t)cmd);
}

//设置开关灯（关灯保留当前模式和档位，开灯时恢复）
static void process_m11(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float cmd = 0;
    static uint8_t save_mode;
    static uint8_t save_level;
    static uint8_t save_valid = 0;  // 仅在上次执行过关灯后才恢复，避免上电后直接开灯误用未保存的0,0
    if (mk_str_get_value_by_tag(data, size, 0, 'A', &cmd))
    {
        if (cmd == 0)
        {
            save_mode = app_light_get_mode();
            save_level = app_light_get_level();
            save_valid = 1;
            printf("[M11] off save mode=%d level=%d\r\n", save_mode, save_level);
            app_light_set_mode(APP_LIGHT_MODE_OFF);
        }
        else
        {
            if (save_valid)
            {
                printf("[M11] on restore mode=%d level=%d\r\n", save_mode, save_level);
                app_light_set_mode(save_mode);
                app_light_set_level(save_level);
            }
            else
            {
                printf("[M11] on no saved state, keep current mode/level\r\n");
            }
        }
    }
    mcode_reply(port_id, "M11 A%d\r\n", (uint8_t)cmd);
}

//查询和设置音量
static void process_m12(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float cmd = 0;
    int volume = app_mp3_get_volume();
    if (mk_str_get_value_by_tag(data, size, 0, 'A', &cmd))
    {
        if (cmd == 1)
        {
            volume += 1;
        }
        else if (cmd == 2)
        {
            volume -= 1;
        }
        volume = volume < 0 ? 0 : volume;
        volume = volume > 30 ? 30 : volume;
        app_mp3_set_volume(volume);
    }
    volume = app_mp3_get_volume();
    /* 音量 0~30 对应 0%~100% 返回给上位机 */
    int percent = (volume * 100) / 30;
    printf("[M12] volume=%d%%\r\n", percent);
    mcode_reply(port_id, "M12 A%d\r\n", percent);
}

// 设置专注时间状态
static void process_m13(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float cmd = 0;
    if (mk_str_get_value_by_tag(data, size, 0, 'A', &cmd))
    {
        switch ((uint8_t)cmd)
        {
            case 1:
                drv_timer_reset(TIMER_ID_4);
                drv_timer_start(TIMER_ID_4);
                break;
            case 2:
                drv_timer_pause(TIMER_ID_4);
                break;
            case 3:
                drv_timer_reset(TIMER_ID_4);
                break;
            default:
                break;
        }
    }
    mcode_reply(port_id, "M13 A%d\r\n", (uint8_t)cmd);
}

// 获取专注时间
static void process_m14(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    uint8_t h = 0, m = 0, s = 0;
    drv_timer_get_focus_time(&h, &m, &s);
    mcode_reply(port_id, "M14 A%d B%d C%d\r\n", (int)h, (int)m, (int)s);
}

//获取当前环境亮度
static void process_m15(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    uint8_t brightness = 0;
    brightness = app_periph_get_brightness();
    mcode_reply(port_id, "M15 A%d\r\n", brightness);
}

//获取当前电量
static void process_m16(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    uint8_t power = 0;
    power = app_periph_get_power();
    mcode_reply(port_id, "M16 A%d\r\n", power);
}

//获取RTC星期
static void process_m17(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    uint16_t rtc_time_test[7] = {0};
    drv_rtc_read_time(rtc_time_test);
    mcode_reply(port_id, "M17 A%d\r\n", (uint16_t)rtc_time_test[3]);
}

//设置rgb灯光
static void process_m25(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float r_value = 0;
    float g_value = 0;
    float b_value = 0;
	uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint32_t color = 0;
    if (mk_str_get_value_by_tag(data, size, 0, 'R', &r_value) && mk_str_get_value_by_tag(data, size, 0, 'G', &g_value) && mk_str_get_value_by_tag(data, size, 0, 'B', &b_value))
    {
		app_light_set_mode(APP_LIGHT_MODE_RGB);
        r = (uint8_t)r_value;
        g = (uint8_t)g_value;
        b = (uint8_t)b_value;
        color = (uint32_t)g << 16 | (uint32_t)r << 8 | (uint32_t)b;
        app_light_set_color(color);
    }
    mcode_reply(port_id, "M25 R%f G%f B%f\r\n", r_value, g_value, b_value);
}

//设置状态灯
static void process_m26(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float cmd_a = 0;
    float cmd_b = 0;
    float cmd_c = 0;
    float cmd_d = 0;
    if (mk_str_get_value_by_tag(data, size, 0, 'A', &cmd_a))
    {
        switch ((uint8_t)cmd_a)
        {
            case 1:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 1);
                break;
            case 2:
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 1);
                break;
            case 3:
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 1);
                break;
            case 4:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 1);
                break;
            case 5:
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 1);
                break;
            case 6:
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 1);
                break;
        }
    }
    if (mk_str_get_value_by_tag(data, size, 0, 'B', &cmd_b))
    {
        switch ((uint8_t)cmd_b)
        {
            case 1:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 0);
                break;
            case 2:
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 0);
                break;
            case 3:
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 0);
                break;
            case 4:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 0);
                break;
            case 5:
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 0);
                break;
            case 6:
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 0);
                break;
        }
    }
    if (mk_str_get_value_by_tag(data, size, 0, 'C', &cmd_c))
    {
        switch ((uint8_t)cmd_c)
        {
            case 1:
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_RED);
                break;
            case 2:
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_GREEN);
                break;
            case 3:
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_BLUE);
                break;
            case 4:
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_YELLOW);
                break;
            case 5:
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_PURPLE);
                break;
            case 6:
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_CYAN);
                break;
            case 7:
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_WHITE);
                break;
            case 8:
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_BLACK);
                break;
            default:
                break;
        }
    }
    if (mk_str_get_value_by_tag(data, size, 0, 'D', &cmd_d))
    {
        switch ((uint8_t)cmd_d)
        {
            case 1:
                plugin_led_set_color(LED_ID_MODE, LED_COLOR_RED);
                break;
            case 2:
                plugin_led_set_color(LED_ID_MODE, LED_COLOR_GREEN);
                break;
            case 3:
                plugin_led_set_color(LED_ID_MODE, LED_COLOR_BLUE);
                break;
            case 4:
                plugin_led_set_color(LED_ID_MODE, LED_COLOR_YELLOW);
                break;
            case 5:
                plugin_led_set_color(LED_ID_MODE, LED_COLOR_PURPLE);
                break;
            case 6:
                plugin_led_set_color(LED_ID_MODE, LED_COLOR_CYAN);
                break;
            case 7:
                plugin_led_set_color(LED_ID_MODE, LED_COLOR_WHITE);
                break;
            case 8:
                plugin_led_set_color(LED_ID_MODE, LED_COLOR_BLACK);
                break;
            default:
                break;
        }
    }
    mcode_reply(port_id, "M26 A%d B%d C%d D%d\r\n", (uint8_t)cmd_a, (uint8_t)cmd_b, (uint8_t)cmd_c, (uint8_t)cmd_d);
}

//升级：收到 M99 时设置升级标志并重启，进入 Bootloader IAP
static void process_m99(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    
    if (upgrade_set_upgrade_flag(APP_FIRMWARE_VERSION))
    {
        mcode_reply(port_id, "M99 S1\r\n");  // 升级成功，发送ACK
        drv_usart_wait_transmit_complete(port_id);
        upgrade_system_reset();
    }
    else
    {
        mcode_reply(port_id, "M99 S0\r\n");  // 升级失败，发送NACK
    }
    
}


/**
 * @brief M代码命令处理函数：接收自哪一端口，回复即发往该端口
 * @param port_id 接收数据的端口ID（回复将发往此端口）
 * @param data 数据指针
 * @param size 数据长度
 */
void process_m_code(TE_DRV_USART_ID port_id, char *data, uint32_t size)
{
    float command = -1;

    if (port_id >= DRV_USART_ID_MAX)
    {
        return;
    }
    if (mk_str_get_value_by_tag(data, size, 0, 'M', &command) == true)
    {
        int command_int = (int)command;
        switch (command_int)
        {
            MCODE_CMD_LIST

            default:
                mcode_reply(port_id, "Unsupported M code commands: M%d\r\n", command_int);
                break;
        }
    }
}

/******************************* End of File (C)****************************/ 
