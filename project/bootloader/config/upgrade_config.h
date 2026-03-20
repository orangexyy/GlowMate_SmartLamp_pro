#ifndef UPGRADE_CONFIG_H
#define UPGRADE_CONFIG_H

/**************************************************************************\
                               Includes
\**************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/**************************************************************************\
                            Macro definitions
\**************************************************************************/

#define FLASH_ONE_PAGE_SIZE     			(1024)
#define FLASH_BASE_ADDR         			(0x08000000)

// 配置固件升级信息      
#define UPGRADE_INFO_ADDRESS         	(FLASH_BASE_ADDR + 15*1024)    //固件升级信息地址
#define UPGRADE_INFO_SIZE            	(FLASH_ONE_PAGE_SIZE)          //固件升级信息大小

#define BOOT_START_ADDRESS           	(FLASH_BASE_ADDR)              //BOOT 起始地址 (0x08000000)
#define APP0_START_ADDRESS           	(FLASH_BASE_ADDR + 16*1024)    //APP0 应用区起始地址 (0x08004000)

// 分区大小定义
#define BOOT_SIZE                    	(16*1024)                       //BOOT 大小 16KB (0x08000000-0x08004000)
#define APP0_SIZE                   	(40*1024)                       //APP0 大小 40KB (0x08004000-0x0800E000)

/**************************************************************************\
                            Typedef definitions
\**************************************************************************/

// 升级标志宏定义
#define UPGRADE_FLAG_NEED_UPGRADE    (0x12345678)  // 需要升级标志
#define UPGRADE_FLAG_NO_UPGRADE      (0x87654321)  // 不需要升级标志
#define UPGRADE_MAGIC                (0xABCDEF00)  // 升级信息魔数，用于验证数据有效性



// 升级信息结构体
typedef struct
{
    uint32_t magic;             // 魔数：0xABCDEF00，用于验证升级信息有效性
    uint32_t upgrade_flag;      // 升级标志：0x12345678表示需要升级，0x87654321表示不需要升级
    uint32_t version;           // 应用固件版本号（可选，由 APP 触发升级时写入）
    uint32_t crc32;             // 升级信息CRC32校验值（可选）
    uint32_t boot_version;      // Bootloader 版本号（由 Boot 初始化/升级信息时写入）
    uint32_t reserved[251];     // 保留字段，使结构体大小为1024字节（一页Flash）
} upgrade_info_t;

/**************************************************************************\
                         Global functions definitions
\**************************************************************************/

#endif

/**************************** End of File (H) ****************************/ 
