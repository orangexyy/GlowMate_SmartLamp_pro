/******************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  mk_common.h
**  功能描述 :  公共资源头文件
**  作    者 :   
**  日    期 :   
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2022.03.08
                1 首次创建
\******************************************************************************/

#ifndef _MK_COMMON_H_
#define _MK_COMMON_H_
/******************************************************************************\
                             Includes
\******************************************************************************/
#include <string.h>
/******************************************************************************\
                           Macro definitions
\******************************************************************************/

/******************************************************************************\
                        Global variables and functions
\******************************************************************************/

#define MK_LIB_VERSION             "v1.1.0" 

#define MK_SUCCESS                 (0)     /**< 成功           */
#define MK_FAIL                    (-1)   /**< 失败         */
#define MK_ERROR_NO_MEM            (-2)    /**< 无内存操作     */
#define MK_ERROR_NOT_FOUND         (-3)    /**< 没有发现       */
#define MK_ERROR_NOT_SUPPORTED     (-4)    /**< 不支持         */
#define MK_ERROR_INVALID_PARAM     (-5)    /**< 无效参数       */
#define MK_ERROR_INVALID_STATE     (-6)    /**< 无效状态       */
#define MK_ERROR_INVALID_LENGTH    (-7)    /**< 无效长度       */
#define MK_ERROR_INVALID_FLAGS     (-8)    /**< 无效标志       */
#define MK_ERROR_INVALID_DATA      (-9)    /**< 无效数据       */
#define MK_ERROR_DATA_SIZE         (-10)    /**< 数据大小不正确 */
#define MK_ERROR_MIN               (-11)   /**< 小于           */
#define MK_ERROR_MAX               (-12)   /**< 大于           */
#define MK_ERROR_TIMEOUT           (-13)   /**< 超时           */
#define MK_ERROR_NULL_POINTER      (-14)   /**< 空指针         */
#define MK_ERROR_FORBIDDEN         (-15)   /**< 被禁止的操作   */
#define MK_ERROR_INVALID_ADDR      (-16)   /**< 无效地址       */
#define MK_ERROR_BUSY              (-17)   /**< 忙             */
#define MK_ERROR_ID                (-18)   /**< 错误ID         */
#define MK_ERROR_READ              (-19)   /**< 读错误         */
#define MK_ERROR_WRITE             (-20)   /**< 写错误         */
#define MK_ERROR_CHECK             (-21)   /**< 校验错误       */
#define MK_ERROR_TYPE              (-22)   /**< 类型不匹配错误 */
#define MK_ERROR_UNINITIALIZED     (-23)   /**< 未初始化       */
#define MK_ERROR_OUT_OF_SCOPE      (-24)   /**< 超范围         */
#define MK_ERROR_LIB               (-255)  /**< 底层库返回错误 */ 

/******************************************************************************\
                             Includes
\******************************************************************************/

/******************************************************************************\
                         Typedef definitions
\******************************************************************************/
// 空指针
#define V_NULL              ((void *)(0))

// 宏定义：将数字转换为十六进制字符串
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// 宏定义：开启/关闭总中断
#define SYSTEM_DISABLE_ALL_IRQ()      __disable_irq()  /**< 关闭总中断 */
#define SYSTEM_ENABLE_ALL_IRQ()       __enable_irq()   /**< 开启总中断 */

// 宏定义：弱定义
#if (__ARMCC_VERSION >= 6000000)
#define __WEAK_DEF   __attribute__((weak))
#else
#define __WEAK_DEF   __weak
#endif

// 宏定义：断言错误
#if (MK_DBG_ERR_EN)
#define MK_ASSERT_ERROR(err)                                   \
    if (err)                                                   \
    {                                                          \
        LOG_E("%s return (%s)", __FUNCTION__, #err); \
    }                                                          \
    return err
#else
#define MK_ASSERT_ERROR(err)  return err
#endif
   
#endif

/******************************* End of File (H) ******************************/
