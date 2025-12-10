/******************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  svc_mode.h
**  功能描述 :  对系统模式进行统一管理，实现不同模式的切换、调度逻辑，在同一个时刻只有某一个模式生效
**  作    者 :  王滨泉
**  日    期 :  2022.04.25
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2022.04.25
                1 首次创建
\******************************************************************************/
#ifndef _SVC_MODE_H_
#define _SVC_MODE_H_
/******************************************************************************\
                             Includes
\******************************************************************************/
#include <stdbool.h>
#include "event_manager.h"
/******************************************************************************\
                          Macro definitions
\******************************************************************************/
// 默认模式, 自定义模式从6开始, 在app_mode.h中定义           
#define SYSTEM_MODE_IDLE            (0)               // 空闲
#define SYSTEM_MODE_WORK            (1)               // 工作
#define SYSTEM_MODE_UPGRADE         (2)               // 升级
#define SYSTEM_MODE_SLEEP           (3)               // 睡眠
#define SYSTEM_MODE_ERROR           (4)               // 故障 
#define SYSTEM_MODE_TESTING         (5)               // 测试模式 

#ifndef SYSTEM_MODE_MAX
#define SYSTEM_MODE_MAX             (10)
#endif
/******************************************************************************\
                         Typedef definitions
\******************************************************************************/
// 模式管理结构体
typedef struct
{
    char *name;
    void (*init)(void);
    int (*entry)(TS_EVENT_DATA* event);
    int (*translate)(TS_EVENT_DATA *evt);
    void (*exit)(void);
} TS_SVC_MODE;

typedef struct
{
    uint8_t mode;
    const TS_SVC_MODE *(*install)(void);
} TS_SVC_MODE_REG;

/******************************************************************************\
                         Global variables definitions
\******************************************************************************/

/******************************************************************************\
                         Global functions definitions
\******************************************************************************/

/**
 * \brief 模式管理初始化
 * \param[in]  p_mode_table - 模式表
 * \param[in]  mode_num - 模式数量
 * \return 成功返回MK_SUCCESS, 失败返回MK_FAIL
 */
int svc_mode_init(const TS_SVC_MODE_REG *p_mode_table, uint8_t mode_num);

/**
 * \brief 获取当前系统模式
 * \return 当前系统运行的模式
 */
uint8_t svc_mode_get_current_mode(void);

#endif
/******************************* End of File (H) ******************************/
