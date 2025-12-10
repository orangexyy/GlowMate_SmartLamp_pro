/**************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 : 
**  功能描述 : 
**  作    者 : 
**  日    期 :  
**  版    本 :  
**  变更记录 :  
\**************************************************************************/   
 
#ifndef __SVC_LED_H__
#define __SVC_LED_H__

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
#ifndef LED_MAX_NUM
#define LED_MAX_NUM          (5)       /**< 最大LED数量 */
#endif

#ifndef LED_SCAN_INTERVAL  
#define LED_SCAN_INTERVAL    (10)
#endif

#ifndef LED_MIN_BRIGHTNESS  
#define LED_MIN_BRIGHTNESS   (0)
#endif

/**************************************************************************\
                            Typedef definitions
\**************************************************************************/
typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
}TS_COLOR;

typedef struct
{
    /** 
     *  显示字符表示方法:
     *  ' ' : 表示灭, 
     *  '-' : 表示亮, 
     *  'v' : 表示呼吸灯样式, 会一直循环, 直到调用Stop为止
     *  '/' : 表示渐亮, 
     *  '\\': 表示渐灭,  
     */
	char    *type;          /**< 显示样式表 */ 
    uint8_t interval;       /**< 读表间隔, 默认单位10ms, n*10ms */  
    uint8_t brightness;     /**< 亮度0-255 */ 
    uint8_t repeat_tms;     /**< 重复次数, 0表示无限循环 */
    TS_COLOR color;         /**< RGB值, 单色灯时为{0, 0, 0} */
} TS_LED_TABLE;

/**
 * \brief LED结束事件
 * \param[in] id 设备id 
 * \param[in] Type 显示模式
 */
typedef void (*LED_FINISH_CB)(uint8_t id, uint8_t type);

/**
 * \brief LED自定义
 * \param[in] id 设备id  
 */
typedef void (*LED_COSTOM_CB)(uint8_t id, char c_type, uint8_t brightness, TS_COLOR *color);
/**************************************************************************\
                         Global variables definitions
\**************************************************************************/

/**************************************************************************\
                         Global functions definitions
\**************************************************************************/

/**
 * \brief 注册LED, 注册成功则返回id值
 * \param[in] pThis 配置结构体
 * \retval 错误代码或id值
 *           \arg 大于0为设备id
 *           \arg 小于0为错误代码 \ref mk_common.h
 */
int svc_led_init(uint8_t id, const TS_LED_TABLE *type_table, uint8_t type_num);

/**
 * \brief 根据设备个数
 * \return 设备个数
 */
uint8_t svc_led_get_num(void);
 
/**
 * \brief 获取led模式总个数 
 * \param[in] id 设备id 
 * \retval 错误代码 \ref mk_common.h
 */
int svc_led_get_mode_num(uint8_t id);

/**
 * \brief 注册显示结束回调函数 
 * \param[in] id 设备id
 * \param[in] CB 回调函数
 * \retval 错误代码 \ref mk_common.h
 */
int svc_led_reg_finish_cb(uint8_t id, LED_FINISH_CB cb);

/**
 * \brief 注册自定义接口
 * \param[in] id 设备id
 * \param[in] cb 回调函数
 * \retval 错误代码 \ref mk_common.h
 */
int svc_led_reg_costom_cb(uint8_t id, LED_COSTOM_CB cb);

/**
 * \brief 停止某个LED设备
 * \param[in] id id号
 * \return 错误代码 \ref mk_common.h
 */
int svc_led_stop(uint8_t id);

/**
 * \brief 停止所有LED设备
 * \return 错误代码 \ref mk_common.h
 */
int svc_led_stop_all(void);

/**
 * \brief 设定LED的工作模式
 * \param[in] id id号
 * \param[in] mode 模式编号
 * \return 错误代码 \ref mk_common.h
 */
int svc_led_set_mode(uint8_t id, uint8_t mode);

/**
 * \brief 获取LED的工作模式
 * \return mode 模式编号
 */
uint8_t svc_led_get_mode(uint8_t id);

/**
 * \brief 获取设备当前是否忙
 * \param[in] id id号
 * \retval 错误代码或是否忙
 *           \arg 0 空闲
 *           \arg 1 忙
 *           \arg 小于0为错误代码 \ref mk_common.h
 */
int svc_led_is_busy(uint8_t id);

/**
 * \brief LED测试
 * \param[in] id id号 
 */
void svc_led_test(uint8_t id);

#ifdef __cplusplus
}
#endif 

#endif
/**************************** End of File (H) ****************************/ 
