#ifndef __UPGRADE_IAP_H
#define __UPGRADE_IAP_H

#include "stm32f10x.h"
#include "upgrade_config.h"
#include "project_config.h"

/**************************************************************************\
                         Typedef definitions
\**************************************************************************/

typedef struct 
{
    uint8_t time;                       // 时间计数
    uint32_t receive_buf_num;           // 接收到的包数
    uint16_t receive_crc;               // 接收到的CRC
} XMODEM_PROTOCOL_DATA;

typedef void (*set_pc)(void);

/**************************************************************************\
                         Function Declarations
\**************************************************************************/

/* ==================== 系统控制 ==================== */
/**
 * @brief 反初始化外设（跳转前调用）
 */
void upgrade_deinit_periph(void);

/**
 * @brief 设置SP指针（汇编函数）
 * @param addr 栈指针地址
 */
__asm void SET_SP(uint32_t addr);

/* ==================== 应用跳转 ==================== */
/**
 * @brief 跳转到应用区
 * @param addr 应用区起始地址
 * @return 0-应用无效未跳转，1-已跳转（不返回）
 */
int upgrade_load_a_block(uint32_t addr);

/* ==================== 应用验证 ==================== */
/**
 * @brief 检查应用分区固件是否有效
 * @param addr 应用分区起始地址
 * @return 1-有效，0-无效
 */
uint8_t upgrade_check_app_valid(uint32_t addr);

/* ==================== IAP升级 ==================== */
/**
 * @brief 启动IAP下载
 */
void upgrade_iap_start(void);

/**
 * @brief IAP就绪状态（发送'C'字符请求数据）
 */
void upgrade_iap_ready(void);

/**
 * @brief IAP接收数据
 */
void upgrade_iap_receive(void);

/**
 * @brief IAP结束处理
 */
void upgrade_iap_end(void);

/**
 * @brief IAP接收超时检测
 * @return 1-超时检测通过，0-超时
 */
uint8_t upgrade_iap_receive_timeout_detect(void);

/* ==================== Xmodem协议 ==================== */
/**
 * @brief Xmodem CRC16计算
 * @param data 数据指针
 * @param datalen 数据长度
 * @return CRC16值
 */
uint16_t xmodem_crc16(uint8_t *data, uint16_t datalen);

#endif /* __UPGRADE_IAP_H */
