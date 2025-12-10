
#ifndef __MK_LOG_DEF_H__
#define __MK_LOG_DEF_H__

#include <stdint.h>

#include "device_config.h"

/* ======================== printf log config ======================== */
#define PRINTF_LOG_DISABLE   (0) /**< 不开启打印 */
#define PRINTF_LOG_MODE_UART (1) /**< UART打印方式 */
#define PRINTF_LOG_MODE_RTT  (2) /**< RTT打印方式 */

#define _DEBUG_LOG_MODE      (PRINTF_LOG_MODE_UART) /**< LOG打印的方式, 1-UART 2-RTT */

#define _LOG_VERB_EN         (1) /**< VERBOSE级别的日志功能, 1-开启 0-关闭  */
#define _LOG_DBUG_EN         (1) /**< DEBUG级别的日志功能, 1-开启 0-关闭  */
#define _LOG_INFO_EN         (1) /**< Info级别的日志功能, 1-开启 0-关闭  */
#define _LOG_ASSERT_EN       (1) /**< ASSERT级别的日志功能, 1-开启 0-关闭  */
#define _LOG_WARN_EN         (1) /**< WARN级别的日志功能, 1-开启 0-关闭  */
#define _LOG_PRINT_EN        (1) /**< Print级别的日志功能, 1-开启 0-关闭 */
#define _LOG_ERROR_EN        (1) /**< Error级别的日志功能, 1-开启 0-关闭 */

#define _LOG_DEFAULT_LEV     (E_LOG_I) /**< 默认打印等级 */

#define _LOG_WRITE_ONCE_SIZE (256) /**< 单次打印的字符最大个数 */

/* ======================== file log config ======================== */
#define _LOG_FILE_EN        (0) /**< 文件 日志开关, 1-开启 0-关闭 */
#if _LOG_FILE_EN
#define _LOG_FILE_TOTAL_SIZE  (16 * 1024)     /**< 日志文件大小 */
#define _LOG_FILE_BUF_SIZE    (1024)          /**< 日志缓存大小 */
#define _LOG_FILE_NAME        "0:/log.txt"    /**< 日志名 */
#define _LOG_FILE_BACKUP_NAME "0:/log2.txt"   /**< 日志备份名 */
#define _LOG_FILE_HALTED_NAME "0:/halted.txt" /**< 死机日志名 */
#endif

/* ======================== flash log config ======================== */
#define _LOG_FLASH_EN       (0) /**< Flash日志功能, 1-开启 0-关闭 */
#if _LOG_FLASH_EN
#define _LOG_FLASH_START_ADDR (0x002F0000) /**< Flash日志起始地址 */
#define _LOG_FLASH_PAGE_NUM   (10)         /**< Flash日志页数 */
#define _LOG_FLASH_PAGE_SIZE  (4096)       /**< Flash日志页大小 */
#define _LOG_FLASH_BUF_SIZE   (1024)       /**< Flash日志缓存大小，0不使用缓存模式, 非0为缓存大小 */
#define _LOG_FLASH_WRITE_GRAN (1)          /**< 写字节颗粒度 1/2/4*/

#include "mk_hal_flash.h"
#define flash_log_write(addr, data, len) mk_hal_flash_write(addr, (uint8_t *)data, len)
#define flash_log_read(addr, data, len)  mk_hal_flash_read(addr, data, len)
#define flash_log_erase(addr, len)       mk_hal_flash_erase(addr, len)
#endif

#if (_LOG_FILE_EN | _LOG_FLASH_EN)
#define log_get_rtc(year, month, date, hour, min, sec) // plugin_rtc_get_log_time(year, month, date, hour, min, sec)
#endif

#endif
