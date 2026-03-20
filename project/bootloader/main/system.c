#include "system.h"
#include "stm32f10x.h"                  // Device header
#include "stm32f10x.h"                  // Device header

// 延时倍乘数
static u8  fac_us = 0;		// us延时倍乘数
uint32_t g_system_tick = 0;

/**
 * @brief  初始化延时函数，配置SysTick为1ms周期性中断
 * @param  SYSCLK: 系统时钟频率（单位：MHz），例如72表示72MHz
 * @note   SYSTICK的时钟固定为HCLK时钟的1/8
 *         同时配置SysTick为1ms周期性中断
 */
void SysTick_Init(u8 SYSCLK)
{
	u32 reload;
	
	// 更新系统时钟变量（确保 SystemCoreClock 是最新的）
	SystemCoreClockUpdate();
	
	// 选择外部时钟 HCLK/8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	
	// 计算us延时倍乘数 (系统时钟的1/8)
	fac_us = SystemCoreClock / 8000000;
	
	// 配置SysTick为1ms周期性中断
	// 使用 HCLK/8 作为时钟源，所以需要除以 8
	// reload = SystemCoreClock / 8 / 1000 = SystemCoreClock / 8000
	reload = SystemCoreClock / 8000;  // 每1ms中断一次
	
	// 检查重装载值是否超出范围 (24位寄存器，最大值 0xFFFFFF)
	if(reload > 0xFFFFFF)
	{
		reload = 0xFFFFFF;
	}
	reload -= 1;  // 减1是因为从0开始计数
	
	// 先禁用SysTick和中断，确保配置过程不受干扰
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
	
	// 配置SysTick寄存器
	SysTick->LOAD = reload;   // 设置重装载值
	SysTick->VAL = 0;                 // 清空计数器
	
	// 设置SysTick中断优先级（使用最低优先级，避免影响其他中断）
	NVIC_SetPriority(SysTick_IRQn, 0xF0);
	
	// 配置CTRL寄存器：使能中断和SysTick
	// 注意：CLKSOURCE 位已经由 SysTick_CLKSourceConfig 设置，这里只设置中断和使能位
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;     // 使能中断
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      // 使能SysTick
}

/**
 * @brief  延时nus
 * @param  nus: 要延时的us数
 */
void delay_us(u32 nus)
{
	u32 temp;
	u32 ctrl_backup;
	u32 load_backup;
	
	// 保存当前配置
	ctrl_backup = SysTick->CTRL;
	load_backup = SysTick->LOAD;
	
	// 临时禁用中断
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	
	// 时间加载
	SysTick->LOAD = nus * fac_us;
	
	// 清空计数器
	SysTick->VAL = 0x00;
	
	// 开始倒数
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	// 等待时间到达
	do
	{
		temp = SysTick->CTRL;
	} while((temp & 0x01) && !(temp & (1 << 16)));
	
	// 关闭计数器
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	
	// 恢复配置
	SysTick->LOAD = load_backup;
	SysTick->VAL = 0x00;
	SysTick->CTRL = ctrl_backup;
}

/**
 * @brief  延时nms
 * @param  nms: 要延时的ms数
 * @note   基于系统滴答计数实现，不会影响SysTick中断
 *         支持任意长度的延时，不受24位寄存器限制
 */
void delay_ms(u16 nms)
{
	u32 start_tick = g_system_tick;
	
	// 等待达到目标滴答数（处理u32溢出情况）
	while ((u32)(g_system_tick - start_tick) < nms);
}


uint32_t get_system_tick(void)
{
	return g_system_tick;
}


