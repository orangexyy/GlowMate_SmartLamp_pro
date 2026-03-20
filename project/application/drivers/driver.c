/****************************************************************************\
**  文件名称 :  driver.c
**  功能描述 :  驱动层总入口。初始化各子驱动、注册串口接收回调与 M 码处理，并创建驱动协程任务。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "drv_flash.h"
#include "system.h"
#include "driver.h"
#include "drv_comm.h"
#include "drv_key.h"
#include "drv_adc.h"
#include "drv_rgb.h"
#include "drv_dht11.h"
#include "drv_rtc.h"
#include "drv_input.h"
#include "drv_output.h"
#include "drv_timer.h"
#include "plugin_rgb.h"
#include "plugin_led.h"
#include "thread.h"
#include "upgrade_manager.h"
#include "mcode_command.h"
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
static char driver_task (thread_t* pt);

/** 串口1 接收回调：回显并解析 M 码 */
static void usart1_rx_callback_handler(uint8_t *data, uint16_t len)
{
	// printf("%s\r\n", data);
	drv_usart_send_data(DRV_USART_ID_1, data, len);
	process_m_code(DRV_USART_ID_1, (char *)data, len);
}

/** 串口2 接收回调：回显并解析 M 码 */
static void usart2_rx_callback_handler(uint8_t *data, uint16_t len)
{
	process_m_code(DRV_USART_ID_2, (char *)data, len);
}


/**
 * \brief 驱动初始化
 */
void driver_init(void)
{   
    drv_flash_init();
    drv_usart_init(DRV_USART_ID_1, 115200, usart1_rx_callback_handler);         //调试、升级、电脑通信
    drv_usart_init(DRV_USART_ID_2, 115200, usart2_rx_callback_handler);         //语音通信
	drv_usart_init(DRV_USART_ID_3, 9600, NULL);                                 //MP3播放
	drv_key_init();                                                                                 //按键
	drv_adc_init();                                                                                 //ADC
	drv_rgb_init();                                                                                 //RGB
	drv_input_init();
	drv_output_init();
	drv_dht11_init();                                                                               //DHT11
	drv_rtc_init();                                                                                 //RTC
	drv_timer3_init();                                                                              //定时器3
	drv_timer4_init();                                                                              //定时器4
	plugin_rgb_init();                                                                              //RGB插件
	plugin_led_init();
	
	
    thread_create(driver_task);
}


/**
 * \brief 任务
 */
static char driver_task(thread_t* pt)
{
    thread_begin
    {
		
        while (1)
        {

            thread_sleep(500);
        }
    }
    thread_end
}


