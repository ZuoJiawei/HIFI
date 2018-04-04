#include "i2s.h"  
#include "usart.h"


void IIS_plugin(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(I2Sx_WS_GPIO_CLK|I2Sx_CK_GPIO_CLK|I2Sx_SD_GPIO_CLK|I2Sx_MCLK_GPIO_CLK|RCC_AHB1Periph_GPIOF, ENABLE);			//使能外设GPIOB,GPIOC时钟
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PF3用来做CPLD FIFO端信号的同步，当pf3被拉高时,fifo的读和写都被使能 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化	
	//PB3/5复用功能输出
	GPIO_InitStructure.GPIO_Pin = I2Sx_CK_PIN | I2Sx_SD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(I2Sx_CK_GPIO_PORT, &GPIO_InitStructure);//初始化
	
	//PC2/PC3/PC6复用功能输出
	GPIO_InitStructure.GPIO_Pin = I2Sx_WS_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(I2Sx_WS_GPIO_PORT, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = I2Sx_MCLK_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(I2Sx_MCLK_GPIO_PORT, &GPIO_InitStructure);//初始化
	
	GPIO_PinAFConfig(I2Sx_WS_GPIO_PORT,I2Sx_WS_SOURCE,I2Sx_WS_AF); //PB12,AF5  I2S_LRCK
	GPIO_PinAFConfig(I2Sx_SD_GPIO_PORT,I2Sx_SD_SOURCE,I2Sx_SD_AF);	//PB13,AF5  I2S_SCLK 
	GPIO_PinAFConfig(I2Sx_CK_GPIO_PORT,I2Sx_CK_SOURCE,I2Sx_CK_AF);	//PC3 ,AF5  I2S_DACDATA 
	GPIO_PinAFConfig(I2Sx_MCLK_GPIO_PORT,I2Sx_MCLK_SOURCE,I2Sx_MCLK_AF);	//PC6 ,AF5  I2S_MCK
//	GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF6_SPI2);	//PC2 ,AF6  I2S_ADCDATA  I2S2ext_SD是AF6!!!

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);			//使能外设GPIOB,GPIOC时钟
     
	//PB12/13 复用功能输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	//PC2/PC3/PC6复用功能输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2); //PB12,AF5  I2S_LRCK
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);	//PB13,AF5  I2S_SCLK 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);	//PC3 ,AF5  I2S_DACDATA 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_SPI2);	//PC6 ,AF5  I2S_MCK
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF6_SPI2);	//PC2 ,AF6  I2S_ADCDATA  I2S2ext_SD是AF6!!!
	IIS_BEGIN=0;
}

void I2S3_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat)
{ 
  I2S_InitTypeDef I2S_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//使能SPI3时钟
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE); //复位SPI3
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);//结束复位
  
	I2S_InitStructure.I2S_Mode=I2S_Mode;//IIS模式
	I2S_InitStructure.I2S_Standard=I2S_Standard;//IIS标准
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat;//IIS数据长度
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;//主时钟输出禁止
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_Default;//IIS频率设置
	I2S_InitStructure.I2S_CPOL=I2S_Clock_Polarity;//空闲状态时钟电平
	I2S_Init(SPI3,&I2S_InitStructure);//初始化IIS

 
	SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);//SPI3 TX DMA请求使能.
  I2S_Cmd(SPI3,ENABLE);//SPI3 I2S EN使能.	
} 
//I2S2初始化

//参数I2S_Standard:  @ref SPI_I2S_Standard  I2S标准,
//I2S_Standard_Phillips,飞利浦标准;
//I2S_Standard_MSB,MSB对齐标准(右对齐);
//I2S_Standard_LSB,LSB对齐标准(左对齐);
//I2S_Standard_PCMShort,I2S_Standard_PCMLong:PCM标准
//参数I2S_Mode:  @ref SPI_I2S_Mode  I2S_Mode_SlaveTx:从机发送;I2S_Mode_SlaveRx:从机接收;I2S_Mode_MasterTx:主机发送;I2S_Mode_MasterRx:主机接收;
//参数I2S_Clock_Polarity   @ref SPI_I2S_Clock_Polarity:  I2S_CPOL_Low,时钟低电平有效;I2S_CPOL_High,时钟高电平有效
//参数I2S_DataFormat： @ref SPI_I2S_Data_Format :数据长度,I2S_DataFormat_16b,16位标准;I2S_DataFormat_16bextended,16位扩展(frame=32bit);I2S_DataFormat_24b,24位;I2S_DataFormat_32b,32位.
void I2S2_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat)
{ 
  I2S_InitTypeDef I2S_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//使能SPI2时钟
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE); //复位SPI2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//结束复位
  
	I2S_InitStructure.I2S_Mode=I2S_Mode;//IIS模式
	I2S_InitStructure.I2S_Standard=I2S_Standard;//IIS标准
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat;//IIS数据长度
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;//主时钟输出禁止
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_Default;//IIS频率设置
	I2S_InitStructure.I2S_CPOL=I2S_Clock_Polarity;//空闲状态时钟电平
	I2S_Init(SPI2,&I2S_InitStructure);//初始化IIS

 
	SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);//SPI2 TX DMA请求使能.
  I2S_Cmd(SPI2,ENABLE);//SPI2 I2S EN使能.	
} 
//采样率计算公式:Fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//一般HSE=8Mhz 
//pllm:在Sys_Clock_Set设置的时候确定，一般是8
//PLLI2SN:一般是192~432 
//PLLI2SR:2~7
//I2SDIV:2~255
//ODD:0/1
//I2S分频系数表@pllm=8,HSE=8Mhz,即vco输入频率为1Mhz
//表格式:采样率/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const u16 I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz采样率
	{1102,429,4,19,0},		//11.025Khz采样率 
	{1600,213,2,13,0},		//16Khz采样率
	{2205,429,4, 9,1},		//22.05Khz采样率
	{3200,213,2, 6,1},		//32Khz采样率
	{4410,271,2, 6,0},		//44.1Khz采样率
	{4800,258,3, 3,1},		//48Khz采样率
	{8820,316,2, 3,1},		//88.2Khz采样率
	{9600,344,2, 3,1},  	//96Khz采样率
	{17640,361,2,2,0},  	//176.4Khz采样率 
	{19200,393,2,2,0},  	//192Khz采样率
};  
//设置IIS的采样率(@MCKEN)
//samplerate:采样率,单位:Hz
//返回值:0,设置成功;1,无法设置.
u8 I2S2_SampleRate_Set(u32 samplerate)
{ 
	u8 i=0; 
	u32 tempreg=0;
	samplerate/=10;//缩小10倍   
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//看看改采样率是否可以支持
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
 
	RCC_PLLI2SCmd(DISABLE);//先关闭PLLI2S
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//搜遍了也找不到
	RCC_PLLI2SConfig((u32)I2S_PSC_TBL[i][1],(u32)I2S_PSC_TBL[i][2]);//设置I2SxCLK的频率(x=2)  设置PLLI2SN PLLI2SR
 
	RCC->CR|=1<<26;					//开启I2S时钟
	while((RCC->CR&1<<27)==0);		//等待I2S时钟开启成功. 
	tempreg=I2S_PSC_TBL[i][3]<<0;	//设置I2SDIV
	tempreg|=I2S_PSC_TBL[i][4]<<8;	//设置ODD位
	tempreg|=1<<9;					//使能MCKOE位,输出MCK
	SPI2->I2SPR=tempreg;			//设置I2SPR寄存器 
	return 0;
}  

//设置IIS的采样率(@MCKEN)
//samplerate:采样率,单位:Hz
//返回值:0,设置成功;1,无法设置.
u8 I2S3_SampleRate_Set(u32 samplerate)
{ 
	u8 i=0; 
	u32 tempreg=0;
	samplerate/=10;//缩小10倍   
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//看看改采样率是否可以支持
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
 
	RCC_PLLI2SCmd(DISABLE);//先关闭PLLI2S
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//搜遍了也找不到
	RCC_PLLI2SConfig((u32)I2S_PSC_TBL[i][1],(u32)I2S_PSC_TBL[i][2]);//设置I2SxCLK的频率(x=2)  设置PLLI2SN PLLI2SR
 
	RCC->CR|=1<<26;					//开启I2S时钟
	while((RCC->CR&1<<27)==0);		//等待I2S时钟开启成功. 
	tempreg=I2S_PSC_TBL[i][3]<<0;	//设置I2SDIV
	tempreg|=I2S_PSC_TBL[i][4]<<8;	//设置ODD位
	tempreg|=1<<9;					//使能MCKOE位,输出MCK
	SPI3->I2SPR=tempreg;			//设置I2SPR寄存器 
	return 0;
} 
//I2S2 TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
void I2S2_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num)
{  
	NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能 
	
	DMA_DeInit(DMA1_Stream4);
	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}//等待DMA1_Stream1可配置 
		
  /* 配置 DMA Stream */

  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //通道0 SPI2_TX通道 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR;//外设地址为:(u32)&SPI2->DR
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
  DMA_InitStructure.DMA_BufferSize = num;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度：16位 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//高优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //不使用FIFO模式        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//外设突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//存储器突发单次传输
  DMA_Init(DMA1_Stream4, &DMA_InitStructure);//初始化DMA Stream
	
	DMA_DoubleBufferModeConfig(DMA1_Stream4,(u32)buf1,DMA_Memory_0);//双缓冲模式配置
 
  DMA_DoubleBufferModeCmd(DMA1_Stream4,ENABLE);//双缓冲模式开启
 
  DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);//开启传输完成中断
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
  
} 

//I2S2 TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
void I2S3_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num)
{  
	#if  USE_IIS
	NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能 
	
	DMA_DeInit(DMA1_Stream5);
	while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}//等待DMA1_Stream1可配置 
		
  /* 配置 DMA Stream */

  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //通道0 SPI3_TX通道 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI3->DR;//外设地址为:(u32)&SPI2->DR
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
  DMA_InitStructure.DMA_BufferSize = num;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度：16位 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//高优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //不使用FIFO模式        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//外设突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//存储器突发单次传输
  DMA_Init(DMA1_Stream5, &DMA_InitStructure);//初始化DMA Stream
	
	DMA_DoubleBufferModeConfig(DMA1_Stream5,(u32)buf1,DMA_Memory_0);//双缓冲模式配置
 
  DMA_DoubleBufferModeCmd(DMA1_Stream5,ENABLE);//双缓冲模式开启
 
  DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);//开启传输完成中断
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	#elif USE_FSMC

		
  /* 配置 DMA Stream */


 #endif
}
//I2S DMA回调函数指针
void (*i2s_tx_callback)(void);	//TX回调函数 
//DMA1_Stream4中断服务函数


void DMA1_Stream5_IRQHandler(void)
{      
	if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5)==SET)////DMA1_Stream5,传输完成标志
	{ 
		DMA_ClearITPendingBit(DMA1_Stream5,DMA_IT_TCIF5);
      	i2s_tx_callback();	//执行回调函数,读取数据等操作在这里面处理  
	}   											 
} 

void DMA1_Stream4_IRQHandler(void)
{      
	if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4)==SET)////DMA1_Stream4,传输完成标志
	{ 
		DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
      	i2s_tx_callback();	//执行回调函数,读取数据等操作在这里面处理  
	}   											 
}

//I2S开始播放
void I2S_Play_Start(u8 mode)
{  
	switch(mode)
	{//dsf模式
		case 0x02:
			DMA_Cmd(DMA1_Stream5,ENABLE);//开启DMA TX传输,开始播放
			DMA_Cmd(DMA1_Stream4,ENABLE);//开启DMA TX传输,开始播放 
		break;
		case 0x01:
			IIS_BEGIN=1;
			DMA1_Stream5->CR |= (uint32_t)DMA_SxCR_EN;
//			DMA_Cmd(DMA1_Stream5,ENABLE);//开启DMA TX传输,开始播放
		break;
		case 0x03:
			DMA_Cmd(DMA1_Stream5,ENABLE);//开启DMA TX传输,开始播放
		break;
		default:
		break;
	}
}
//关闭I2S播放
void I2S_Play_Stop(u8 mode)
{   
	switch(mode)
	{//dsf模式
		case 0x02:
			DMA_Cmd(DMA1_Stream5,DISABLE);//开启DMA TX传输,开始播放
			DMA_Cmd(DMA1_Stream4,DISABLE);//开启DMA TX传输,开始播放 
		break;
		case 0x01:
			DMA1_Stream5->CR &= ~(uint32_t)DMA_SxCR_EN;
			IIS_BEGIN=0;
//			DMA_Cmd(DMA1_Stream5,DISABLE);//开启DMA TX传输,开始播放
		break;
		case 0x03:
			DMA_Cmd(DMA1_Stream5,DISABLE);//开启DMA TX传输,开始播放
		break;
		default:
		break;
	}
} 








