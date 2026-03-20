/****************************************************************************\
**  文件名称 :  upgrade_iap.c
**  功能描述 :  IAP 升级。反初始化外设、检查应用固件有效性、Xmodem 接收与写 Flash、跳转应用。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

#include "upgrade_iap.h"
#include "drv_comm.h"
#include "drv_flash.h"
#include "system.h"
#include <string.h>
#include <stdio.h>

/** 串口1：用于接收固件数据（Xmodem 协议） */
extern uint8_t usart1_rx_buffer[USART1_RX_BUF_SIZE];
extern uint16_t usart1_rx_len;
extern volatile bool usart1_rx_flag;

// 接收缓冲区（用于缓存数据包，每8包写入一页Flash）
static uint8_t update_buffer[FLASH_ONE_PAGE_SIZE];

// Xmodem协议数据结构
XMODEM_PROTOCOL_DATA xmodem_protocol_struct;

// PC指针函数指针
set_pc SET_PC;

/**
 * @brief 反初始化外设（跳转前调用）
 */
void upgrade_deinit_periph(void)
{
    USART_DeInit(USART1);
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
}

/**
 * @brief 设置SP指针（汇编函数）
 * @param addr 栈指针地址
 */
__asm void SET_SP(uint32_t addr)
{
    MSR MSP, r0
    BX r14 
}

/**
 * @brief 检查应用分区固件是否有效
 * @param addr 应用分区起始地址
 * @return 1-有效，0-无效
 */
uint8_t upgrade_check_app_valid(uint32_t addr)
{
    uint32_t sp_value, pc_value;
    uint32_t i;
    uint8_t check_buffer[256];
    uint32_t non_ff_count = 0;
    
    // 读取向量表
    sp_value = *(uint32_t *)addr;           // 栈指针
    pc_value = *(uint32_t *)(addr + 4);    // 程序计数器（复位向量）
    
    // 打印向量表信息
    printf("Checking Application Firmware...\r\n");
    printf("  SP: 0x%08X\r\n", (unsigned int)sp_value);
    printf("  PC: 0x%08X\r\n", (unsigned int)pc_value);
    
    // 检查1：SP指针是否在有效范围内（RAM地址范围）
    if (sp_value < 0x20000000 || sp_value > 0x20004FFF)
    {
        printf("  [FAIL] SP pointer out of range\r\n");
        return 0;
    }
    printf("  [OK] SP pointer valid\r\n");
    
    // 检查2：PC指针是否在Flash地址范围内
    if (pc_value < 0x08000000 || pc_value > 0x0801FFFF)
    {
        printf("  [FAIL] PC pointer out of range\r\n");
        return 0;
    }
    printf("  [OK] PC pointer valid\r\n");
    
    // 检查3：检查Flash是否为空（全为0xFF）
    // 读取前几页数据检查是否有有效数据
    for (i = 0; i < 4; i++)  // 检查前4页（4KB）
    {
        uint32_t check_addr = addr + i * FLASH_ONE_PAGE_SIZE;
        if (check_addr >= APP0_START_ADDRESS + APP0_SIZE)
        {
            break;
        }
        
        if (drv_flash_read(check_addr, check_buffer, sizeof(check_buffer)) == FLASH_OK)
        {
            uint32_t j;
            for (j = 0; j < sizeof(check_buffer); j++)
            {
                if (check_buffer[j] != 0xFF)
                {
                    non_ff_count++;
                }
            }
        }
    }
    
    if (non_ff_count == 0)
    {
        printf("  [FAIL] Flash is empty (all 0xFF)\r\n");
        return 0;
    }
    
    printf("  [OK] Found %lu bytes of data (not 0xFF)\r\n", (unsigned long)non_ff_count);
    
    // 检查4：检查PC指向的地址是否在应用分区内
    if (pc_value < addr || pc_value >= addr + APP0_SIZE)
    {
        printf("  [FAIL] PC pointer not in application partition\r\n");
        return 0;
    }
    printf("  [OK] PC pointer in application partition\r\n");
    
    // 所有检查通过
    printf("Application firmware is VALID\r\n");
    return 1;
}

/**
 * @brief Xmodem CRC16计算
 * @param data 数据指针
 * @param datalen 数据长度
 * @return CRC16值
 */
uint16_t xmodem_crc16(uint8_t *data, uint16_t datalen)
{
    uint8_t i;
    uint16_t crcinit = 0x0000;
    uint16_t crcipoly = 0x1021;

    while(datalen--)
    {
        crcinit = (*data << 8) ^ crcinit;
        for (i=0; i<8; i++)
        {
            if(crcinit&0x8000)
                crcinit = (crcinit << 1) ^ crcipoly;
            else
                crcinit = (crcinit << 1);
        }
        data++;
    }
    return crcinit;
}

/**
 * @brief 启动IAP下载
 * @note 不立即擦除APP0分区，等到接收到第一个数据包时再擦除，避免断电时丢失原有固件
 */
void upgrade_iap_start(void)
{
    // 初始化变量，但不立即擦除APP0分区
    // 擦除操作将在接收到第一个数据包时进行，这样可以避免在断电时丢失原有固件
    xmodem_protocol_struct.time = 0;
    xmodem_protocol_struct.receive_buf_num = 0;
    printf("Serial IAP Download To A Block, Use Bin Format File\r\n");
    printf("Start Serial IAP Download\r\n");
    printf("Waiting for first packet before erasing APP0...\r\n");
}

/**
 * @brief IAP就绪状态（发送'C'字符请求数据）
 * 注意：'C'通过串口1发送，因为Xmodem数据从串口1接收
 */
void upgrade_iap_ready(void)
{
    delay_ms(10);
    if (xmodem_protocol_struct.time >= 100)
    {
        drv_usart_send_byte(DRV_USART_ID_1, 'C');  // 通过串口1发送'C'请求数据
        xmodem_protocol_struct.time = 0;
    }
    xmodem_protocol_struct.time++;
}

/**
 * @brief IAP接收数据
 * @note 从串口1接收Xmodem数据包，ACK/NACK也通过串口1发送
 */
void upgrade_iap_receive(void)
{
    // 计算CRC校验（从串口1接收的数据）
    xmodem_protocol_struct.receive_crc = xmodem_crc16(&usart1_rx_buffer[3], 128);
    
    // 校验CRC
    if (xmodem_protocol_struct.receive_crc == usart1_rx_buffer[131] * 256 + usart1_rx_buffer[132])
    {
        // CRC校验通过
        
        // 如果是第一个数据包，先擦除APP0分区
        // 这样可以避免在断电时丢失原有固件（只有在确认有数据要接收时才擦除）
        if (xmodem_protocol_struct.receive_buf_num == 0)
        {
            printf("First packet received, erasing APP0 partition...\r\n");
            uint8_t result = drv_flash_erase(APP0_START_ADDRESS, APP0_SIZE);
            if (result != FLASH_OK)
            {
                printf("Erase APP0 Failed\r\n");
                drv_usart_send_byte(DRV_USART_ID_1, 0x15);  // 擦除失败，发送NACK
                return;
            }
            printf("APP0 partition erased successfully\r\n");
        }
        
        xmodem_protocol_struct.receive_buf_num++;   // 接收到的包数（一包128byte）累加
        
        // 复制到更新A区的buffer里
        memcpy(&update_buffer[((xmodem_protocol_struct.receive_buf_num - 1) % (FLASH_ONE_PAGE_SIZE / 128)) * 128], 
               &usart1_rx_buffer[3], 128);
        
        // 每接收到8包（1024byte）写入一页flash
        if ((xmodem_protocol_struct.receive_buf_num % (FLASH_ONE_PAGE_SIZE / 128)) == 0)
        {
            uint8_t result = drv_flash_write(APP0_START_ADDRESS + ((xmodem_protocol_struct.receive_buf_num / (FLASH_ONE_PAGE_SIZE / 128)) - 1) * FLASH_ONE_PAGE_SIZE, 
                       update_buffer, FLASH_ONE_PAGE_SIZE);
            if (result != FLASH_OK)
            {
                drv_usart_send_byte(DRV_USART_ID_1, 0x15);  // Flash写入失败，通过串口1发送NACK
                // 注意：在接收数据时避免打印，以免干扰数据接收
                // printf("Flash Write Failed\r\n");
                return;
            }
        }
        
        drv_usart_send_byte(DRV_USART_ID_1, 0x06);     // 检验通过，通过串口1发送ACK
    }
    else
    {
        drv_usart_send_byte(DRV_USART_ID_1, 0x15);     // 检验失败，通过串口1发送NACK
    }
}

/**
 * @brief IAP接收超时检测（简化版本，始终返回1）
 */
uint8_t upgrade_iap_receive_timeout_detect(void)
{
    return 1;
}

/**
 * @brief IAP结束处理
 */
void upgrade_iap_end(void)
{
    drv_usart_send_byte(DRV_USART_ID_1, 0x06);         // 接收数据结束，通过串口1发送ACK
    
    // 处理接收到的剩余不满8包（1024byte）写入一页flash的数据
    if ((xmodem_protocol_struct.receive_buf_num % (FLASH_ONE_PAGE_SIZE / 128)) != 0)
    { 
        uint16_t remaining_size = (xmodem_protocol_struct.receive_buf_num % (FLASH_ONE_PAGE_SIZE / 128)) * 128;
        uint8_t result = drv_flash_write(APP0_START_ADDRESS + (xmodem_protocol_struct.receive_buf_num / (FLASH_ONE_PAGE_SIZE / 128)) * FLASH_ONE_PAGE_SIZE, 
                   update_buffer, remaining_size);
        if (result != FLASH_OK)
        {
            printf("Flash Write Failed\r\n");
        }
    }
    
    delay_ms(100);
    printf("Program Download Success\r\n");
    
    // 校验新固件
    printf("Verifying downloaded firmware...\r\n");
    if (upgrade_check_app_valid(APP0_START_ADDRESS))
    {
        printf("Firmware verification OK\r\n");
        printf("Jumping to application...\r\n");
        delay_ms(500);
        // 直接跳转到应用程序，而不是复位
        upgrade_load_a_block(APP0_START_ADDRESS);
    }
    else
    {
        printf("Firmware verification FAILED\r\n");
        printf("System Will Reset\r\n");
        delay_ms(1000);
        NVIC_SystemReset();
    }
}

/**
 * @brief 跳转到A区（应用区）
 * @param addr A区起始地址
 */
void upgrade_load_a_block(uint32_t addr)
{
    // 先检查固件是否有效
    if (upgrade_check_app_valid(addr) == 0)
    {
        printf("Application firmware is INVALID, stay in bootloader\r\n");
        printf("Enter Bootloader Command\r\n");
        return;
    }
    
    // 固件有效，准备跳转
    uint32_t sp_value = *(uint32_t *)addr;
    uint32_t pc_value = *(uint32_t *)(addr + 4);
    
    printf("Jumping to application at 0x%08X...\r\n", (unsigned int)addr);
    delay_ms(100);
    
    SET_SP(sp_value);                      // 将向量表中的第一个成员的内容给到SP指针
    SET_PC = (set_pc)pc_value;             // 将向量表中的第二个成员的内容给到PC指针
    upgrade_deinit_periph();
    SET_PC();
}
