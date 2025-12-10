
#ifndef __MK_LOG_PRINTF_H__
#define __MK_LOG_PRINTF_H__

#include <stdint.h>
#include "mk_log.h"

#if _PRINTF_LOG_MODE

#if _LOG_VERB_EN
#define LOG_V(msg...)                                         \
    do                                                        \
    {                                                         \
        if (mk_log_get_level() > E_LOG_V)                         \
        {                                                     \
            break;                                            \
        }                                                     \
        mk_log_printf("V> [%s:%d]: ", __FUNCTION__, __LINE__); \
        mk_log_printf(msg);                                    \
        mk_log_printf("\r\n");                                 \
    } while (0)

#else
#define LOG_V(msg...)
#endif

#if _LOG_DBUG_EN
#define LOG_D(msg...)                                         \
    do                                                        \
    {                                                         \
        if (mk_log_get_level() > E_LOG_D)                         \
        {                                                     \
            break;                                            \
        }                                                     \
        mk_log_printf("D> [%s:%d]: ", __FUNCTION__, __LINE__); \
        mk_log_printf(msg);                                    \
        mk_log_printf("\r\n");                                 \
    } while (0)

#else
#define LOG_D(msg...)
#endif

#if _LOG_INFO_EN
#define LOG_I(msg...)                                         \
    do                                                        \
    {                                                         \
        if (mk_log_get_level() > E_LOG_I)                         \
        {                                                     \
            break;                                            \
        }                                                     \
        mk_log_printf("I> [%s:%d]: ", __FUNCTION__, __LINE__); \
        mk_log_printf(msg);                                    \
        mk_log_printf("\r\n");                                 \
    } while (0)
#else
#define LOG_I(msg...)
#endif

#if _LOG_WARN_EN
#define LOG_W(msg...)                                         \
    do                                                        \
    {                                                         \
        if (mk_log_get_level() > E_LOG_W)                         \
        {                                                     \
            break;                                            \
        }                                                     \
        mk_log_printf("W> [%s:%d]: ", __FUNCTION__, __LINE__); \
        mk_log_printf(msg);                                    \
        mk_log_printf("\r\n");                                 \
    } while (0)

#else
#define LOG_W(msg...)
#endif

#if _LOG_ASSERT_EN
#define LOG_A(msg...)                                         \
    do                                                        \
    {                                                         \
        if (mk_log_get_level() > E_LOG_A)                         \
        {                                                     \
            break;                                            \
        }                                                     \
        mk_log_printf("A> [%s:%d]: ", __FUNCTION__, __LINE__); \
        mk_log_printf(msg);                                    \
        mk_log_printf("\r\n");                                 \
    } while (0)

#else
#define LOG_A(msg...)
#endif

#if _LOG_ERROR_EN
#define LOG_E(msg...)                                         \
    do                                                        \
    {                                                         \
        if (mk_log_get_level() > E_LOG_E)                         \
        {                                                     \
            break;                                            \
        }                                                     \
        mk_log_printf("E> [%s:%d]: ", __FUNCTION__, __LINE__); \
        mk_log_printf(msg);                                    \
        mk_log_printf("\r\n");                                 \
    } while (0)
#else
#define LOG_E(msg...)
#endif

#if _LOG_PRINT_EN
#define LOG_PRINT(msg...)  \
    do                     \
    {                      \
        mk_log_printf(msg); \
    } while (0)

#else
#define DBG_LOG_PRINT(msg...)
#endif

#define LOG_HEXDUMP(buff, size)           \
    do                                    \
    {                                     \
        mk_log_printf_hexdump(buff, size); \
    } while (0)

#else /* _DBG_LOG_EN */
#define LOG_V(...)
#define LOG_D(...)
#define LOG_I(...)
#define LOG_W(...)
#define LOG_E(...)
#define LOG_A(...)
#endif /* _DBG_LOG_EN */

/**
 * \brief 设置打印级别
 * \param[in] eLevel 打印级别 
 */
int mk_log_set_level(TS_LOG_LEVEL eLevel);

/**
 * \brief 获取打印级别
 * \retval   eLevel 打印级别
 */
TS_LOG_LEVEL mk_log_get_level(void);

/**
 * \brief LOG打印
 * \param[in] format 打印内容 
 */
int mk_log_printf(const char *format, ...);

/**
 * \brief LOG打印数组
 * \param[in] pBuf 数组指针
 * \param[in] Size 数组大小 
 */
int mk_log_printf_hexdump(uint8_t *pBuf, uint16_t Size);

#endif
