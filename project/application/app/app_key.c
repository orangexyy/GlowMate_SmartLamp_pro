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
#include <stdio.h>
#include <string.h>
#include "app_key.h"
#include "app.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/

/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
#define KEY_MODE_LEVEL     (0)
#define KEY_SHORT_TIME     (50 / KEY_SCAN_TICK_MS)
#define KEY_LONG_TIME      (1000 / KEY_SCAN_TICK_MS)    
#define KEY_LONG_HOLD_TIME (3000 / KEY_SCAN_TICK_MS)
/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/**
 * \brief 模式键事件处理函数
 */
static void _key_mode_handler(uint8_t key_id, uint8_t evt, uint8_t comb_key, uint8_t click_cnt)
{
    printf("mode key press: %d %d", evt, click_cnt);
    switch (evt)
    {
        case KEY_PRESS_SHORT_UP:
            app_data.mode++;
            if (app_data.mode > MODE_MAX - 1)
            {
                app_data.mode = 0;
            }
            break;

        case KEY_PRESS_LONG_HOLD:
            break;

        default:
            break;
    }
}

/**
 * \brief 设定键事件处理函数
 */
//static void _key_setting_handler(uint8_t key_id, uint8_t evt, uint8_t comb_key, uint8_t click_cnt)
// {
//     LOG_I("mode key press: %d %d", evt, click_cnt);
//     switch (evt)
//     {
//        case KEY_PRESS_SHORT_UP:
//            break;

//        case KEY_PRESS_LONG_HOLD:
//            break;

//        default:
//            break;
//    }
//}

const static TS_KEY_CFG s_key_tab[] = {
    {MODE_KEY, KEY_SHORT_TIME, KEY_LONG_TIME, KEY_LONG_HOLD_TIME, 0, 0, _key_mode_handler},
    // {SETTING_KEY, KEY_SHORT_TIME, KEY_LONG_TIME, KEY_LONG_HOLD_TIME, 0, 0, _key_setting_handler},
};

/**
 * \brief 用户按键初始化
 */
void app_key_init(void)
{
    svc_key_init(s_key_tab, sizeof(s_key_tab) / sizeof(TS_KEY_CFG));
}

/******************************* End of File (C）****************************/
