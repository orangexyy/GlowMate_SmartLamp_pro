/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  
**  功能描述 :  
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/  

/****************************************************************************\
                               Includes
\****************************************************************************/

#include "mk_common.h" 
#include "svc_key.h"
#include "drv_key.h" 
#include "thread.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
 
#define COMB_UP_TIME_VALID      (10)

#define EVENT_SET_AND_EXEC_CB(evt) \
            do                             \
            {                              \
                target->priv.event = evt;  \
                if(target->cfg->cb)        \
                    target->cfg->cb(i, evt, target->priv.key_val, target->priv.click_cnt); \
            } while(0)
        
enum
{
    KEY_STAGE_DEFAULT        = 0,
    KEY_STAGE_DOWN           = 1,
    KEY_STAGE_MULTIPLE_CLICK = 2,
    KEY_STAGE_COMBINATION    = 3,
}; 
 
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

typedef struct
{
    uint8_t     active : 1;
    uint8_t     status : 3;
    uint8_t     event  : 4;
    uint8_t     key_val;
    uint8_t     click_cnt;
    uint16_t    scan_cnt;
    uint16_t    timeout_cnt; 
} TS_KEY_PRIV;

typedef struct
{ 
    const TS_KEY_CFG    *cfg;
    TS_KEY_PRIV         priv;
} TS_KEY;

/****************************************************************************\
                            Variables definitions
\****************************************************************************/
static TS_KEY s_key_list[MK_PLUGIN_KEY_NUM]; 
static uint8_t s_key_num = 0;
/****************************************************************************\
                            Functions definitions
\****************************************************************************/
 
static int _svc_key_scan(void)
{
    TS_KEY *target = s_key_list; 
    
    for (uint8_t i=0; i<MK_PLUGIN_KEY_NUM; i++, target = &s_key_list[i])
    {
        if (target->priv.active == 0)
        {
            continue;
        }
                
        uint8_t key_value = drv_key_read(target->cfg->key_id);

        if (target->priv.status > KEY_STAGE_DEFAULT)
        {
            if (++target->priv.scan_cnt > 0x8000)
            {
                target->priv.scan_cnt = target->cfg->longHoldMS;
            }
        }
        
        switch (target->priv.status)
        {
            case KEY_STAGE_DEFAULT:
                if (key_value) /* is pressed */
                {
                    target->priv.scan_cnt  = 0;
                    target->priv.click_cnt = 0;
                    target->priv.timeout_cnt = 0;
                    target->priv.status = KEY_STAGE_DOWN;
                }
                else
                {
                    target->priv.event = KEY_PRESS_NONE;
                }
                break;
            
            case KEY_STAGE_DOWN:
                if (key_value) /* is pressed */
                {
                    if (target->priv.scan_cnt >= target->cfg->longHoldMS)
                    {
                        if ((target->priv.event != KEY_PRESS_LONG_HOLD) && (target->priv.event != KEY_PRESS_LONG_KEEP))
                        {
                            EVENT_SET_AND_EXEC_CB(KEY_PRESS_LONG_HOLD);
                            target->priv.timeout_cnt = 1;
                        }
                        
                        if (target->cfg->longKeepTrigMs != 0)
                        {
                            if (target->priv.scan_cnt >= (target->priv.timeout_cnt * target->cfg->longKeepTrigMs) + 
                                                                    target->cfg->longPressMS)
                            {
                                target->priv.timeout_cnt++;
                                EVENT_SET_AND_EXEC_CB(KEY_PRESS_LONG_KEEP);
                            }
                        }
                    }
                    else if (target->priv.scan_cnt >= target->cfg->longPressMS)
                    {
                        if ((target->priv.event != KEY_PRESS_LONG_DOWN) && (target->priv.event != KEY_PRESS_LONG_KEEP))
                        {
                            EVENT_SET_AND_EXEC_CB(KEY_PRESS_LONG_DOWN);
                            target->priv.timeout_cnt = 1;
                        }
                        else if (target->cfg->longKeepTrigMs != 0)
                        {
                            if (target->priv.scan_cnt >= (target->priv.timeout_cnt * target->cfg->longKeepTrigMs) + 
                                                                    target->cfg->longPressMS)
                            {
                                target->priv.timeout_cnt++;
                                EVENT_SET_AND_EXEC_CB(KEY_PRESS_LONG_KEEP);
                            }
                        }
                    }
                    else if (target->priv.scan_cnt >= target->cfg->shortPressMS)
                    {
                        if (target->priv.event != KEY_PRESS_SHORT_DOWN)
                        {
                            target->priv.key_val = key_value;
                            EVENT_SET_AND_EXEC_CB(KEY_PRESS_SHORT_DOWN);
                        }
                    }
                    
                    if (target->priv.scan_cnt >= target->cfg->shortPressMS)
                    {
                        if (target->priv.key_val != key_value) // 如果键值发生变化,则判断组合键发生变化,等待抬起
                        {
                            target->priv.timeout_cnt = target->priv.scan_cnt;
                            target->priv.status = KEY_STAGE_COMBINATION;
                        }
                    }
                }
                else /* button up */
                {
                    if (target->priv.scan_cnt >= target->cfg->longHoldMS)
                    {
                        EVENT_SET_AND_EXEC_CB(KEY_PRESS_LONG_HOLD_UP);
                        target->priv.status = KEY_STAGE_DEFAULT;
                    }
                    else if (target->priv.scan_cnt >= target->cfg->longPressMS)
                    {
                        EVENT_SET_AND_EXEC_CB(KEY_PRESS_LONG_UP);
                        target->priv.status = KEY_STAGE_DEFAULT;
                    }
                    else if (target->priv.scan_cnt >= target->cfg->shortPressMS)
                    {
                        /* swtich to multiple click stage */
                        target->priv.status = KEY_STAGE_MULTIPLE_CLICK;
                        target->priv.click_cnt++;
                        target->priv.timeout_cnt = target->priv.scan_cnt;
                    }
                    else
                    {
                        target->priv.status = KEY_STAGE_DEFAULT;
                    }
                }
                break;
            
            case KEY_STAGE_MULTIPLE_CLICK:
                if (key_value == target->priv.key_val)/* is pressed */
                {
                    /* swtich to button down stage */
                    target->priv.status  = KEY_STAGE_DOWN;
                    target->priv.scan_cnt = 0;
                }
                else
                {
                    if (target->cfg->multipleClickMs > 0) /* 多击超时不为0则使用多击功能 */
                    {
                        if ((target->priv.scan_cnt - target->priv.timeout_cnt) > target->cfg->multipleClickMs)
                        {
                            if (target->priv.click_cnt > 1) /* multiple click */
                            {
                                EVENT_SET_AND_EXEC_CB(KEY_PRESS_REPEAT_CLICK);
                                target->priv.status = KEY_STAGE_DEFAULT;
                            }
                            else /* 单次抬起 */
                            {
                                EVENT_SET_AND_EXEC_CB(KEY_PRESS_SHORT_UP);
                                target->priv.status = KEY_STAGE_DEFAULT;
                            }
                        }
                    }
                    else
                    {
                        EVENT_SET_AND_EXEC_CB(KEY_PRESS_SHORT_UP);
                        target->priv.status = KEY_STAGE_DEFAULT;
                    }
                }
                break;
                
            case KEY_STAGE_COMBINATION:
                if (key_value == 0) // 组合状态等待松手
                {
                    // 如果100ms内送手, 则发送各类抬起事件, 否则重新开始
                    if ((target->priv.scan_cnt - target->priv.timeout_cnt) < COMB_UP_TIME_VALID) 
                    {
                        if (target->priv.scan_cnt >= target->cfg->longHoldMS)
                        {
                            EVENT_SET_AND_EXEC_CB(KEY_PRESS_LONG_HOLD_UP);
                        }
                        else if (target->priv.scan_cnt >= target->cfg->longPressMS)
                        {
                            EVENT_SET_AND_EXEC_CB(KEY_PRESS_LONG_UP);
                        }
                        else if (target->priv.scan_cnt >= target->cfg->shortPressMS)
                        {
                            EVENT_SET_AND_EXEC_CB(KEY_PRESS_SHORT_UP);
                        }
                    }
                    
                    target->priv.status = KEY_STAGE_DEFAULT; 
                }
                break;
            
            default:
                break;
        }
    }

    return MK_SUCCESS;
}

/**
 * \brief Key轮询检测任务
 */
static char svc_key_task(thread_t* pt)
{
    thread_begin
    {
        while (1)
        {
            thread_sleep(KEY_SCAN_TICK_MS);
            _svc_key_scan();
        }
    }
    thread_end
} 
 
/**
 * \brief 按键注册  
 * \param[in]  pThis 配置数据
 * \return 错误代码 \ref mk_common.h    
 */
int svc_key_init(const TS_KEY_CFG *key_tab, uint8_t key_num)
{ 
    uint8_t i = 0;
    
    if (key_tab == NULL)
        return MK_ERROR_NULL_POINTER;
    
    for (; i<key_num; i++) {
        memset(&s_key_list[i], 0, sizeof(TS_KEY));
        s_key_list[i].cfg = &key_tab[i];
        s_key_list[i].priv.active = 1;
    }
    
    thread_create(svc_key_task);

    s_key_num = i;
    return i;
}

/**
 * \brief 获取按键个数  
 * \retval 错误代码 \ref mk_common.h
 */
int svc_key_get_num(void)
{    
    return s_key_num;
}

/**
 * \brief 挂起某个按键 
 * \param[in] id 设备id
 * \retval 错误代码 \ref mk_common.h
 */
int svc_key_suspend(uint8_t id)
{    
    s_key_list[id].priv.active = 0;
    return MK_SUCCESS;
}

/**
 * \brief 恢复某个按键 
 * \param[in] id 设备id
 * \retval 错误代码 \ref mk_common.h
 */
int svc_key_resume(uint8_t id)
{  
    s_key_list[id].priv.active   = 1;
    s_key_list[id].priv.click_cnt = 0;
    s_key_list[id].priv.scan_cnt  = 0;
    s_key_list[id].priv.event    = KEY_STAGE_DEFAULT;
    return MK_SUCCESS;
}

/**
 * \brief 挂起所有按键 
 * \retval 错误代码 \ref mk_common.h
 */
int svc_key_suspend_all(void)
{ 
    for (uint8_t i=0; i<s_key_num; i++)
    {
        s_key_list[i].priv.active = 0; 
    }
    return MK_SUCCESS;
}

/**
 * \brief 恢复所有按键 
 * \retval 错误代码 \ref mk_common.h
 */
int svc_key_resume_all(void)
{ 
    for (uint8_t i=0; i<s_key_num; i++)
    {
        s_key_list[i].priv.active   = 1;
        s_key_list[i].priv.click_cnt = 0;
        s_key_list[i].priv.scan_cnt  = 0;
        s_key_list[i].priv.event    = KEY_STAGE_DEFAULT; 
    }
    return MK_SUCCESS;
}

/******************************* End of File (C）****************************/
