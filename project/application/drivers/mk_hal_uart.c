/****************************************************************************\
**  版    权 :  深圳市创客工场科技有限公司(MakeBlock)所有（2030）
**  文件名称 :  mk_hal_uart.c
**  功能描述 :  uart驱动
**  作    者 :  王滨泉
**  日    期 :  2022.11.14
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2022.11.14
                1、首次创建
\****************************************************************************/

/****************************************************************************\
                            Includes
\****************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "gd32l23x.h"
#include "mk_hal_uart.h"  
#include "Ring_Buf.h"
#include "mk_hal_system.h" 
#include "neuron_manager.h"
#include "mk_common.h"
/****************************************************************************\
                            Macro definitions
\****************************************************************************/
#define USART0_TX_BUF_SIZE   256
#define USART0_RX_BUF_SIZE   256

#define USART1_TX_BUF_SIZE   256
#define USART1_RX_BUF_SIZE   256

#define UART3_TX_BUF_SIZE   256
#define UART3_RX_BUF_SIZE   256

#define UART4_TX_BUF_SIZE   256
#define UART4_RX_BUF_SIZE   256
/****************************************************************************\
                            Typedef definitions
\****************************************************************************/

//------------------------------------------------------------------------------
// USART 引脚序号
typedef enum
{ 
    USART_TX = 0,
    USART_RX,
    USART_MAX
}TE_MK_USART_GPIO_NUM;
//------------------------------------------------------------------------------
// USART 配置句柄
typedef struct
{
    uint32_t                usart_num;          // USART外设编号
    rcu_periph_enum         clock_periph;       // USART外设时钟
//    uint32_t                alt_func;           // USART引脚复用信息组
    IRQn_Type               nvic_irq;           // USART中断
    MK_HAL_USART_CALLBACK   usart_cb;           // USART回调函数指针
    rcu_periph_enum         dma_clock_periph;   // USART DMA外设
//    uint32_t                dma_periph;         // USART DMA外设
    dma_channel_enum        tx_channelx;        // USART DMA通道
    dma_channel_enum        rx_channelx;        // USART DMA通道
//    dma_subperipheral_enum  sub_periph;         // USART DMA子外设 
    IRQn_Type               dma_irq;            // USART DMA 中断
		uint32_t								dmamux_request_tx;			// USART DMAMUX TX脚 请求
		uint32_t								dmamux_request_rx;			// USART DMAMUX RX脚 请求
}TS_MK_USART_CONFIG;
/****************************************************************************\
                            Variables definitions
\****************************************************************************/

// uasrt配置缓存
static TS_MK_USART_CONFIG s_usart_config[MK_USART_ID_MAX] = {
    {USART0, RCU_USART0, USART0_IRQn, 0, RCU_DMA, DMA_CH5, DMA_CH4, DMA_Channel4_IRQn,DMA_REQUEST_USART0_TX,DMA_REQUEST_USART0_RX},
    {USART1, RCU_USART1, USART1_IRQn, 0, RCU_DMA, DMA_CH1, DMA_CH2, DMA_Channel2_IRQn,DMA_REQUEST_USART1_TX,DMA_REQUEST_USART1_RX},
    {UART3 , RCU_UART3 , UART3_IRQn , 0, RCU_DMA, DMA_CH1, DMA_CH2, DMA_Channel3_IRQn,DMA_REQUEST_UART3_TX ,DMA_REQUEST_UART3_RX },
    {UART4 , RCU_UART4 , UART4_IRQn , 0, RCU_DMA, DMA_CH1, DMA_CH4, DMA_Channel4_IRQn,DMA_REQUEST_UART4_TX ,DMA_REQUEST_UART4_RX }
};

// 串口数据环形缓冲区
static TS_RING_BUF_DEF s_usart_tx[MK_USART_ID_MAX];
static TS_RING_BUF_DEF s_usart_rx[MK_USART_ID_MAX];

// 串口数据缓存指针
static uint8_t *s_usart_tx_buf[MK_USART_ID_MAX];
static uint8_t *s_usart_rx_buf[MK_USART_ID_MAX];

// 串口发送缓存大小
static uint32_t s_usart_tx_size[MK_USART_ID_MAX] = {
    USART0_TX_BUF_SIZE,
    USART1_TX_BUF_SIZE,
    UART3_TX_BUF_SIZE ,
    UART4_TX_BUF_SIZE ,
};

// 串口接收缓存大小
static uint32_t s_usart_rx_size[MK_USART_ID_MAX] = {
    USART0_RX_BUF_SIZE,
    USART1_RX_BUF_SIZE,
    UART3_RX_BUF_SIZE	,
    UART4_RX_BUF_SIZE	,
};

static bool s_usart_tx_busy[MK_USART_ID_MAX] = {0};
/****************************************************************************\
                            Functions definitions
\****************************************************************************/

/*
 *  函数名称：mk_hal_usart_clock_enable
 *  功能描述：usart引脚时钟开关控制
 *  参    数：gpio_periph - gpio外设
 *  返回数值：错误类型(int)
 *  示    例：mk_hal_usart_clock_enable(gpio_periph);
 */
/****************************************************************************/
static int mk_hal_usart_clock_enable(uint32_t gpio_periph)
/****************************************************************************/
{
    //开启引脚时钟
    if(gpio_periph == GPIOA)
    {
        rcu_periph_clock_enable(RCU_GPIOA);
    }        
	else if(gpio_periph == GPIOB)
    {
        rcu_periph_clock_enable(RCU_GPIOB);
    }
    else
    {
        return MK_ERROR_INVALID_PARAM;
    }
    return MK_SUCCESS;
}

/*
* 函数名称 : mk_hal_usart_open
* 功能描述 : 串口初始化函数
* 参    数 : p_this     - 串口结构体
             FunctionCB - 接收回调函数
* 返回值   : int：返回错误代码
* 示    例 : mk_hal_usart_open(&p_this, FunctionCB);	 
*/
/******************************************************************************/
int mk_hal_usart_open(TS_MK_USART_HANDLE *p_this, MK_HAL_USART_CALLBACK FunctionCB)
/******************************************************************************/
{
    uint32_t port[USART_MAX],pin[USART_MAX];
//    dma_single_data_parameter_struct dma_init_struct;
    
    //判断参数合法性
    if (p_this == 0)
    {
        MK_ASSERT_ERROR(MK_ERROR_NULL_POINTER);
    }
    
    // 注册usart回调处理函数
    if(FunctionCB)
    {
        s_usart_config[p_this->id].usart_cb = FunctionCB;
    }
    
    // 开启USART时钟
    rcu_periph_clock_enable(s_usart_config[p_this->id].clock_periph);
    
    
    // 配置USART_TX引脚
    if((p_this->Mode & MK_USART_MODE_TX)== MK_USART_MODE_TX)
    {
        // 获取引脚信息
        port[USART_TX] = MKP2PORT(p_this->io.TxPin); 
        pin[USART_TX] = MKP2PIN(p_this->io.TxPin);
        // 开始引脚时钟
        mk_hal_usart_clock_enable(port[USART_TX]);
        // 配置引脚串口TX功能

        if ((MK_GPIOB_06 == p_this->io.TxPin) || (MK_GPIOA_13 == p_this->io.TxPin) || \
            (MK_GPIOA_02 == p_this->io.TxPin) || (MK_GPIOA_09 == p_this->io.TxPin))
        {
            gpio_af_set(port[USART_TX], GPIO_AF_7, pin[USART_TX]);
        }
        else
        {
            gpio_af_set(port[USART_TX], GPIO_AF_8, pin[USART_TX]);
        }
        
		
        gpio_mode_set(port[USART_TX], GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin[USART_TX]);
        gpio_output_options_set(port[USART_TX], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin[USART_TX]);
        // 申请发送缓存空间
        s_usart_tx_buf[p_this->id] = (uint8_t *)malloc(s_usart_tx_size[p_this->id]);
        ring_buf_init(&s_usart_tx[p_this->id], s_usart_tx_buf[p_this->id], s_usart_tx_size[p_this->id]);

    }
    
    // 配置USART_RX引脚
    if((p_this->Mode & MK_USART_MODE_RX) == MK_USART_MODE_RX)
    {
        // 获取引脚信息
        port[USART_RX] = MKP2PORT(p_this->io.RxPin); 
        pin[USART_RX] = MKP2PIN(p_this->io.RxPin);
        // 开始引脚时钟
        mk_hal_usart_clock_enable(port[USART_RX]);
        // 配置引脚串口RX功能
        if ((MK_GPIOB_07 == p_this->io.RxPin) || (MK_GPIOA_14 == p_this->io.RxPin) || \
            (MK_GPIOA_03 == p_this->io.RxPin) || (MK_GPIOA_10 == p_this->io.RxPin) || \
            (MK_GPIOA_15 == p_this->io.RxPin))
        {
            gpio_af_set(port[USART_RX], GPIO_AF_7, pin[USART_RX]);
        }
        else
        {
            gpio_af_set(port[USART_RX], GPIO_AF_8, pin[USART_RX]);
        }
				
        gpio_mode_set(port[USART_RX], GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin[USART_RX]);
        gpio_output_options_set(port[USART_RX], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin[USART_RX]);
        // 申请接收缓存空间
        s_usart_rx_buf[p_this->id] = (uint8_t *)malloc(s_usart_rx_size[p_this->id]);
        ring_buf_init(&s_usart_rx[p_this->id], s_usart_rx_buf[p_this->id], s_usart_rx_size[p_this->id]);
    }
        
    usart_deinit(s_usart_config[p_this->id].usart_num);
    
    usart_baudrate_set(s_usart_config[p_this->id].usart_num, p_this->BaudRate);
    
    usart_parity_config(s_usart_config[p_this->id].usart_num, USART_PM_NONE);
    
    usart_word_length_set(s_usart_config[p_this->id].usart_num, USART_WL_8BIT);
    
    usart_stop_bit_set(s_usart_config[p_this->id].usart_num, USART_STB_1BIT);
    
    // 开启半双工模式
    if((p_this->Mode & MK_USART_MODE_HALFDUPLEX) == MK_USART_MODE_HALFDUPLEX)
    {
        usart_halfduplex_enable(s_usart_config[p_this->id].usart_num);
        if((p_this->Mode & MK_USART_MODE_TX) != MK_USART_MODE_TX)
        {
            // 申请发送缓存空间
            s_usart_tx_buf[p_this->id] = (uint8_t *)malloc(s_usart_tx_size[p_this->id]);
            ring_buf_init(&s_usart_tx[p_this->id], s_usart_tx_buf[p_this->id], s_usart_tx_size[p_this->id]);
        }
        if((p_this->Mode & MK_USART_MODE_RX) != MK_USART_MODE_RX)
        {
            // 申请接收缓存空间
            s_usart_rx_buf[p_this->id] = (uint8_t *)malloc(s_usart_rx_size[p_this->id]);
            ring_buf_init(&s_usart_rx[p_this->id], s_usart_rx_buf[p_this->id], s_usart_rx_size[p_this->id]);
        }
    }
    
    // 开启串口发送
    if((p_this->Mode & MK_USART_MODE_TX)== MK_USART_MODE_TX && (p_this->Mode & MK_USART_MODE_TX_DMA) != MK_USART_MODE_TX_DMA)
    {
        usart_transmit_config(s_usart_config[p_this->id].usart_num, USART_TRANSMIT_ENABLE);
    }
    
    // 开启串口接收
    if((p_this->Mode & MK_USART_MODE_RX) == MK_USART_MODE_RX)
    {
        usart_receive_config(s_usart_config[p_this->id].usart_num, USART_RECEIVE_ENABLE);
        
        // 开启接收中断
        usart_interrupt_enable(s_usart_config[p_this->id].usart_num, USART_INT_RBNE);
        
    }
    usart_overrun_disable(s_usart_config[p_this->id].usart_num);
    usart_enable(s_usart_config[p_this->id].usart_num);
    
    nvic_irq_enable(s_usart_config[p_this->id].nvic_irq, 1);
    
    return MK_SUCCESS;
    
}

/*
* 函数名称 : mk_hal_usart_dma_open
* 功能描述 : 串口初始化函数
* 参    数 : p_this     - 串口结构体
             FunctionCB - 接收回调函数
* 返回值   : int：返回错误代码
* 示    例 : mk_hal_usart_open(&p_this, FunctionCB);	 
*/
/******************************************************************************/
int mk_hal_usart_dma_open(TS_MK_USART_HANDLE *p_this, MK_HAL_USART_CALLBACK FunctionCB)
/******************************************************************************/
{
    uint32_t port[USART_MAX],pin[USART_MAX];
    dma_parameter_struct dma_init_struct;
    
    //判断参数合法性
    if (p_this == 0)
    {
        MK_ASSERT_ERROR(MK_ERROR_NULL_POINTER);
    }
    
    // 注册usart回调处理函数
    s_usart_config[p_this->id].usart_cb = FunctionCB;
    
    // 开启USART时钟
    rcu_periph_clock_enable(s_usart_config[p_this->id].clock_periph);
    
    // 配置USART_TX引脚
    if((p_this->Mode & MK_USART_MODE_TX)== MK_USART_MODE_TX)
    {
        // 获取引脚信息
        port[USART_TX] = MKP2PORT(p_this->io.TxPin); 
        pin[USART_TX] = MKP2PIN(p_this->io.TxPin);
        // 开始引脚时钟
        mk_hal_usart_clock_enable(port[USART_TX]);
        // 配置引脚串口TX功能
        if ((MK_GPIOB_06 == p_this->io.TxPin) || (MK_GPIOA_13 == p_this->io.TxPin) || \
            (MK_GPIOA_02 == p_this->io.TxPin) || (MK_GPIOA_09 == p_this->io.TxPin))
        {
            gpio_af_set(port[USART_TX], GPIO_AF_7, pin[USART_TX]);
        }
        else
        {
            gpio_af_set(port[USART_TX], GPIO_AF_8, pin[USART_TX]);
        }
				
        gpio_mode_set(port[USART_TX], GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin[USART_TX]);
        gpio_output_options_set(port[USART_TX], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin[USART_TX]);
        // 申请发送缓存空间
        s_usart_tx_buf[p_this->id] = (uint8_t *)malloc(s_usart_tx_size[p_this->id]);
        ring_buf_init(&s_usart_tx[p_this->id], s_usart_tx_buf[p_this->id], s_usart_tx_size[p_this->id]);
    }
    
    // 配置USART_RX引脚
    if((p_this->Mode & MK_USART_MODE_RX) == MK_USART_MODE_RX)
    {
        // 获取引脚信息
        port[USART_RX] = MKP2PORT(p_this->io.RxPin); 
        pin[USART_RX] = MKP2PIN(p_this->io.RxPin);
        // 开始引脚时钟
        mk_hal_usart_clock_enable(port[USART_RX]);
        // 配置引脚串口RX功能
        if ((MK_GPIOB_07 == p_this->io.RxPin) || (MK_GPIOA_14 == p_this->io.RxPin) || \
            (MK_GPIOA_03 == p_this->io.RxPin) || (MK_GPIOA_10 == p_this->io.RxPin) || \
            (MK_GPIOA_15 == p_this->io.RxPin))
        {
            gpio_af_set(port[USART_RX], GPIO_AF_7, pin[USART_RX]);
        }
        else
        {
            gpio_af_set(port[USART_RX], GPIO_AF_8, pin[USART_RX]);
        }

        gpio_mode_set(port[USART_RX], GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin[USART_RX]);
        gpio_output_options_set(port[USART_RX], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin[USART_RX]);
        // 申请接收缓存空间
        s_usart_rx_buf[p_this->id] = (uint8_t *)malloc(s_usart_rx_size[p_this->id]);
        ring_buf_init(&s_usart_rx[p_this->id], s_usart_rx_buf[p_this->id], s_usart_rx_size[p_this->id]);
    }
        
    usart_deinit(s_usart_config[p_this->id].usart_num);
    usart_baudrate_set(s_usart_config[p_this->id].usart_num, p_this->BaudRate);    
    usart_parity_config(s_usart_config[p_this->id].usart_num, USART_PM_NONE);    
    usart_word_length_set(s_usart_config[p_this->id].usart_num, USART_WL_8BIT);    
    usart_stop_bit_set(s_usart_config[p_this->id].usart_num, USART_STB_1BIT);
    
    usart_transmit_config(s_usart_config[p_this->id].usart_num, USART_TRANSMIT_ENABLE);
    usart_receive_config(s_usart_config[p_this->id].usart_num, USART_RECEIVE_ENABLE);
	usart_interrupt_flag_clear(s_usart_config[p_this->id].usart_num, USART_INT_FLAG_IDLE);
    usart_interrupt_enable(s_usart_config[p_this->id].usart_num, USART_INT_IDLE);
    
    nvic_irq_enable(s_usart_config[p_this->id].dma_irq, 2);
    nvic_irq_enable(s_usart_config[p_this->id].nvic_irq, 1);
     
    dma_struct_para_init(&dma_init_struct);
    rcu_periph_clock_enable(RCU_DMA);
		
 		// TX 初始化DMA结构体
    dma_deinit(s_usart_config[p_this->id].tx_channelx);
    dma_init_struct.request 		= s_usart_config[p_this->id].dmamux_request_tx;
    dma_init_struct.direction 		= DMA_MEMORY_TO_PERIPHERAL;//p_this->TxDMA.Direction;             // DMA方向为内存->外设
    dma_init_struct.memory_addr 	= (uint32_t)s_usart_tx_buf[p_this->id];                    // 内存地址
    dma_init_struct.memory_inc 		= DMA_MEMORY_INCREASE_ENABLE;//p_this->TxDMA.MemInc;         // 内存地址自增模式
    dma_init_struct.number 			= s_usart_tx_size[p_this->id];                                   // 内存大小
    dma_init_struct.periph_width 	= DMA_PERIPHERAL_WIDTH_8BIT;                // 数据宽度
    dma_init_struct.periph_addr     = ((uint32_t)&USART_TDATA(s_usart_config[p_this->id].usart_num));    // 串口外设地址
    dma_init_struct.periph_inc      = DMA_PERIPH_INCREASE_DISABLE;//p_this->TxDMA.PeriphInc;     // DMA地址固定模式
    dma_init_struct.priority        = DMA_PRIORITY_ULTRA_HIGH;                                      // 优先级
    dma_init(s_usart_config[p_this->id].tx_channelx, &dma_init_struct);
    
    dma_circulation_disable(s_usart_config[p_this->id].tx_channelx); // 关闭循环模式
    usart_dma_transmit_config(s_usart_config[p_this->id].usart_num, USART_TRANSMIT_DMA_ENABLE);
		

    // RX 初始化DMA结构体
    dma_deinit(s_usart_config[p_this->id].rx_channelx);
    dma_init_struct.request 			= s_usart_config[p_this->id].dmamux_request_rx;
    dma_init_struct.direction           = DMA_PERIPHERAL_TO_MEMORY; // DMA方向为外设->内存
    dma_init_struct.memory_addr        	= (uint32_t)s_usart_rx_buf[p_this->id];
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_width 		= DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.number              = s_usart_rx_size[p_this->id];
    dma_init_struct.periph_addr         = ((uint32_t)&USART_RDATA(s_usart_config[p_this->id].usart_num));
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(s_usart_config[p_this->id].rx_channelx, &dma_init_struct);

    dma_circulation_disable(s_usart_config[p_this->id].rx_channelx);
    dma_interrupt_enable(s_usart_config[p_this->id].rx_channelx, DMA_CHXCTL_FTFIE);
    usart_dma_receive_config(s_usart_config[p_this->id].usart_num, USART_RECEIVE_DMA_ENABLE);
    dma_channel_enable(s_usart_config[p_this->id].rx_channelx);

    usart_enable(s_usart_config[p_this->id].usart_num);
  
    return MK_SUCCESS;    
}




/*
* 函数名称 : mk_hal_uart_write
* 功能描述 : 串口输出发送(多字节)
* 参    数 : id   - id
             buf  - 数据指针
             size - 数据长度
* 返回值   : MK_SUCCESS   - 数据写入成功
             MK_FAIL - 数据写入失败
*/
/******************************************************************************/
int mk_hal_uart_write(TE_MK_USART_ID id, uint8_t* buf, uint32_t size)
/******************************************************************************/
{
    // 判断ID合法
    if(id >= MK_USART_ID_MAX)
    {
       MK_ASSERT_ERROR(MK_ERROR_INVALID_PARAM);
    }
    
    s_usart_tx_busy[id] = true;

    if(ring_buf_write(&s_usart_tx[id], buf, size) == false)
    {
        // 发送缓存不足，数据写入失败，打开发送中断，释放发送缓存
        usart_interrupt_enable(s_usart_config[id].usart_num, USART_INT_TC);
        return MK_FAIL;         
    }
    else
    {
        usart_interrupt_enable(s_usart_config[id].usart_num, USART_INT_TC);
    }
    return MK_SUCCESS;
}

/*
* 函数名称 : mk_hal_uart_write_sync
* 功能描述 : 串口输出发送(多字节)，不入缓冲区
* 参    数 : id   - id
             buf  - 数据指针
             size - 数据长度
* 返回值   : MK_SUCCESS   - 数据写入成功
             MK_FAIL - 数据写入失败
*/
int mk_hal_uart_write_sync(TE_MK_USART_ID id, uint8_t* buf, uint32_t size)
{
    uint32_t i;
    // 判断ID合法
    if(id >= MK_USART_ID_MAX)
    {
       MK_ASSERT_ERROR(MK_ERROR_INVALID_PARAM);
    }

    for(i=0; i<size; i++)
    {
        while (RESET == usart_flag_get(s_usart_config[id].usart_num, USART_FLAG_TBE));
        usart_data_transmit(s_usart_config[id].usart_num, (uint8_t)buf[i]);
    }  
    
    return MK_SUCCESS;  
}

/*
* 函数名称 : mk_hal_uart_dma_write
* 功能描述 : 串口输出发送(多字节)
* 参    数 : channel - 通道
             buf - 数据指针
             size - 数据长度
* 返回值   : 无
*/
/******************************************************************************/
int mk_hal_uart_dma_write(TE_MK_USART_ID id, uint8_t* buf, uint32_t size)
/******************************************************************************/
{
    // 判断ID合法
    if(id >= MK_USART_ID_MAX)
    {
       MK_ASSERT_ERROR(MK_ERROR_INVALID_PARAM);
    }

//    while(RingBuf_Write(&s_usart_tx[id], buf, size) == false);              // 等待发送数据写入缓存，避免缓存空间不够导致写入数据丢失
    
    dma_channel_disable(s_usart_config[id].tx_channelx);
		
		dma_flag_clear(s_usart_config[id].tx_channelx, DMA_FLAG_FTF);  // 清除DMA传输完成标志位
		
    dma_memory_address_config(s_usart_config[id].tx_channelx, (uint32_t)buf);
    
    dma_transfer_number_config(s_usart_config[id].tx_channelx, size);
    
    dma_channel_enable(s_usart_config[id].tx_channelx);

//    /* wait DMA Channel transfer complete */
//    while(RESET == dma_flag_get(s_usart_config[id].tx_channelx, DMA_FLAG_FTF)) {
//    }

    return MK_SUCCESS;
}

/*
* 函数名称 : mk_hal_uart_get_tx_busy
* 功能描述 : 获取发送忙状态
* 参    数 : id - usart id
* 返回值   : 无
*/
/******************************************************************************/
bool mk_hal_uart_get_tx_busy(TE_MK_USART_ID id)
/******************************************************************************/
{
    return s_usart_tx_busy[id];
}

/*
* 函数名称 : mk_hal_uart_read
* 功能描述 : 串口接收(多字节)
* 参    数 : channel - 通道
* 返回值   : 数据
*/
/******************************************************************************/
uint8_t mk_hal_uart_read(uint8_t channel)
/******************************************************************************/
{
    while(RESET == usart_flag_get(s_usart_config[channel].usart_num, USART_FLAG_RBNE));
    return usart_data_receive(s_usart_config[channel].usart_num);     
}

/*
* 函数名称 : mk_hal_uart_set_baudrate
* 功能描述 : 串口波特率设置
* 参    数 : id - id
             baudrate - 波特率
* 返回值   : 无
*/
/******************************************************************************/
void mk_hal_uart_set_baudrate(uint8_t id, uint32_t baudrate)
/******************************************************************************/
{
    usart_baudrate_set(s_usart_config[id].usart_num, baudrate);

}

/*
* 函数名称 : mk_hal_uart_set_usart_cb
* 功能描述 : 数据接收回调设置
* 参    数 : id - id
             FunctionCB - 接收回调函数
* 返回值   : 无
*/
/******************************************************************************/
void mk_hal_uart_set_usart_cb(uint8_t id, MK_HAL_USART_CALLBACK FunctionCB)
/******************************************************************************/
{
    if(FunctionCB)
    {
        s_usart_config[id].usart_cb = FunctionCB;
    }
}


static void uart_recv_irq_handler(TE_MK_USART_ID uart_id)
{
    dma_channel_disable(s_usart_config[uart_id].rx_channelx);
    s_usart_config[uart_id].usart_cb(s_usart_rx_buf[uart_id], \
                                    s_usart_rx_size[uart_id] - (dma_transfer_number_get(s_usart_config[uart_id].rx_channelx)));
    usart_interrupt_flag_clear(s_usart_config[uart_id].usart_num,USART_INT_FLAG_IDLE);
    dma_flag_clear( s_usart_config[uart_id].rx_channelx, DMA_FLAG_FTF);
    dma_transfer_number_config(s_usart_config[uart_id].rx_channelx, s_usart_rx_size[uart_id]);
    dma_channel_enable(s_usart_config[uart_id].rx_channelx);
}


/*
* 函数名称 : USART0_IRQHandler
* 功能描述 : 串口0中断处理函数
* 参    数 : 无
* 返回值   : 无
* 示    例 : USART0_IRQHandler();
*/
/******************************************************************************/
void USART0_IRQHandler(void)
/******************************************************************************/
{
	uint8_t inputData = 0;
	uint8_t data;
    uint32_t data_len;
	
	if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) && 
      (RESET != usart_flag_get(USART0, USART_FLAG_RBNE)))
	{
	    /* Read one byte from the receive data register */
	    inputData = usart_data_receive(USART0);
        s_usart_config[MK_USART_ID_0].usart_cb(&inputData, 1);
	}
    
    if((RESET != usart_flag_get(USART0, USART_FLAG_TC)) && 
       (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TC)))
    {
        /* write one byte to the transmit data register */
        data_len = ring_buf_count(&s_usart_tx[MK_USART_ID_0]);
        if(data_len)
        {
            ring_buf_read(&s_usart_tx[MK_USART_ID_0], 1, &data);
            usart_data_transmit(USART0, (uint32_t)data);
        }
        else
        {
            /* disable the USART0 transmit interrupt */
            usart_interrupt_disable(USART0, USART_INT_TC);
            s_usart_tx_busy[MK_USART_ID_0] = false;
        }
    }
    
    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)) {
		
        usart_data_receive(USART0);
        uart_recv_irq_handler(MK_USART_ID_0);
    }
}
/*
* 函数名称 : USART1_IRQHandler
* 功能描述 : 串口1中断处理函数
* 参    数 : 无
* 返回值   : 无
* 示    例 : USART1_IRQHandler();
*/
/******************************************************************************/
void USART1_IRQHandler(void)
/******************************************************************************/
{
	uint8_t inputData = 0;
    uint8_t data;
    uint32_t data_len;
	
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE) && 		//用来获取串口中断状态//USART_INT_FLAG_RBNE：读取缓冲区不空中断标志
        (RESET != usart_flag_get(USART1, USART_FLAG_RBNE)))					// 获取串口状态//读数据缓冲区不为空标志
    {
            inputData = usart_data_receive(USART1);
                s_usart_config[MK_USART_ID_1].usart_cb(&inputData, 1);
    }
    
    if((RESET != usart_flag_get(USART1, USART_FLAG_TC)) && 							//获取当前串口状态//USART_FLAG_TC：传输完成标志位
       (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_TC)))	//获取当前串口中断状态//USART_INT_FLAG_TC：传输完成中断标志位
    {
        data_len = ring_buf_count(&s_usart_tx[MK_USART_ID_1]);
        if(data_len)
        {
            ring_buf_read(&s_usart_tx[MK_USART_ID_1], 1, &data);
            usart_data_transmit(USART1, (uint32_t)data);
        }
        else
        {
            usart_interrupt_disable(USART1, USART_INT_TC);
            s_usart_tx_busy[MK_USART_ID_1] = false;
        }
    }
    
    if (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE)) {  //获取当前串口中断状态  //检测到空闲线路中断标志
		
        usart_data_receive(USART1);
        uart_recv_irq_handler(MK_USART_ID_1);
    }
}


void UART3_IRQHandler()
{
    uint8_t inputData = 0;
    uint8_t data;
    uint32_t data_len;
	
    if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE) && 		//用来获取串口中断状态//USART_INT_FLAG_RBNE：读取缓冲区不空中断标志
        (RESET != usart_flag_get(UART3, USART_FLAG_RBNE)))					// 获取串口状态//读数据缓冲区不为空标志
    {
            inputData = usart_data_receive(UART3);
            s_usart_config[MK_UART_ID_3].usart_cb(&inputData, 1);
    }
    
    if((RESET != usart_flag_get(UART3, USART_FLAG_TC)) && 							//获取当前串口状态//USART_FLAG_TC：传输完成标志位
       (RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_TC)))	//获取当前串口中断状态//USART_INT_FLAG_TC：传输完成中断标志位
    {
        data_len = ring_buf_count(&s_usart_tx[MK_UART_ID_3]);
        if(data_len)
        {
            ring_buf_read(&s_usart_tx[MK_UART_ID_3], 1, &data);
            usart_data_transmit(UART3, (uint32_t)data);
        }
        else
        {
            usart_interrupt_disable(UART3, USART_INT_TC);
            s_usart_tx_busy[MK_UART_ID_3] = false;
        }
    }
    
    if (RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_IDLE)) {  //获取当前串口中断状态  //检测到空闲线路中断标志
            usart_data_receive(UART3);
            uart_recv_irq_handler(MK_UART_ID_3);
    }
}


//// uart0 rx 
//void DMA_Channel1_IRQHandler(void)
//{
//    if(dma_interrupt_flag_get(DMA_CH1, DMA_INT_FLAG_FTF)) {
//        dma_interrupt_flag_clear(DMA_CH1, DMA_INT_FLAG_FTF);
//        uart_recv_irq_handler(MK_USART_ID_0);
//    }
//}

// uart1 rx 
//void DMA_Channel2_IRQHandler(void)
//{
//    if(dma_interrupt_flag_get(DMA_CH2, DMA_INT_FLAG_FTF)) {
//        dma_interrupt_flag_clear(DMA_CH2, DMA_INT_FLAG_FTF);
//        uart_recv_irq_handler(MK_USART_ID_1);
//    }
//}

//void DMA_Channel3_IRQHandler(void)
//{
//    if(dma_interrupt_flag_get(DMA_CH3, DMA_INT_FLAG_FTF)) {
//        dma_interrupt_flag_clear(DMA_CH3, DMA_INT_FLAG_FTF);
//        uart_recv_irq_handler(MK_UART_ID_3);
//    }
//}

//void DMA_Channel4_IRQHandler(void)
//{
//    if(dma_interrupt_flag_get(DMA_CH4, DMA_INT_FLAG_FTF)) {
//        dma_interrupt_flag_clear(DMA_CH4, DMA_INT_FLAG_FTF);
//        uart_recv_irq_handler(MK_UART_ID_4);
//    }
//}