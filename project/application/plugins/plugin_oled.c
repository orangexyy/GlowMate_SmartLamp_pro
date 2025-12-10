#include "stm32f10x.h"
#include "sys.h"
#include "stdint.h"
#include "drv_oled.h"
#include "plugin_oled.h"
#include "plugin_oled_font.h"


/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void plugin_oled_write_command(uint8_t Command)
{
	drv_oled_i2c_start();
	drv_oled_i2c_send_byte(0x78);		//从机地址
	drv_oled_i2c_send_byte(0x00);		//写命令
	drv_oled_i2c_send_byte(Command); 
	drv_oled_i2c_stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void plugin_oled_write_data(uint8_t Data)
{
	drv_oled_i2c_start();
	drv_oled_i2c_send_byte(0x78);		//从机地址
	drv_oled_i2c_send_byte(0x40);		//写数据
	drv_oled_i2c_send_byte(Data);
	drv_oled_i2c_stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void plugin_oled_set_cursor(uint8_t Y, uint8_t X)
{
	plugin_oled_write_command(0xB0 | Y);					//设置Y位置
	plugin_oled_write_command(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	plugin_oled_write_command(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void plugin_oled_clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		plugin_oled_set_cursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			plugin_oled_write_data(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void plugin_oled_show_char(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	plugin_oled_set_cursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		plugin_oled_write_data(plugin_oled_font_8x16[Char - ' '][i]);			//显示上半部分内容
	}
	plugin_oled_set_cursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		plugin_oled_write_data(plugin_oled_font_8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void plugin_oled_show_string(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		plugin_oled_show_char(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t plugin_oled_pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void plugin_oled_show_num(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		plugin_oled_show_char(Line, Column + i, Number / plugin_oled_pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void plugin_oled_show_signed_num(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		plugin_oled_show_char(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		plugin_oled_show_char(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		plugin_oled_show_char(Line, Column + i + 1, Number1 / plugin_oled_pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void plugin_oled_show_hex_num(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / plugin_oled_pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			plugin_oled_show_char(Line, Column + i, SingleNumber + '0');
		}	
		else
		{
			plugin_oled_show_char(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void plugin_oled_show_bin_num(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		plugin_oled_show_char(Line, Column + i, Number / plugin_oled_pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void plugin_oled_init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	drv_oled_i2c_init();			//端口初始化
	
	plugin_oled_write_command(0xAE);	//关闭显示
	
	plugin_oled_write_command(0xD5);	//设置显示时钟分频比/振荡器频率
	plugin_oled_write_command(0x80);
	
	plugin_oled_write_command(0xA8);	//设置多路复用率
	plugin_oled_write_command(0x3F);
	
	plugin_oled_write_command(0xD3);	//设置显示偏移
	plugin_oled_write_command(0x00);
	
	plugin_oled_write_command(0x40);	//设置显示开始行
	
	plugin_oled_write_command(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	plugin_oled_write_command(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	plugin_oled_write_command(0xDA);	//设置COM引脚硬件配置
	plugin_oled_write_command(0x12);
	
	plugin_oled_write_command(0x81);	//设置对比度控制
	plugin_oled_write_command(0xCF);

	plugin_oled_write_command(0xD9);	//设置预充电周期
	plugin_oled_write_command(0xF1);

	plugin_oled_write_command(0xDB);	//设置VCOMH取消选择级别
	plugin_oled_write_command(0x30);

	plugin_oled_write_command(0xA4);	//设置整个显示打开/关闭

	plugin_oled_write_command(0xA6);	//设置正常/倒转显示

	plugin_oled_write_command(0x8D);	//设置充电泵
	plugin_oled_write_command(0x14);

	plugin_oled_write_command(0xAF);	//开启显示
		
	plugin_oled_clear();				//OLED清屏
}
