/****************************************************************************\
**  文件名称 :  app_light.h
**  功能描述 :  灯光应用模块头文件。定义灯光档位/模式枚举、灯光数据结构及
**             模式/档位/亮度/颜色/挥手等对外接口声明。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                             Includes
\******************************************************************************/
#ifndef APP_LIGHT_H
#define APP_LIGHT_H
#include "stdint.h"
/******************************************************************************\
                          Macro definitions
\******************************************************************************/

/******************************************************************************\
                         Typedef definitions
\******************************************************************************/
typedef enum
{
    APP_LIGHT_LEVEL_NONE   = 0,
    APP_LIGHT_LEVEL_LOW    = 1,
    APP_LIGHT_LEVEL_MID    = 2,
    APP_LIGHT_LEVEL_HIGH   = 3,
    APP_LIGHT_LEVEL_MAX    = 4,
} TE_APP_LIGHT_LEVEL;

typedef enum
{
    APP_LIGHT_MODE_COOL_WHITE ,             //绿色
    APP_LIGHT_MODE_NORMAL_WHITE,            //绿色
    APP_LIGHT_MODE_WARM_WHITE,              //绿色
    APP_LIGHT_MODE_AUTO,                    //绿色
    APP_LIGHT_MODE_COLOR,                   //红色
    APP_LIGHT_MODE_NIGHT,                   //黄色
    APP_LIGHT_MODE_STUDY,                   //蓝色
    APP_LIGHT_MODE_RGB,                     //紫色
    APP_LIGHT_MODE_OFF,                     //白色
    APP_LIGHT_MODE_MAX,
} TE_APP_LIGHT_MODE;

typedef struct {
    uint8_t mode;
    uint8_t level;
    uint8_t brightness;
    uint32_t color;
} TS_APP_LIGHT_DATA;    
/******************************************************************************\
                         Global variables definitions
\******************************************************************************/
extern TS_APP_LIGHT_DATA s_light_data;
/******************************************************************************\
                         Global functions definitions
\******************************************************************************/

/**
 * \brief 应用层通信初始化
 * \return 无
 */
void app_light_init(void);

/**
 * \brief 开关灯光
 * \return 无
 */
void app_light_switch(void);

/**
 * \brief 调整灯光
 * \return 无
 */
void app_light_adjust(void);

/**
 * \brief 设置灯光
 * \return 无
 */
void app_light_set(void);



/**
 * \brief 设置灯光模式
 * \param mode 灯光模式
 */
void app_light_set_mode(uint8_t mode);

/**
 * \brief 获取灯光模式
 * \return 灯光模式
 */
uint8_t app_light_get_mode(void);

/**
 * \brief 设置灯光挡位
 * \param level 灯光挡位
 */
void app_light_set_level(uint8_t level);

/**
 * \brief 获取灯光挡位
 * \return 灯光挡位
 */
uint8_t app_light_get_level(void);

/**
 * \brief 获取灯光亮度
 * \return 灯光亮度
 */
void app_light_set_brightness(uint8_t brightness);

/**
 * \brief 获取灯光亮度
 * \return 灯光亮度
 */
uint8_t app_light_get_brightness(void);

/**
 * \brief 设置灯光颜色
 * \param color 灯光颜色
 */
void app_light_set_color(uint32_t color);

/**
 * \brief 获取灯光颜色
 * \return 灯光颜色
 */
uint32_t app_light_get_color(void);

/**
 * \brief 设置灯光颜色状态
 * \param state 1:渐变模式 0:静态模式
 */
void app_light_set_color_state(uint8_t state);

/**
 * \brief 获取灯光颜色状态
 * \return 灯光颜色状态
 */

uint8_t app_light_get_color_state(void);

/**
 * \brief 设置调整灯光亮度状态
 * \param status 0:按键调整亮度 1:滚轮调整亮度
 */
void app_light_set_adjust_brightness_status(uint8_t status);

/**
 * \brief 获取调整灯光亮度状态
 * \return 调整灯光亮度状态
 */
uint8_t app_light_get_adjust_brightness_status(void);

#endif
/******************************* End of File (C) ******************************/
