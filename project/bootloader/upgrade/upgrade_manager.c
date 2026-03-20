/****************************************************************************\
**  文件名称 :  upgrade_manager.c
**  功能描述 :  Bootloader 升级管理。读取/初始化升级信息、串口接收固件、Xmodem、写 Flash、跳转与重启。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

#include "upgrade_manager.h"
#include "upgrade_iap.h"
#include "drv_comm.h"
#include "drv_flash.h"
#include "system.h"
#include "project_config.h"
#include <string.h>
#include <stdio.h>
#include "thread.h"

/** 串口1：用于接收固件数据（Xmodem 协议） */
extern uint8_t usart1_rx_buffer[USART1_RX_BUF_SIZE];
extern uint16_t usart1_rx_len;
extern volatile bool usart1_rx_flag;

// 当前升级事件
static uint8_t upgrade_current_event = UPGRADE_MANAGER_EVENT_NONE;

/**
 * @brief 读取升级信息
 * @param info 升级信息结构体指针
 * @return 1-成功，0-失败
 */
static uint8_t upgrade_read_upgrade_info(upgrade_info_t *info)
{
    if (info == NULL)
    {
        return 0;
    }
    
    if (drv_flash_read(UPGRADE_INFO_ADDRESS, (uint8_t *)info, sizeof(upgrade_info_t)) == FLASH_OK)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief 初始化升级信息区域（首次使用或魔数无效时调用）
 * @return 1-成功，0-失败
 */
uint8_t upgrade_init_upgrade_info(void)
{
    upgrade_info_t info;
    
    printf("Initializing upgrade info area...\r\n");
    
    // 先擦除升级信息页
    if (drv_flash_erase(UPGRADE_INFO_ADDRESS, UPGRADE_INFO_SIZE) != FLASH_OK)
    {
        printf("Failed to erase upgrade info area\r\n");
        return 0;
    }
    
    // 初始化结构体
    memset(&info, 0xFF, sizeof(upgrade_info_t));
    info.magic = UPGRADE_MAGIC;
    info.upgrade_flag = UPGRADE_FLAG_NO_UPGRADE;
    info.version = 0xFFFFFFFF;  // 无效版本号
    info.crc32 = 0xFFFFFFFF;    // 无效CRC32
    info.boot_version = BOOTLOADER_VERSION;  // 记录当前 Boot 版本

    // 写入升级信息（前20字节：magic + upgrade_flag + version + crc32 + boot_version）
    if (drv_flash_write(UPGRADE_INFO_ADDRESS, (uint8_t *)&info, sizeof(uint32_t) * 5) != FLASH_OK)
    {
        printf("Failed to write upgrade info\r\n");
        return 0;
    }
    
    printf("Upgrade info initialized successfully\r\n");
    printf("  Magic: 0x%08X\r\n", (unsigned int)info.magic);
    printf("  Upgrade flag: 0x%08X (No upgrade)\r\n", (unsigned int)info.upgrade_flag);
    printf("  Boot version: %s (0x%08X)\r\n", BOOTLOADER_VERSION_STR, (unsigned int)info.boot_version);
    return 1;
}

/**
 * @brief 系统重启函数（可在application中调用）
 */
void upgrade_system_reset(void)
{
    // 禁用所有中断，确保复位操作能够执行
    __disable_irq();
    
    // 等待一小段时间，确保所有操作完成
    delay_ms(10);
    
    // 执行系统复位
    NVIC_SystemReset();
    
    // 如果复位失败，进入死循环
    while(1);
}

/**
 * @brief 设置升级标志（可在application中调用）
 * @param version 固件版本号（可选，0xFFFFFFFF表示无效）
 * @return 1-成功，0-失败
 * @note 调用此函数后，需要调用upgrade_system_reset()重启系统才能进入升级模式
 */
uint8_t upgrade_set_upgrade_flag(uint32_t version)
{
    upgrade_info_t info;
    uint8_t need_erase = 0;
    
    printf("Reading upgrade info from address 0x%08X...\r\n", (unsigned int)UPGRADE_INFO_ADDRESS);
    
    // 读取当前升级信息
    if (upgrade_read_upgrade_info(&info))
    {
        printf("Upgrade info read OK, magic=0x%08X\r\n", (unsigned int)info.magic);
        
        // 确保魔数有效
        if (info.magic != UPGRADE_MAGIC)
        {
            printf("Magic number invalid, need to erase and initialize\r\n");
            need_erase = 1;
        }
        else
        {
            printf("Magic number valid, current flag=0x%08X\r\n", (unsigned int)info.upgrade_flag);
            
            // 检查是否需要修改标志值
            // 如果当前标志不是需要升级的标志，则需要擦除（因为Flash只能将1变为0，不能将0变为1）
            if (info.upgrade_flag != UPGRADE_FLAG_NEED_UPGRADE)
            {
                printf("Flag value needs to change, need to erase before write\r\n");
                need_erase = 1;
            }
        }
        
        // 如果需要擦除，先擦除Flash页
        if (need_erase)
        {
            printf("Erasing upgrade info area...\r\n");
            if (drv_flash_erase(UPGRADE_INFO_ADDRESS, UPGRADE_INFO_SIZE) != FLASH_OK)
            {
                printf("Failed to erase upgrade info area\r\n");
                return 0;
            }
            printf("Upgrade info area erased successfully\r\n");
            
            // 初始化结构体
            memset(&info, 0xFF, sizeof(upgrade_info_t));
            info.magic = UPGRADE_MAGIC;
        }
        
        // 设置升级标志为需要升级
        info.upgrade_flag = UPGRADE_FLAG_NEED_UPGRADE;
        
        // 设置版本号（如果提供）
        if (version != 0xFFFFFFFF)
        {
            info.version = version;
            printf("Setting version: 0x%08X\r\n", (unsigned int)version);
        }
        
        printf("Writing upgrade flag (magic=0x%08X, flag=0x%08X)...\r\n", 
               (unsigned int)info.magic, (unsigned int)info.upgrade_flag);
        
        // 写入修改后的信息（写入前12字节：magic + upgrade_flag + version）
        if (drv_flash_write(UPGRADE_INFO_ADDRESS, (uint8_t *)&info.magic, sizeof(uint32_t) * 3) != FLASH_OK)
        {
            printf("Failed to write upgrade flag to Flash\r\n");
            return 0;
        }
        
        printf("Upgrade flag written successfully\r\n");
        return 1;
    }
    else
    {
        printf("Failed to read upgrade info, trying to initialize...\r\n");
        // 如果读取失败，尝试初始化
        if (upgrade_init_upgrade_info())
        {
            printf("Upgrade info initialized, reading again...\r\n");
            // 重新读取
            if (upgrade_read_upgrade_info(&info))
            {
                info.upgrade_flag = UPGRADE_FLAG_NEED_UPGRADE;
                if (version != 0xFFFFFFFF)
                {
                    info.version = version;
                }
                printf("Writing upgrade flag (flag=0x%08X)...\r\n", (unsigned int)info.upgrade_flag);
                if (drv_flash_write(UPGRADE_INFO_ADDRESS, (uint8_t *)&info.magic, sizeof(uint32_t) * 3) != FLASH_OK)
                {
                    printf("Failed to write upgrade flag after initialization\r\n");
                    return 0;
                }
                printf("Upgrade flag written successfully after initialization\r\n");
                return 1;
            }
            else
            {
                printf("Failed to read upgrade info after initialization\r\n");
            }
        }
        else
        {
            printf("Failed to initialize upgrade info\r\n");
        }
        return 0;
    }
}

/**
 * @brief 清除升级标志（升级完成后调用）
 */
void upgrade_clear_upgrade_flag(void)
{
    upgrade_info_t info;
    uint8_t need_erase = 0;
    
    // 读取当前升级信息
    if (upgrade_read_upgrade_info(&info))
    {
        // 确保魔数有效
        if (info.magic != UPGRADE_MAGIC)
        {
            // 如果魔数无效，需要先擦除再初始化
            printf("Magic number invalid when clearing flag, need to erase\r\n");
            need_erase = 1;
            memset(&info, 0xFF, sizeof(upgrade_info_t));
            info.magic = UPGRADE_MAGIC;
        }
        else
        {
            // 检查是否需要修改标志值
            // 如果当前标志不是不需要升级的标志，则需要擦除（因为Flash只能将1变为0，不能将0变为1）
            if (info.upgrade_flag != UPGRADE_FLAG_NO_UPGRADE)
            {
                printf("Flag value needs to change when clearing, need to erase before write\r\n");
                need_erase = 1;
            }
        }
        
        // 如果需要擦除，先擦除Flash页
        if (need_erase)
        {
            printf("Erasing upgrade info area before clearing flag...\r\n");
            if (drv_flash_erase(UPGRADE_INFO_ADDRESS, UPGRADE_INFO_SIZE) != FLASH_OK)
            {
                printf("Failed to erase upgrade info area\r\n");
                return;
            }
            // 重新初始化结构体（保留 version/crc32 的当前值，补全 boot_version，避免清除标志后 APP 读不到 boot 版本）
            memset(&info, 0xFF, sizeof(upgrade_info_t));
            info.magic = UPGRADE_MAGIC;
            info.boot_version = BOOTLOADER_VERSION;
        }
        
        // 修改标志字段为不需要升级
        info.upgrade_flag = UPGRADE_FLAG_NO_UPGRADE;
        
        // 写入前 20 字节（magic + upgrade_flag + version + crc32 + boot_version），避免只写 8 字节导致 boot_version 丢失
        if (drv_flash_write(UPGRADE_INFO_ADDRESS, (uint8_t *)&info.magic, sizeof(uint32_t) * 5) != FLASH_OK)
        {
            printf("Failed to clear upgrade flag\r\n");
        }
        else
        {
            printf("Upgrade flag cleared\r\n");
        }
    }
    else
    {
        printf("Failed to read upgrade info for clearing flag\r\n");
    }
}

/**
 * @brief 若升级区尚未写入（magic 为 0xFF），则直接写入 20 字节（不擦除），便于首次升级后 APP 能读到 boot_version
 */
void upgrade_ensure_boot_version_if_needed(void)
{
    uint32_t magic_read = 0xFFFFFFFF;

    if (drv_flash_read(UPGRADE_INFO_ADDRESS, (uint8_t *)&magic_read, sizeof(uint32_t)) != FLASH_OK)
    {
        return;
    }
    if (magic_read != 0xFFFFFFFF)
    {
        return; /* 已写过，不覆盖 */
    }
    /* 升级区仍为 0xFF（首次从“APP 无效”进入 IAP 完成升级）：直接写 20 字节，无需擦除 */
    {
        upgrade_info_t info;

        memset(&info, 0xFF, sizeof(upgrade_info_t));
        info.magic = UPGRADE_MAGIC;
        info.upgrade_flag = UPGRADE_FLAG_NO_UPGRADE;
        info.version = 0xFFFFFFFF;
        info.crc32 = 0xFFFFFFFF;
        info.boot_version = BOOTLOADER_VERSION;

        if (drv_flash_write(UPGRADE_INFO_ADDRESS, (uint8_t *)&info.magic, sizeof(uint32_t) * 5) == FLASH_OK)
        {
            printf("Upgrade info written (boot_version %s) for first upgrade\r\n", BOOTLOADER_VERSION_STR);
        }
    }
}

/**
 * @brief 检查是否需要升级
 * @return 1-需要升级，0-不需要升级
 */
uint8_t upgrade_check_upgrade_flag(void)
{
    upgrade_info_t info;
    
    printf("Checking upgrade flag...\r\n");
    
    if (upgrade_read_upgrade_info(&info))
    {
        printf("Upgrade info read OK\r\n");
        printf("  Magic: 0x%08X (expected: 0x%08X)\r\n", 
               (unsigned int)info.magic, (unsigned int)UPGRADE_MAGIC);
        printf("  Flag: 0x%08X\r\n", (unsigned int)info.upgrade_flag);
        
        // 首先检查魔数，验证升级信息有效性
        if (info.magic != UPGRADE_MAGIC)
        {
            printf("Upgrade info magic invalid!\r\n");
            /* 全片擦除后首次上电：升级区为 0xFF，且与 Boot 同扇区，若此时擦除可能影响运行或导致复位。
             * 不在此处初始化，直接当作“无升级标志”，后续会因 APP 无效而进入 IAP 并发送 'C'；
             * 升级信息区将在后续（如清除升级标志、APP 设置标志）时再写入。 */
            return 0;
        }
        else if (info.boot_version == 0xFFFFFFFF)
        {
            /* 魔数有效但 boot_version 未写入（多为 APP 先初始化了升级区只写 16 字节），补写 boot_version */
            printf("Boot version not set, repairing (write boot_version)...\r\n");
            if (drv_flash_erase(UPGRADE_INFO_ADDRESS, UPGRADE_INFO_SIZE) != FLASH_OK)
            {
                printf("Repair erase failed, skip\r\n");
            }
            else
            {
                info.boot_version = BOOTLOADER_VERSION;
                if (drv_flash_write(UPGRADE_INFO_ADDRESS, (uint8_t *)&info.magic, sizeof(uint32_t) * 5) == FLASH_OK)
                {
                    printf("  Boot version: %s (0x%08X)\r\n", BOOTLOADER_VERSION_STR, (unsigned int)info.boot_version);
                }
            }
        }
        
        // 检查升级标志
        if (info.upgrade_flag == UPGRADE_FLAG_NEED_UPGRADE)
        {
            printf("Upgrade flag detected: Need upgrade!\r\n");
            if (info.version != 0xFFFFFFFF)
            {
                printf("  Firmware version: 0x%08X\r\n", (unsigned int)info.version);
            }
            return 1;
        }
        else if (info.upgrade_flag == UPGRADE_FLAG_NO_UPGRADE)
        {
            printf("Upgrade flag: No upgrade needed\r\n");
            return 0;
        }
        else
        {
            // 未知的标志值，默认为不需要升级
            printf("Upgrade flag: Unknown value (0x%08X), assume no upgrade needed\r\n", 
                   (unsigned int)info.upgrade_flag);
            return 0;
        }
    }
    else
    {
        printf("Failed to read upgrade info, assume no upgrade needed\r\n");
        return 0;
    }
}

/**
 * @brief 升级分支处理（主流程）
 */
void upgrade_branch(void)
{
    // 设置printf输出到串口1
    drv_usart_set_printf_port(DRV_USART_ID_1);
    
    // 延迟，确保printf串口设置生效
    delay_ms(100);
    
    // 先发送一些测试字符，确保串口工作
    drv_usart_send_string(DRV_USART_ID_1, "\r\n");
    delay_ms(50);
    
    printf("\r\n========================================\r\n");
    printf("Bootloader Starting...\r\n");
    printf("Bootloader version: %s\r\n", BOOTLOADER_VERSION_STR);
    printf("System reset detected, checking upgrade flag...\r\n");
    delay_ms(200);  // 延迟，确保串口输出完成
    
    printf("Reading upgrade info from address 0x%08X...\r\n", (unsigned int)UPGRADE_INFO_ADDRESS);
    delay_ms(100);
    
    // 检查升级标志
    if (upgrade_check_upgrade_flag())
    {
        // 需要升级，进入升级流程
        printf("========================================\r\n");
        printf("Entering upgrade mode...\r\n");
        printf("Waiting for Xmodem data...\r\n");
        printf("========================================\r\n");
        
        // 清除升级标志（避免下次启动再次进入升级模式）
        printf("Clearing upgrade flag...\r\n");
        upgrade_clear_upgrade_flag();
        
        // 启动IAP升级
        upgrade_current_event = UPGRADE_MANAGER_EVENT_IAP_START;
    }
    else
    {
        // 不需要升级，校验APP分区并跳转
        printf("========================================\r\n");
        printf("No upgrade needed, checking application...\r\n");
        printf("========================================\r\n");
        if (upgrade_load_a_block(APP0_START_ADDRESS) == 0)
        {
            // 应用无效，自动进入 IAP 模式等待通过串口1发送固件（Xmodem）
            printf("========================================\r\n");
            printf("Waiting for Xmodem data on UART1...\r\n");
            printf("========================================\r\n");
            upgrade_current_event = UPGRADE_MANAGER_EVENT_IAP_START;
        }
    }
}

/**
 * @brief 事件检测
 * @note 串口1：接收Xmodem固件数据
 */
void upgrade_event_detect(void)
{
    // 检测串口1的输入
    if (usart1_rx_flag)
    {
        usart1_rx_flag = false;  // 清除标志
        
        // 检查是否是Xmodem数据包（133字节：1字节SOH + 1字节包号 + 1字节包号取反 + 128字节数据 + 2字节CRC）
        if ((usart1_rx_len == 133) && (usart1_rx_buffer[0] == 0x01))
        {
            upgrade_current_event = UPGRADE_MANAGER_EVENT_IAP_RECEIVE_DATA;
        }
        // 检查是否是EOT（传输结束）
        else if ((usart1_rx_len == 1) && (usart1_rx_buffer[0] == 0x04))
        {
            upgrade_current_event = UPGRADE_MANAGER_EVENT_IAP_END;
        }
    }
}

/**
 * @brief 事件处理
 */
void upgrade_event_handle(void)
{
    switch (upgrade_current_event)
    {
        case UPGRADE_MANAGER_EVENT_NONE:
            break;
            
        case UPGRADE_MANAGER_EVENT_IAP_START: 
            upgrade_iap_start();
            upgrade_current_event = UPGRADE_MANAGER_EVENT_IAP_READY;
            break;
            
        case UPGRADE_MANAGER_EVENT_IAP_READY: 
            upgrade_iap_ready();
            break;
            
        case UPGRADE_MANAGER_EVENT_IAP_RECEIVE_DATA: 
            upgrade_iap_receive();
            upgrade_current_event = UPGRADE_MANAGER_EVENT_IAP_RECEIVE_DETECT;
            break;
            
        case UPGRADE_MANAGER_EVENT_IAP_END: 
            upgrade_iap_end();
            upgrade_current_event = UPGRADE_MANAGER_EVENT_NONE;
            break;
            
        case UPGRADE_MANAGER_EVENT_IAP_RECEIVE_DETECT: 
            if (upgrade_iap_receive_timeout_detect() == 1)
            {
                upgrade_current_event = UPGRADE_MANAGER_EVENT_NONE;
            }
            break;
            
        default:
            break;
    }
}

static char upgrade_task (thread_t* pt);

/**
 * \brief app初始化
 */
void upgrade_init(void)
{
    // 执行升级分支处理
    upgrade_branch();
    thread_create(upgrade_task);
}

/**
 * \brief 任务
 */
static char upgrade_task(thread_t* pt)
{
    thread_begin
    {
        while (1)
        {
            upgrade_event_detect();
            upgrade_event_handle();
            thread_sleep(1);
        }
    }
    thread_end
}
