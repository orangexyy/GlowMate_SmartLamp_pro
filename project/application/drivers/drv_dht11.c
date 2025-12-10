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
#include "stm32f4xx.h"                  // Device header
#include "drv_dht11.h"
#include "system.h"
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
 * @brief 初始化DHT11的IO口 DQ 同时检测DHT11的存在
 * @return 1:不存在
 * @return 0:存在    	 
 */
uint8_t drv_dht11_init(void)
{	 
 	// GPIO_InitTypeDef  GPIO_InitStructure;	
 	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 
 	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 
 	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
 	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	// GPIO_Init(GPIOA, &GPIO_InitStructure);			
 	// GPIO_SetBits(GPIOA,GPIO_Pin_12);			
			    
	// drv_dht11_rst();
	// return drv_dht11_check();
} 
      
/**
 * @brief 复位DHT11
 */
void drv_dht11_rst(void)	   
{                 
	DHT11_IO_OUT(); 	
	DHT11_DQ_OUT=0; 	
	delay_ms(30);    	
	DHT11_DQ_OUT=1; 	
	delay_us(30);    
}

/**
 * @brief 等待DHT11的回应
 * @return 1:未检测到DHT11的存在
 * @return 0:存在
 */
uint8_t drv_dht11_check(void) 	   
{   
	u8 retry=0;
	DHT11_IO_IN();
    while (DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

/**
 * @brief 从DHT11读取一个位
 * @return 1/0
 */
uint8_t drv_dht11_read_bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}

/**
 * @brief 从DHT11读取一个字节
 * @return 读到的数据
 */
uint8_t drv_dht11_read_byte(void)    
{        
	u8 i,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
		dat<<=1; 
		dat|=drv_dht11_read_bit();
	}						    
	return dat;
}

/**
 * @brief 从DHT11读取一次数据
 * @param temp:温度值(范围:0~50°)
 * @param humi:湿度值(范围:20%~90%)
 * @return 0,正常;1,读取失败
 */
uint8_t drv_dht11_read_data(uint8_t *temp,uint8_t *humi)    
{        
 	u8 buf[5];
	u8 i;
	drv_dht11_rst();
	if(drv_dht11_check()==0)
	{
		for(i=0;i<5;i++)
		{
			buf[i]=drv_dht11_read_byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
	}
	else return 1;
	return 0;	    
}



