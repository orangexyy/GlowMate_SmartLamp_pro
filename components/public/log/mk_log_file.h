
#ifndef __MK_LOG_FILE_H__
#define __MK_LOG_FILE_H__

#include <stdint.h>
#include "mk_log.h"

#if _LOG_FILE_EN

typedef struct
{
    char     databuf[_LOG_FILE_BUF_SIZE];
    uint16_t data_len;
    bool     write_flag;
} TS_LOG_SYS;

#if _FLASH_LOG_EN
#if _LOG_FLASH_EN
#define LOG_FILE(msg...)     \
    do                       \
    {                        \
        log_file_write(msg); \
    } while (0)
#else
#define LOG_FILE(msg...)
#endif
#else
#define LOG_FILE(msg...)
#endif

void log_file_init(void);

/* 
* 函数名称 : log_write
* 功能描述 : 写入日志
* 参	数 : data - 日志信息
* 返回值   : 无
*/
void log_file_write(char *format, ...);
/* 
* 函数名称 : log_sync
* 功能描述 : 同步日志
* 参	数 : data - 日志信息
* 返回值   : 无
*/
void log_file_flush(void);

int log_file_read(uint8_t file_type, int (*print)(char *buf, uint16_t len));

void log_file_halted_open(void);
void log_file_halted_write(const char *format, ...);
void log_file_halted_close(void);
#endif

#endif
