/****************************************************************************\
**  文件名称 :  plugin_mp3.c
**  功能描述 :  Bootloader MP3 插件（若使用）。串口控制播放等（与 application 类似）。
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
#include "drv_comm.h"
#include "drv_adc.h"
#include "plugin_mp3.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
static uint8_t s_mp3_state = 0;
static uint8_t s_mp3_volume = 10;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/**
 * \brief MP3初始化
 * \return 无
 */
void plugin_mp3_init(void)
{
    drv_usart_init(DRV_USART_ID_3, 9600, NULL);
    
    //设置循环播放
    plugin_mp3_set_loop(1);

}

/**
 * \brief 播放
 * \return 无
 */
void plugin_mp3_play(void)
{
    s_mp3_state = MP3_STATE_PLAY;
    uint8_t data[] = {0x7E, 0xFF, 0x06, 0x0D, 0x00, 0x00, 0x00, 0xEF}; //播放命令
    drv_usart_send_data(DRV_USART_ID_3, data, sizeof(data));
}

/**
 * \brief 暂停
 * \return 无
 */
void plugin_mp3_pause(void)
{
    s_mp3_state = MP3_STATE_PAUSE;
    uint8_t data[] = {0x7E, 0xFF, 0x06, 0x0E, 0x00, 0x00, 0x00, 0xEF}; //暂停命令
    drv_usart_send_data(DRV_USART_ID_3, data, sizeof(data));
}

/**
 * \brief 播放上一首
 * \return 无
 */
void plugin_mp3_previous(void)
{
    uint8_t data[] = {0x7E, 0xFF, 0x06, 0x01, 0x00, 0x00, 0x00, 0xEF}; //上一首命令
    drv_usart_send_data(DRV_USART_ID_3, data, sizeof(data));
}

/**
 * \brief 播放下一首
 * \return 无
 */
void plugin_mp3_next(void)
{
    uint8_t data[] = {0x7E, 0xFF, 0x06, 0x02, 0x00, 0x00, 0x00, 0xEF};
    drv_usart_send_data(DRV_USART_ID_3, data, sizeof(data));
}

/**
 * \brief 设置循环播放
 * \return 无
 */
void plugin_mp3_set_loop(uint8_t loop)
{
    uint8_t data[] = {0x7E, 0xFF, 0x06, 0x11, 0x00, 0x00, 0x01, 0xEF};
//    if (loop)
//    {
//        data[6] = 0x01;
//    }
//    else
//    {
//        data[6] = 0x00;
//    }
    drv_usart_send_data(DRV_USART_ID_3, data, sizeof(data));
}

uint8_t plugin_mp3_get_state(void)
{
    return s_mp3_state;
}

uint8_t plugin_mp3_get_volume(void)
{
    return s_mp3_volume;
}

uint8_t plugin_mp3_set_volume(uint8_t volume)
{
    s_mp3_volume = volume;
    uint8_t data[] = {0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, 0x00, 0xEF}; //设置音量命令
    data[6] = s_mp3_volume;
    drv_usart_send_data(DRV_USART_ID_3, data, sizeof(data));
    return 0;
}




