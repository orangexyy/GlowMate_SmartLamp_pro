#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
#include "stm32f4xx_dma.h"

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr);//≈‰÷√DMA1_CHx
void MYDMA_Config1(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr);
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx);
		   
#endif



