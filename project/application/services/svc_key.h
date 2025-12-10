/**************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 : 
**  功能描述 : 
**  作    者 : 
**  日    期 :  
**  版    本 :  
**  变更记录 :  
\**************************************************************************/
 
#ifndef __SVC_KEY_H__
#define __SVC_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************\
                               Includes
\**************************************************************************/
#include <stdint.h>
   
/**************************************************************************\
                            Macro definitions
\**************************************************************************/
#ifndef KEY_SCAN_TICK_MS
#define KEY_SCAN_TICK_MS        (10)
#endif

#ifndef MK_PLUGIN_KEY_NUM
#define MK_PLUGIN_KEY_NUM       (5)
#endif
/**************************************************************************\
                            Typedef definitions
\**************************************************************************/
 
/**
 * \brief 按键事件回调
 * \param[in]  keyId 按键ID
 * \param[in]  Evt 按键事件 \ref keyEvt_t
 * \param[in]  Combkey 组合键值 
 * \param[in]  ClickCnt 多击事件产生的点击次数
 */
typedef void (*KEY_EVT_CB)(uint8_t key_id, uint8_t evt, uint8_t comb_key, uint8_t click_cnt);

typedef enum
{
    KEY_PRESS_DOWN         = 0, /**< 按下(无消抖) */
    KEY_PRESS_SHORT_DOWN   = 1, /**< 按下(带消抖, 消抖时间为ShortPressMS) */
    KEY_PRESS_SHORT_UP     = 2, /**< 抬起 */
    KEY_PRESS_LONG_DOWN    = 3, /**< 长按按下 */
    KEY_PRESS_LONG_UP      = 4, /**< 长按抬起 */
    KEY_PRESS_LONG_HOLD    = 5, /**< 超长按 */
    KEY_PRESS_LONG_HOLD_UP = 6, /**< 超长按抬起 */
    KEY_PRESS_LONG_KEEP    = 7, /**< 长按保持, 间隔LongKeepTrigMs触发一次 */
    KEY_PRESS_REPEAT_CLICK = 8, /**< 多击,抬起MultipleClickMs内才能触发多击 */
    KEY_PRESS_NONE,
} TE_KEY_EVT;

typedef struct
{ 
    uint8_t  key_id;            /**< 按键ID */
    uint16_t shortPressMS;      /**< 短按的持续时间 */
    uint16_t longPressMS;       /**< 长按的持续时间 */
    uint16_t longHoldMS;        /**< 超长按的持续时间 */
    uint16_t multipleClickMs;   /**< 多击有效时间, 为0不使用该功能 */
    uint16_t longKeepTrigMs;    /**< 长按保持触发时间间隔, longPressMS后开始, 为0不使用该功能*/
    KEY_EVT_CB cb;
} TS_KEY_CFG;

/**************************************************************************\
                         Global variables definitions
\**************************************************************************/
   
/**************************************************************************\
                         Global functions definitions
\**************************************************************************/
 
/**
 * \brief 按键注册, \note 电源键必须第一个注册
 * \param[in]  key_tab 配置按键组
 * \param[in]  key_num 按键个数
 * \return 错误代码 \ref mk_common.h    
 */
int svc_key_init(const TS_KEY_CFG *key_tab, uint8_t key_num);

/**
 * \brief Key轮询检测
 */
void svc_key_run(void);

/**
 * \brief 挂起某个按键 
 * \param[in] ID 设备ID
 * \retval 错误代码 \ref mk_common.h
 */
int svc_key_suspend(uint8_t ID);

/**
 * \brief 恢复某个按键 
 * \param[in] ID 设备ID
 * \retval 错误代码 \ref mk_common.h
 */
int svc_key_resume(uint8_t ID);

/**
 * \brief 挂起所有按键 
 * \retval 错误代码 \ref mk_common.h
 */
int svc_key_suspend_all(void);

/**
 * \brief 恢复所有按键 
 * \retval 错误代码 \ref mk_common.h
 */
int svc_key_resume_all(void);

 
#ifdef __cplusplus
}
#endif 

#endif
/**************************** End of File (H) ****************************/ 
