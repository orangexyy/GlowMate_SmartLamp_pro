/******************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  svc_mode.cpp
**  功能描述 :  对系统模式进行统一管理，实现不同模式的切换、调度逻辑，在同一个时刻只有某一个模式生效
**  作    者 :  王滨泉
**  日    期 :  2022.04.25
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2022.04.25
                1 首次创建
\******************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include "svc_mode.h"
#include "event_manager.h"
#include "thread.h"
#include "mk_log.h"
#include "mk_common.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
static uint8_t s_system_current_mode;
static const TS_SVC_MODE *p_mode_state[SYSTEM_MODE_MAX];
/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/**
 * \brief 请求切换系统模式
 * \param[in]  mode - 需要切换的模式
 * \param[in]  event 事件 \ref TS_MODE_EVENT
 * \return true - 切换成功
 *         false - 切换失败
 */
static bool svc_mode_request_to_change_system_mode(uint8_t next_mode, TS_EVENT_DATA* event)
{
    if(s_system_current_mode == next_mode)
    {
        LOG_W("switch same mode: %s.", p_mode_state[next_mode]->name);
		return true;
    }
    
    LOG_I("exit %s mode", p_mode_state[s_system_current_mode]->name);
    p_mode_state[s_system_current_mode]->exit();
    LOG_I("entry %s mode", p_mode_state[next_mode]->name);
    if (0 == p_mode_state[next_mode]->entry(event))
    {
        s_system_current_mode = next_mode;
        return true;
    }

    return false;
}

/**
 * \brief 处理系统事件
 * \param[in]  event - 事件参数句柄
 * \return true - 切换成功
 *         false - 切换失败
 */
static bool svc_mode_process_event(TS_EVENT_DATA* event)
{     
    int next_mode = p_mode_state[s_system_current_mode]->translate(event);
    if (next_mode >= 0)
    {
        LOG_I("start switch mode <%s> to <%s>.", p_mode_state[s_system_current_mode]->name, p_mode_state[next_mode]->name);
        if (svc_mode_request_to_change_system_mode(next_mode, event) == false)
        {
            LOG_W("switch mode fail.");
        }
    }
    return true;
}

/**
 * \brief 模式管理任务
 */
static char mode_task(thread_t* pt)
{
    TS_EVENT_DATA event;
    thread_begin
    {
        while (1)
        {
            if (event_manager_read(&event))
            {
                svc_mode_process_event(&event);
            }
            thread_yield();
        }
    }
    thread_end
}

/**
 * \brief 模式管理初始化
 * \param[in]  p_mode_table - 模式表
 * \param[in]  mode_num - 模式数量
 * \return 成功返回MK_SUCCESS, 失败返回MK_FAIL
 */
int svc_mode_init(const TS_SVC_MODE_REG *p_mode_table, uint8_t mode_num)
{
    if (p_mode_table == NULL || mode_num > SYSTEM_MODE_MAX)
    {
        LOG_E("mode init failed.");
        return MK_FAIL;
    }

    event_manager_init();

    for(uint8_t i = 0; i < mode_num; i++)
    {
        if (p_mode_table[i].mode > SYSTEM_MODE_MAX)
        {
            LOG_E("mode %d is out of range.", p_mode_table[i].mode);
            continue;
        }
        p_mode_state[i] = (TS_SVC_MODE *)p_mode_table[i].install();
        p_mode_state[i]->init();
        LOG_I("init %s mode", p_mode_state[i]->name);
    }

    s_system_current_mode = SYSTEM_MODE_IDLE;
    p_mode_state[s_system_current_mode]->entry(NULL);

    thread_create(mode_task);
    return MK_SUCCESS;
}

/**
 * \brief 获取当前系统模式
 * \return 当前系统运行的模式
 */
uint8_t svc_mode_get_current_mode(void)
{
    return s_system_current_mode;
}

/******************************* End of File (C) ******************************/
