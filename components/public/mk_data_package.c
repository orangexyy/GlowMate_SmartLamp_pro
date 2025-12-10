/******************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  mk_data_package.cpp
**  功能描述 :  数据打包
**  作    者 :  王滨泉
**  日    期 :  2022.06.28
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2022.06.28
                1 首次创建
\******************************************************************************/

/******************************************************************************\
                               Includes
\******************************************************************************/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mk_data_package.h"
#include "public/mk_log.h"
#include "modules/motion/com_usart.h"
/******************************************************************************\
                            Macro definitions
\******************************************************************************/
#define CHECK_PROF_BUF_LEN(obj, len)                                                             \
    do                                                                                           \
    {                                                                                            \
        if (obj->buf_len + len > obj->buf_size)                                                  \
        {                                                                                        \
            USR_LOG_INFO("[OBJ LEN] Exceeds max size(%d), at line %d", obj->buf_size, __LINE__); \
            return -1;                                                                           \
        }                                                                                        \
    } while (0)

/******************************************************************************\
                            Typedef definitions
\******************************************************************************/

/******************************************************************************\
                            Variables definitions
\******************************************************************************/

/******************************************************************************\
                            Functions definitions
\******************************************************************************/

/* 
* 函数名称 : mk_object_init
* 功能描述 : 字符转换资源初始化
* 参	数 : max_size - 最大长度
* 返回值   : 数据目标句柄
*/
/******************************************************************************/
TS_OBJECT *mk_object_init(int max_size)
/******************************************************************************/
{
    TS_OBJECT *obj = NULL;

    if (max_size <= 0)
    {
        return obj;
    }

    obj = (TS_OBJECT *)malloc(sizeof(TS_OBJECT));
    if (obj == NULL)
    {
        USR_LOG_INFO("object malloc fail");
        return obj;
    }

    obj->buf = (char *)malloc(max_size);
    if (obj->buf == NULL)
    {
        USR_LOG_INFO("object malloc fail");
        free(obj);
        return NULL;
    }

    obj->buf_size = max_size;
    obj->buf_len  = 0;

    return obj;
}
/* 
* 函数名称 : mk_object_deinit
* 功能描述 : 字符资源释放
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 无
*/
/******************************************************************************/
void mk_object_deinit(TS_OBJECT *obj)
/******************************************************************************/
{
    if (obj)
    {
        if (obj->buf)
        {
            free(obj->buf);
        }
        free(obj);
    }
}

/* 
* 函数名称 : get_cur_obj_buf
* 功能描述 : 获取当前指针
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 当前指针
*/
/******************************************************************************/
char *get_cur_obj_buf(TS_OBJECT *obj)
/******************************************************************************/
{
    return (obj == NULL || obj->buf == NULL) ? NULL : obj->buf + obj->buf_len;
}
/* 
* 函数名称 : add_cur_json_obj_buf
* 功能描述 : 当前使用buf长度累加
* 参	数 : obj - 数据目标句柄指针
             len - buf长度
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int add_cur_obj_buf(TS_OBJECT *obj, int len)
/******************************************************************************/
{
    if (obj == NULL || obj->buf == NULL)
    {
        return -1;
    }

    CHECK_PROF_BUF_LEN(obj, len);
    obj->buf_len += len;

    return 0;
}
/* 
* 函数名称 : set_symbol
* 功能描述 : 设置特定符号
* 参	数 : obj - 数据目标句柄指针
             symbol - 符号
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_symbol(TS_OBJECT *obj, char symbol)
/******************************************************************************/
{
    if (obj == NULL || obj->buf == NULL)
    {
        return -1;
    }

    CHECK_PROF_BUF_LEN(obj, 1);
    obj->buf[obj->buf_len++] = symbol;

    return 0;
}
/* 
* 函数名称 : set_obj_start
* 功能描述 : 设置起始符号
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_obj_start(TS_OBJECT *obj)
/******************************************************************************/
{
    return set_symbol(obj, '{');
}
/* 
* 函数名称 : set_obj_end
* 功能描述 : 设置结束符号
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_obj_end(TS_OBJECT *obj)
/******************************************************************************/
{
    return set_symbol(obj, '}');
}
/* 
* 函数名称 : set_struct_start
* 功能描述 : 设置子成员开始符号
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_struct_start(TS_OBJECT *obj)
/******************************************************************************/
{
    return set_symbol(obj, '[');
}
/* 
* 函数名称 : set_struct_end
* 功能描述 : 设置子成员结束符号
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_struct_end(TS_OBJECT *obj)
/******************************************************************************/
{
    return set_symbol(obj, ']');
}
/* 
* 函数名称 : set_comma
* 功能描述 : 设置逗号
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_comma(TS_OBJECT *obj)
/******************************************************************************/
{
    return set_symbol(obj, ',');
}
/* 
* 函数名称 : set_tab
* 功能描述 : 设置tab
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_tab(TS_OBJECT *obj)
/******************************************************************************/
{
    return set_symbol(obj, '\t');
}

/* 
* 函数名称 : set_line_break
* 功能描述 : 设置换行符
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_line_break(TS_OBJECT *obj)
/******************************************************************************/
{
    return set_symbol(obj, '\n');
}

/* 
* 函数名称 : set_quotation_mark
* 功能描述 : 设置双引号
* 参	数 : obj - json句柄指针
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_quotation_mark(TS_OBJECT *obj)
/******************************************************************************/
{
    return set_symbol(obj, '"');
}

/* 
* 函数名称 : set_objs
* 功能描述 : 设置json成员字符串
* 参	数 : obj - json句柄指针
             name - 名称
             value - 数据
             type - 类型
* 返回值   : 0 - 设置成功
            -1 - 设置出错
*/
/******************************************************************************/
int set_objs(TS_OBJECT *obj, const char *name, void *value, TE_OBJECT_TYPE type)
/******************************************************************************/
{
    if (obj == NULL || obj->buf == NULL)
    {
        return -1;
    }

    if (name == NULL)
    {
        return -1;
    }

    CHECK_PROF_BUF_LEN(obj, 3 + strlen(name));
    sprintf(obj->buf + obj->buf_len, "\"%s\":", name);
    obj->buf_len += 3 + strlen(name);
    if (value != NULL)
    {
        if (type <= OBJ_TYPE_INT2STR)
        {
            char int2str[12] = {0};
            if (type == OBJ_TYPE_INT)
            {
                sprintf(int2str, "%d", *(int *)value);
            }
            else if (type == OBJ_TYPE_UINT)
            {
                sprintf(int2str, "%u", *(uint32_t *)value);
            }
            else if (type == OBJ_TYPE_FLOAT)
            {
                sprintf(int2str, "%f", *(float *)value);
            }
            else if (type == OBJ_TYPE_DOUBLE)
            {
                sprintf(int2str, "%g", *(double *)value);
            }
            else
            {
                sprintf(int2str, "\"%d\"", *(int *)value);
            }

            CHECK_PROF_BUF_LEN(obj, strlen(int2str));
            memcpy(obj->buf + obj->buf_len, int2str, strlen(int2str));
            obj->buf_len += strlen(int2str);
        }
        else if (type <= OBJ_TYPE_STRING)
        {
            if (type == OBJ_TYPE_STRING &&
                *(const char *)value != '[' &&
                *(const char *)value != '{')
            {
                CHECK_PROF_BUF_LEN(obj, 2 + strlen((const char *)value));
                sprintf(obj->buf + obj->buf_len, "\"%s\"", (const char *)value);
                obj->buf_len += 2 + strlen((const char *)value);
            }
            else
            {
                CHECK_PROF_BUF_LEN(obj, strlen((const char *)value));
                sprintf(obj->buf + obj->buf_len, "%s", (const char *)value);
                obj->buf_len += strlen((const char *)value);
            }
        }
    }

    return 0;
}
/* 
* 函数名称 : objs_show
* 功能描述 : 数据目标打印
* 参	数 : obj - 数据目标句柄指针
* 返回值   : 无
*/
/******************************************************************************/
void objs_show(TS_OBJECT *obj)
/******************************************************************************/
{
    if (obj == NULL || obj->buf == NULL)
    {
        return;
    }

    obj->buf[obj->buf_len] = '\0';
    printf("\r\n[%d]\r\n%s\r\n", obj->buf_len, obj->buf);
}
/* 
* 函数名称 : json_show_by_len
* 功能描述 : 打印json格式数据
* 参	数 : json - 数据指针
             len - 数据长度
* 返回值   : 无
*/
/******************************************************************************/
void json_show_by_len(const char *json, int len)
/******************************************************************************/
{
    int i;

    printf("\r\n");
    for (i = 0; json != NULL && i < len; i++)
    {
        printf("%c", json[i]);
        if (json[i] == '{' || json[i] == '}' ||
            json[i] == '[' || json[i] == ']' ||
            json[i] == ',' || (i + 1 < len && json[i + 1] == '}'))
        {
            printf("\r\n");
        }
    }

    printf("\r\n");
}
//------------------------------------------------------------------------------
/******************************* End of File (C) ******************************/