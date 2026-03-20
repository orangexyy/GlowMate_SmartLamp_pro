/****************************************************************************\
**  文件名称 :  plugin_rgb.c
**  功能描述 :  RGB 灯带插件。WS2812B 灯珠 GRB 设置、全灯颜色/亮度、渐变与刷新接口。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "stm32f10x.h"
#include "sys.h"
#include "stdint.h"
#include "system.h"
#include "drv_rgb.h"
#include "plugin_rgb.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/

/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/

/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/**
 * \brief RGB初始化
 * \return 无
 */
void plugin_rgb_init(void)
{
    drv_rgb_init();
    plugin_rgb_clear_all();
}


/**
 * \brief 设置单个灯珠的 RGB 颜色（GRB 顺序，WS2812B）
 * \param pixel_idx 灯珠索引（0 ~ Led_Num-1）
 * \param g 绿色分量（0~255）
 * \param r 红色分量（0~255）
 * \param b 蓝色分量（0~255）
 */
void plugin_rgb_set_pixel_rgb(uint16_t pixel_idx, uint8_t g, uint8_t r, uint8_t b)
{
    // 检查灯珠索引是否超出范围
    if (pixel_idx >= DRV_RGB_LED_NUM)
    {
        return;
    }

    uint16_t start_idx = pixel_idx * 24;  // 该灯珠的起始数据索引
    uint8_t bit;

    // 填充绿色分量（8位，高位在前）
    for (bit = 0; bit < 8; bit++)
    {
        drv_rgb_value[start_idx + bit] = (g & (0x80 >> bit)) ? DRV_RGB_1_CODE : DRV_RGB_0_CODE;
    }

    // 填充红色分量（8位，高位在前）
    for (bit = 0; bit < 8; bit++)
    {
        drv_rgb_value[start_idx + 8 + bit] = (r & (0x80 >> bit)) ? DRV_RGB_1_CODE : DRV_RGB_0_CODE;
    }

    // 填充蓝色分量（8位，高位在前）
    for (bit = 0; bit < 8; bit++)
    {
        drv_rgb_value[start_idx + 16 + bit] = (b & (0x80 >> bit)) ? DRV_RGB_1_CODE : DRV_RGB_0_CODE;
    }
}

/**
 * @brief  设置单个灯珠的32位颜色值（GRB格式：0xGGRRBB）
 * @param  pixel_idx：灯珠索引（0 ~ Led_Num-1）
 * @param  color：颜色值（0x000000 ~ 0xFFFFFF，如0xFF0000为绿色）
 */
void plugin_rgb_set_pixel_color(uint16_t pixel_idx, uint32_t color)
{
    uint8_t g = (color >> 16) & 0xFF;  // 绿色分量（高8位）
    uint8_t r = (color >> 8) & 0xFF;   // 红色分量（中8位）
    uint8_t b = color & 0xFF;          // 蓝色分量（低8位）
    plugin_rgb_set_pixel_rgb(pixel_idx, g, r, b);
}

/**
 * @brief  设置所有灯珠为相同的RGB颜色
 * @param  g：绿色分量（0~255）
 * @param  r：红色分量（0~255）
 * @param  b：蓝色分量（0~255）
 */
void plugin_rgb_set_all_rgb(uint8_t g, uint8_t r, uint8_t b)
{
    uint16_t i;
    for (i = 0; i < DRV_RGB_LED_NUM; i++)
    {
        plugin_rgb_set_pixel_rgb(i, g, r, b);
    }
    // 显示所有灯珠（最后一个灯珠索引为Led_Num-1）
    drv_rgb_show(DRV_RGB_LED_NUM - 1);
}

/**
 * @brief  设置所有灯珠为相同的32位颜色值（GRB格式：0xGGRRBB）
 * @param  color：颜色值（0x000000 ~ 0xFFFFFF）
 */
void plugin_rgb_set_all_color(uint32_t color)
{
    uint16_t i;
    for (i = 0; i < DRV_RGB_LED_NUM; i++)
    {
        plugin_rgb_set_pixel_color(i, color);
    }
    // 显示所有灯珠
    drv_rgb_show(DRV_RGB_LED_NUM - 1);
}

/**
 * @brief  清除所有灯珠（设为黑色）
 */
void plugin_rgb_clear_all(void)
{
    plugin_rgb_set_all_color(PLUGIN_RGB_COLOR_BLACK);
}

/**
 * @brief  流水灯效果（单个颜色灯珠循环移动）
 * @param  color：灯珠颜色（0xGGRRBB）
 * @param  delay_ms_val：移动间隔（毫秒）
 */
void plugin_rgb_running_light(uint32_t color, uint16_t delay_ms_val)
{
    uint16_t i;
    for (i = 0; i < DRV_RGB_LED_NUM; i++)
    {
        // 清除所有灯珠
        plugin_rgb_clear_all();
        // 设置当前灯珠颜色
        plugin_rgb_set_pixel_color(i, color);
        // 显示当前灯珠
        drv_rgb_show(i);
        // 延时
        delay_ms(delay_ms_val);
    }
}

/**
 * @brief  渐变颜色效果（优化版：整数运算+范围限制，GRB顺序）
 * @param  color_start：起始颜色（0xGGRRBB格式，GRB）
 * @param  color_end：结束颜色（0xGGRRBB格式，GRB）
 * @param  step：渐变步数（越大越平滑）
 * @param  delay_ms_val：每步延时（毫秒）
 */
void plugin_rgb_gradient_color(uint32_t color_start, uint32_t color_end, uint16_t step, uint16_t delay_ms_val)
{
    // 提取GRB分量（0xGGRRBB）
    int32_t g_start = (color_start >> 16) & 0xFF;
    int32_t r_start = (color_start >> 8) & 0xFF;
    int32_t b_start = color_start & 0xFF;
    int32_t g_end = (color_end >> 16) & 0xFF;
    int32_t r_end = (color_end >> 8) & 0xFF;
    int32_t b_end = color_end & 0xFF;

    uint16_t i;
    for (i = 0; i <= step; i++)
    {
        // 整数运算+四舍五入
        int32_t g = ((g_end - g_start) * i + step / 2) / step + g_start;
        int32_t r = ((r_end - r_start) * i + step / 2) / step + r_start;
        int32_t b = ((b_end - b_start) * i + step / 2) / step + b_start;

        // 范围限制：防止溢出导致的色阶跳变
        g = g < 0 ? 0 : (g > 255 ? 255 : g);
        r = r < 0 ? 0 : (r > 255 ? 255 : r);
        b = b < 0 ? 0 : (b > 255 ? 255 : b);

        // GRB顺序设置颜色
        plugin_rgb_set_all_rgb((uint8_t)g, (uint8_t)r, (uint8_t)b);
        delay_ms(delay_ms_val);
    }
}

/**
 * @brief  RGB灯全颜色渐变（GRB顺序专用：单分量逐次渐变，根除同步突变）
 * @param  step：每段渐变的步数（越大越平滑，建议200）
 * @param  delay_ms_val：每步延时（建议5~10ms，帧率>20Hz）
 */
void plugin_rgb_gradient_all_color_node(uint16_t step, uint16_t delay_ms_val)
{
    // 定义GRB顺序的颜色节点（单分量变化，无同步反向）
    uint32_t color_red    = 0x00FF00; // 红（G=0, R=255, B=0）
    uint32_t color_orange = 0x40FF00; // 橙（G=64, R=255, B=0）
    uint32_t color_yellow = 0xFFEE00; // 黄（G=255, R=238, B=0）
    uint32_t color_green  = 0xFF0000; // 绿（G=255, R=0, B=0）
    uint32_t color_green1 = 0x800000; // 绿半降（G=128, R=0, B=0）→ 单分量G变化
    uint32_t color_cyan1  = 0x000080; // 青半升（G=0, R=0, B=128）→ 单分量B变化
    uint32_t color_cyan   = 0x0000FF; // 青（G=0, R=0, B=255）
    uint32_t color_blue1  = 0x000080; // 蓝半降（G=0, R=0, B=128）→ 单分量B变化
    uint32_t color_blue   = 0x4000FF; // 蓝（G=64, R=0, B=255）→ 单分量G变化
    uint32_t color_purple = 0x8000FF; // 紫（G=128, R=0, B=255）→ 单分量G变化
    uint32_t color_purple1= 0x008080; // 紫半降（G=0, R=128, B=128）→ 单分量R/B变化
    uint32_t color_red_back=0x00FF00; // 回红

    // 逐段渐变：每段仅一个/两个分量小幅度变化，无同步反向
    plugin_rgb_gradient_color(color_red,    color_orange, step, delay_ms_val);
    plugin_rgb_gradient_color(color_orange, color_yellow, step, delay_ms_val);
    plugin_rgb_gradient_color(color_yellow, color_green,  step, delay_ms_val);
    plugin_rgb_gradient_color(color_green,  color_green1, step, delay_ms_val); // G降
    plugin_rgb_gradient_color(color_green1, color_cyan1,  step, delay_ms_val); // B升
    plugin_rgb_gradient_color(color_cyan1,  color_cyan,   step, delay_ms_val); // B升
    plugin_rgb_gradient_color(color_cyan,   color_blue1,  step, delay_ms_val); // B降
    plugin_rgb_gradient_color(color_blue1,  color_blue,   step, delay_ms_val); // G升
    plugin_rgb_gradient_color(color_blue,   color_purple, step, delay_ms_val); // G升
    plugin_rgb_gradient_color(color_purple, color_purple1,step, delay_ms_val);// R/B变
    plugin_rgb_gradient_color(color_purple1,color_red_back,step,delay_ms_val);
}

/**
 * @brief  GRB顺序全颜色极致平滑渐变（无节点，单分量遍历）
 * @param  delay_ms_val：每步延时（5ms，帧率200Hz）
 */
void plugin_rgb_gradient_smooth_grb(uint16_t delay_ms_val)
{
    uint8_t g, r, b;

    // 红→黄：G从0→255，R=255，B=0（仅G变化）
    for (g = 0; g <= 255; g++)
    {
        plugin_rgb_set_all_rgb(g, 255, 0);
        delay_ms(delay_ms_val);
    }

    // 黄→绿：R从255→0，G=255，B=0（仅R变化）
    for (r = 255; r > 0; r--)
    {
        plugin_rgb_set_all_rgb(255, r, 0);
        delay_ms(delay_ms_val);
    }

    // 绿→青：G从255→0，B从0→255（拆分为两步，避免同步变化）
    for (g = 255; g > 0; g--)
    {
        plugin_rgb_set_all_rgb(g, 0, 0);
        delay_ms(delay_ms_val);
    }
    for (b = 0; b <= 255; b++)
    {
        plugin_rgb_set_all_rgb(0, 0, b);
        delay_ms(delay_ms_val);
    }

    // 青→蓝：G从0→64，B=255（仅G变化）
    for (g = 0; g <= 64; g++)
    {
        plugin_rgb_set_all_rgb(g, 0, 255);
        delay_ms(delay_ms_val);
    }

    // 蓝→紫：G从64→128，B=255（仅G变化）
    for (g = 64; g <= 128; g++)
    {
        plugin_rgb_set_all_rgb(g, 0, 255);
        delay_ms(delay_ms_val);
    }

    // 紫→红：G从128→0，R从0→255，B从255→0（拆分为单分量变化）
    for (r = 0; r <= 255; r++)
    {
        plugin_rgb_set_all_rgb(0, r, 255);
        delay_ms(delay_ms_val);
    }
    for (b = 255; b > 0; b--)
    {
        plugin_rgb_set_all_rgb(0, 255, b);
        delay_ms(delay_ms_val);
    }
}


/**
 * @brief  通过占空比设置单个灯珠的亮度（基于指定颜色）
 * @param  pixel_idx：灯珠索引（0 ~ Led_Num-1）
 * @param  color：基础颜色值（0xGGRRBB）
 * @param  duty_cycle：占空比（0~100，表示亮度百分比）
 * @note   占空比为0时灯珠熄灭，100时显示完整亮度
 */
void plugin_rgb_set_pixel_brightness(uint16_t pixel_idx, uint32_t color, uint8_t duty_cycle)
{
    // 检查灯珠索引是否超出范围
    if (pixel_idx >= DRV_RGB_LED_NUM)
    {
        return;
    }

    // 限制占空比范围
    if (duty_cycle > 100)
    {
        duty_cycle = 100;
    }

    // 提取颜色分量
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t r = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    // 根据占空比调整亮度（线性调整）
    g = (uint8_t)((g * duty_cycle) / 100);
    r = (uint8_t)((r * duty_cycle) / 100);
    b = (uint8_t)((b * duty_cycle) / 100);

    // 设置灯珠颜色
    plugin_rgb_set_pixel_rgb(pixel_idx, g, r, b);
}

/**
 * @brief  通过占空比设置所有灯珠的亮度（基于指定颜色）
 * @param  color：基础颜色值（0xGGRRBB）
 * @param  duty_cycle：占空比（0~100，表示亮度百分比）
 * @note   占空比为0时所有灯珠熄灭，100时显示完整亮度
 */
void plugin_rgb_set_all_brightness(uint32_t color, uint8_t duty_cycle)
{
    uint16_t i;
    for (i = 0; i < DRV_RGB_LED_NUM; i++)
    {
        plugin_rgb_set_pixel_brightness(i, color, duty_cycle);
    }
    // 显示所有灯珠
    drv_rgb_show(DRV_RGB_LED_NUM - 1);
}

/**
 * @brief  通过占空比设置单个灯珠的RGB亮度
 * @param  pixel_idx：灯珠索引（0 ~ Led_Num-1）
 * @param  g：绿色分量（0~255）
 * @param  r：红色分量（0~255）
 * @param  b：蓝色分量（0~255）
 * @param  duty_cycle：占空比（0~100，表示亮度百分比）
 * @note   占空比为0时灯珠熄灭，100时显示完整亮度
 */
void plugin_rgb_set_pixel_rgb_brightness(uint16_t pixel_idx, uint8_t g, uint8_t r, uint8_t b, uint8_t duty_cycle)
{
    // 检查灯珠索引是否超出范围
    if (pixel_idx >= DRV_RGB_LED_NUM)
    {
        return;
    }

    // 限制占空比范围
    if (duty_cycle > 100)
    {
        duty_cycle = 100;
    }

    // 根据占空比调整亮度（线性调整）
    g = (uint8_t)((g * duty_cycle) / 100);
    r = (uint8_t)((r * duty_cycle) / 100);
    b = (uint8_t)((b * duty_cycle) / 100);

    // 设置灯珠颜色
    plugin_rgb_set_pixel_rgb(pixel_idx, g, r, b);
}

/**
 * @brief  通过占空比设置所有灯珠的RGB亮度
 * @param  g：绿色分量（0~255）
 * @param  r：红色分量（0~255）
 * @param  b：蓝色分量（0~255）
 * @param  duty_cycle：占空比（0~100，表示亮度百分比）
 * @note   占空比为0时所有灯珠熄灭，100时显示完整亮度
 */
void plugin_rgb_set_all_rgb_brightness(uint8_t g, uint8_t r, uint8_t b, uint8_t duty_cycle)
{
    uint16_t i;
    for (i = 0; i < DRV_RGB_LED_NUM; i++)
    {
        plugin_rgb_set_pixel_rgb_brightness(i, g, r, b, duty_cycle);
    }
    // 显示所有灯珠
    drv_rgb_show(DRV_RGB_LED_NUM - 1);
}

/**
 * @brief  通过占空比调整已设置灯珠的亮度（不改变颜色）
 * @param  pixel_idx：灯珠索引（0 ~ Led_Num-1）
 * @param  duty_cycle：占空比（0~100，表示亮度百分比）
 * @note   此函数会读取当前灯珠的颜色，然后按占空比调整亮度
 */
void plugin_rgb_adjust_pixel_brightness(uint16_t pixel_idx, uint8_t duty_cycle)
{
    // 检查灯珠索引是否超出范围
    if (pixel_idx >= DRV_RGB_LED_NUM)
    {
        return;
    }

    // 限制占空比范围
    if (duty_cycle > 100)
    {
        duty_cycle = 100;
    }

    uint16_t start_idx = pixel_idx * 24;
    uint8_t g = 0, r = 0, b = 0;
    uint8_t bit;

    // 从缓冲区读取当前绿色分量
    for (bit = 0; bit < 8; bit++)
    {
        if (drv_rgb_value[start_idx + bit] == DRV_RGB_1_CODE)
        {
            g |= (0x80 >> bit);
        }
    }

    // 从缓冲区读取当前红色分量
    for (bit = 0; bit < 8; bit++)
    {
        if (drv_rgb_value[start_idx + 8 + bit] == DRV_RGB_1_CODE)
        {
            r |= (0x80 >> bit);
        }
    }

    // 从缓冲区读取当前蓝色分量
    for (bit = 0; bit < 8; bit++)
    {
        if (drv_rgb_value[start_idx + 16 + bit] == DRV_RGB_1_CODE)
        {
            b |= (0x80 >> bit);
        }
    }

    // 根据占空比调整亮度
    g = (uint8_t)((g * duty_cycle) / 100);
    r = (uint8_t)((r * duty_cycle) / 100);
    b = (uint8_t)((b * duty_cycle) / 100);

    // 重新设置灯珠颜色
    plugin_rgb_set_pixel_rgb(pixel_idx, g, r, b);
}

/**
 * @brief  通过占空比调整所有已设置灯珠的亮度（不改变颜色）
 * @param  duty_cycle：占空比（0~100，表示亮度百分比）
 * @note   此函数会读取所有灯珠的当前颜色，然后按占空比调整亮度
 */
void plugin_rgb_adjust_all_brightness(uint8_t duty_cycle)
{
    uint16_t i;
    for (i = 0; i < DRV_RGB_LED_NUM; i++)
    {
        plugin_rgb_adjust_pixel_brightness(i, duty_cycle);
    }
    // 显示所有灯珠
    drv_rgb_show(DRV_RGB_LED_NUM - 1);
}


/**
 * @brief  非阻塞实现：GRB顺序全颜色单分量渐变（根除同步突变）
 * @param  step：每段渐变的总步数（越大越平滑，建议200）
 * @param  delay_ms_val：每步延时（ms，建议5~10ms）
 * @note   需在主循环中反复调用，内部通过静态变量保存状态
 */
void plugin_rgb_gradient_all_color_node_nonblocking(uint16_t step, uint16_t delay_ms_val)
{
    // --------------- 1. 静态变量保存状态（仅初始化一次）---------------
    // 原函数的颜色节点数组（GRB顺序，单分量变化）
    static const uint32_t color_list[] = {
        0x00FF00, // color_red    红
        0x40FF00, // color_orange 橙
        0xFFEE00, // color_yellow 黄
        0xFF0000, // color_green  绿
        0x800000, // color_green1 绿半降
        0x000080, // color_cyan1  青半升
        0x0000FF, // color_cyan   青
        0x000080, // color_blue1  蓝半降
        0x4000FF, // color_blue   蓝
        0x8000FF, // color_purple 紫
        0x008080, // color_purple1 紫半降
        0x00FF00  // color_red_back 回红
    };
    static const uint8_t seg_count = sizeof(color_list) / sizeof(uint32_t) - 1; // 渐变段数=节点数-1

    // 渐变状态变量
    static uint8_t curr_seg = 0;        // 当前渐变段索引（0~seg_count-1）
    static uint16_t curr_step = 0;      // 当前段的执行步数
    static uint32_t last_update = 0;    // 上次更新的时间戳（ms）
    static uint32_t start_color = 0;    // 当前段的起始颜色
    static uint32_t end_color = 0;      // 当前段的结束颜色
    // GRB分量缓存（避免重复提取）
    static int32_t g_start, r_start, b_start;
    static int32_t g_end, r_end, b_end;
    static uint8_t init_flag = 0;       // 首次初始化标志

    // --------------- 2. 首次调用初始化 ---------------
    if (!init_flag)
    {
        last_update = get_system_tick();                  // 初始化时间戳
        curr_seg = 0;                               // 从第一个段开始
        curr_step = 0;                              // 重置步数
        start_color = color_list[curr_seg];         // 初始化起始颜色
        end_color = color_list[curr_seg + 1];       // 初始化结束颜色
        // 提取GRB分量（与原函数逻辑一致）
        g_start = (start_color >> 16) & 0xFF;
        r_start = (start_color >> 8) & 0xFF;
        b_start = start_color & 0xFF;
        g_end = (end_color >> 16) & 0xFF;
        r_end = (end_color >> 8) & 0xFF;
        b_end = end_color & 0xFF;
        init_flag = 1;                              // 标记初始化完成
        return;                                     // 首次调用不执行渐变，避免突变
    }

    // --------------- 3. 非阻塞延时判断 ---------------
    uint32_t now = get_system_tick();
    if (now - last_update < delay_ms_val)
    {
        return; // 未到延时时间，直接返回
    }
    last_update = now; // 更新时间戳

    // --------------- 4. 单步渐变计算（与原函数逻辑一致）---------------
    if (curr_step <= step)
    {
        // 整数运算+四舍五入（避免浮点误差）
        int32_t g = ((g_end - g_start) * curr_step + step / 2) / step + g_start;
        int32_t r = ((r_end - r_start) * curr_step + step / 2) / step + r_start;
        int32_t b = ((b_end - b_start) * curr_step + step / 2) / step + b_start;

        // 范围限制：防止溢出导致的色阶跳变
        g = g < 0 ? 0 : (g > 255 ? 255 : g);
        r = r < 0 ? 0 : (r > 255 ? 255 : r);
        b = b < 0 ? 0 : (b > 255 ? 255 : b);

        // GRB顺序设置颜色
        plugin_rgb_set_all_rgb((uint8_t)g, (uint8_t)r, (uint8_t)b);

        // 步数递增
        curr_step++;
    }
    // --------------- 5. 切换到下一个渐变段 ---------------
    else
    {
        curr_seg++;                          // 段索引递增
        if (curr_seg >= seg_count)           // 所有段执行完毕
        {
            curr_seg = 0;                    // 循环渐变：重置为第一个段
            // 若需要单次渐变（执行完停止），可注释上面一行，添加：init_flag = 0; return;
        }

        // 更新当前段的起始/结束颜色
        start_color = color_list[curr_seg];
        end_color = color_list[curr_seg + 1];
        // 重新提取GRB分量
        g_start = (start_color >> 16) & 0xFF;
        r_start = (start_color >> 8) & 0xFF;
        b_start = start_color & 0xFF;
        g_end = (end_color >> 16) & 0xFF;
        r_end = (end_color >> 8) & 0xFF;
        b_end = end_color & 0xFF;

        curr_step = 0; // 重置当前段步数
    }
}
