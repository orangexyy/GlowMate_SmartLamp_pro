/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  drv_led.c
**  功能描述 :  LED 驱动封装。初始化及按 LED ID 写值（与 plugin_led 配合驱动状态/模式灯）。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include "drv_led.h"
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
 * \brief 初始化LED
 */
void drv_led_init(void)
{

}

/**
 * \brief 设置LED值
 * \param[in] led_id LED ID
 * \param[in] value LED值
 */
void drv_led_write_value(uint8_t led_id, uint8_t value)
{
    switch (led_id)
    {
        case LED_ID_POWER: 
            break;

        case LED_ID_WIFI: 
            break; 
        
        default:
            break;
    }
}

/**
 * \brief 设置LED RGB值, 0-255
 */
void drv_led_write_rgb(uint8_t led_id, uint8_t red, uint8_t green, uint8_t blue)
{
    switch (led_id)
    {
        case LED_ID_POWER: 
            break;

        case LED_ID_WIFI: 

            break;      

        default:
            break;
    }
}

/**
* \brief 设置LED RGB值, 0-255
*/
void drv_led_write_rgbw(uint8_t led_id, uint8_t red, uint8_t green, uint8_t blue, uint8_t white)
{
    switch (led_id)
    {
        case LED_ID_POWER: 
            break;

        case LED_ID_WIFI: 
            break;      

        default:
            break;
    }
}
 

/******************************* End of File (C) ******************************/
