
#ifndef __MK_LOG_H__
#define __MK_LOG_H__

#include <stdint.h>

#include "mk_log_def.h"

#define LOG_OK              (0)  /**< 执行成功 */
#define LOG_ERROR           (-1) /**< 执行错误 */
#define LOG_NO_MEM          (-2) /**< 无空间 */

#ifndef _PRINTF_LOG_MODE
#define _PRINTF_LOG_MODE    (_DEBUG_LOG_MODE) /**< LOG打印的方式, 1-UART 2-RTT */
#endif

#ifndef _LOG_VERB_EN
#define _LOG_VERB_EN (1) /**< 写字节颗粒度 1/2/4*/
#endif

#ifndef _LOG_DBUG_EN
#define _LOG_DBUG_EN (1) /**< 写字节颗粒度 1/2/4*/
#endif

#ifndef _LOG_INFO_EN
#define _LOG_INFO_EN (1) /**< 写字节颗粒度 1/2/4*/
#endif

#ifndef _LOG_WARN_EN
#define _LOG_WARN_EN (1) /**< 写字节颗粒度 1/2/4*/
#endif

#ifndef _LOG_ERROR_EN
#define _LOG_ERROR_EN (1) /**< 写字节颗粒度 1/2/4*/
#endif

#ifndef _LOG_ASSERT_EN
#define _LOG_ASSERT_EN (1) /**< 写字节颗粒度 1/2/4*/
#endif

#ifndef _LOG_FILE_EN
#define _LOG_FILE_EN (0) /**< 文件 LOG总开关, 1-开启 0-关闭 */
#endif

#if _LOG_FILE_EN
#ifndef _LOG_FILE_TOTAL_SIZE
#define _LOG_FILE_TOTAL_SIZE (16 * 1024) /**< 日志文件大小 */
#endif

#ifndef _LOG_FILE_BUF_SIZE
#define _LOG_FILE_BUF_SIZE (1024) /**< 日志缓存大小 */
#endif

#ifndef _LOG_FILE_NAME
#define _LOG_FILE_NAME "0:/log.txt" /**< 日志名 */
#endif

#ifndef _LOG_FILE_BACKUP_NAME
#define _LOG_FILE_BACKUP_NAME "0:/log2.txt" /**< 日志备份名 */
#endif

#ifndef _LOG_FILE_HALTED_NAME
#define _LOG_FILE_HALTED_NAME "0:/halted.txt" /**< 死机日志名 */
#endif
#endif

#ifndef _LOG_FLASH_EN
#define _LOG_FLASH_EN (0) /**< Flash日志功能, 1-开启 0-关闭 */
#endif

#if _LOG_FLASH_EN
#ifndef _LOG_FLASH_START_ADDR
#define _LOG_FLASH_START_ADDR (0x0803FC00) /**< Flash日志起始地址 */
#endif

#ifndef _LOG_FLASH_PAGE_NUM
#define _LOG_FLASH_PAGE_NUM (10) /**< Flash日志页数 */
#endif

#ifndef _LOG_FLASH_PAGE_SIZE
#define _LOG_FLASH_PAGE_SIZE (4096) /**< Flash日志页大小 */
#endif

#ifndef _LOG_FLASH_BUF_SIZE
#define _LOG_FLASH_BUF_SIZE (1024) /**< Flash日志缓存大小，0不使用缓存模式, 非0为缓存大小 */
#endif

#ifndef _LOG_FLASH_WRITE_GRAN
#define _LOG_FLASH_WRITE_GRAN (1) /**< 写字节颗粒度 1/2/4*/
#endif
#endif

#ifndef _LOG_DEFAULT_LEV
#define _LOG_DEFAULT_LEV (E_LOG_I) /**< 默认打印等级 */
#endif

#ifndef _LOG_WRITE_ONCE_SIZE
#define _LOG_WRITE_ONCE_SIZE (256) /**< 单次打印的字符个数 */
#endif

#if (_PRINTF_LOG_MODE == PRINTF_LOG_MODE_RTT)
#include "SEGGER_RTT.h"
#endif

#if (_FILE_LOG_EN || _LOG_FLASH_EN)
#ifndef log_get_rtc
#define log_get_rtc(year, month, date, hour, min, sec)
#endif
#endif

#if _LOG_FLASH_EN
#ifndef flash_log_write
#warning "flash_log_write must be defined. exp: flash_log_write(addr, data, len) hal_flash_write(addr, data, len) "
#endif

#ifndef flash_log_read
#warning "flash_log_read must be defined. exp: flash_log_read(addr, data, len) hal_flash_read(addr, data, len) "
#endif

#ifndef flash_log_erase
#warning "flash_log_erase must be defined. exp: flash_log_erase(addr, data, len) hal_flash_erase(addr, data, len) "
#endif
#endif

typedef enum
{
    E_LOG_V = 0, // verbose
    E_LOG_D,     // debug
    E_LOG_I,     // information
    E_LOG_W,     // warning
    E_LOG_E,     // error
    E_LOG_A,     // assert
    E_LOG_OFF,   // off
} TS_LOG_LEVEL;

#include "mk_log_printf.h"
#include "mk_log_flash.h"
#include "mk_log_file.h"

#endif
