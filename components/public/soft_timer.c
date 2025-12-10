
#include <stdio.h>
#include <string.h>
#include "soft_timer.h"

/**\defgroup LOG_Private_TypesDefinitions
 * \{
 */

/**
 * \}
 */

/**\defgroup LOG_Private_Defines
 * \{
 */
#define SOFT_TIMER_OK (0)     /**< 执行成功 */
#define SOFT_TIMER_ERROR (-1) /**< 执行错误 */

/**
 * \}
 */

/**\defgroup LOG_Private_Macros
 * \{
 */

/**
 * \}
 */

/**\defgroup LOG_Private_Variables
 * \{
 */
static TS_SOFT_TIMER *s_timer_list = NULL; // the head of the soft timer list
/**
 * \}
 */

/**\defgroup LOG_Private_Functions
 * \{
 */

/**
 * \}
 */

/**\addtogroup LOG_Exported_Functions
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
int soft_timer_create(TS_SOFT_TIMER *pThis, TE_SOFT_TIMER_MODE mode, TimerOut_CB CB)
{
    TS_SOFT_TIMER *p_timer = NULL;

    if (pThis == NULL)
    {
        return SOFT_TIMER_ERROR;
    }

    pThis->mode = mode;
    pThis->timeoutCB = CB;
    pThis->status = SOFT_TIMER_STATUS_UNUSED;
    pThis->next = NULL;

    if (s_timer_list == NULL)
    {
        s_timer_list = pThis;
    }
    else
    {
        p_timer = s_timer_list;
        if (pThis == p_timer) // 定时器已存在, 则直接返回OK
        {
            return SOFT_TIMER_OK;
        }

        while (p_timer->next != NULL)
        {
            if (pThis == p_timer->next) // 定时器已存在, 则直接返回OK
            {
                return SOFT_TIMER_OK;
            }
            p_timer = p_timer->next;
        }
        p_timer->next = pThis; // 新的定时器插入尾部
    }

    return SOFT_TIMER_OK;
}

/**
 * \brief 删除定时器
 * \param[in] pThis 要删除的定时器结构体地址
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败
 */
int soft_timer_delete(TS_SOFT_TIMER *pThis)
{
    TS_SOFT_TIMER *cur_timer = s_timer_list;
    TS_SOFT_TIMER *pre_timer = NULL;

    if (pThis == NULL)
    {
        return SOFT_TIMER_ERROR;
    }

    while (cur_timer != NULL)
    {
        if (cur_timer == pThis)
        {
            if (cur_timer == s_timer_list) /**< 如果头结点即为要删除的定时器 */
                s_timer_list = cur_timer->next;
            else
                pre_timer->next = cur_timer->next;

            break;
        }
        pre_timer = cur_timer;
        cur_timer = cur_timer->next;
    }

    if (cur_timer == NULL) // 未查到对应的定时器
        return SOFT_TIMER_ERROR;
    else
        return SOFT_TIMER_OK;
}

/**
 * \brief 开启定时器
 * \param[in] pThis   定时器结构体
 * \param[in] Ticks   定时的时间,单位(ms)
 * \param[in] context 回调的参数
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败
 */
int soft_timer_start(TS_SOFT_TIMER *pThis, uint32_t Ticks, void *context)
{
    if (Ticks == 0)
    {
        return SOFT_TIMER_ERROR;
    }

    pThis->timeout = Ticks;
    pThis->count = Ticks;
    pThis->context = context;
    pThis->status = SOFT_TIMER_STATUS_RUNNING;

    return SOFT_TIMER_OK;
}

/**
 * \brief 停止定时器
 * \param[in] pThis 定时器结构体
 */
void soft_timer_stop(TS_SOFT_TIMER *pThis)
{
    pThis->status = SOFT_TIMER_STATUS_STOP;
}

/**
 * \brief 重载定时器
 * \param[in] pThis 定时器结构体
 */
void soft_timer_reload(TS_SOFT_TIMER *pThis)
{
    if (pThis->timeout == 0)
    {
        return;
    }

    pThis->count = pThis->timeout;
    pThis->status = SOFT_TIMER_STATUS_RUNNING;
}

/**
 * \brief 获取定时器状态
 * \param[in] pThis 定时器结构体
 * \return 返回当前定时器状态 \ref SoftTimerstatus_t
 */
uint8_t soft_timer_get_status(TS_SOFT_TIMER *pThis)
{
    return pThis->status;
}

/**
 * \brief 软件定时器调度器,放置于1ms的硬件定时器回调中
 */
void soft_timer_scheduler(void)
{
    TS_SOFT_TIMER *p_timer = s_timer_list;

    while (p_timer != NULL)
    {
        if (p_timer->status == SOFT_TIMER_STATUS_RUNNING)
        {
            p_timer->count--;

            if (p_timer->count == 0)
            {
                if (p_timer->mode == SOFT_TIMER_MODE_SINGLE)
                {
                    p_timer->status = SOFT_TIMER_STATUS_COMPLETED;
                    if (p_timer->timeoutCB != NULL)
                    {
                        p_timer->timeoutCB(p_timer->context);
                    }
                }
                else if (p_timer->mode == SOFT_TIMER_MODE_RERIOD)
                {
                    p_timer->status = SOFT_TIMER_STATUS_RUNNING;
                    p_timer->count = p_timer->timeout;
                    if (p_timer->timeoutCB != NULL)
                    {
                        p_timer->timeoutCB(p_timer->context);
                    }
                }
            }
        }

        p_timer = p_timer->next;
    }
}
/**
 * \}
 */
