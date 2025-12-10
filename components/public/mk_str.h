#ifndef MK_STR_H
#define MK_STR_H
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 标签值结构体
 */
typedef struct
{
    char  tag;    /**< 标签字符 */
    float value;  /**< 对应的数值 */
} TAG_VALUE_STRUCT;

/**
 * @brief 获取指定字符在字符串中的位置
 * @param str 输入字符串
 * @param len 字符串长度
 * @param start_index 开始搜索的位置
 * @param c 要查找的字符
 * @return 字符在字符串中的位置，如果未找到返回-1
 */
int32_t mk_str_get_char_index(char *str, uint32_t len, uint32_t start_index, char c);

/**
 * @brief 获取指定标签后面的数值
 * @param str 输入字符串
 * @param len 字符串长度
 * @param start_index 开始搜索的位置
 * @param tag 要查找的标签
 * @param value 用于存储找到的数值
 * @return 是否成功找到并解析数值
 */
bool mk_str_get_value_by_tag(char *str, uint32_t len, uint32_t start_index, char tag, float *value);

/**
 * @brief 获取字符串中所有标签及对应的数值
 * @param str 输入字符串
 * @param len 字符串长度
 * @param start_index 开始搜索的位置
 * @param tag 用于存储标签值结构体的数组
 * @param tag_max 数组最大容量
 * @return 找到的标签值对的数量
 */
int32_t mk_str_get_tag_value_all(char *str, uint32_t len, uint32_t start_index, TAG_VALUE_STRUCT *tag, uint32_t tag_max);

/**
 * @brief 获取指定标签的结束位置
 * @param str 输入字符串
 * @param len 字符串长度
 * @param start_index 开始搜索的位置
 * @param tag 要查找的标签
 * @return 标签结束位置，如果未找到返回-1
 */
int32_t mk_str_get_tag_end_index(char *str, uint32_t len, uint32_t start_index, char tag);

/**
 * @brief 获取数字的结束位置
 * @param str 输入字符串
 * @param len 字符串长度
 * @param start_index 开始搜索的位置
 * @return 数字结束位置，如果未找到返回-1
 */
int32_t mk_str_digit_end_index(char *str, uint32_t len, uint32_t start_index);

/**
 * @brief 获取字符串中字母的数量
 * @param str 输入字符串
 * @param len 字符串长度
 * @param start_index 开始搜索的位置
 * @return 字母的数量
 */
int32_t mk_str_get_letter_amount(char *str, uint32_t len, uint32_t start_index);

/**
 * @brief 查找从指定位置开始首个字母的位置
 * @param str 输入字符串
 * @param len 字符串长度
 * @param start_index 开始搜索的位置
 * @return 首个字母的位置，如果未找到返回-1
 */
int32_t mk_str_find_first_letter_index(char *str, uint32_t len, uint32_t start_index);

/**
 * @brief 获取命令后面的字符串内容
 * @param str 输入字符串
 * @param len 字符串长度
 * @param content_buf 用于存储内容的缓冲区
 * @param content_buf_len 缓冲区长度
 * @return 内容长度，如果解析失败返回-1
 */
int32_t mk_str_get_string_content(char *str, uint32_t len, char *content_buf, uint32_t content_buf_len);

/**
 * @brief 打印字符串内容（调试用）
 * @param str 要打印的字符串
 * @param len 字符串长度
 */
void mk_str_printf(char *str, uint32_t len);

#endif
