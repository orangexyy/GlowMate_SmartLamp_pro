
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
#include "stdint.h"
#include "drv_lcd.h"
#include "plugin_lcd.h"
#include "plugin_lcd_font.h"
/******************************************************************************\
                             Macro definitions
\******************************************************************************/
#define LCD_RES_LOW()  	GPIO_ResetBits(GPIOC, GPIO_Pin_4)   	//RES
#define LCD_RES_HIGH()  GPIO_SetBits(GPIOC, GPIO_Pin_4)

#define LCD_DC_LOW()   	GPIO_ResetBits(GPIOC, GPIO_Pin_5) 		//DC
#define LCD_DC_HIGH()   GPIO_SetBits(GPIOC, GPIO_Pin_5)

#define LCD_BLK_LOW()  	GPIO_ResetBits(GPIOB, GPIO_Pin_0)   	//BLK
#define LCD_BLK_HIGH()  GPIO_SetBits(GPIOB, GPIO_Pin_0)
 		     
#define LCD_CS_LOW()   	GPIO_ResetBits(GPIOB, GPIO_Pin_1)   	//CS
#define LCD_CS_HIGH()   GPIO_SetBits(GPIOB, GPIO_Pin_1)


#define USE_HORIZONTAL 1  	


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 128
#endif
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
 * \brief 初始化函数
 * \return 无
 */
void plugin_lcd_init(void)
{
    drv_lcd_init();

    // 复位屏幕
    LCD_RES_LOW();
    delay_ms(100);
    LCD_RES_HIGH();
    delay_ms(120);  // 确保复位完成

    LCD_BLK_HIGH();  // 打开背光
    delay_ms(50);

    // ST7735初始化序列（通用版）
    drv_lcd_write_reg(0x11);  // 退出睡眠模式
    delay_ms(120);

    // 内存方向配置（根据屏幕调整）
    drv_lcd_write_reg(0x36);
    if(USE_HORIZONTAL==0) drv_lcd_write_byte(0x00);
    else if(USE_HORIZONTAL==1) drv_lcd_write_byte(0xC0);  // 水平翻转
    else if(USE_HORIZONTAL==2) drv_lcd_write_byte(0x70);
    else drv_lcd_write_byte(0xA0);

    // 像素格式：65K色
    drv_lcd_write_reg(0x3A);
    drv_lcd_write_byte(0x05);

    // 显示打开
    drv_lcd_write_reg(0x29);
}

/**
 * \brief 地址设置函数
 * \param x1: 起始X坐标
 * \param y1: 起始Y坐标
 * \param x2: 结束X坐标
 * \param y2: 结束Y坐标
 * \return 无
 */
void plugin_lcd_address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    if(USE_HORIZONTAL==0)
	{
		drv_lcd_write_reg(0x2a);//列地址设置
		drv_lcd_write_halfword(x1+2);
		drv_lcd_write_halfword(x2+2);
		drv_lcd_write_reg(0x2b);//行地址设置
		drv_lcd_write_halfword(y1+1);
		drv_lcd_write_halfword(y2+1);
		drv_lcd_write_reg(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		drv_lcd_write_reg(0x2a);//列地址设置
		drv_lcd_write_halfword(x1+2);
		drv_lcd_write_halfword(x2+2);
		drv_lcd_write_reg(0x2b);//行地址设置
		drv_lcd_write_halfword(y1+1);
		drv_lcd_write_halfword(y2+1);
		drv_lcd_write_reg(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		drv_lcd_write_reg(0x2a);//列地址设置
		drv_lcd_write_halfword(x1+1);
		drv_lcd_write_halfword(x2+1);
		drv_lcd_write_reg(0x2b);//行地址设置
		drv_lcd_write_halfword(y1+2);
		drv_lcd_write_halfword(y2+2);
		drv_lcd_write_reg(0x2c);//储存器写
	}
	else
	{
		drv_lcd_write_reg(0x2a);//列地址设置
		drv_lcd_write_halfword(x1+1);
		drv_lcd_write_halfword(x2+1);
		drv_lcd_write_reg(0x2b);//行地址设置
		drv_lcd_write_halfword(y1+2);
		drv_lcd_write_halfword(y2+2);
		drv_lcd_write_reg(0x2c);//储存器写
	}
}

/**
 * \brief LVGL DMA刷屏函数（传输指定颜色缓冲区数据）
 * \param x1: 起始X坐标（包含）
 * \param y1: 起始Y坐标（包含）
 * \param x2: 结束X坐标（包含）
 * \param y2: 结束Y坐标（包含）
 * \param color_map: 16位颜色数组指针（存储待显示的像素数据）
 * \return 无
 */
void plugin_lcd_color_fill_dma(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const uint16_t *color_map)
{
    if (x1 > x2 || y1 > y2 || color_map == NULL) return;

    uint32_t pixel_count = (uint32_t)(x2 - x1 + 1) * (y2 - y1 + 1);
    if (pixel_count == 0) return;

    // 【关键1：缓冲区大小设为“屏幕宽度的整数倍”（如128像素=256字节），避免行跨块】
    static uint8_t dma_buf[256] __attribute__((aligned(4))); // 128像素（16位色），32位对齐
    uint32_t buf_pixel_max = sizeof(dma_buf) / 2;  // 128像素（必须是屏幕宽度的整数倍！）

    plugin_lcd_address_set(x1, y1, x2, y2);
    LCD_CS_LOW();
    LCD_DC_HIGH();

    uint32_t remaining_pixel = pixel_count;
    const uint16_t *color_ptr = color_map;

    while (remaining_pixel > 0)
    {
        // 【关键2：计算当前块的像素数，严格按缓冲区容量取整】
        uint32_t send_pixel = remaining_pixel > buf_pixel_max ? buf_pixel_max : remaining_pixel;
        uint32_t send_bytes = send_pixel * 2;  // 16位色→字节数，必须是偶数！

        // 【关键3：填充缓冲区时，严格循环send_pixel次，不越界】
        for (uint32_t i = 0; i < send_pixel; i++)
        {
            dma_buf[i * 2] = color_ptr[i] >> 8;       // 高8位
            dma_buf[i * 2 + 1] = color_ptr[i] & 0xFF; // 低8位
        }

        // DMA发送当前块
        drv_lcd_dma_send_data(dma_buf, send_bytes);

        // 【关键4：更新指针和剩余像素数，避免漏更/错更】
        remaining_pixel -= send_pixel;
        color_ptr += send_pixel;
    }

    LCD_CS_HIGH();
}
	
/**
 * \brief 填充函数
 * \param xsta: 起始X坐标
 * \param ysta: 起始Y坐标
 * \param xend: 结束X坐标
 * \param yend: 结束Y坐标
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_fill_dma(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{          
    uint32_t pixel_count = (xend - xsta) * (yend - ysta);
    uint8_t *color_buf = NULL;
    
    // 计算所需缓冲区大小，使用栈内存或动态分配
    // 注意：实际应用中建议使用静态缓冲区或根据情况调整大小
    uint8_t temp_buf[1024];  // 临时缓冲区，可根据实际情况调整
    color_buf = temp_buf;
    
    // 填充颜色数据(将16位颜色转换为两个字节)
    uint32_t fill_size = sizeof(temp_buf) / 2;
    for (uint32_t i = 0; i < fill_size; i++)
    {
        color_buf[i*2] = color >> 8;       // 高8位
        color_buf[i*2 + 1] = color & 0xFF; // 低8位
    }
    
    // 设置显示区域
    plugin_lcd_address_set(xsta, ysta, xend - 1, yend - 1);
    
    // 选中LCD
    LCD_CS_LOW();
    LCD_DC_HIGH();  // 数据模式
    
    // 分块发送数据
    uint32_t remaining = pixel_count * 2;  // 总字节数
    const uint8_t *p = color_buf;
    
    while (remaining > 0)
    {
        uint32_t send_len = remaining > sizeof(temp_buf) ? sizeof(temp_buf) : remaining;
        drv_lcd_dma_send_data(p, send_len);
        remaining -= send_len;
    }
    
    // 取消选中
    LCD_CS_HIGH();
}

/**
 * \brief 填充函数
 * \param xsta: 起始X坐标
 * \param ysta: 起始Y坐标
 * \param xend: 结束X坐标
 * \param yend: 结束Y坐标
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{          
    uint16_t i,j; 
    plugin_lcd_address_set(xsta,ysta,xend-1,yend-1);
    for(i=ysta;i<yend;i++)
        for(j=xsta;j<xend;j++)
            drv_lcd_write_halfword(color);
}


/**
 * \brief 画点函数
 * \param x: X坐标
 * \param y: Y坐标
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_draw_point(uint16_t x,uint16_t y,uint16_t color)
{
	plugin_lcd_address_set(x,y,x,y);//设置光标位置  
	drv_lcd_write_halfword(color);
} 


/**
 * \brief 画线函数
 * \param x1: 起始X坐标
 * \param y1: 起始Y坐标
 * \param x2: 结束X坐标
 * \param y2: 结束Y坐标
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_draw_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		plugin_lcd_draw_point(uRow,uCol,color);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/**
 * \brief 画矩形函数
 * \param x1: 起始X坐标
 * \param y1: 起始Y坐标
 * \param x2: 结束X坐标
 * \param y2: 结束Y坐标
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_draw_rectangle(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	plugin_lcd_draw_line(x1,y1,x2,y1,color);
	plugin_lcd_draw_line(x1,y1,x1,y2,color);
	plugin_lcd_draw_line(x1,y2,x2,y2,color);
	plugin_lcd_draw_line(x2,y1,x2,y2,color);
}



/**
 * \brief 画圆函数
 * \param x0: 圆心X坐标
 * \param y0: 圆心Y坐标
 * \param r: 半径
 * \param color: 颜色
 * \return 无
 */
void plugin_lcd_draw_circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		plugin_lcd_draw_point(x0-b,y0-a,color);             //3           
		plugin_lcd_draw_point(x0+b,y0-a,color);             //0           
		plugin_lcd_draw_point(x0-a,y0+b,color);             //1                
		plugin_lcd_draw_point(x0-a,y0-b,color);             //2             
		plugin_lcd_draw_point(x0+b,y0+a,color);             //4               
		plugin_lcd_draw_point(x0+a,y0-b,color);             //5
		plugin_lcd_draw_point(x0+a,y0+b,color);             //6 
		plugin_lcd_draw_point(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//判断要画的点是否过远
		{
			b--;
		}
	}
}

/**
 * \brief 显示字符函数
 * \param x: X坐标
 * \param y: Y坐标
 * \param num: 字符
 * \param fc: 前景色
 * \param bc: 背景色
 * \param sizey: 字符大小
 * \param mode: 模式
 * \return 无
 */
void plugin_lcd_show_char(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	u8 temp,sizex,t,m=0;
	u16 i,TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	sizex=sizey/2;
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';    //得到偏移后的值
	plugin_lcd_address_set(x,y,x+sizex-1,y+sizey-1);  //设置光标位置 
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==12)temp=ascii_1206[num][i];		       //调用6x12字体
		else if(sizey==16)temp=ascii_1608[num][i];		 //调用8x16字体
		else if(sizey==24)temp=ascii_2412[num][i];		 //调用12x24字体
		else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)//非叠加模式
			{
				if(temp&(0x01<<t))drv_lcd_write_halfword(fc);
				else drv_lcd_write_halfword(bc);
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
			}
			else//叠加模式
			{
				if(temp&(0x01<<t))plugin_lcd_draw_point(x,y,fc);//画一个点
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}   	 	  
}


/**
 * \brief 显示字符串函数
 * \param x: X坐标
 * \param y: Y坐标
 * \param p: 字符串
 * \param fc: 前景色
 * \param bc: 背景色
 * \param sizey: 字符大小
 * \param mode: 模式
 * \return 无
 */
void plugin_lcd_show_string(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{         
	while(*p!='\0')
	{       
		plugin_lcd_show_char(x,y,*p,fc,bc,sizey,mode);
		x+=sizey/2;
		p++;
	}  
}

/**
 * \brief 求幂函数
 * \param m: 底数
 * \param n: 指数
 * \return 结果
 */
u32 plugin_lcd_mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;
	return result;
}

/**
 * \brief 显示整数函数
 * \param x: X坐标
 * \param y: Y坐标
 * \param num: 整数
 * \param len: 长度
 * \param fc: 前景色
 * \param bc: 背景色
 * \param sizey: 字符大小
 * \return 无
 */
void plugin_lcd_show_int_num(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{         	
	u8 t,temp;
	u8 enshow=0;
	u8 sizex=sizey/2;
	for(t=0;t<len;t++)
	{
		temp=(num/plugin_lcd_mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				plugin_lcd_show_char(x+t*sizex,y,' ',fc,bc,sizey,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	plugin_lcd_show_char(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
} 


/**
 * \brief 显示浮点数函数
 * \param x: X坐标
 * \param y: Y坐标
 * \param num: 浮点数
 * \param len: 长度
 * \param fc: 前景色
 * \param bc: 背景色
 * \param sizey: 字符大小
 * \return 无
 */
void plugin_lcd_show_float_num(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{         	
	u8 t,temp,sizex;
	u16 num1;
	sizex=sizey/2;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/plugin_lcd_mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			plugin_lcd_show_char(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
			t++;
			len+=1;
		}
	 	plugin_lcd_show_char(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
}


/**
 * \brief 显示图片函数
 * \param x: X坐标
 * \param y: Y坐标
 * \param length: 长度
 * \param width: 宽度
 * \param pic: 图片
 * \return 无
 */
void plugin_lcd_show_picture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[])
{
	u16 i,j;
	u32 k=0;
	plugin_lcd_address_set(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			drv_lcd_write_byte(pic[k*2]);
			drv_lcd_write_byte(pic[k*2+1]);
			k++;
		}
	}
}
