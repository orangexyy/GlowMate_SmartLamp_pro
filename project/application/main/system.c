/****************************************************************************\
**  版    权 : 
**  文件名称 :  
**  功能描述 :  
**  作    者 :  
**  日    期 :  
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/
                1 首次创建
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "system.h"
#include "thread.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
/** @brief 每秒微秒数 */
#define US_PER_SECOND       1000000U

/** @brief 每秒毫秒数 */
#define MS_PER_SECOND       1000U

/** @brief 每毫秒微秒数 */
#define US_PER_MS           1000U

/** @brief 24位SysTick寄存器的最大重载值 (0x00FFFFFF = 16777215) */
#define SYSTICK_MAX_RELOAD  0x00FFFFFFUL
/******************************************************************************\
                             Typedef definitions
\******************************************************************************/

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
uint32_t g_system_tick = 0;

/** @brief 微秒延时倍乘系数：每微秒的节拍数 */
static uint32_t fac_us = 0;

/** @brief SysTick重载值，用于1ms中断周期 */
static uint32_t reload_value = 0;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/**
 * @brief  初始化SysTick定时器用于延时函数
 * @param  SYSCLK 系统时钟频率，单位MHz（保留参数，供将来使用）
 * @note   - 配置SysTick使用HCLK（AHB时钟）作为时钟源
 *         - 设置重载值以产生1ms周期性中断
 *         - 计算fac_us以实现微秒级延时精度
 *         - 使能SysTick中断和计数器
 * @warning 重载值限制为24位（0x00FFFFFF）。如果SystemCoreClock过高，
 *          实际中断周期可能大于1ms。
 */
void SysTick_Init(uint8_t SYSCLK)
{
    (void)SYSCLK;  /* 保留参数，用于兼容性 */
    
    /* 配置SysTick使用外部时钟源（HCLK = AHB时钟） */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    
    /* 计算每微秒的节拍数，用于delay_us()函数
     * 示例：168MHz / 1000000 = 168 节拍/微秒 */
    fac_us = SystemCoreClock / US_PER_SECOND;
    
    /* 计算1ms中断周期的重载值
     * 示例：168MHz / 1000 = 168000 节拍/毫秒 */
    reload_value = SystemCoreClock / MS_PER_SECOND;
    
    /* 安全检查：SysTick LOAD寄存器仅为24位
     * 如果重载值超过限制，则限制为最大值 */
    if (reload_value > SYSTICK_MAX_RELOAD)
    {
        reload_value = SYSTICK_MAX_RELOAD;
    }
    
    /* 使能SysTick异常请求（中断） */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    
    /* 设置重载值：计数器达到零后将重载此值 */
    SysTick->LOAD = reload_value;
    
    /* 使能SysTick计数器：从重载值开始向下计数 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief  微秒级忙等待延时
 * @param  nus 要延时的微秒数
 * @note   - 这是一个使用忙等待的阻塞函数
 *         - 最大延时：(2^32 - 1) / fac_us
 *         - 示例：@ 168MHz，fac_us=168，最大延时 ≈ 25,565,392 微秒（≈25.5秒）
 *         - 函数通过轮询SysTick->VAL寄存器来测量已过时间
 *         - 正确处理计数器溢出回绕
 * @warning 如果延时时间较长，不要在中断上下文中调用此函数
 */
void delay_us(uint32_t nus)
{
    uint32_t ticks;          /* 延时所需的节拍数 */
    uint32_t told;           /* 上一次SysTick计数器值 */
    uint32_t tnow;           /* 当前SysTick计数器值 */
    uint32_t tcnt = 0;       /* 累计已过节拍数 */
    uint32_t reload;         /* 当前SysTick重载值 */
    
    /* 获取当前重载值（如果SysTick被重新配置可能会改变） */
    reload = SysTick->LOAD;
    
    /* 计算所需的总节拍数：延时时间 * 每微秒节拍数 */
    ticks = nus * fac_us;
    
    /* 记录初始计数器值 */
    told = SysTick->VAL;
    
    /* 忙等待循环：轮询计数器直到所需节拍数已过 */
    while (1)
    {
        /* 读取当前计数器值 */
        tnow = SysTick->VAL;
        
        /* 仅在计数器值改变时处理（避免不必要的计算） */
        if (tnow != told)
        {
            /* SysTick是递减计数器，因此计算已过节拍数 */
            if (tnow < told)
            {
                /* 正常情况：计数器递减（told > tnow）
                 * 已过节拍数 = told - tnow */
                tcnt += told - tnow;
            }
            else
            {
                /* 计数器溢出回绕：计数器从LOAD寄存器重载
                 * 已过节拍数 = (reload - tnow) + told
                 * 这处理了计数器达到0并重载的情况 */
                tcnt += reload - tnow + told;
            }
            
            /* 更新上一次的值，用于下次迭代 */
            told = tnow;
            
            /* 检查是否已达到延时时间 */
            if (tcnt >= ticks)
            {
                break;  /* 延时完成，退出循环 */
            }
        }
    }
}

/**
 * @brief  毫秒级忙等待延时
 * @param  nms 要延时的毫秒数（范围：0 ~ 65535）
 * @note   - 此函数内部调用delay_us()
 *         - 对于大于65535ms的延时，请直接使用delay_us()并传入适当的值
 *         - 示例：delay_ms(100) 延时100毫秒
 * @warning 这是一个阻塞函数。在多任务环境中，请考虑使用RTOS延时函数
 */
void delay_ms(uint32_t nms)
{
    /* 将毫秒转换为微秒并调用delay_us */
    delay_us(nms * US_PER_MS);
}

/**
 * @brief  毫秒级忙等待延时（非抢占版本）
 * @param  nms 要延时的毫秒数
 * @note   - 此函数不会将CPU让给其他任务
 *         - 使用循环多次调用delay_us()（每次迭代1ms）
 *         - 适用于需要精确时序而不进行任务切换的场景
 *         - 与delay_ms()类似，但实现方式不同以保持兼容性
 * @warning 此函数完全阻塞CPU。不要在时间关键的中断处理函数中使用，
 *          或在需要实时响应的场景中使用。
 */
void delay_xms(uint32_t nms)
{
    uint32_t i;
    
    /* 每次延时1ms，重复nms次 */
    for (i = 0; i < nms; i++)
    {
        delay_us(US_PER_MS);
    }
}




