// 模块功能：字符串库文件，统一实现字符串的变换操作
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mk_str.h"
#include "math_lib.h"

// 查找字符的在字符串的位置
int32_t mk_str_get_char_index(char *str, uint32_t len, uint32_t start_index, char c)
{
    uint32_t i = 0;

    for (i = start_index; i < len; i++)
    {
        if (str[i] == c)
        {
            return i;
        }
    }

    return -1;
}

// 获取标签后面的数字
bool mk_str_get_value_by_tag(char *str, uint32_t len, uint32_t start_index, char tag, float *value)
{
    bool     flag = false;
    int32_t  tag_index;
    uint32_t char_counter;

    tag_index = mk_str_get_char_index(str, len, start_index, tag);
    if (tag_index != -1)
    {
        char_counter = tag_index + 1;
        math_read_float(str, &char_counter, value);
        // *value = strtod(str+tag_index+1, NULL);
        // printf("value:%f\n", *value);
        flag = true;
    }

    return flag;
}

int32_t mk_str_get_tag_value_all(char *str, uint32_t len, uint32_t start_index, TAG_VALUE_STRUCT *tag, uint32_t tag_max)
{
    uint32_t i;
    char     data  = 0;
    uint8_t  index = 0;
    uint32_t char_counter;

    for (i = start_index; i < len; i++)
    {
        data = *(str + i);
        if ((('a' <= data) && (data <= 'z')) ||
            (('A' <= data) && (data <= 'Z')))
        {
            tag[index].tag = data;
            char_counter   = i + 1;
            math_read_float(str, &char_counter, &tag[index].value);
            // tag[index].value = strtod(str + i + 1, NULL);
            index++;
            if (index >= tag_max)
            {
                break;
            }
        }
    }

    return index;
}

int32_t mk_str_get_tag_end_index(char *str, uint32_t len, uint32_t start_index, char tag)
{
    int index;
    int tag_index;

    tag_index = mk_str_get_char_index(str, len, start_index, tag);
    if (tag_index != -1)
    {
        index = mk_str_digit_end_index(str, len, tag_index + 1);
    }
    else
    {
        index = -1;
    }

    return index;
}

// 获取数字在字符串的结束位置
int32_t mk_str_digit_end_index(char *str, uint32_t len, uint32_t start_index)
{
    uint32_t end_index;
    char    *end_ptr;

    strtod(str + start_index, &end_ptr);
    end_index = end_ptr - str;

    return end_index;
}

int32_t mk_str_get_letter_amount(char *str, uint32_t len, uint32_t start_index)
{
    uint32_t num  = 0;
    uint32_t i    = 0;
    char     data = 0;

    for (i = start_index; i < len; i++)
    {
        data = *(str + i);
        if ((('a' <= data) && (data <= 'z')) ||
            (('A' <= data) && (data <= 'Z')))
        {
            num++;
        }
    }

    return num;
}

int32_t mk_str_find_first_letter_index(char *str, uint32_t len, uint32_t start_index)
{
    int32_t  index = -1;
    uint32_t i     = 0;
    char     data  = 0;

    for (i = start_index; i < len; i++)
    {
        data = *(str + i);
        if ((('a' <= data) && (data <= 'z')) ||
            (('A' <= data) && (data <= 'Z')))
        {
            index = i;
            break;
        }
    }

    return index;
}

int32_t mk_str_get_string_content(char *str, uint32_t len, char *content_buf, uint32_t content_buf_len)
{
    uint32_t i               = 0;
    int32_t  digit_end_index = -1;
    int32_t  actual_len      = 0;

    memset(content_buf, 0, content_buf_len);
    digit_end_index = mk_str_digit_end_index(str, len, 1);
    if (digit_end_index == -1)
    {
        return -1;
    }

    for (i = digit_end_index; i < len; i++)
    {
        // 跳过内容前面的空格
        if (str[i] != ' ')
        {
            break;
        }
    }

    actual_len = len - i;
    if (actual_len > content_buf_len)
    {
        actual_len = content_buf_len;
    }
    for (i = 0; i < actual_len; i++)
    {
        content_buf[i] = str[len - actual_len + i];
    }

    //去掉最后的换行符，是不是有副作用还需要注意
    if (content_buf[actual_len - 1] == '\n')
    {
        content_buf[actual_len - 1] = 0;
        actual_len                  = actual_len - 1;
    }

    return actual_len;
}

void mk_str_printf(char *str, uint32_t len)
{
    uint32_t i = 0;

    for (i = 0; i < len; i++)
    {
        printf("%c", str[i]);
    }
}
