#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#ifndef EVENT_QUEUE_SIZE
#define EVENT_QUEUE_SIZE        (32)
#endif

// 事件数据结构
typedef struct {
    uint16_t module;          // 事件模块
    uint16_t type;            // 事件类型
    uint32_t param;           // 事件参数
    void* data;               // 事件数据指针
    uint32_t data_size;       // 事件数据大小
} TS_EVENT_DATA;

/**
 * @brief 初始化事件管理器
 * 
 * 初始化事件队列
 */
void event_manager_init(void);

/**
 * @brief 写入事件
 * 
 * @param module 事件模块
 * @param type 事件类型
 * @param param 事件参数
 * @param data 事件数据
 * @param data_size 数据大小
 * @return true 写入成功
 * @return false 写入失败
 */
bool event_manager_write(uint16_t module, uint16_t type, uint32_t param, void* data, uint32_t data_size);

/**
 * @brief 读取事件
 * 
 * @param event 事件数据指针
 * @return true 读取成功
 * @return false 读取失败（队列为空）
 */
bool event_manager_read(TS_EVENT_DATA* event);

/**
 * @brief 检查队列是否为空
 * 
 * @return true 队列为空
 * @return false 队列非空
 */
bool event_manager_is_empty(void);

/**
 * @brief 获取队列中事件数量
 * 
 * @return uint32_t 事件数量
 */
uint32_t event_manager_count(void);

#endif
