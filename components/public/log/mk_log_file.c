

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "plugin_file.h"
#include "mk_sd_log.h"
#include "mk_printf_log.h"

#if _LOG_FILE_EN
#include "ff.h"
#endif

/**\defgroup LOG_Private_TypesDefinitions
 * \{
 */
#define _SINGLE_FILE_LOG_SIZE (_LOG_FILE_TOTAL_SIZE >> 1)

/**
 * \}
 */

/**\defgroup LOG_Private_Defines
 * \{
 */
#define LOG_OK                (0)  /**< 执行成功 */
#define LOG_ERROR             (-1) /**< 执行错误 */
#define LOG_NO_MEM            (-2) /**< 无空间 */

#define READ_BUF_SIZE         (256) /**< 读buff大小 */

typedef enum
{
    E_LOG_SYNC_NULL,
    E_LOG_SYNC_START,
    E_LOG_SYNC_ING,
    E_LOG_SYNC_END,
} TS_LOG_SYNC_STEP;
/**
 * \}
 */

/**\defgroup LOG_Private_Macros
 * \{
 */

/**
 * \}
 */

#if _LOG_FILE_EN
static FIL        s_log_file_buf_file;
static FIL        s_log_file_halted_file;
static TS_LOG_SYS s_log_file_buf;

static void log_file_check(void)
{
    FILINFO info;
    FRESULT res = f_stat(_LOG_FILE_NAME, &info);
    if (res != FR_OK)
    {
        return;
    }

    if (info.fsize >= _SINGLE_FILE_LOG_SIZE)
    {
        f_rmdir(_LOG_FILE_BACKUP_NAME);
        f_rename(_LOG_FILE_NAME, _LOG_FILE_BACKUP_NAME);
    }
}

void log_file_init(void)
{
    bool     f_ret;
    uint32_t write_bytes;

    memset(s_log_file_buf.databuf, 0, sizeof(s_log_file_buf.databuf));

    log_file_check();

    f_ret = f_open(&s_log_file_buf_file, _LOG_FILE_NAME, FA_OPEN_APPEND | FA_WRITE | FA_READ);
    if (f_ret == FR_OK)
    {
        f_ret = f_lseek(&s_log_file_buf_file, f_size(&s_log_file_buf_file));
        if (f_ret == FR_OK)
        {
            f_ret = f_write(&s_log_file_buf_file, "The log file is started successfully\n", strlen("The log file is started successfully\n"), &write_bytes);
            if (f_ret != FR_OK)
            {
                LOG_E("log write failed, error code %d!!!", f_ret);
            }
        }
        else
        {
            LOG_E("log lseek failed, error code %d!!!", f_ret);
        }
        f_close(&s_log_file_buf_file);
    }
    else
    {
        LOG_E("log open failed, error code %d!!!", f_ret);
    }
}

/* 
* 函数名称 : log_write
* 功能描述 : 写入日志
* 参	数 : data - 日志信息
* 返回值   : 无
*/
void log_file_write(char *format, ...)
{
    char     str_buf[_LOG_WRITE_ONCE_SIZE] = {0};
    uint32_t str_len                       = 0;
    uint16_t year;
    uint8_t  month, date, hour, min, sec;

    if (Log_GetLevel() > E_LOG_V)
    {
        return;
    }

    log_get_rtc(&year, &month, &date, &hour, &min, &sec);
    snprintf(str_buf, sizeof(str_buf), "[%04d-%02d-%02d %02d:%02d:%02d] ", year, month, date, hour, min, sec);

    str_len = strlen(str_buf);

    va_list arg;
    va_start(arg, format);
    vsnprintf(str_buf + str_len, _LOG_WRITE_ONCE_SIZE - 1 - str_len, format, arg);
    va_end(arg);

    str_len = strlen(str_buf);
    if (s_log_file_buf.data_len + str_len > sizeof(s_log_file_buf.databuf))
    {
        LOG_W("log buf not enough memory!!!");
    }
    else
    {
        s_log_file_buf.data_len += str_len;
        strcat(s_log_file_buf.databuf, str_buf);
    }

    s_log_file_buf.write_flag = true;
}

/* 
* 函数名称 : log_sync
* 功能描述 : 同步日志
* 参	数 : data - 日志信息
* 返回值   : 无
*/
void log_file_flush(void)
{
    uint32_t bw;
    FRESULT  f_ret;

    if (s_log_file_buf.write_flag == true)
    {
        log_file_check();

        f_ret = f_open(&s_log_file_buf_file, _LOG_FILE_NAME, FA_OPEN_APPEND | FA_WRITE | FA_READ);
        if (f_ret == FR_OK)
        {
            f_ret = f_write(&s_log_file_buf_file, s_log_file_buf.databuf, s_log_file_buf.data_len, &bw);
            if (f_ret == FR_OK)
            {
                if (bw != s_log_file_buf.data_len)
                {
                    LOG_W("log write len:%d, bw:%d\n", s_log_file_buf.data_len, bw);
                }
                s_log_file_buf.data_len   = 0;
                s_log_file_buf.write_flag = false;
                memset(s_log_file_buf.databuf, 0, sizeof(s_log_file_buf.databuf));
                if (f_ret != FR_OK)
                {
                    LOG_W("log close failed, code %d!!!", f_ret);
                }
            }
            else
            {
                LOG_W("log write failed, code %d!!!", f_ret);
            }
            f_ret = f_close(&s_log_file_buf_file);
            if (f_ret != FR_OK)
            {
                LOG_W("log close failed, code %d!!!", f_ret);
            }
        }
    }
}

int log_file_read(uint8_t file_type, int (*print)(char *buf, uint16_t len))
{
    FRESULT  f_ret;
    uint32_t rbw;
    FIL      s_sync_file;
    FILINFO  info;
    char     str_buf[_LOG_WRITE_ONCE_SIZE] = {0};

    if (file_type)
    {
        f_ret = f_open(&s_sync_file, _LOG_FILE_HALTED_NAME, FA_OPEN_EXISTING | FA_READ);
        if (f_ret == FR_OK)
        {
            f_ret = f_stat(_LOG_FILE_HALTED_NAME, &info);
            if (f_ret == FR_OK)
            {
                for (uint32_t i = 0; i < info.fsize; i += _LOG_WRITE_ONCE_SIZE)
                {
                    f_ret = f_read(&s_sync_file, str_buf, _LOG_WRITE_ONCE_SIZE, &rbw);
                    if (f_ret != FR_OK)
                    {
                        break;
                    }
                    if (print(str_buf, rbw) != 0)
                    {
                        break;
                    }
                }
            }
        }
    }
    else
    {
        f_ret = f_open(&s_sync_file, _LOG_FILE_BACKUP_NAME, FA_OPEN_EXISTING | FA_READ);
        if (f_ret == FR_OK)
        {
            f_ret = f_stat(_LOG_FILE_BACKUP_NAME, &info);
            if (f_ret == FR_OK)
            {
                for (uint32_t i = 0; i < info.fsize; i += _LOG_WRITE_ONCE_SIZE)
                {
                    f_ret = f_read(&s_sync_file, str_buf, _LOG_WRITE_ONCE_SIZE, &rbw);
                    if (f_ret != FR_OK)
                    {
                        break;
                    }
                    if (print(str_buf, rbw) != 0)
                    {
                        break;
                    }
                }
            }
            f_close(&s_sync_file);
        }

        f_ret = f_open(&s_sync_file, _LOG_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
        if (f_ret == FR_OK)
        {
            f_ret = f_stat(_LOG_FILE_NAME, &info);
            if (f_ret == FR_OK)
            {
                for (uint32_t i = 0; i < info.fsize; i += _LOG_WRITE_ONCE_SIZE)
                {
                    f_ret = f_read(&s_sync_file, str_buf, _LOG_WRITE_ONCE_SIZE, &rbw);
                    if (f_ret != FR_OK)
                    {
                        break;
                    }
                    if (print(str_buf, rbw) != 0)
                    {
                        break;
                    }
                }
            }
        }
    }

    f_close(&s_sync_file);
    return 0;
}

void log_file_halted_open(void)
{
    f_open(&s_log_file_halted_file, _LOG_FILE_HALTED_NAME, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
}

void log_file_halted_write(const char *format, ...)
{
    uint32_t bw;
    char     str_buf[_LOG_WRITE_ONCE_SIZE] = {0};
    uint32_t str_len                       = 0;
    uint16_t year;
    uint8_t  month, date, hour, min, sec;

    log_get_rtc(&year, &month, &date, &hour, &min, &sec);
    snprintf(str_buf, sizeof(str_buf), "[%04d-%02d-%02d %02d:%02d:%02d] ", year, month, date, hour, min, sec);

    str_len = strlen(str_buf);

    va_list arg;
    va_start(arg, format);
    vsnprintf(str_buf + str_len, _LOG_WRITE_ONCE_SIZE - 1 - str_len, format, arg);
    va_end(arg);

    f_write(&s_log_file_halted_file, str_buf, strlen(str_buf), &bw);
}

void log_file_halted_close(void)
{
    f_close(&s_log_file_halted_file);
}

#endif
