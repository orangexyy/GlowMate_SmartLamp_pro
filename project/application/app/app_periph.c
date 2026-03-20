/****************************************************************************\
**  文件名称 :  app_periph.c
**  功能描述 :  外设状态应用。周期读取 DHT11 温湿度、光感/电源/充电 ADC，并据充电与电量控制状态灯颜色。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "app.h"
#include "driver.h"
#include "plugin_led.h"
#include "app_periph.h"
#include "thread.h"

/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
static uint8_t s_periph_dht11_temp = 0;
static uint8_t s_periph_dht11_humi = 0;
static uint8_t s_periph_brightness = 0;
static uint8_t s_periph_power = 0;
static uint8_t s_periph_charge = 0;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
static char app_periph_task (thread_t* pt);

/**
 * \brief 外设应用初始化：创建外设状态轮询协程
 */
void app_periph_init(void)
{

    thread_create(app_periph_task);
}

/**
 * \brief 外设协程任务：周期读取温湿度与 ADC，更新状态灯
 */
static char app_periph_task(thread_t* pt)
{
    thread_begin
    {
        while (1)
        {
			drv_dht11_read_data(&s_periph_dht11_temp, &s_periph_dht11_humi);
            s_periph_brightness = drv_adc_get_value(ADC_ID_LIGHT_SENSOR_1) * 100 / 4095;
            s_periph_power = drv_adc_get_value(ADC_ID_POWER_DETECT) * 100 / 4095;
            s_periph_charge = drv_adc_get_value(ADC_ID_LIGHT_SENSOR_2) * 100 / 4095;

            printf("s_periph_charge: %d\r\n", s_periph_charge);
            printf("s_periph_power: %d\r\n", s_periph_power);
            printf("s_periph_brightness: %d\r\n", s_periph_brightness);

            if (s_periph_charge > 50)
            {
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_GREEN);
            }
            else if (s_periph_power < 10)
            {
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_RED);
            }
            else
            {
                plugin_led_set_color(LED_ID_STATE, LED_COLOR_WHITE);
            }



		    thread_sleep(1000);
        }
    }
    thread_end
}

void app_periph_get_dht11_data(uint8_t *temp, uint8_t *humi)
{
    *temp = s_periph_dht11_temp;
    *humi = s_periph_dht11_humi;
}

uint8_t app_periph_get_brightness(void)
{
    return s_periph_brightness;
}

uint8_t app_periph_get_power(void)
{
    return s_periph_power;
}

/******************************* End of File (C) ******************************/
