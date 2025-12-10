#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "mk_log_printf.h"

#if (_PRINTF_LOG_MODE == PRINTF_LOG_MODE_RTT)
#include "SEGGER_RTT.h"
#endif

static TS_LOG_LEVEL s_eLogLevel = _LOG_DEFAULT_LEV;

/**
 * \brief 设置打印级别
 * \param[in] eLevel 打印级别 
 */
int mk_log_set_level(TS_LOG_LEVEL eLevel)
{
    s_eLogLevel = eLevel;
    return LOG_OK;
}

/**
 * \brief 获取打印级别
 * \retval   eLevel 打印级别
 */
TS_LOG_LEVEL mk_log_get_level(void)
{
    return s_eLogLevel;
}

/**
 * \brief LOG打印
 * \param[in] format 打印内容 
 */
int mk_log_printf(const char *format, ...)
{
    char str_buf[_LOG_WRITE_ONCE_SIZE] = {0};

    va_list arg;
    va_start(arg, format);
    vsnprintf(str_buf, sizeof(str_buf) - 1, format, arg);
    va_end(arg);

#if (_PRINTF_LOG_MODE == PRINTF_LOG_MODE_UART)
    printf("%s", str_buf);
#elif (_PRINTF_LOG_MODE == PRINTF_LOG_MODE_RTT)
    SEGGER_RTT_printf(0, "%s", log);
#endif
    return LOG_OK;
}

/**
 * \brief LOG打印数组
 * \param[in] pBuf 数组指针
 * \param[in] Size 数组大小 
 */
int mk_log_printf_hexdump(uint8_t *pBuf, uint16_t Size)
{
#if (_PRINTF_LOG_MODE != 0)
    mk_log_printf("0x%8X: ", (uint32_t)pBuf);
    for (uint16_t j = 0; j < Size; j++)
    {
        mk_log_printf("%02X ", pBuf[j]);
        if ((j & 0x00000F) == 0x00000F)
        {
            mk_log_printf("\n0x%8X: ", (uint32_t)(pBuf + j + 1));
        }
    }
    mk_log_printf("\n");
#endif
    return 0;
}
