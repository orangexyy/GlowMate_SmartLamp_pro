/****************************************************************************\
**  文件名称 :  upgrade_manager.c
**  功能描述 :  升级管理。读取/初始化升级信息、串口接收固件、Xmodem 解析、写 Flash、跳转应用及重启。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

#include "upgrade_manager.h"
#include "upgrade_iap.h"
#include "drv_comm.h"
#include "drv_flash.h"
#include "system.h"
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
 * \brief 从 Flash 读取升级信息
 * \param info 升级信息结构体指针
 * \return 1 成功，0 失败
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
 * \brief 初始化升级信息区域（首次使用或魔数无效时调用）
 * \return 1 成功，0 失败
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
    
    // 写入升级信息（写入前16字节：magic + upgrade_flag + version + crc32）
    if (drv_flash_write(UPGRADE_INFO_ADDRESS, (uint8_t *)&info, sizeof(uint32_t) * 4) != FLASH_OK)
    {
        printf("Failed to write upgrade info\r\n");
        return 0;
    }
    
    printf("Upgrade info initialized successfully\r\n");
    printf("  Magic: 0x%08X\r\n", (unsigned int)info.magic);
    printf("  Upgrade flag: 0x%08X (No upgrade)\r\n", (unsigned int)info.upgrade_flag);
    return 1;
}

/**
 * \brief 系统软件重启（可在 application 中调用）
 */
void upgrade_system_reset(void)
{
    // 禁用所有中断，确保复位操作能够执行
    __disable_irq();
    
    // 关闭SysTick，避免在复位过程中产生中断
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    
    // 重置向量表到bootloader地址（0x08000000）
    // 这确保复位后从bootloader启动，而不是继续使用application的向量表
    SCB->VTOR = 0x08000000;
    
    // 确保内存访问完成
    __DSB();
    __ISB();
    
    // 等待一小段时间，确保所有设置完成（使用简单循环，不依赖SysTick）
    for(volatile uint32_t i = 0; i < 500000; i++);  // 增加延时，确保设置完成
    
    // 执行系统复位
    // 注意：NVIC_SystemReset() 会触发系统复位，复位后从0x08000000（bootloader）启动
    SCB->AIRCR = ((0x5FA << 16) | (SCB->AIRCR & 0x00000700) | (1 << 2));  // 直接写寄存器
    __DSB();  // 确保写操作完成
    while(1);  // 等待复位（应该不会执行到这里）
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
            // 重新初始化结构体
            memset(&info, 0xFF, sizeof(upgrade_info_t));
            info.magic = UPGRADE_MAGIC;
        }
        
        // 修改标志字段为不需要升级
        info.upgrade_flag = UPGRADE_FLAG_NO_UPGRADE;
        
        // 写入修改后的标志（写入前8字节：magic + upgrade_flag）
        if (drv_flash_write(UPGRADE_INFO_ADDRESS, (uint8_t *)&info.magic, sizeof(uint32_t) * 2) != FLASH_OK)
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
 * @brief 读取当前记录的 Bootloader 版本号
 * @return Boot 版本号 (如 0x01000000)，无效或未写入时返回 0xFFFFFFFF
 */
uint32_t upgrade_get_boot_version(void)
{
    upgrade_info_t info;

    if (!upgrade_read_upgrade_info(&info) || info.magic != UPGRADE_MAGIC)
    {
        return 0xFFFFFFFF;
    }
    return info.boot_version;
}

/**
 * @brief 获取 Boot 版本号字符串 "x.y.z"
 * @return 指向版本号字符串的指针，无效时返回 "N/A"
 */
const char* upgrade_get_boot_version_str(void)
{
    static char s_boot_ver_str[16];
    uint32_t ver = upgrade_get_boot_version();

    if (ver == 0xFFFFFFFF)
    {
        return "N/A";
    }
    snprintf(s_boot_ver_str, sizeof(s_boot_ver_str), "%u.%u.%u",
             (unsigned)((ver >> 24) & 0xFF),
             (unsigned)((ver >> 16) & 0xFF),
             (unsigned)((ver >> 8) & 0xFF));
    return s_boot_ver_str;
}

/**
 * @brief 检查是否需要升级
 * @return 1-需要升级，0-不需要升级
 */
uint8_t upgrade_check_upgrade_flag(void)
{
    upgrade_info_t info;
    
    if (upgrade_read_upgrade_info(&info))
    {
        // 首先检查魔数，验证升级信息有效性
        if (info.magic != UPGRADE_MAGIC)
        {
            printf("Upgrade info magic invalid (magic=0x%08X, expected=0x%08X)\r\n", 
                   (unsigned int)info.magic, (unsigned int)UPGRADE_MAGIC);
            printf("Initializing upgrade info area...\r\n");
            
            // 魔数无效，初始化升级信息区域
            if (upgrade_init_upgrade_info())
            {
                // 重新读取升级信息
                if (upgrade_read_upgrade_info(&info))
                {
                    printf("Upgrade info initialized, no upgrade needed\r\n");
                    return 0;
                }
            }
            else
            {
                printf("Failed to initialize upgrade info, assume no upgrade needed\r\n");
                return 0;
            }
        }
        
        // 检查升级标志
        if (info.upgrade_flag == UPGRADE_FLAG_NEED_UPGRADE)
        {
            printf("Upgrade flag detected: Need upgrade (flag=0x%08X)\r\n", 
                   (unsigned int)info.upgrade_flag);
            if (info.version != 0xFFFFFFFF)
            {
                printf("  Firmware version: 0x%08X\r\n", (unsigned int)info.version);
            }
            return 1;
        }
        else if (info.upgrade_flag == UPGRADE_FLAG_NO_UPGRADE)
        {
            printf("Upgrade flag: No upgrade needed (flag=0x%08X)\r\n", 
                   (unsigned int)info.upgrade_flag);
            return 0;
        }
        else
        {
            // 未知的标志值，默认为不需要升级
            printf("Upgrade flag: Unknown value (flag=0x%08X), assume no upgrade needed\r\n", 
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
    
    printf("Bootloader Starting...\r\n");
    
    // 检查升级标志
    if (upgrade_check_upgrade_flag())
    {
        // 需要升级，进入升级流程
        printf("Entering upgrade mode...\r\n");
        printf("Waiting for Xmodem data...\r\n");
        
        // 清除升级标志（避免下次启动再次进入升级模式）
        upgrade_clear_upgrade_flag();
        
        // 启动IAP升级
        upgrade_current_event = UPGRADE_MANAGER_EVENT_IAP_START;
    }
    else
    {
        // 不需要升级，校验APP分区并跳转
        printf("No upgrade needed, checking application...\r\n");
        upgrade_load_a_block(APP0_START_ADDRESS);
    }
}

/**
 * @brief 事件检测
 * @note 串口1：接收升级指令或Xmodem固件数据
 */
void upgrade_event_detect(void)
{
    // 检测串口1的输入
    if (usart1_rx_flag)
    {
        usart1_rx_flag = false;  // 清除标志
        
        // 升级入口已改为 M 代码：收到 "M99\n" 时由 mcode_command 的 process_m99 处理并重启进入 Bootloader IAP
        
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
            thread_sleep(100);
        }
    }
    thread_end
}
