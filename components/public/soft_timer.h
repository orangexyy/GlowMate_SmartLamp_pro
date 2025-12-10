
#ifndef _SOFT_TIMER_H_
#define _SOFT_TIMER_H_

#include <stdint.h>

/**\defgroup LOG_Exported_TypesDefinitions
 * \{
 */

/**
 * \brief 定时器模式
 */
typedef enum
{
    SOFT_TIMER_MODE_UNUSED = 0,
    SOFT_TIMER_MODE_SINGLE = 1,
    SOFT_TIMER_MODE_RERIOD = 2,
} TE_SOFT_TIMER_MODE;

/**
 * \brief 定时器状态
 */
typedef enum
{
    SOFT_TIMER_STATUS_UNUSED = 0,
    SOFT_TIMER_STATUS_STOP = 1,
    SOFT_TIMER_STATUS_RUNNING = 2,
    SOFT_TIMER_STATUS_COMPLETED = 3,
} TE_SOFT_TIMER_STATUS;

/**
 * \brief 定时器超时回调
 */
typedef void (*TimerOut_CB)(void *Param);

/**
 * \brief 定时器参数结构体
 */
typedef struct TS_SOFT_TIMER
{
    TE_SOFT_TIMER_MODE mode;     /**< 模式 */
    TE_SOFT_TIMER_STATUS status; /**< 状态 */
    uint32_t timeout;            /**< 定时周期 */
    uint32_t count;              /**< 计数值 */
    void *context;               /**< 回调参数 */
    TimerOut_CB timeoutCB;       /**< 超时回调 */
    struct TS_SOFT_TIMER *next;  /**< 下一个定时器 */
} TS_SOFT_TIMER;

/**
 * \}
 */

/**\defgroup LOG_Exported_Defines
 * \{
 */

/**
 * \}
 */

/**\defgroup LOG_Exported_Macros
 * \{
 */

/**
 * \}
 */

/**\defgroup LOG_Exported_Variables
 * \{
 */

/**
 * \}
 */

/**\defgroup LOG_Exported_Functions
 * \{
 */

/**
 * \brief 创建定时器
 * \param[in] pThis 定时器结构体
 * \param[in] mode  定时器模式
 * \param[in] cb    定时器回调函数
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败
 */
int soft_timer_create(TS_SOFT_TIMER *pThis, TE_SOFT_TIMER_MODE Mode, TimerOut_CB CB);

/**
 * \brief 删除定时器
 * \param[in] pThis 要删除的定时器结构体地址
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败
 */
int soft_timer_delete(TS_SOFT_TIMER *pThis);

/**
 * \brief 开启定时器
 * \param[in] pThis   定时器结构体
 * \param[in] Ticks   定时的时间,单位(ms)
 * \param[in] Context 回调的参数
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败
 */
int soft_timer_start(TS_SOFT_TIMER *pThis, uint32_t Ticks, void *Context);

/**
 * \brief 停止定时器
 * \param[in] pThis 定时器结构体
 */
void soft_timer_stop(TS_SOFT_TIMER *pThis);

/**
 * \brief 重载定时器
 * \param[in] pThis 定时器结构体
 */
void soft_timer_reload(TS_SOFT_TIMER *pThis);

/**
 * \brief 获取定时器状态
 * \param[in] pThis 定时器结构体
 * \return 返回当前定时器状态 \ref SoftTimerStatus_t
 */
uint8_t soft_timer_get_status(TS_SOFT_TIMER *pThis);

/**
 * \brief 软件定时器调度器,放置于1ms的硬件定时器回调中
 */
void soft_timer_scheduler(void);

/**
 * \}
 */
#endif
