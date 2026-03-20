/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  app_key.c
**  功能描述 :  按键应用层。注册模式/设定/播放/上/下/返回键配置与回调，将按键事件写入事件管理器。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/****************************************************************************\
                               Includes
\****************************************************************************/
#include <cstdint>
#include <stdio.h>
#include <string.h>
#include "app_key.h"
#include "app_light.h"
#include "app_event_process.h"
#include "event_manager.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
#define KEY_MODE_LEVEL     (0)
#define KEY_SHORT_TIME     (50 / KEY_SCAN_TICK_MS)
#define KEY_LONG_TIME      (1000 / KEY_SCAN_TICK_MS)    
#define KEY_LONG_HOLD_TIME (10000 / KEY_SCAN_TICK_MS)
/****************************************************************************\
                            Variables definitions
\****************************************************************************/

/****************************************************************************\
                            Functions definitions
\****************************************************************************/
/**
 * \brief 模式键事件处理函数
 */
static void _key_mode_handler(uint8_t key_id, uint8_t evt, uint8_t comb_key, uint8_t click_cnt)
{
    printf("mode key press: %d %d", evt, key_id);
    event_manager_write(EVENT_MODULE_KEY, evt, key_id, NULL, 0);
}

/**
 * \brief 设定键事件处理函数
 */
static void _key_setting_handler(uint8_t key_id, uint8_t evt, uint8_t comb_key, uint8_t click_cnt)
{
    printf("setting key press: %d %d", evt, key_id);
    event_manager_write(EVENT_MODULE_KEY, evt, key_id, NULL, 0);
}

/**
 * \brief 播放键事件处理函数
 */
static void _key_play_handler(uint8_t key_id, uint8_t evt, uint8_t comb_key, uint8_t click_cnt)
{
    printf("play key press: %d %d", evt, key_id);
    event_manager_write(EVENT_MODULE_KEY, evt, key_id, NULL, 0);
}

/**
* \brief 上键事件处理函数
 */
static void _key_up_handler(uint8_t key_id, uint8_t evt, uint8_t comb_key, uint8_t click_cnt)
{
    printf("up key press: %d %d", evt, key_id);
    event_manager_write(EVENT_MODULE_KEY, evt, key_id, NULL, 0);
}

/**
 * \brief 下键事件处理函数
 */
static void _key_down_handler(uint8_t key_id, uint8_t evt, uint8_t comb_key, uint8_t click_cnt)
{
    printf("down key press: %d %d", evt, key_id);
    event_manager_write(EVENT_MODULE_KEY, evt, key_id, NULL, 0);
}

/**
 * \brief 键事件处理函数
 */
static void _key_back_handler(uint8_t key_id, uint8_t evt, uint8_t comb_key, uint8_t click_cnt)
{
    printf("back key press: %d %d", evt, key_id);
    event_manager_write(EVENT_MODULE_KEY, evt, key_id, NULL, 0);
}


const static TS_KEY_CFG s_key_tab[] = {
    {MODE_KEY, KEY_SHORT_TIME, KEY_LONG_TIME, KEY_LONG_HOLD_TIME, 0, 0, _key_mode_handler},
    {SETTING_KEY, KEY_SHORT_TIME, KEY_LONG_TIME, KEY_LONG_HOLD_TIME, 0, 0, _key_setting_handler},
	{PLAY_KEY, KEY_SHORT_TIME, KEY_LONG_TIME, KEY_LONG_HOLD_TIME, 0, 0, _key_play_handler},
    {UP_KEY, KEY_SHORT_TIME, KEY_LONG_TIME, KEY_LONG_HOLD_TIME, 0, 0, _key_up_handler},
    {DOWN_KEY, KEY_SHORT_TIME, KEY_LONG_TIME, KEY_LONG_HOLD_TIME, 0, 0, _key_down_handler},
    {BACK_KEY, KEY_SHORT_TIME, KEY_LONG_TIME, KEY_LONG_HOLD_TIME, 0, 0, _key_back_handler},
};

/**
 * \brief 用户按键初始化
 */
void app_key_init(void)
{
    svc_key_init(s_key_tab, sizeof(s_key_tab) / sizeof(TS_KEY_CFG));
}

/******************************* End of File (C）****************************/
