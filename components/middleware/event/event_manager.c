/**
 * @file event_manager.c
 * @brief 事件管理器实现，提供事件的写入、读取和队列管理功能
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "event_manager.h"
#include "mk_queue.h"

// 全局变量定义
static TS_QUEUE s_event_queue;                    // 事件队列
static TS_EVENT_DATA s_event_buffer[EVENT_QUEUE_SIZE];  // 事件缓冲区

/**
 * @brief 初始化事件管理器
 * @details 初始化事件队列和缓冲区
 */
void event_manager_init(void)
{
    mk_queue_init(&s_event_queue, s_event_buffer, EVENT_QUEUE_SIZE, sizeof(TS_EVENT_DATA));
}

/**
 * @brief 写入事件到队列
 * @param module 模块ID
 * @param type 事件类型
 * @param param 事件参数
 * @param data 事件数据指针
 * @param data_size 事件数据大小
 * @return true: 写入成功, false: 写入失败
 */
bool event_manager_write(uint16_t module, uint16_t type, uint32_t param, void* data, uint32_t data_size)
{
    TS_EVENT_DATA event = {
        .module = module,
        .type = type,
        .param = param,
        .data = data,
        .data_size = data_size
    };
    
    return mk_queue_write(&s_event_queue, &event, 1);
}

/**
 * @brief 从队列读取事件
 * @param event 事件数据指针，用于存储读取的事件
 * @return true: 读取成功, false: 读取失败或队列为空
 */
bool event_manager_read(TS_EVENT_DATA* event)
{
    if (event == NULL || mk_queue_is_empty(&s_event_queue)) {
        return false;
    }
    
    return mk_queue_read(&s_event_queue, event, 1) > 0;
}

/**
 * @brief 检查事件队列是否为空
 * @return true: 队列为空, false: 队列非空
 */
bool event_manager_is_empty(void)
{
    return mk_queue_is_empty(&s_event_queue);
}

/**
 * @brief 获取事件队列中的事件数量
 * @return 当前队列中的事件数量
 */
uint32_t event_manager_count(void)
{
    return mk_queue_count(&s_event_queue);
}
