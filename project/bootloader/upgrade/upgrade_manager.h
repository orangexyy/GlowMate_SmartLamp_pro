#ifndef __UPGRADE_MANAGER_H
#define __UPGRADE_MANAGER_H

#include "stm32f10x.h"
#include "sys.h"       
#include "upgrade_config.h"
#include "project_config.h"

/**************************************************************************\
                            Typedef definitions
\**************************************************************************/

typedef enum
{
    UPGRADE_MANAGER_EVENT_NONE,              // 初始状态
    UPGRADE_MANAGER_EVENT_IAP_START,         // IAP开始
    UPGRADE_MANAGER_EVENT_IAP_READY,         // IAP就绪（发送'C'）
    UPGRADE_MANAGER_EVENT_IAP_RECEIVE_DATA,  // IAP接收数据
    UPGRADE_MANAGER_EVENT_IAP_RECEIVE_DETECT,// IAP接收检测
    UPGRADE_MANAGER_EVENT_IAP_END,           // IAP结束
} UPGRADE_MANAGER_EVENT_DATA;

/**************************************************************************\
                         Includes (Sub-modules)
\**************************************************************************/

// 包含IAP模块头文件
#include "upgrade_iap.h"

/**************************************************************************\
                         Function Declarations
\**************************************************************************/

/* ==================== 升级流程控制 ==================== */
/**
 * @brief 升级分支处理（主流程）
 * @note 上电时调用，根据升级标志决定进入升级模式或跳转到应用
 */
void upgrade_branch(void);

/* ==================== 事件处理 ==================== */
/**
 * @brief 事件检测
 */
void upgrade_event_detect(void);

/**
 * @brief 事件处理
 */
void upgrade_event_handle(void);

/* ==================== 系统控制 ==================== */
/**
 * @brief 系统重启函数（可在application中调用）
 */
void upgrade_system_reset(void);

/* ==================== 升级标志管理 ==================== */
/**
 * @brief 检查是否需要升级
 * @return 1-需要升级，0-不需要升级
 */
uint8_t upgrade_check_upgrade_flag(void);

/**
 * @brief 设置升级标志（可在application中调用）
 * @param version 固件版本号（可选，0xFFFFFFFF表示无效）
 * @return 1-成功，0-失败
 */
uint8_t upgrade_set_upgrade_flag(uint32_t version);

/**
 * @brief 清除升级标志（升级完成后调用）
 */
void upgrade_clear_upgrade_flag(void);

/**
 * @brief 初始化升级信息区域（首次使用或魔数无效时调用）
 * @return 1-成功，0-失败
 */
uint8_t upgrade_init_upgrade_info(void);

/**
 * @brief 若升级区尚未写入（全 0xFF），则写入 magic + NO_UPGRADE + boot_version（不擦除，直接写）
 * @note 用于首次从“APP 无效”进入 IAP 完成升级后，跳转 APP 前补写 boot_version，便于 APP 读取
 */
void upgrade_ensure_boot_version_if_needed(void);

/* ==================== 升级任务 ==================== */
/**
 * @brief 初始化升级任务
 */
void upgrade_init(void);

#endif /* __UPGRADE_MANAGER_H */
