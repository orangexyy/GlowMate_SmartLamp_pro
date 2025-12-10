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
#include "app_led.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define COLOR_RED       {255, 0, 0},     // red
#define COLOR_GREEN     {0, 255, 0},     // green
#define COLOR_BLUE      {0, 0, 255},     // blue
#define COLOR_YELLOW    {255, 255, 0},   // yellow
#define COLOR_HOT_PINK  {255, 105, 180}, // hot pink
#define COLOR_DEEP_PINK {255, 20, 147},  // deep pink
#define COLOR_PURPLE    {75, 0, 128},    // purple
#define COLOR_CYAN      {0, 255, 255},   // cyan
#define COLOR_DARK_BLUE {0, 0, 139},     // dark blue
#define COLOR_ORANGE    {255, 165, 0},   // orange
#define COLOR_BROWN     {165, 42, 42},   // brown
#define COLOR_WHITE     {255, 255, 255}, // white
#define COLOR_BLACK     {0, 0, 0},       // black
#define COLOR_TEST      {0, 250, 154},   // TEST
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/
/**
 * \brief 电源LED样式表
 */
static const TS_LED_TABLE s_PowerLedTable[] =
{
    //样式表        间隔                        亮度    重复次数   
    {"- ",         500/LED_SCAN_INTERVAL,        255,        0},  
    {"- ",         100/LED_SCAN_INTERVAL,        255,        0}, 
};

/** 
 *  显示字符表示方法:
 *  ' ' : 表示灭, 
 *  '-' : 表示亮, 
 *  'v' : 表示呼吸灯样式, 会一直循环, 直到调用Stop为止
 *  '/' : 表示由灭慢慢到亮, 
 *  '\' : 表示由亮慢慢到灭,  
 */
//static const TS_LED_TABLE s_LedTable[] =
//{
//    //样式表        间隔                         亮度    重复次数      色值表
//    {"- ",         100/LED_SCAN_INTERVAL,        255,        0,      COLOR_RED},
//    {"- ",         200/LED_SCAN_INTERVAL,        100,        0,      COLOR_GREEN},
//    {"- ",         300/LED_SCAN_INTERVAL,        150,        0,      COLOR_BLUE},
//    {"-",          500/LED_SCAN_INTERVAL,        230,        0,      COLOR_ORANGE},
//    {"v",           20/LED_SCAN_INTERVAL,         80,        0,      COLOR_DEEP_PINK},
//    {"/---   ",     20/LED_SCAN_INTERVAL,        255,        0,      COLOR_ORANGE},
//    {"---\\  ",     20/LED_SCAN_INTERVAL,        150,        0,      COLOR_HOT_PINK},
//};

/****************************************************************************\
                            Variables definitions
\****************************************************************************/

/****************************************************************************\
                            Functions definitions
\****************************************************************************/

void app_led_init(void)
{
    svc_led_init(LED_ID_POWER, (TS_LED_TABLE *)s_PowerLedTable, sizeof(s_PowerLedTable) / sizeof(TS_LED_TABLE)); 

    svc_led_set_mode(LED_ID_POWER, 0);
}

/******************************* End of File (C）****************************/
