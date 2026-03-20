/****************************************************************************\
**  文件名称 :  mcode_command.h
**  功能描述 :  M 码指令头文件。指令列表宏、process_m_code 及 M 码处理声明。
**  作    者 :  -
**  日    期 :  -
**  版    本 :  V0.0.1
\****************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/

#ifndef __MCODE_COMMAND_H__
#define __MCODE_COMMAND_H__

#include <stdint.h>
#include <stdbool.h>
#include "drv_comm.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define MCODE_CMD_LIST \
    MCODE_CASE(1) \
    MCODE_CASE(2) \
    MCODE_CASE(3) \
    MCODE_CASE(4) \
    MCODE_CASE(5) \
    MCODE_CASE(6) \
    MCODE_CASE(7) \
	MCODE_CASE(8) \
	MCODE_CASE(9) \
	MCODE_CASE(10) \
	MCODE_CASE(11) \
	MCODE_CASE(12) \
	MCODE_CASE(13) \
	MCODE_CASE(14) \
	MCODE_CASE(15) \
	MCODE_CASE(16) \
	MCODE_CASE(17) \
	MCODE_CASE(25) \
	MCODE_CASE(26) \
    MCODE_CASE(99) \

/****************************************************************************\
                            Functions declarations
\****************************************************************************/
/** 处理 M 代码；port_id 为接收数据的端口，回复将发往该端口 */
void process_m_code(TE_DRV_USART_ID port_id, char *data, uint32_t size);

#endif /* __MCODE_COMMAND_H__ */
/**************************** End of File (H) ****************************/ 
