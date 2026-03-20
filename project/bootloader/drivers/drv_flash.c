/****************************************************************************\
**  版    权 :  
**  文件名称 :  drv_flash.c
**  功能描述 :  Flash 操作封装。解锁、擦除页、读写及 IAP 相关（与 application 共用逻辑）。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                               Includes
\******************************************************************************/
#include "drv_flash.h"
#include "stm32f10x_flash.h"
#include <string.h>
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
#define FLASH_PAGE_SIZE            (1024)
#define FLASH_BASE_ADDR            (0x08000000)
#define FLASH_TOTAL_SIZE           (128*1024)
#define FLASH_START_ADDR           (0x08000000)
#define FLASH_END_ADDR             (0x08000000 + 128*1024)
#define FLASH_PAGE_NUM             (FLASH_TOTAL_SIZE / FLASH_PAGE_SIZE)
#define FLASH_PAGE_ADDR            (FLASH_START_ADDR + FLASH_PAGE_SIZE)
#define FLASH_PAGE_END_ADDR        (FLASH_START_ADDR + FLASH_PAGE_NUM * FLASH_PAGE_SIZE)
/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/

/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/**
 * @brief 初始化Flash IAP
 */
uint8_t drv_flash_init(void)
{
    // 解锁Flash
    FLASH_Unlock();
    
    // 设置Flash等待周期（根据系统时钟配置）
    FLASH_SetLatency(FLASH_Latency_2);
    
    // 使能预取指
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    
    return FLASH_OK;
}

/**
 * @brief 擦除Flash页
 */
uint8_t drv_flash_erase(uint32_t start_addr, uint32_t size)
{
    FLASH_Status status;
    uint32_t page_addr;
    uint32_t end_addr;
    uint32_t page_num;
    
    // 检查地址范围
    if (start_addr < FLASH_START_ADDR || start_addr >= FLASH_END_ADDR)
    {
        return FLASH_ERROR_ADDR;
    }
    
    // 检查地址对齐
    if (start_addr % FLASH_PAGE_SIZE != 0)
    {
        return FLASH_ERROR_ADDR;
    }
    
    // 检查大小对齐
    if (size % FLASH_PAGE_SIZE != 0)
    {
        return FLASH_ERROR_SIZE;
    }
    
    // 检查是否超出范围
    end_addr = start_addr + size;
    if (end_addr > FLASH_END_ADDR)
    {
        return FLASH_ERROR_SIZE;
    }
    
    // 解锁Flash
    FLASH_Unlock();
    
    // 计算需要擦除的页数
    page_num = size / FLASH_PAGE_SIZE;
    page_addr = start_addr;
    
    // 擦除每一页
    for (uint32_t i = 0; i < page_num; i++)
    {
        status = FLASH_ErasePage(page_addr);
        if (status != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return FLASH_ERROR_ERASE;
        }
        page_addr += FLASH_PAGE_SIZE;
    }
    
    // 锁定Flash
    FLASH_Lock();
    
    return FLASH_OK;
}

/**
 * @brief 写入数据到Flash
 */
uint8_t drv_flash_write(uint32_t addr, const uint8_t *data, uint32_t size)
{
    FLASH_Status status;
    uint16_t *p_data = (uint16_t *)data;
    uint32_t half_word_count;
    uint32_t i;
    
    // 检查地址范围
    if (addr < FLASH_START_ADDR || addr >= FLASH_END_ADDR)
    {
        return FLASH_ERROR_ADDR;
    }
    
    // 检查地址对齐（必须是半字对齐）
    if (addr % 2 != 0)
    {
        return FLASH_ERROR_ADDR;
    }
    
    // 检查大小（必须是半字的整数倍）
    if (size % 2 != 0)
    {
        return FLASH_ERROR_SIZE;
    }
    
    // 检查是否超出范围
    if (addr + size > FLASH_END_ADDR)
    {
        return FLASH_ERROR_SIZE;
    }
    
    // 解锁Flash
    FLASH_Unlock();
    
    // 计算半字数量
    half_word_count = size / 2;
    
    // 写入数据（按半字写入）
    for (i = 0; i < half_word_count; i++)
    {
        status = FLASH_ProgramHalfWord(addr + i * 2, p_data[i]);
        if (status != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return FLASH_ERROR_WRITE;
        }
    }
    
    // 锁定Flash
    FLASH_Lock();
    
    return FLASH_OK;
}

/**
 * @brief 从Flash读取数据
 */
uint8_t drv_flash_read(uint32_t addr, uint8_t *data, uint32_t size)
{
    // 检查地址范围
    if (addr < FLASH_BASE_ADDR || addr + size > FLASH_BASE_ADDR + FLASH_TOTAL_SIZE)
    {
        return FLASH_ERROR_ADDR;
    }
    
    // 直接内存读取
    memcpy(data, (const void *)addr, size);
    
    return FLASH_OK;
}

/**
 * @brief 校验Flash数据
 */
uint8_t drv_flash_verify(uint32_t addr, const uint8_t *data, uint32_t size)
{
    uint8_t *flash_data = (uint8_t *)addr;
    
    // 检查地址范围
    if (addr < FLASH_BASE_ADDR || addr + size > FLASH_BASE_ADDR + FLASH_TOTAL_SIZE)
    {
        return FLASH_ERROR_ADDR;
    }
    
    // 逐字节比较
    for (uint32_t i = 0; i < size; i++)
    {
        if (flash_data[i] != data[i])
        {
            return FLASH_ERROR_VERIFY;
        }
    }
    
    return FLASH_OK;
}


