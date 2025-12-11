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

/****************************************************************************\
                               Includes
\****************************************************************************/
#ifndef __PLUGIN_LCD_H
#define __PLUGIN_LCD_H

#include "stdint.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
//画笔颜色
#define WHITE         	    0xFFFF
#define BLACK         	    0x0000	  
#define BLUE           	    0x001F  
#define BRED                0XF81F
#define GRED 			    0XFFE0
#define GBLUE			    0X07FF
#define RED           	    0xF800
#define MAGENTA       	    0xF81F
#define GREEN         	    0x07E0
#define CYAN          	    0x7FFF
#define YELLOW        	    0xFFE0
#define BROWN 			    0XBC40 //棕色
#define BRRED 			    0XFC07 //棕红色
#define GRAY  			    0X8430 //灰色
#define DARKBLUE      	    0X01CF	//深蓝色
#define LIGHTBLUE      	    0X7D7C	//浅蓝色  
#define GRAYBLUE       	    0X5458 //灰蓝色
#define LIGHTGREEN     	    0X841F //浅绿色
#define LGRAY 			    0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE           0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE              0X2B12 //浅棕蓝色(选择条目的反色)
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

/****************************************************************************\
                            Variables definitions
\****************************************************************************/

/****************************************************************************\
                            Functions definitions
\****************************************************************************/
/**
 * \brief LCD初始化
 * \return 无
 */
void plugin_lcd_init(void);

/**
 * \brief 地址设置
 * \param x1: 起始X坐标
 * \param y1: 起始Y坐标
 * \param x2: 结束X坐标
 * \param y2: 结束Y坐标
 * \return 无
 */
void plugin_lcd_address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);

/**
 * \brief LVGL DMA刷屏函数（传输指定颜色缓冲区数据）
 * \param x1: 起始X坐标（包含）
 * \param y1: 起始Y坐标（包含）
 * \param x2: 结束X坐标（包含）
 * \param y2: 结束Y坐标（包含）
 * \param color_map: 16位颜色数组指针（存储待显示的像素数据）
 * \return 无
 */
 void plugin_lcd_color_fill_dma(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const uint16_t *color_map);

/**
 * \brief 使用DMA批量发送数据
 * \param xsta: 起始X坐标
 * \param ysta: 起始Y坐标
 * \param xend: 结束X坐标
 * \param yend: 结束Y坐标
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_fill_dma(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);

/**
 * \brief 清屏
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_clear(uint16_t color);

/**
 * \brief 填充
 * \param xsta: 起始X坐标
 * \param ysta: 起始Y坐标
 * \param xend: 结束X坐标
 * \param yend: 结束Y坐标
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);

/**
 * \brief 颜色填充
 * \param xsta: 起始X坐标
 * \param ysta: 起始Y坐标
 * \param xend: 结束X坐标
 * \param yend: 结束Y坐标
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_color_fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t *color);    

void plugin_lcd_draw_point(uint16_t x,uint16_t y,uint16_t color);
void plugin_lcd_draw_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void plugin_lcd_draw_rectangle(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void plugin_lcd_draw_circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);
void plugin_lcd_show_char(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void plugin_lcd_show_string(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void plugin_lcd_show_int_num(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);
void plugin_lcd_show_float_num(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);
void plugin_lcd_show_picture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[]);
uint32_t plugin_lcd_mypow(uint8_t m,uint8_t n);

#endif
