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
#ifndef __DRV_LCD_H
#define __DRV_LCD_H

#include "stdint.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define USE_HORIZONTAL 1  	


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 128
#endif

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
void drv_lcd_init(void);

/**
 * \brief 使用DMA批量发送数据
 * \param data: 数据缓冲区
 * \param len: 数据长度(字节)
 * \return 无
 */
void drv_lcd_dma_send_data(const uint8_t *data, uint32_t len);

/**
 * \brief SPI读写一个字节
 * \param byte: 要读写的字节
 * \return 读取到的字节
 */
uint8_t drv_lcd_spi_read_write_byte(uint8_t byte);

/**
 * \brief 写寄存器
 * \param reg: 寄存器地址
 * \return 无
 */
void drv_lcd_write_reg(uint8_t reg);

/**
 * \brief 写数据
 * \param data: 数据
 * \return 无
 */ 
void drv_lcd_write_byte(uint8_t data);

/**
 * \brief 写半字
 * \param data: 数据
 * \return 无
 */
void drv_lcd_write_halfword(uint16_t data);

/**
 * \brief 写半字数组
 * \param dat: 半字数据数组
 * \param len: 数据长度
 * \return 无
 */
void drv_lcd_write_halfword_array(const uint16_t *dat, uint32_t len);

#endif
/****************************************************************************\
                            End of File
\****************************************************************************/


