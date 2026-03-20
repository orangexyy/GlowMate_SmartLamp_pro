/****************************************************************************\
**  文件名称 :  plugin_rgb.h
**  功能描述 :  RGB 灯带插件头文件。灯珠数量、颜色宏及 RGB 接口声明。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/****************************************************************************\
                               Includes
\****************************************************************************/
#ifndef __PLUGIN_RGB_H
#define __PLUGIN_RGB_H

#include "stdint.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
#define PLUGIN_RGB_COLOR_COOL_WHITE         0xC0C8FF // 冷白色（G=192, R=200, B=255）
#define PLUGIN_RGB_COLOR_WARM_WHITE         0xFFFFF60 // 暖白色（G=255, R=255, B=246）
#define PLUGIN_RGB_COLOR_WHITE              0xFFFFFF  // 白色（G=255, R=255, B=255）
#define PLUGIN_RGB_COLOR_BLACK              0x000000  // 黑色（全灭）
#define PLUGIN_RGB_COLOR_RED                0x00FF00  // 红色（R=255, G=0, B=0）
#define PLUGIN_RGB_COLOR_GREEN              0xFF0000  // 绿色（G=255, R=0, B=0）
#define PLUGIN_RGB_COLOR_BLUE               0x0000FF  // 蓝色（B=255, G=0, R=0）
#define PLUGIN_RGB_COLOR_YELLOW             0xFFFF00  // 黄色（G=255, R=255, B=0）
#define PLUGIN_RGB_COLOR_PURPLE             0x00FFFF  // 紫色（R=255, B=255, G=0）
#define PLUGIN_RGB_COLOR_CYAN               0xFF00FF  // 青色（G=255, B=255, R=0）
#define PLUGIN_RGB_COLOR_ORANGE             0xFF8000  // 橙色（G=255, R=128, B=0）
#define PLUGIN_RGB_COLOR_PINK               0x80FFFF  // 粉色（G=128, R=255, B=255）
/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/

/******************************************************************************\
                             Functions definitions
\******************************************************************************/

void plugin_rgb_init(void);
void plugin_rgb_set_all_color(uint32_t color);
void plugin_rgb_clear_all(void);
void plugin_rgb_running_light(uint32_t color, uint16_t delay_ms_val);
void plugin_rgb_gradient_color(uint32_t color_start, uint32_t color_end, uint16_t step, uint16_t delay_ms_val);
void plugin_rgb_gradient_all_color_node(uint16_t step, uint16_t delay_ms_val);
void plugin_rgb_gradient_smooth_grb(uint16_t delay_ms_val);
void plugin_rgb_gradient_all_color_node_nonblocking(uint16_t step, uint16_t delay_ms_val);

void plugin_rgb_set_pixel_rgb(uint16_t pixel_idx, uint8_t g, uint8_t r, uint8_t b);
void plugin_rgb_set_pixel_color(uint16_t pixel_idx, uint32_t color);
void plugin_rgb_set_all_rgb(uint8_t g, uint8_t r, uint8_t b);   

// 占空比亮度控制函数
void plugin_rgb_set_pixel_brightness(uint16_t pixel_idx, uint32_t color, uint8_t duty_cycle);
void plugin_rgb_set_all_brightness(uint32_t color, uint8_t duty_cycle);
void plugin_rgb_set_pixel_rgb_brightness(uint16_t pixel_idx, uint8_t g, uint8_t r, uint8_t b, uint8_t duty_cycle);
void plugin_rgb_set_all_rgb_brightness(uint8_t g, uint8_t r, uint8_t b, uint8_t duty_cycle);
void plugin_rgb_adjust_pixel_brightness(uint16_t pixel_idx, uint8_t duty_cycle);
void plugin_rgb_adjust_all_brightness(uint8_t duty_cycle);

#endif
