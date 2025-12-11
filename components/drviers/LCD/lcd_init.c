#include "lcd_init.h"
#include "spi.h"
#include "SysTick.h"

void LCD_GPIO_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);//使能时钟


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	

}


/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	LCD_CS_Clr();
	SPI1_ReadWriteByte(dat);
	LCD_CS_Set();
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
}

void LCD_Init(void)
{
	
	LCD_GPIO_Init();//初始化GPIO
	SPI1_Init();
	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);
//	DMA_init();
	
	delay_ms(25);
	LCD_RES_Clr();//复位
	delay_ms(250);
	LCD_RES_Set();
	delay_ms(50);
	
	LCD_BLK_Set();//打开背光
	delay_ms(100);
	
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11);
	delay_ms(100); //Delay 120ms
	LCD_WR_REG(0X36);// Memory Access Control
	LCD_WR_DATA8(0x00);
//	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
//	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
//	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
//	else LCD_WR_DATA8(0xA0);

	
	LCD_WR_REG(0x3a);
	LCD_WR_DATA8(0x65);
	//--------------------------------ST7789V Frame rate setting----------------------------------//
	LCD_WR_REG(0xb2);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);
	LCD_WR_REG(0xb7);
	LCD_WR_DATA8(0x72);
	//---------------------------------ST7789V Power setting--------------------------------------//
	LCD_WR_REG(0xbb);
	LCD_WR_DATA8(0x3D);
	LCD_WR_REG(0xc0);
	LCD_WR_DATA8(0x2c);
	LCD_WR_REG(0xc2);
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0xc3);
	LCD_WR_DATA8(0x19);
	LCD_WR_REG(0xc4);
	LCD_WR_DATA8(0x20);
	LCD_WR_REG(0xc6);
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0xd0);
	LCD_WR_DATA8(0xa4);
	LCD_WR_DATA8(0xa1);
//	LCD_WR_REG(0xd6);
//	LCD_WR_DATA8(0xa1);
	//--------------------------------ST7789V gamma setting---------------------------------------//
	LCD_WR_REG(0xe0);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x31);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x48);
	LCD_WR_DATA8(0x17);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x31);
	LCD_WR_DATA8(0x34);
	LCD_WR_REG(0xe1);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x31);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x48);
	LCD_WR_DATA8(0x17);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x31);
	LCD_WR_DATA8(0x34);
//	LCD_WR_REG(0xe4);
//	LCD_WR_DATA8(0x25);
//	LCD_WR_DATA8(0x00);
//	LCD_WR_DATA8(0x00);
//	LCD_WR_REG(0x21);

	LCD_WR_REG(0x21);
	LCD_WR_REG(0x11);
	LCD_WR_REG(0x13);
	LCD_WR_REG(0x29);
	delay_ms(50);
	
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	
//	LCD_WR_REG(0x11);
//	delay_ms (120);
	
//	LCD_Clear(RED);                        /*先手动清屏再显示，防止花屏显示*/
//	Lcd_GramScan(1);   //横竖屏修改

//	LCD_WR_REG(0x29);
} 








