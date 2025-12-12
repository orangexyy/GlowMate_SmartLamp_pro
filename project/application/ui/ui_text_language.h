/**************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 : 
**  功能描述 : 
**  作    者 : 
**  日    期 :  
**  版    本 :  
**  变更记录 :  
\**************************************************************************/

#ifndef UI_TEXT_LANGUAGE_H
#define UI_TEXT_LANGUAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************\
                               Includes
\**************************************************************************/
#include "lvgl.h"
#include <string.h>
/**************************************************************************\
                            Macro definitions
\**************************************************************************/

#ifndef NULL
    #define NULL                0U
#endif

/**************************************************************************\
                            Typedef definitions
\**************************************************************************/

// // 语言类型定义
// typedef enum
// {
//     LANG_TRADITIONAL_CHINESE=0,   // 繁体中文
//     LANG_CHINESE,               // 中文
//     LANG_ENGLISH,               // 英文
//     LANG_GERMAN,                // 德语
//     LANG_SPANISH,               // 西班牙语
//     LANG_FRENCH,                // 法语
//     LANG_ITALIAN,               // 意大利语
//     LANG_JAPANESE,              // 日语
//     LANG_KOREAN,                // 韩语

//     LANG_MAX,
//     // ... 其他语言
// } language_t;

// 语言定义
typedef enum {
    TE_UI_TEXT_LANGUAGE_CHINESE = 0,
    TE_UI_TEXT_LANGUAGE_ENGLISH,
    TE_UI_TEXT_LANGUAGE_KOREAN,
    TE_UI_TEXT_LANGUAGE_FRENCH
} TE_UI_TEXT_LANGUAGE;

// 语言信息结构体
typedef struct {

    const char *string1;
    const char *string2;
    const char *string3;

    const char *auto_mode;
    const char *error_mode;
    const char *idle_mode;
    const char *work_mode;
    // ... 其他内容

} TS_UI_TEXT_LANGUAGE_DATA;






/**************************************************************************\
                         Global variables definitions
\**************************************************************************/

/**************************************************************************\
                         Global functions definitions
\**************************************************************************/

/*
* 初始化语言
* @return 无返回值
*/
void ui_text_language_init(void);

/*
* 获取当前语言
* @return 当前语言
*/
TE_UI_TEXT_LANGUAGE ui_text_get_current_language(void);

/*
* 设置当前语言
* @param language 语言
* @return 无返回值
*/
void ui_text_set_current_language(TE_UI_TEXT_LANGUAGE lang);

/*
* 获取指定语言的字符串
* @param key 字符串键值
* @return 字符串
*/
const char *ui_text_get_string_for_language(const char *key);



#ifdef __cplusplus
}
#endif

#endif
/**************************** End of File (H) ****************************/

