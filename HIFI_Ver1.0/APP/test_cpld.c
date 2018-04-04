#include "wavplay.h" 
#include "audioplay.h"
#include "usart.h" 
#include "delay.h" 
#include "malloc.h"
#include "ff.h"
#include "i2s.h"
#include "wm8978.h"
#include "key.h"
#include "led.h"
#include "AK4595.h"
#include "myiic.h"
#include "nokia5110.h"
#include "test_cpld.h"
vu8 testtransferend=0;	//i2s传输完成标志
vu8 testwitchbuf=0;		//i2sbufx指示标志
void test_I2Sdma(void);
void test_i2s_dma_tx_callback(void) ;
u8 test_buf_fill(u8 *buf,u16 size,u8 bits);

void test_I2Sdma(void)
{
  u32 fillnum=0; 
	audiodev.i2sbuf1=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audiodev.i2sbuf2=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audiodev.tbuf=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
  AK4495_I2S_Cfg(0x00,32,192000);	//飞利浦标准,32位数据长度
	I2S3_Init(I2S_Standard_LSB,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_32b);
	I2S3_SampleRate_Set(192000);//设置采样率
	I2S3_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE/2); //配置TX DMA
	audio_stop(0x01); 
	i2s_tx_callback=test_i2s_dma_tx_callback;			//回调函数指test_i2s_dma_callback
	fillnum=test_buf_fill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,32);
	fillnum=test_buf_fill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,32);
	audio_start(0x01); 
	while(1)
	{
		if(testwitchbuf)fillnum=test_buf_fill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,32);//填充buf2
		else fillnum=test_buf_fill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,32);//填充buf1	
		while(testtransferend==0);//等待test数据传输完成; 
		testtransferend=0;
	}	
}

void test_i2s_dma_tx_callback(void) 
{   
	u16 i;
	if(DMA1_Stream5->CR&(1<<19))
	{
		testwitchbuf=0;
	}
	else 
	{
		testwitchbuf=1;
	}
	testtransferend=1;
} 

u8 test_buf_fill(u8 *buf,u16 size,u8 bits)
{
	u32 i;
	for(i=0;i<size;)
	{
		buf[i++]=0x55;
		buf[i++]=0x00;
	} 
	return 0;
}
