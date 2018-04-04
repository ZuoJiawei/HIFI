#ifndef __I2S_H
#define __I2S_H
#include "sys.h"    									
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//I2S ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define IIS_BEGIN     PFout(3) 
#if     RED_DRAGONBOARD
#define I2Sx_WS_PIN                    GPIO_Pin_15
#define I2Sx_WS_GPIO_PORT              GPIOA
#define I2Sx_WS_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define I2Sx_WS_SOURCE                 GPIO_PinSource15
#define I2Sx_WS_AF                     GPIO_AF_SPI3

#define I2Sx_CK_PIN                    GPIO_Pin_3
#define I2Sx_CK_GPIO_PORT              GPIOB
#define I2Sx_CK_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define I2Sx_CK_SOURCE                 GPIO_PinSource3
#define I2Sx_CK_AF                     GPIO_AF_SPI3

#define I2Sx_SD_PIN                    GPIO_Pin_5
#define I2Sx_SD_GPIO_PORT              GPIOB
#define I2Sx_SD_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define I2Sx_SD_SOURCE                 GPIO_PinSource5
#define I2Sx_SD_AF                     GPIO_AF_SPI3

#define I2Sx_MCLK_PIN                    GPIO_Pin_7
#define I2Sx_MCLK_GPIO_PORT              GPIOC
#define I2Sx_MCLK_GPIO_CLK               RCC_AHB1Periph_GPIOC
#define I2Sx_MCLK_SOURCE                 GPIO_PinSource7
#define I2Sx_MCLK_AF                     GPIO_AF_SPI3
#endif
extern void (*i2s_tx_callback)(void);		//IIS TX�ص�����ָ��  
void IIS_plugin(void);
void I2S2_Init(u16 I2S_Standard ,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat);  
u8 I2S2_SampleRate_Set(u32 samplerate);
void I2S2_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num); 
void I2S_Play_Start(u8 mode); 
void I2S_Play_Stop(u8 mode);  

void I2S3_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat);
u8 I2S3_SampleRate_Set(u32 samplerate);
void I2S3_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num);
#endif





















