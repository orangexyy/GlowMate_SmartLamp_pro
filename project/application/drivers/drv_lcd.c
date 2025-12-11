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
#include <stdio.h>
#include "stm32f4xx.h"
#include "drv_lcd.h"

/******************************************************************************\
                             Macro definitions
\******************************************************************************/
// 引脚定义（保持不变）
#define LCD_RES_LOW()   GPIO_ResetBits(GPIOC, GPIO_Pin_4)
#define LCD_RES_HIGH()  GPIO_SetBits(GPIOC, GPIO_Pin_4)
#define LCD_DC_LOW()    GPIO_ResetBits(GPIOC, GPIO_Pin_5)  // 命令模式
#define LCD_DC_HIGH()   GPIO_SetBits(GPIOC, GPIO_Pin_5)   // 数据模式
#define LCD_BLK_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define LCD_BLK_HIGH()  GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define LCD_CS_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define LCD_CS_HIGH()   GPIO_SetBits(GPIOB, GPIO_Pin_1)

// DMA句柄定义（根据实际硬件调整通道）
#define LCD_SPI_DMA_STREAM    DMA2_Stream3
#define LCD_SPI_DMA_CHANNEL   DMA_Channel_3
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
 * \brief SPI初始化
 * \return 无
 */
static void drv_lcd_spi_init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;           // 修正：时钟空闲时为低
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;          // 修正：第一个时钟沿采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  // 降低速率（如84MHz/16=5.25MHz）
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
}

/**
* \brief 初始化SPI DMA传输
* \return 无
*/
static void drv_lcd_dma_init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    // 使能DMA时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    
    // 复位DMA流
    DMA_DeInit(LCD_SPI_DMA_STREAM);
    
    // 等待DMA流复位完成
    while (DMA_GetCmdStatus(LCD_SPI_DMA_STREAM) != DISABLE);
    
    // 配置DMA参数
    DMA_InitStructure.DMA_Channel = LCD_SPI_DMA_CHANNEL;                      // SPI对应的DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;            // 外设地址(SPI数据寄存器)
    DMA_InitStructure.DMA_Memory0BaseAddr = 0;                                 // 内存地址(临时设置)
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                    // 内存到外设模式
    DMA_InitStructure.DMA_BufferSize = 0;                                      // 传输长度(临时设置)
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;           // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                    // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;    // 外设数据宽度(字节)
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;            // 内存数据宽度(字节)
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                              // 正常模式(非循环)
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                        // 高优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    
    DMA_Init(LCD_SPI_DMA_STREAM, &DMA_InitStructure);
    DMA_Cmd(LCD_SPI_DMA_STREAM, ENABLE);
}

/**
 * \brief GPIO初始化
 * \return 无
 */
static void drv_lcd_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);

    // 配置DC、RES、BLK、CS为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5; // RES(PC4), DC(PC5)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_4|GPIO_Pin_5);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0; // CS(PB1), BLK(PB0)
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_1 | GPIO_Pin_0);

    // 配置SPI1引脚(PA5=SCK, PA6=MISO, PA7=MOSI)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource7, GPIO_AF_SPI1);
}

/**
 * \brief LCD初始化
 * \return 无
 */
void drv_lcd_init(void)
{
    drv_lcd_gpio_init();
    drv_lcd_spi_init();
    drv_lcd_dma_init();  // 初始化DMA
}

/**
 * \brief 使用DMA批量发送数据
 * \param data: 数据缓冲区
 * \param len: 数据长度(字节)
 * \return 无
 */
void drv_lcd_dma_send_data(const uint8_t *data, uint32_t len)
{
    if (len == 0 || data == NULL) return;
    
    // 关闭DMA
    DMA_Cmd(LCD_SPI_DMA_STREAM, DISABLE);
    while (DMA_GetCmdStatus(LCD_SPI_DMA_STREAM) != DISABLE);
    
    // 设置传输参数
    LCD_SPI_DMA_STREAM->M0AR = (uint32_t)data;          // 内存地址
    LCD_SPI_DMA_STREAM->NDTR = len;                     // 传输长度
	
    DMA_ClearFlag(LCD_SPI_DMA_STREAM, DMA_FLAG_TCIF3);
    
    // 使能SPI DMA发送
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(LCD_SPI_DMA_STREAM, ENABLE);
    
    // 等待传输完成
    while (DMA_GetFlagStatus(LCD_SPI_DMA_STREAM, DMA_FLAG_TCIF3) == RESET);

    // 【关键添加：等待SPI发送缓冲区为空】
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    // 等待SPI总线忙标志清除（最后一个字节发送完毕）
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
    
    // 关闭SPI DMA
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
}

/**
 * \brief SPI读写一个字节
 * \param byte: 要读写的字节
 * \return 读取到的字节
 */
uint8_t drv_lcd_spi_read_write_byte(uint8_t byte)
{
    uint32_t retry = 0;
    if (SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_OVR) != RESET) SPI_I2S_ClearFlag(SPI1, SPI_FLAG_OVR);
    if (SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_MODF) != RESET) SPI_I2S_ClearFlag(SPI1, SPI_FLAG_MODF);

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        if (++retry > 1000) return 0xFF;
    retry = 0;

    SPI_I2S_SendData(SPI1, byte);

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        if (++retry > 1000) return 0xFF;
    retry = 0;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) != RESET)
        if (++retry > 1000) return 0xFF;

    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

/**
 * \brief 写寄存器
 * \param reg: 寄存器地址
 * \return 无
 */
void drv_lcd_write_reg(uint8_t reg)
{
    LCD_CS_LOW();    // 选中LCD
    LCD_DC_LOW();    // 命令模式
    drv_lcd_spi_read_write_byte(reg);
    LCD_CS_HIGH();   // 取消选中
}

/**
 * \brief 写数据
 * \param data: 数据
 * \return 无
 */
void drv_lcd_write_byte(uint8_t data)
{
    LCD_CS_LOW();
    LCD_DC_HIGH();   // 数据模式
    drv_lcd_spi_read_write_byte(data);
    LCD_CS_HIGH();
}

/**
 * \brief 写半字
 * \param data: 数据
 * \return 无
 */
void drv_lcd_write_halfword(uint16_t data)
{
    LCD_CS_LOW();
    LCD_DC_HIGH();   // 数据模式
    drv_lcd_spi_read_write_byte(data >> 8);  // 高8位
    drv_lcd_spi_read_write_byte(data & 0xFF); // 低8位
    LCD_CS_HIGH();
}

/**
 * \brief 批量写半字
 * \param dat: 数据
 * \param len: 数据长度
 * \return 无
 */
void drv_lcd_write_halfword_array(const uint16_t *dat, uint32_t len)
{
    int i;
    LCD_CS_LOW();
    LCD_DC_HIGH();
    for(i = 0; i < len; i++)
    {
        drv_lcd_spi_read_write_byte(dat[i] >> 8);
        drv_lcd_spi_read_write_byte(dat[i] & 0xFF);
    }
    LCD_CS_HIGH();
}

