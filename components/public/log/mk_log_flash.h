
#ifndef __MK_LOG_FLASH_H__
#define __MK_LOG_FLASH_H__

#include <stdint.h>
#include "mk_log.h"

#if _LOG_FLASH_EN
#define LOG_FLASH(msg...) \
    do                    \
    {                     \
        mk_log_flash(msg);   \
    } while (0)

#define LOG_FLASH_RAW(msg...) \
    do                        \
    {                         \
        mk_log_flash_raw(msg);   \
    } while (0)
#else
#define LOG_FLASH(msg...)
#define LOG_FLASH_RAW(msg...)
#endif

/**
 * \brief Flash日志初始化
 * \param[in] pThis 配置信息
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功    
 * 				        \arg -1 失败 
 */
int mk_log_flash_init(void);

/**
 * \brief FLASH LOG已使用空间大小
 * \retval 已使用空间
 */
uint32_t mk_log_flash_get_used_size(void);

/**
 * \brief 立即写入缓存的所有flash日志
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_flush(void);

/**
 * \brief 读取一段flash日志
 * \param[in] offset 读取的日志偏移, 取值范围 0 至 total size-1
 * \param[out] pBuf 读取的日志存放地址
 * \param[in] size 要读取的日志大小
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_read_offset(uint32_t offset, uint8_t *pBuf, uint16_t size);

/**
 * \brief 读取最近的flash日志
 * \param[out] pBuf 读取的flash日志存储地址
 * \param[in] size 要读取的日志大小
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_read_recent(uint8_t *pBuf, uint32_t size);

/**
 * \brief 读取所有的flash日志
 * \param[out] pBuf 读取的flash日志存储地址
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_read_all(uint8_t *pBuf);

/**
 * \brief 清除所有flash日志
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_clean_all(void);

/**
 * \brief FLASH LOG打印
 * \param[in] format 打印内容
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash(const char *format, ...);

int mk_log_flash_raw(const char *format, ...);

#endif
