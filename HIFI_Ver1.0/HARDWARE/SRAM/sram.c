#include "sram.h"	  
#include "usart.h"	     
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

#define FSMC_BASE_AADR             0X60000001;
#define FSMC_DMA                   DMA2
#define FSMC_DMA_CLK               RCC_AHB1Periph_DMA2

#define FSMC_DMA_STREAM            DMA2_Stream7
#define FSMC_DMA_CHANNEL           DMA_Channel_1
#define FSMC_DMA_FLAG_FEIF         DMA_FLAG_FEIF7
#define FSMC_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF7
#define FSMC_DMA_FLAG_TEIF         DMA_FLAG_TEIF7
#define FSMC_DMA_FLAG_HTIF         DMA_FLAG_HTIF7
#define FSMC_DMA_FLAG_TCIF         DMA_FLAG_TCIF7
#define FSMC_DMA_IRQn              DMA2_Stream7_IRQn
#define FSMC_DMA_IRQHANDLER        DMA2_Stream7_IRQHandler

#define FSMC_DMA2                   DMA2
#define FSMC_DMA2_CLK               RCC_AHB1Periph_DMA2
#define FSMC_DMA2_STREAM            DMA2_Stream6
#define FSMC_DMA2_CHANNEL           DMA_Channel_1
#define FSMC_DMA2_FLAG_FEIF         DMA_FLAG_FEIF6
#define FSMC_DMA2_FLAG_DMEIF        DMA_FLAG_DMEIF6
#define FSMC_DMA2_FLAG_TEIF         DMA_FLAG_TEIF6
#define FSMC_DMA2_FLAG_HTIF         DMA_FLAG_HTIF6
#define FSMC_DMA2_FLAG_TCIF         DMA_FLAG_TCIF6
#define FSMC_DMA2_IRQn              DMA2_Stream6_IRQn
#define FSMC_DMA2_IRQHANDLER        DMA2_Stream6_IRQHandler
//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))	
 

vu8 fsmctransferend=0;	//i2s传输完成标志
vu8 fsmcwitchbuf=0;		//i2sbufx指示标志
u8 fsmc_buf_fill(u8 *buf,u16 size,u8 bits);
void test_FSMCdma(void);
void fsmc_dma_tx_callback(void) ;
//初始化外部SRAM
void FSMC_SRAM_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);//使能PD,PE,PF,PG时钟  
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//使能FSMC时钟  
   
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PB15 推挽输出,控制背光
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化 //PB15 推挽输出,控制背光

	GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(0XFF<<8);//PD0,1,4,5,8~15 AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化  
	
  GPIO_InitStructure.GPIO_Pin = (3<<0)|(0X1FF<<7);//PE0,1,7~15,AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化  
	
 	GPIO_InitStructure.GPIO_Pin = (0X3F<<0)|(0XF<<12); 	//PF0~5,12~15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化  

	GPIO_InitStructure.GPIO_Pin =(0X3F<<0)| GPIO_Pin_10;//PG0~5,10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化 
 
 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//PD0,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//PD1,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//PD15,AF12
 
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12
 
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_FSMC);//PF0,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_FSMC);//PF1,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource2,GPIO_AF_FSMC);//PF2,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource3,GPIO_AF_FSMC);//PF3,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource4,GPIO_AF_FSMC);//PF4,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource5,GPIO_AF_FSMC);//PF5,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);//PF12,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource13,GPIO_AF_FSMC);//PF13,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource14,GPIO_AF_FSMC);//PF14,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource15,GPIO_AF_FSMC);//PF15,AF12
	
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource0,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource1,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource2,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource3,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource5,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource10,GPIO_AF_FSMC);
	
 	  
 	readWriteTiming.FSMC_AddressSetupTime = 0x00;	 //地址建立时间（ADDSET）为1个HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
  readWriteTiming.FSMC_DataSetupTime = 0x08;		 ////数据保持时间（DATAST）为9个HCLK 6*9=54ns	 	 
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    

 
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;//  这里我们使用NE3 ，也就对应BTCR[4],[5]。
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit  
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//存储器写使能 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; // 读写使用相同的时序
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming; //读写同样时序

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // 使能BANK3										  
											
}
	  														  
//在指定地址(WriteAddr+Bank1_SRAM3_ADDR)开始,连续写入n个字节.
//pBuffer:字节指针
//WriteAddr:要写入的地址
//n:要写入的字节数
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)  
	{										    
		*(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;	  
		WriteAddr++;
		pBuffer++;
	}   
}																			    
//在指定地址((WriteAddr+Bank1_SRAM3_ADDR))开始,连续读出n个字节.
//pBuffer:字节指针
//ReadAddr:要读出的起始地址
//n:要写入的字节数
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)  
	{											    
		*pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);    
		ReadAddr++;
	}  
} 
////////////////////////////////////////////////////////////////////////////////////////
//测试函数
//在指定地址写入1个字节
//addr:地址
//data:要写入的数据
void fsmc_sram_test_write(u32 addr,u8 data)
{			   
	FSMC_SRAM_WriteBuffer(&data,addr,1);//写入1个字节
}
//读取1个字节
//addr:要读取的地址
//返回值:读取到的数据
u8 fsmc_sram_test_read(u32 addr)
{
	u8 data;
	FSMC_SRAM_ReadBuffer(&data,addr,1);
	return data;
}

///************************************************************************************
//FSMC TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
//************************************************************************************/
static void FSMC_DMA1_Init(u8* buf0,u8 *buf1,u16 num)
{        
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;    
    /* Enable the DMA2 Clock */
    RCC_AHB1PeriphClockCmd(FSMC_DMA_CLK, ENABLE);
    /* DMA2 Stream7 disable */
    DMA_Cmd(FSMC_DMA_STREAM, DISABLE);
    DMA_ClearFlag(FSMC_DMA_STREAM, FSMC_DMA_FLAG_FEIF | FSMC_DMA_FLAG_DMEIF | FSMC_DMA_FLAG_TEIF | 
                  FSMC_DMA_FLAG_HTIF | FSMC_DMA_FLAG_TCIF);
    DMA_DeInit(FSMC_DMA_STREAM);			
    DMA_InitStructure.DMA_Channel = FSMC_DMA_CHANNEL;                           
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)buf1;           
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buf0;                        
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;                    
    DMA_InitStructure.DMA_BufferSize = (uint32_t)num;                        
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                      
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;               
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                 
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         

    DMA_Init(FSMC_DMA_STREAM, &DMA_InitStructure);
}

///************************************************************************************
//FSMC TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
//************************************************************************************/
static void FSMC_DMA2_Init(u8* buf0,u8 *buf1,u16 num)
{        
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;    
    /* Enable the DMA2 Clock */
    RCC_AHB1PeriphClockCmd(FSMC_DMA2_CLK, ENABLE);
    /* DMA2 Stream7 disable */
    DMA_Cmd(FSMC_DMA2_STREAM, DISABLE);
    DMA_ClearFlag(FSMC_DMA2_STREAM, FSMC_DMA2_FLAG_FEIF | FSMC_DMA2_FLAG_DMEIF | FSMC_DMA2_FLAG_TEIF | 
                  FSMC_DMA2_FLAG_HTIF | FSMC_DMA2_FLAG_TCIF);
    DMA_DeInit(FSMC_DMA2_STREAM);			
    DMA_InitStructure.DMA_Channel = FSMC_DMA2_CHANNEL;                           
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)buf1;           
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buf0;                        
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;                    
    DMA_InitStructure.DMA_BufferSize = (uint32_t)num;                        
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                      
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;               
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                 
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         
    DMA_Init(FSMC_DMA_STREAM, &DMA_InitStructure);
}

void test_FSMCdma(void)
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
	i2s_tx_callback=fsmc_dma_tx_callback;			//回调函数指test_i2s_dma_callback
	fillnum=fsmc_buf_fill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,32);
	fillnum=fsmc_buf_fill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,32);
	audio_start(0x01); 
	while(1)
	{
		if(fsmcwitchbuf)fillnum=fsmc_buf_fill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,32);//填充buf2
		else fillnum=fsmc_buf_fill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,32);//填充buf1	
		while(fsmctransferend==0);//等待test数据传输完成; 
		fsmctransferend=0;
	}	
}

void fsmc_dma_tx_callback(void) 
{   
	u16 i;
	if(DMA2_Stream6->CR&(1<<19))
	{
		fsmcwitchbuf=0;
	}
	else if (DMA2_Stream7->CR&(1<<19))
	{
		fsmcwitchbuf=1;
	}
	fsmctransferend=1;
} 

u8 fsmc_buf_fill(u8 *buf,u16 size,u8 bits)
{
	u32 i;
	for(i=0;i<size;)
	{
		buf[i++]=0x55;
		buf[i++]=0x00;
	} 
	return 0;
}



