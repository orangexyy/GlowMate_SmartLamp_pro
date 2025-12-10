

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "mk_log_flash.h"

#if _LOG_FLASH_EN

#define READ_BUF_SIZE   (256) /**< 读buff大小 */

#define BYTE_ALIGN(len) ((4 - (len & (_LOG_FLASH_WRITE_GRAN - 1))) & (_LOG_FLASH_WRITE_GRAN - 1))

typedef enum
{
    SECTOR_EMPTY,
    SECTOR_USING,
    SECTOR_FULL,
} SecSta_t;

typedef struct
{
    uint32_t Head;
    uint32_t Tail;
} Index_t;

typedef struct
{
    uint8_t  writeF;
    uint8_t  wBuff[_LOG_FLASH_BUF_SIZE];
    uint16_t BufPos;
    Index_t  Index;
} mk_log_t;

static mk_log_t s_Log = {0};

/**
 * \brief 查找结束地址
 * \param[in] addr 地址
 * \param[in] sec_size 范围大小
 * \return 结束地址
 */
static uint32_t _mk_log_flash_find_using_end_addr(uint32_t addr, uint32_t sec_size)
{
    uint32_t start = addr, continue_ff = 0;
    uint8_t  buf[READ_BUF_SIZE];

    /**
     * 计算连续的0xFF个数
     */
    while (start < addr + sec_size)
    {
        flash_log_read(start, buf, READ_BUF_SIZE);
        for (uint32_t i = 0; i < READ_BUF_SIZE; i++)
        {
            if (buf[i] == 0xFF)
            {
                continue_ff++;
            }
            else
            {
                continue_ff = 0;
            }
        }
        start += READ_BUF_SIZE;
    }

    /**
     * 结束地址-4,避免与开始地址重合,无法判断是否为空还是为满
     */
    if (continue_ff == sec_size)
    {
        return (addr >= 4) ? addr - 4 : 0;
    }
    else if (continue_ff >= 4)
    {
        return (((addr + sec_size - continue_ff) >> 2) << 2) - 4;
    }
    else
    {
        return addr + sec_size - 4;
    }
}

/**
 * \brief 获取扇区状态
 * \param[in] addr 扇区地址
 * \param[in] sec_size 扇区大小
 * \param[out] offset_addr 偏移地址
 * \return 扇区状态
 */
static SecSta_t _mk_log_flash_get_sector_status(uint32_t addr, uint32_t sec_size, uint32_t *offset_addr)
{
    uint32_t cur_using_addr = _mk_log_flash_find_using_end_addr(addr, sec_size);

    if (offset_addr != NULL)
    {
        *offset_addr = cur_using_addr;
    }

    if ((cur_using_addr == 0) || (cur_using_addr == addr - 4))
    {
        return SECTOR_EMPTY;
    }
    else if (cur_using_addr == (addr + sec_size - 4))
    {
        return SECTOR_FULL;
    }
    else
    {
        return SECTOR_USING;
    }
}

/**
 * \brief 查找LOG存储的开始和结束地址
 */
static void _mk_log_flash_FindStartAndEndAddr(void)
{
    uint32_t last_using_addr = 0;
    uint32_t cur_using_addr  = 0;

    SecSta_t last_sec_status;
    SecSta_t cur_sec_status;

    uint32_t empty_sec_counts   = 0;
    uint32_t using_sec_counts   = 0;
    uint32_t full_sector_counts = 0;

    uint32_t log_end_addr = _LOG_FLASH_START_ADDR + _LOG_FLASH_PAGE_NUM * _LOG_FLASH_PAGE_SIZE; /**< log结束地址 */
    uint32_t addr_offset  = _LOG_FLASH_START_ADDR + _LOG_FLASH_PAGE_SIZE;

    cur_sec_status  = _mk_log_flash_get_sector_status(_LOG_FLASH_START_ADDR, _LOG_FLASH_PAGE_SIZE, &last_using_addr);
    last_sec_status = cur_sec_status;
    for (; addr_offset < log_end_addr; addr_offset += _LOG_FLASH_PAGE_SIZE)
    {
        cur_sec_status = _mk_log_flash_get_sector_status(addr_offset, _LOG_FLASH_PAGE_SIZE, &cur_using_addr);
        switch (last_sec_status)
        {
            case SECTOR_EMPTY:
            {
                switch (cur_sec_status)
                {
                    case SECTOR_EMPTY:
                        break;

                    case SECTOR_USING:
                        mk_log_flash_clean_all(); /**< 错误结果, 清掉所有记录 */
                        return;

                    case SECTOR_FULL:
                        s_Log.Index.Head = addr_offset;
                        if (s_Log.Index.Tail == 0) /**< 上一页使用,当前页为空,下一页为满的情况 */
                        {
                            s_Log.Index.Tail = last_using_addr;
                        }
                        break;
                }
                empty_sec_counts++;
                break;
            }

            case SECTOR_USING:
            {
                switch (cur_sec_status)
                {
                    case SECTOR_EMPTY:
                        s_Log.Index.Head = _LOG_FLASH_START_ADDR;
                        s_Log.Index.Tail = last_using_addr;
                        break;

                    case SECTOR_USING:
                        mk_log_flash_clean_all(); /**< 错误结果, 清掉所有记录 */
                        return;

                    case SECTOR_FULL:
                        s_Log.Index.Head = addr_offset;
                        s_Log.Index.Tail = last_using_addr;
                        break;
                }
                using_sec_counts++;
                break;
            }

            case SECTOR_FULL:
            {
                if (cur_sec_status != SECTOR_FULL)
                {
                    if (cur_sec_status == SECTOR_EMPTY)
                    {
                        s_Log.Index.Tail = addr_offset - 4;
                    }
                    else
                    {
                        s_Log.Index.Tail = cur_using_addr;
                    }

                    if (addr_offset + _LOG_FLASH_PAGE_SIZE < log_end_addr)
                    {
                        uint32_t sec_status = _mk_log_flash_get_sector_status(addr_offset + _LOG_FLASH_PAGE_SIZE,
                                                                         _LOG_FLASH_PAGE_SIZE, NULL);
                        if (sec_status == SECTOR_EMPTY)
                        {
                            s_Log.Index.Head = _LOG_FLASH_START_ADDR;
                        }
                        else if (sec_status == SECTOR_FULL)
                        {
                            s_Log.Index.Head = addr_offset + _LOG_FLASH_PAGE_SIZE;
                        }
                        else
                        {
                            mk_log_flash_clean_all(); /**< 错误结果, 清掉所有记录 */
                        }
                    }
                    else
                    {
                        if (s_Log.Index.Head == 0)
                        {
                            s_Log.Index.Head = _LOG_FLASH_START_ADDR;
                        }
                    }
                }

                full_sector_counts++;
                break;
            }
        }

        last_sec_status = cur_sec_status;
        last_using_addr = cur_using_addr;
    }

    if (cur_sec_status == SECTOR_EMPTY)
    {
        empty_sec_counts++;
    }
    else if (cur_sec_status == SECTOR_USING)
    {
        using_sec_counts++;
    }
    else if (cur_sec_status == SECTOR_FULL)
    {
        full_sector_counts++;
    }

    if (using_sec_counts > 1)
    {
        mk_log_flash_clean_all(); /**< 错误结果, 清掉所有记录 */
    }
    else if (empty_sec_counts == _LOG_FLASH_PAGE_NUM)
    {
        s_Log.Index.Head = _LOG_FLASH_START_ADDR;
        s_Log.Index.Tail = _LOG_FLASH_START_ADDR;
    }
    else if (full_sector_counts == _LOG_FLASH_PAGE_NUM)
    {
        mk_log_flash_clean_all(); /**< 错误结果, 清掉所有记录 */
    }
}

/**
 * \brief 获取下一个扇区地址
 * \param[in] cur_addr 当前扇区地址
 * \retutn 下一个扇区地址 
 */
static uint32_t _mk_log_flash_GetNextSecAddr(uint32_t cur_addr)
{
    uint32_t cur_sec_id = (cur_addr - _LOG_FLASH_START_ADDR) / _LOG_FLASH_PAGE_SIZE;

    if (cur_sec_id + 1 >= _LOG_FLASH_PAGE_NUM)
    {
        return _LOG_FLASH_START_ADDR;
    }
    else
    {
        return _LOG_FLASH_START_ADDR + (cur_sec_id + 1) * _LOG_FLASH_PAGE_SIZE;
    }
}

/**
 * \brief 获取下一个扇区地址
 * \param[in] cur_addr 当前扇区地址
 * \retutn 下一个扇区地址 
 */
static int _mk_log_flash_CheckWrite(uint32_t addr, const uint8_t *pData, uint32_t size)
{
    uint32_t write_addr = addr;
    uint32_t erase_addr;

    uint32_t log_end_addr = _LOG_FLASH_START_ADDR + _LOG_FLASH_PAGE_NUM * _LOG_FLASH_PAGE_SIZE;

    /**
     * 检查写完当次数据后是否空间刚好全部写满, 是则把开始地址移动到下一页,
     * 避免在写入当次数据后掉电/重启,导致无法查找开始和结束地址
     */
    uint32_t write_end_addr = write_addr + size;
    if ((write_end_addr == s_Log.Index.Head) || (write_end_addr == log_end_addr))
    {
        erase_addr = _mk_log_flash_GetNextSecAddr(write_addr - 4); /**< 计算下一个扇区的地址 */

        /**< 如果开始地址与擦除地址相同,则把开始地址移动到下一个扇区 */
        if (s_Log.Index.Head == erase_addr)
        {
            s_Log.Index.Head = _mk_log_flash_GetNextSecAddr(s_Log.Index.Head);
        }

        if (flash_log_erase(erase_addr, _LOG_FLASH_PAGE_SIZE) == 0)
        {
            return LOG_ERROR;
        }
    }

    if (flash_log_write(write_addr, pData, size) != size)
    {
        return LOG_ERROR;
    }
    return LOG_OK;
}

/**
 * \brief 读取flash日志
 * \param[in] index 要读取的数据偏移
 * \param[out] pBuf 读取的flash日志存储地址
 * \param[in] size 要读取的日志大小
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
static int _mk_log_flash_read(uint32_t index, uint8_t *pBuf, uint16_t size)
{
    uint32_t ret              = 0;
    uint32_t read_uint32_temp = 0;
    uint32_t total_log_size   = (_LOG_FLASH_PAGE_NUM * _LOG_FLASH_PAGE_SIZE);

    if (s_Log.Index.Head < s_Log.Index.Tail)
    {
        ret = (flash_log_read(s_Log.Index.Head + index, pBuf, size) != size);
    }
    else if (s_Log.Index.Head > s_Log.Index.Tail) /**< 已全部写满 */
    {
        if (s_Log.Index.Head + index + size <= _LOG_FLASH_START_ADDR + total_log_size)
        {
            ret = (flash_log_read(s_Log.Index.Head + index, pBuf, size) != size);
        }
        else if (s_Log.Index.Head + index < _LOG_FLASH_START_ADDR + total_log_size)
        {
            read_uint32_temp = (_LOG_FLASH_START_ADDR + total_log_size) - (s_Log.Index.Head + index);
            ret              = (flash_log_read(s_Log.Index.Head + index, pBuf, read_uint32_temp) != read_uint32_temp);
            if (ret == 0)
            {
                ret = (flash_log_read(_LOG_FLASH_START_ADDR, pBuf + read_uint32_temp,
                                      size - read_uint32_temp) != (size - read_uint32_temp));
            }
        }
        else
        {
            ret = (flash_log_read(s_Log.Index.Head + index - total_log_size, pBuf, size) != size);
        }
    }

    if (ret != 0)
    {
        return LOG_ERROR;
    }

    return LOG_OK;
}

/**
 * \brief 写入flash日志
 * \param[in] log  要写入的日志地址
 * \param[in] size 要写入的日志大小
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
static int _mk_log_flash_Write(const uint8_t *log, uint32_t size)
{
    uint32_t writable_size = 0;
    uint32_t write_size    = 0;
    uint32_t write_addr;

    if (s_Log.Index.Head != s_Log.Index.Tail)
    {
        write_addr = s_Log.Index.Tail + 4;
    }
    else
    {
        write_addr = s_Log.Index.Head;
    }

    /**< 计算当前扇区可剩余写入的空间大小  */
    writable_size = _LOG_FLASH_PAGE_SIZE - ((write_addr - _LOG_FLASH_START_ADDR) % _LOG_FLASH_PAGE_SIZE);
    if (writable_size == _LOG_FLASH_PAGE_SIZE)
    {
        write_addr = _mk_log_flash_GetNextSecAddr(write_addr - 4);
    }

    if (size > writable_size) /**< 如果写入大小大于剩余空间,则分多次写入 */
    {
        if (_mk_log_flash_CheckWrite(write_addr, log, writable_size) != 0)
        {
            return LOG_ERROR;
        }

        write_addr = _mk_log_flash_GetNextSecAddr(write_addr - 4);
        write_size += writable_size;
        while (size - write_size > _LOG_FLASH_PAGE_SIZE)
        {
            if (_mk_log_flash_CheckWrite(write_addr, log + write_size, _LOG_FLASH_PAGE_SIZE))
            {
                return LOG_ERROR;
            }

            write_size += _LOG_FLASH_PAGE_SIZE;
            write_addr += _LOG_FLASH_PAGE_SIZE;
            write_addr = _mk_log_flash_GetNextSecAddr(write_addr - 4);
        }

        if (size - write_size > 0)
        {
            if (flash_log_write(write_addr, log + write_size, size - write_size) != (size - write_size))
            {
                return LOG_ERROR;
            }
        }
        s_Log.Index.Tail = write_addr + (size - write_size) - 4;
    }
    else
    {
        if (_mk_log_flash_CheckWrite(write_addr, log, size))
        {
            return LOG_ERROR;
        }

        s_Log.Index.Tail = write_addr + size - 4;
        return LOG_OK;
    }

    return LOG_OK;
}

/**
 * \}
 */

/**\addtogroup LOG_Exported_Functions
 * \{
 */

/**
 * \brief Flash日志初始化
 * \param[in] pThis 配置信息
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功    
 * 				        \arg -1 失败 
 */
int mk_log_flash_init(void)
{
    _mk_log_flash_FindStartAndEndAddr();
    return LOG_OK;
}

/**
 * \brief FLASH LOG已使用空间大小
 * \retval 已使用空间
 */
uint32_t mk_log_flash_GetUsedSize(void)
{
    if (s_Log.Index.Head < s_Log.Index.Tail)
    {
        return s_Log.Index.Tail - s_Log.Index.Head + 4;
    }
    else if (s_Log.Index.Head > s_Log.Index.Tail)
    {
        return (_LOG_FLASH_PAGE_NUM * _LOG_FLASH_PAGE_SIZE) - (s_Log.Index.Head - s_Log.Index.Tail) + 4;
    }
    else
    {
        return 0;
    }
}

/**
 * \brief 立即写入缓存的所有flash日志
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_Flush(void)
{
    if (s_Log.writeF == 0)
    {
        return LOG_OK;
    }

    s_Log.BufPos += BYTE_ALIGN(s_Log.BufPos); // ((4 - (s_Log.BufPos & 0x0003)) & 0x03);
    int ret      = _mk_log_flash_Write((const uint8_t *)s_Log.wBuff, s_Log.BufPos);
    s_Log.BufPos = 0;
    return ret;
}

/**
 * \brief 读取一段flash日志
 * \param[in] index 读取的日志索引, 取值范围 0 至 total size-1
 * \param[out] pBuf 读取的日志存放地址
 * \param[in] size 要读取的日志大小
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_ReadOffset(uint32_t offset, uint8_t *pBuf, uint16_t size)
{
    uint8_t  read_buf[4] = {0};
    uint32_t read_size   = 0;
    uint32_t used_size   = mk_log_flash_GetUsedSize();

    if (offset + size > used_size)
    {
        return LOG_ERROR;
    }

    if (size & 0x0003)
    {
        read_size = size - (size & 0x0003);
        int ret   = _mk_log_flash_read(offset, pBuf, read_size);
        if (ret != LOG_OK)
        {
            return LOG_ERROR;
        }

        ret = _mk_log_flash_read(offset + read_size, read_buf, (size & 0x0003));
        if (ret != LOG_OK)
        {
            return LOG_ERROR;
        }

        memcpy(pBuf + read_size, read_buf, (size & 0x0003));
        return LOG_OK;
    }
    else
    {
        return _mk_log_flash_read(offset, pBuf, size);
    }
}

/**
 * \brief 读取最近的flash日志
 * \param[out] pBuf 读取的flash日志存储地址
 * \param[in] size 要读取的日志大小
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_ReadRecent(uint8_t *pBuf, uint32_t size)
{
    uint32_t max_size = mk_log_flash_GetUsedSize();

    if (size == 0)
    {
        return LOG_ERROR;
    }

    if (size > max_size)
    {
        return LOG_ERROR;
    }

    return mk_log_flash_ReadOffset(max_size - size, pBuf, size);
}

/**
 * \brief 读取所有的flash日志
 * \param[out] pBuf 读取的flash日志存储地址
 * \retval 返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_ReadAll(uint8_t *pBuf)
{
    return mk_log_flash_ReadOffset(0, pBuf, mk_log_flash_GetUsedSize());
}

/**
 * \brief 清除所有flash日志
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_clean_all(void)
{
    s_Log.Index.Head = _LOG_FLASH_START_ADDR;
    s_Log.Index.Tail = _LOG_FLASH_START_ADDR;

    if (flash_log_erase(_LOG_FLASH_START_ADDR, _LOG_FLASH_PAGE_NUM * _LOG_FLASH_PAGE_SIZE) == 0)
    {
        return LOG_ERROR;
    }

    return LOG_OK;
}

/**
 * \brief FLASH LOG打印
 * \param[in] format 打印内容
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash(const char *format, ...)
{
    char     str_buf[_LOG_WRITE_ONCE_SIZE] = {0};
    uint16_t str_len                       = 0;
    uint16_t write_index                   = 0;
    uint16_t write_size                    = 0;

    uint16_t year = 0;
    uint8_t  month = 0, date = 0, hour = 0, min = 0, sec = 0;

    log_get_rtc(&year, &month, &date, &hour, &min, &sec);
    snprintf(str_buf, sizeof(str_buf), "[%04d-%02d-%02d %02d:%02d:%02d] ", year, month, date, hour, min, sec);

    str_len = strlen(str_buf);

    va_list arg;
    va_start(arg, format);
    vsnprintf(str_buf + str_len, _LOG_WRITE_ONCE_SIZE - 1 - str_len, format, arg);
    va_end(arg);

    str_len = strlen(str_buf);
    str_len += BYTE_ALIGN(str_len);  /**< 长度4位对齐 */
    if (str_len > _LOG_WRITE_ONCE_SIZE)
    {
        str_len = _LOG_WRITE_ONCE_SIZE;
    }

    if (_LOG_FLASH_BUF_SIZE > 0) /**< 缓存模式 */
    {
        while (s_Log.BufPos + str_len > _LOG_FLASH_BUF_SIZE)
        {
            write_size = _LOG_FLASH_BUF_SIZE - s_Log.BufPos;
            memcpy(s_Log.wBuff + s_Log.BufPos, str_buf + write_index, write_size);
            write_index += write_size;
            str_len -= write_size;
            s_Log.BufPos += write_size;
            mk_log_flash_Flush();
        }

        memcpy(s_Log.wBuff + s_Log.BufPos, str_buf + write_index, str_len);
        s_Log.BufPos += str_len;
        s_Log.writeF = 1;
    }
    else
    {
        return _mk_log_flash_Write((const uint8_t *)str_buf, str_len);
    }
    return LOG_OK;
}

/**
 * \brief FLASH LOG打印
 * \param[in] format 打印内容
 * \retval   返回执行结果，其值参考如下描述
 *  			        \arg 0  成功
 * 				        \arg -1 失败 
 */
int mk_log_flash_raw(const char *format, ...)
{
    char     str_buf[_LOG_WRITE_ONCE_SIZE] = {0};
    uint16_t str_len                       = 0;
    uint16_t write_index                   = 0;
    uint16_t write_size                    = 0;

    va_list arg;
    va_start(arg, format);
    vsnprintf(str_buf, _LOG_WRITE_ONCE_SIZE - 1, format, arg);
    va_end(arg);

    str_len = strlen(str_buf);
    str_len += BYTE_ALIGN(str_len); /**< 长度4位对齐 */
    if (str_len > _LOG_WRITE_ONCE_SIZE)
    {
        str_len = _LOG_WRITE_ONCE_SIZE;
    }

    if (_LOG_FLASH_BUF_SIZE > 0) /**< 缓存模式 */
    {
        while (s_Log.BufPos + str_len > _LOG_FLASH_BUF_SIZE)
        {
            write_size = _LOG_FLASH_BUF_SIZE - s_Log.BufPos;
            memcpy(s_Log.wBuff + s_Log.BufPos, str_buf + write_index, write_size);
            write_index += write_size;
            str_len -= write_size;
            s_Log.BufPos += write_size;
            mk_log_flash_Flush();
        }

        memcpy(s_Log.wBuff + s_Log.BufPos, str_buf + write_index, str_len);
        s_Log.BufPos += str_len;
        s_Log.writeF = 1;
    }
    else
    {
        return _mk_log_flash_Write((const uint8_t *)str_buf, str_len);
    }
    return LOG_OK;
}
#endif
