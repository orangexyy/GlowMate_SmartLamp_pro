#include "byte_buffer.h"
/**
 * @file    byte_buffer.c
 * @brief   字节缓冲区操作实现，用于协议数据的格式转换
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2017/09/14
 * 
 * @copyright (C) 2012-2017, MakeBlock
 * 
 * @history
 *   - 2017/09/14 hawis.qin 创建文件
 */

union{
  uint8_t byteVal[8];
  double doubleVal;
}val8bytes;

union{
    uint8_t byteVal[4];
    float floatVal;
    long longVal;
}val4bytes;

union{
  uint8_t byteVal[2];
  short shortVal;
}val2bytes;

union{
  uint8_t byteVal[1];
  uint8_t charVal;
}val1bytes;

/**
 * @brief   初始化字节缓冲区
 * @param   byte_buffer   [out] 字节缓冲区结构体指针
 * @param   buf          [in]  数据缓冲区指针
 * @param   buf_size     [in]  缓冲区大小
 * @param   start_index  [in]  起始索引
 * @return  void
 */
void byte_buffer_init(BYTE_BUFFER* byte_buffer, uint8_t* buf, uint32_t buf_size, uint32_t start_index)
{
    byte_buffer->buf = buf;
    byte_buffer->size = buf_size;
    byte_buffer->index = start_index;
}

/**
 * @brief   读取8位字节数据（范围0~127）
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [out] 读取的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_read_byte_8(BYTE_BUFFER* byte_buffer, uint8_t* data)
{
    uint32_t idx;

    idx = byte_buffer->index;
    val1bytes.byteVal[0] = byte_buffer->buf[idx] & 0x7f;

    byte_buffer->index = idx + 1;

    *data = val1bytes.charVal;

    return true;
}

/**
 * @brief   读取16位字节数据（范围0~255）
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [out] 读取的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_read_byte_16(BYTE_BUFFER* byte_buffer, uint8_t* data)
{
    uint8_t temp;
    uint32_t idx;

    idx = byte_buffer->index;
    val1bytes.byteVal[0] = byte_buffer->buf[idx] & 0x7f;
    temp = byte_buffer->buf[idx+1] << 7;
    val1bytes.byteVal[0] |= temp;

    byte_buffer->index = idx + 2;

    *data = val1bytes.charVal;

    return true;
}

/**
 * @brief   读取16位short数据（范围0~16383）
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [out] 读取的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_read_short_16(BYTE_BUFFER* byte_buffer, short* data)
{
    uint8_t temp;
    uint32_t idx;

    idx = byte_buffer->index;
    val2bytes.byteVal[0] = byte_buffer->buf[idx] & 0x7f;
    temp = byte_buffer->buf[idx+1] << 7;
    val2bytes.byteVal[0] |= temp;

    val2bytes.byteVal[1] = (byte_buffer->buf[idx+1] >> 1) & 0x7f;

    byte_buffer->index = idx + 2;

    *data = val2bytes.shortVal;

    return true;
}

/**
 * @brief   读取24位short数据（范围-32767~32767）
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [out] 读取的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_read_short_24(BYTE_BUFFER* byte_buffer, short* data)
{  
    uint8_t temp;
    uint32_t idx;

    idx = byte_buffer->index;
    val2bytes.byteVal[0] = byte_buffer->buf[idx] & 0x7f;
    temp = byte_buffer->buf[idx+1] << 7;
    val2bytes.byteVal[0] |= temp;

    val2bytes.byteVal[1] = (byte_buffer->buf[idx+1] >> 1) & 0x7f;

    temp = (byte_buffer->buf[idx+2] << 6);
    val2bytes.byteVal[1] |= temp;

    byte_buffer->index = idx + 3;

    *data = val2bytes.shortVal;

    return true;
}

/**
 * @brief   读取long类型数据
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [out] 读取的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_read_long(BYTE_BUFFER* byte_buffer, long* data)
{
    uint8_t temp;
    uint32_t idx;

    idx = byte_buffer->index;
    val4bytes.byteVal[0] = byte_buffer->buf[idx] & 0x7f;
    temp = byte_buffer->buf[idx+1] << 7;
    val4bytes.byteVal[0] |= temp;

    val4bytes.byteVal[1] =  (byte_buffer->buf[idx+1] >> 1) & 0x7f;
    temp = (byte_buffer->buf[idx+2] << 6);
    val4bytes.byteVal[1] += temp;

    val4bytes.byteVal[2] =  (byte_buffer->buf[idx+2] >> 2) & 0x7f;
    temp = (byte_buffer->buf[idx+3] << 5);
    val4bytes.byteVal[2] += temp;

    val4bytes.byteVal[3] =  (byte_buffer->buf[idx+3] >> 3) & 0x7f;
    temp = (byte_buffer->buf[idx+4] << 4);
    val4bytes.byteVal[3] += temp;

    byte_buffer->index = idx + 5;

    *data = val4bytes.longVal;

    return true;
}

/**
 * @brief   读取float类型数据
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [out] 读取的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_read_float(BYTE_BUFFER* byte_buffer, float* data)
{
    uint8_t temp;
    uint32_t idx;

    idx = byte_buffer->index;
    val4bytes.byteVal[0] = byte_buffer->buf[idx] & 0x7f;
    temp = byte_buffer->buf[idx+1] << 7;
    val4bytes.byteVal[0] |= temp;

    val4bytes.byteVal[1] =  (byte_buffer->buf[idx+1] >> 1) & 0x7f;
    temp = (byte_buffer->buf[idx+2] << 6);
    val4bytes.byteVal[1] += temp;

    val4bytes.byteVal[2] =  (byte_buffer->buf[idx+2] >> 2) & 0x7f;
    temp = (byte_buffer->buf[idx+3] << 5);
    val4bytes.byteVal[2] += temp;

    val4bytes.byteVal[3] =  (byte_buffer->buf[idx+3] >> 3) & 0x7f;
    temp = (byte_buffer->buf[idx+4] << 4);
    val4bytes.byteVal[3] += temp;

    byte_buffer->index = idx + 5;

    *data = val4bytes.floatVal;

    return true;
}

/**
 * @brief   读取数据缓冲区
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data_size   [in]  要读取的数据长度
 * @param   data_buf    [out] 数据缓冲区指针
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_read_buf(BYTE_BUFFER* byte_buffer, uint32_t data_size, uint8_t* data_buf)
{
    //	uint8_t temp;
    uint32_t idx;
    uint32_t i;

    idx = byte_buffer->index;

    for (i = 0; i < data_size; i++)
    {
    data_buf[i] = byte_buffer->buf[idx + i] & 0x7f;
    }

    byte_buffer->index = idx + data_size;

    return true;
}

/**
 * @brief   写入8位字节数据（范围0~127）
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [in]  要写入的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_write_byte_8(BYTE_BUFFER* byte_buffer, uint8_t data)
{  
    uint32_t idx;

    idx = byte_buffer->index;
    byte_buffer->buf[idx] = data & 0x7f;

    byte_buffer->index = idx + 1;

    return true;
}

/**
 * @brief   写入16位字节数据（范围0~255）
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [in]  要写入的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_write_byte_16(BYTE_BUFFER* byte_buffer, uint8_t data)
{
    uint8_t val_7bit[2]={0};
    uint32_t idx;

    idx = byte_buffer->index;
    val1bytes.charVal = data;

    val_7bit[0] = val1bytes.byteVal[0] & 0x7f;
    byte_buffer->buf[idx] = val_7bit[0];

    val_7bit[1] = (val1bytes.byteVal[0] >> 7) & 0x7f;
    byte_buffer->buf[idx+1] = val_7bit[1];

    byte_buffer->index = idx + 2;  

    return true;
}

/**
 * @brief   写入short类型数据，范围-16383~16383
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [in]  要写入的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_write_short_16(BYTE_BUFFER* byte_buffer, short data)
{
    uint8_t val_7bit[2]={0};
    uint32_t idx;

    idx = byte_buffer->index;
    val2bytes.shortVal = data;

    val_7bit[0] = val2bytes.byteVal[0] & 0x7f;
    byte_buffer->buf[idx] = val_7bit[0];

    val_7bit[1] = ((val2bytes.byteVal[1] << 1) | (val2bytes.byteVal[0] >> 7)) & 0x7f;
    byte_buffer->buf[idx+1] = val_7bit[1];

    byte_buffer->index = idx + 2;

    return true;
}

/**
 * @brief   写入short类型数据，范围-32767~32767
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [in]  要写入的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_write_short_24(BYTE_BUFFER* byte_buffer, short data)
{
    uint8_t val_7bit[3]={0};
    uint32_t idx;

    idx = byte_buffer->index;
    val2bytes.shortVal = data;
    val_7bit[0] = val2bytes.byteVal[0] & 0x7f;
    byte_buffer->buf[idx] = val_7bit[0];

    val_7bit[1] = ((val2bytes.byteVal[1] << 1) | (val2bytes.byteVal[0] >> 7)) & 0x7f;
    byte_buffer->buf[idx+1] = val_7bit[1];

    val_7bit[2] = (val2bytes.byteVal[1] >> 6) & 0x7f;
    byte_buffer->buf[idx+2] = val_7bit[2];

    byte_buffer->index = idx + 3;

    return true;
}

/**
 * @brief   写入long类型数据
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [in]  要写入的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_write_long(BYTE_BUFFER* byte_buffer, long data)
{
    uint8_t val_7bit[5]={0};
    uint32_t idx;

    idx = byte_buffer->index;
    val4bytes.longVal = data;

    val_7bit[0] = val4bytes.byteVal[0] & 0x7f;
    byte_buffer->buf[idx] = val_7bit[0];

    val_7bit[1] = ((val4bytes.byteVal[1] << 1) | (val4bytes.byteVal[0] >> 7)) & 0x7f;
    byte_buffer->buf[idx+1] = val_7bit[1];

    val_7bit[2] = ((val4bytes.byteVal[2] << 2) | (val4bytes.byteVal[1] >> 6)) & 0x7f;
    byte_buffer->buf[idx+2] = val_7bit[2];

    val_7bit[3] = ((val4bytes.byteVal[3] << 3) | (val4bytes.byteVal[2] >> 5)) & 0x7f;
    byte_buffer->buf[idx+3] = val_7bit[3];

    val_7bit[4] = (val4bytes.byteVal[3] >> 4) & 0x7f;
    byte_buffer->buf[idx+4] = val_7bit[4];

    byte_buffer->index = idx + 5;

    return true;
}

/**
 * @brief   写入float类型数据
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data        [in]  要写入的数据
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_write_float(BYTE_BUFFER* byte_buffer, float data)
{
    uint8_t val_7bit[5]={0};
    uint32_t idx;

    idx = byte_buffer->index;
    val4bytes.floatVal = data;

    val_7bit[0] = val4bytes.byteVal[0] & 0x7f;
    byte_buffer->buf[idx] = val_7bit[0];

    val_7bit[1] = ((val4bytes.byteVal[1] << 1) | (val4bytes.byteVal[0] >> 7)) & 0x7f;
    byte_buffer->buf[idx+1] = val_7bit[1];

    val_7bit[2] = ((val4bytes.byteVal[2] << 2) | (val4bytes.byteVal[1] >> 6)) & 0x7f;
    byte_buffer->buf[idx+2] = val_7bit[2];

    val_7bit[3] = ((val4bytes.byteVal[3] << 3) | (val4bytes.byteVal[2] >> 5)) & 0x7f;
    byte_buffer->buf[idx+3] = val_7bit[3];

    val_7bit[4] = (val4bytes.byteVal[3] >> 4) & 0x7f;
    byte_buffer->buf[idx+4] = val_7bit[4];

    byte_buffer->index = idx + 5;

    return true;
}

/**
 * @brief   写入数据缓冲
 * @param   byte_buffer  [in]  字节缓冲区结构体指针
 * @param   data_size   [in]  要写入的数据长度
 * @param   data_buf    [in]  要写入的数据缓冲
 * @return  bool        true:数据有效 false:数据无效
 */
bool byte_buffer_write_buf(BYTE_BUFFER* byte_buffer, uint32_t data_size, uint8_t* data_buf)
{
    //	uint8_t val_7bit[5] = { 0 };
    uint32_t idx;
    uint32_t i = 0;

    idx = byte_buffer->index;

    for (i = 0; i < data_size; i++)
    {
    byte_buffer->buf[idx + i] = data_buf[i];
    }

    byte_buffer->index = idx + data_size;

    return true;
}
