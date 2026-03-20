/****************************************************************************\
**  版    权 :  
**  文件名称 :  drv_flash.h
**  功能描述 :  Flash 操作封装
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
**                1 首次创建
\****************************************************************************/

#ifndef __DRV_FLASH_H
#define __DRV_FLASH_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x.h"

/****************************************************************************\
                             Macro definitions
\****************************************************************************/

/****************************************************************************\
                             Typedef definitions
\****************************************************************************/
/* Flash操作结果 */
typedef enum
{
    FLASH_OK = 0,               // 成功
    FLASH_ERROR,                // 错误
    FLASH_ERROR_ADDR,           // 地址错误
    FLASH_ERROR_SIZE,           // 大小错误
    FLASH_ERROR_ERASE,          // 擦除错误
    FLASH_ERROR_WRITE,          // 写入错误
    FLASH_ERROR_VERIFY          // 校验错误
} TS_FLASH_RESULT;
/****************************************************************************\
                             Functions definitions
\****************************************************************************/
/**
 * @brief 初始化Flash IAP
 * @return flash_iap_result_t 操作结果
 */
uint8_t drv_flash_init(void);

/**
 * @brief 擦除Flash页
 * @param start_addr 起始地址（必须是页对齐）
 * @param size 要擦除的大小（必须是页大小的整数倍）
 * @return flash_iap_result_t 操作结果
 */
uint8_t drv_flash_erase(uint32_t start_addr, uint32_t size);

/**
 * @brief 写入数据到Flash
 * @param addr 写入地址（必须是半字对齐）
 * @param data 数据指针
 * @param size 数据大小（必须是半字的整数倍）
 * @return flash_iap_result_t 操作结果
 */
uint8_t drv_flash_write(uint32_t addr, const uint8_t *data, uint32_t size);

/**
 * @brief 从Flash读取数据
 * @param addr 读取地址
 * @param data 数据缓冲区
 * @param size 读取大小
 * @return flash_iap_result_t 操作结果
 */
uint8_t drv_flash_read(uint32_t addr, uint8_t *data, uint32_t size);

/**
 * @brief 校验Flash数据
 * @param addr 地址
 * @param data 要校验的数据
 * @param size 数据大小
 * @return flash_iap_result_t 操作结果
 */
uint8_t drv_flash_verify(uint32_t addr, const uint8_t *data, uint32_t size);

#endif /* __FLASH_IAP_H */

/****************************************************************************\
                             End of File
\****************************************************************************/

