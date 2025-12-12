#include "ui_text_language.h"


/*
      "繁體中文", "中文", "English", "Deutsch", "Español", "Français", "Italiano", "Nihongo", "한국어"
       繁体中文,    中文，  英文，     德语，      西班牙语，   法语，       意大利语， 日语，      韩语
缩写    zh          zh     en         de         es         fr          it          ja         ko
英文字符 限定0x20-0x7f
注意德语/法语中存在特殊字符需要在生成字库中限定0xa0-0xff
中文/日语/韩语直接在symbols中加入特定字符即可
*/

// 中文语言数据
static const TS_UI_TEXT_LANGUAGE_DATA s_language_chinese = {
    
    .string1 = "字符串1",
    .string2 = "字符串2",
    .string3 = "字符串3",
    // ... 其他字符串
};

// 英语语言数据
static const TS_UI_TEXT_LANGUAGE_DATA s_language_english = {
    
    .string1 = "abc1",
    .string2 = "abc2",
    .string3 = "abc3",
    .work_mode = "work",
    .error_mode = "error",
    .idle_mode = "idle",
    .auto_mode = "auto",

    // ... 其他字符串
};

// 法语数据
static const TS_UI_TEXT_LANGUAGE_DATA s_language_fr = {
    
    .string1 = "string1",
    .string2 = "string2",
    .string3 = "string3",
    // ... 其他字符串
};

// 韩语数据
static const TS_UI_TEXT_LANGUAGE_DATA s_language_ko = {
    
    .string1 = "string1",
    .string2 = "string2",
    .string3 = "string3",
    // ... 其他字符串
};

// 根据语言类型获取语言数据的函数指针数组
static const TS_UI_TEXT_LANGUAGE_DATA *const p_languages_data[] = {
    [TE_UI_TEXT_LANGUAGE_CHINESE] = &s_language_chinese,
    [TE_UI_TEXT_LANGUAGE_ENGLISH] = &s_language_english,
    [TE_UI_TEXT_LANGUAGE_FRENCH]  = &s_language_fr,
    [TE_UI_TEXT_LANGUAGE_KOREAN]  = &s_language_ko,
    //... 其他语言数据
};


TE_UI_TEXT_LANGUAGE s_current_language = TE_UI_TEXT_LANGUAGE_ENGLISH; //全局当前语言


/*
* 初始化语言
* @return 无返回值
*/
void ui_text_language_init(void)
{
	s_current_language = TE_UI_TEXT_LANGUAGE_ENGLISH;
}


/*
* 获取指定语言的字符串
* @param key 字符串键值
* @return 字符串
*/
const char *ui_text_get_string_for_language(const char *key)
{
    const TS_UI_TEXT_LANGUAGE_DATA *p_current_languages_data = p_languages_data[s_current_language];
    if (p_current_languages_data == NULL)
    {
        return NULL; // 未找到对应语言或未初始化
    }
    if (key == NULL)
    {
        return NULL; // 未指定字符串键值
    }
    if (strcmp(key, "string1") == 0)
    {
        return p_current_languages_data->string1;
    }
    else if (strcmp(key, "string2") == 0)
    {
        return p_current_languages_data->string2;
    }
    else if (strcmp(key, "string3") == 0)
    {
        return p_current_languages_data->string3;
    }
    else if (strcmp(key, "work_mode") == 0)
    {
        return p_current_languages_data->work_mode;
    }
    else if (strcmp(key, "error_mode") == 0)
    {
        return p_current_languages_data->error_mode;
    }
    else if (strcmp(key, "idle_mode") == 0)
    {
        return p_current_languages_data->idle_mode;
    }
    else if (strcmp(key, "auto_mode") == 0)
    {
        return p_current_languages_data->auto_mode;
    }
    return NULL; // 未找到对应字符串
}

/*
* 设置当前语言
* @param language 语言
* @return 无返回值
*/
void ui_text_set_current_language(TE_UI_TEXT_LANGUAGE language)
{
    s_current_language = language;
}

/*
* 获取当前语言
* @return 当前语言
*/
TE_UI_TEXT_LANGUAGE ui_text_get_current_language(void)
{
    return s_current_language;
}





    


