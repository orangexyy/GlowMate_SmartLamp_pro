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
#include "drv_lcd.h"
#include "plugin_lcd.h"
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


/**
 * \brief 驱动初始化
 */
void driver_init(void)
{    
	
    // // 初始化屏幕
    // plugin_lcd_init();
    // // 填充红色背景
    // plugin_lcd_fill_dma(0, 0, LCD_W, LCD_H, WHITE);  // 红色RGB565编码
    // thread_create(driver_task);
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
			#if 1
            plugin_lcd_fill_dma (0, 0, LCD_W, LCD_H, WHITE);  // 红色RGB565编码 
            plugin_lcd_fill_dma (0, 0, LCD_W, LCD_H, RED);  // 黑色RGB565编码
            plugin_lcd_fill_dma (0, 0, LCD_W, LCD_H, YELLOW);  // 黄色RGB565编码
            plugin_lcd_fill_dma (0, 0, LCD_W, LCD_H, MAGENTA);  // 洋红色RGB565编码
            plugin_lcd_fill_dma (0, 0, LCD_W, LCD_H, CYAN);  // 青色RGB565编码
            plugin_lcd_fill_dma (0, 0, LCD_W, LCD_H, GREEN);  // 绿色RGB565编码 
            plugin_lcd_fill_dma (0, 0, LCD_W, LCD_H, BLUE);  // 蓝色RGB565编码
            plugin_lcd_fill_dma (0, 0, LCD_W, LCD_H, BROWN);  // 棕色RGB565编码
            plugin_lcd_fill_dma (0, 0, LCD_W, LCD_H, BRRED);  // 棕红色RGB565编码
			
			#else
			plugin_lcd_fill (0, 0, LCD_W, LCD_H, WHITE);  // 红色RGB565编码 
            plugin_lcd_fill (0, 0, LCD_W, LCD_H, RED);  // 黑色RGB565编码
            plugin_lcd_fill (0, 0, LCD_W, LCD_H, YELLOW);  // 黄色RGB565编码
            plugin_lcd_fill (0, 0, LCD_W, LCD_H, MAGENTA);  // 洋红色RGB565编码
            plugin_lcd_fill (0, 0, LCD_W, LCD_H, CYAN);  // 青色RGB565编码
            plugin_lcd_fill (0, 0, LCD_W, LCD_H, GREEN);  // 绿色RGB565编码 
            plugin_lcd_fill (0, 0, LCD_W, LCD_H, BLUE);  // 蓝色RGB565编码
            plugin_lcd_fill (0, 0, LCD_W, LCD_H, BROWN);  // 棕色RGB565编码
            plugin_lcd_fill (0, 0, LCD_W, LCD_H, BRRED);  // 棕红色RGB565编码
			#endif
			
            thread_yield();
        }
    }
    thread_end
}

