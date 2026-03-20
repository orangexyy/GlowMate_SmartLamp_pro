/****************************************************************************\
**  文件名称 :  plugin_led.c
**  功能描述 :  指示灯插件。状态灯/模式灯颜色设置，通过 drv_output 控制 RGB 引脚。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "stm32f10x.h"
#include "sys.h"
#include "stdint.h"
#include "system.h"
#include "drv_output.h"
#include "plugin_led.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/

/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/**
 * \brief LED初始化
 * \return 无
 */
void plugin_led_init(void)
{
    drv_output_init();
	plugin_led_set_color(LED_ID_STATE, LED_COLOR_WHITE);
	plugin_led_set_color(LED_ID_MODE, LED_COLOR_WHITE);
}

/**
 * \brief 根据 LED ID 与颜色枚举设置状态灯或模式灯 RGB
 * \param led_id 灯 ID（状态灯/模式灯）
 * \param color 颜色枚举
 */
void plugin_led_set_color(TE_LED_ID led_id, TE_LED_COLOR color)
{
    if (led_id == LED_ID_STATE)
    {
        switch (color)
        {
            case LED_COLOR_RED:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 1);
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 0);
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 0);
                break;
            case LED_COLOR_GREEN:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 0);
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 1);
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 0);
                break;
            case LED_COLOR_BLUE:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 0);
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 0);
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 1);
                break;
            case LED_COLOR_YELLOW:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 1);
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 1);
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 0);
                break;
            case LED_COLOR_PURPLE:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 1);
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 0);
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 1);
                break;
            case LED_COLOR_CYAN:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 0);
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 1);
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 1);
                break;
            case LED_COLOR_WHITE:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 1);
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 1);
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 1);
                break;
            case LED_COLOR_BLACK:
                drv_output_set_value(OUTPUT_ID_STATE_LED_R, 0);
                drv_output_set_value(OUTPUT_ID_STATE_LED_G, 0);
                drv_output_set_value(OUTPUT_ID_STATE_LED_B, 0);
                break;
            default:
                break;
        }
    }
    else if (led_id == LED_ID_MODE)
    {
        switch (color)
        {
            case LED_COLOR_RED:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 1);
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 0);
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 0);  
                break;
            case LED_COLOR_GREEN:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 0);
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 1);
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 0);
                break;
            case LED_COLOR_BLUE:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 0);
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 0);
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 1);
                break;
            case LED_COLOR_YELLOW:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 1);
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 1);
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 0);
                break;
            case LED_COLOR_PURPLE:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 1);
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 0);
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 1);
                break;
            case LED_COLOR_CYAN:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 0);
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 1);
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 1);
                break;
            case LED_COLOR_WHITE:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 1);
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 1);
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 1);
                break;
            case LED_COLOR_BLACK:
                drv_output_set_value(OUTPUT_ID_MODE_LED_R, 0);
                drv_output_set_value(OUTPUT_ID_MODE_LED_G, 0);
                drv_output_set_value(OUTPUT_ID_MODE_LED_B, 0);
                break;
            default:
                break;
        }
    }
}
