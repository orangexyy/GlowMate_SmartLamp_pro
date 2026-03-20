/****************************************************************************\
**  文件名称 :  sys.c
**  功能描述 :  系统底层接口。WFI 休眠、全局中断开关、设置栈顶地址（MSR_MSP）。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

#include "sys.h"

/** THUMB 指令不支持汇编内联，采用独立汇编实现 WFI */
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
/** 关闭所有中断 */
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
/** 开启所有中断 */
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
/** 设置主栈指针（用于跳转前设置栈顶）
 * \param addr 栈顶地址
 */
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
