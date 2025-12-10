/****************************************************************************\
**  版    权 : 
**  文件名称 :  
**  功能描述 :  
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>

#include "driver.h"
#include "system.h"
#include "drv_comm.h"
#include "drv_key.h"
#include "drv_input.h"
#include "drv_output.h"
#include "drv_pwm.h"    
#include "drv_adc.h"
#include "drv_oled.h"
#include "drv_rtc.h"
#include "drv_timer.h"
#include "drv_rgb.h"
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

/******************************************************************************\
                             Functions definitions
\******************************************************************************/
static char driver_task (thread_t* pt);

static void usart1_rx_callback_handler(uint8_t *data, uint16_t len)
{
	printf("data:%s len:%d\r\n",data, len);
}

uint8_t rgbRed[][3] = {{0xff, 0xff, 0x00}};
uint8_t rgbRed1[][3] = {{0x00, 0xff, 0x00}};
uint8_t rgbRed2[][3] = {{0x00, 0x00, 0xff}};
uint8_t rgbRed3[][3] = {{0xff, 0x00, 0x00}};

// static void usart2_rx_callback_handler(uint8_t *data, uint16_t len)
// {
// 	printf("data:%s len:%d\r\n", data, len);
// 	drv_usart_dma_send_data(DRV_USART_ID_2, (uint8_t*) data, len);
// }

// int cnt = 0;

// static void timer4_irq_callback(void)
// {
// 	cnt++;
	
// 	printf("timer4_irq_callback cnt:%d\r\n",cnt);
// }

/**
 * \brief 驱动初始化
 */
void driver_init(void)
{    
//    drv_usart_init(DRV_USART_ID_1, 115200, usart1_rx_callback_handler);
    // drv_usart_init(DRV_USART_ID_2, 115200, usart2_rx_callback_handler);
    // drv_adc_init();
    // drv_input_init();
    drv_output_init();
    // drv_key_init();
    // drv_oled_i2c_init();
    // drv_pwm_init();

    // TS_TIMER_HANDLE timer4_handle = {
    //     .id = DRV_TIMER_ID_4,
    //     .prescaler = 8399,
    //     .Period = 9999,
    // };
    // drv_timer_init(&timer4_handle, timer4_irq_callback);    
    // drv_rtc_init();
    // drv_rtc_set_alarm(0, 53, 00);
    // drv_dht11_init();
    
    drv_rgb_init();

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
            drv_rgb_send(0x0000ff00);
            delay_ms(1000);
            drv_rgb_send(0x00ff0000);
            delay_ms(1000);
            drv_rgb_send(0xff000000);
            delay_ms(1000);
            thread_sleep(1000);
        }
    }
    thread_end
}

