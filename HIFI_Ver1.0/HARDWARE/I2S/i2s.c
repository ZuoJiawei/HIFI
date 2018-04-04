#include "i2s.h"  
#include "usart.h"


void IIS_plugin(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(I2Sx_WS_GPIO_CLK|I2Sx_CK_GPIO_CLK|I2Sx_SD_GPIO_CLK|I2Sx_MCLK_GPIO_CLK|RCC_AHB1Periph_GPIOF, ENABLE);			//ʹ������GPIOB,GPIOCʱ��
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PF3������CPLD FIFO���źŵ�ͬ������pf3������ʱ,fifo�Ķ���д����ʹ�� 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��	
	//PB3/5���ù������
	GPIO_InitStructure.GPIO_Pin = I2Sx_CK_PIN | I2Sx_SD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(I2Sx_CK_GPIO_PORT, &GPIO_InitStructure);//��ʼ��
	
	//PC2/PC3/PC6���ù������
	GPIO_InitStructure.GPIO_Pin = I2Sx_WS_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(I2Sx_WS_GPIO_PORT, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = I2Sx_MCLK_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(I2Sx_MCLK_GPIO_PORT, &GPIO_InitStructure);//��ʼ��
	
	GPIO_PinAFConfig(I2Sx_WS_GPIO_PORT,I2Sx_WS_SOURCE,I2Sx_WS_AF); //PB12,AF5  I2S_LRCK
	GPIO_PinAFConfig(I2Sx_SD_GPIO_PORT,I2Sx_SD_SOURCE,I2Sx_SD_AF);	//PB13,AF5  I2S_SCLK 
	GPIO_PinAFConfig(I2Sx_CK_GPIO_PORT,I2Sx_CK_SOURCE,I2Sx_CK_AF);	//PC3 ,AF5  I2S_DACDATA 
	GPIO_PinAFConfig(I2Sx_MCLK_GPIO_PORT,I2Sx_MCLK_SOURCE,I2Sx_MCLK_AF);	//PC6 ,AF5  I2S_MCK
//	GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF6_SPI2);	//PC2 ,AF6  I2S_ADCDATA  I2S2ext_SD��AF6!!!

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);			//ʹ������GPIOB,GPIOCʱ��
     
	//PB12/13 ���ù������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	//PC2/PC3/PC6���ù������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2); //PB12,AF5  I2S_LRCK
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);	//PB13,AF5  I2S_SCLK 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);	//PC3 ,AF5  I2S_DACDATA 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_SPI2);	//PC6 ,AF5  I2S_MCK
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF6_SPI2);	//PC2 ,AF6  I2S_ADCDATA  I2S2ext_SD��AF6!!!
	IIS_BEGIN=0;
}

void I2S3_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat)
{ 
  I2S_InitTypeDef I2S_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//ʹ��SPI3ʱ��
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE); //��λSPI3
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);//������λ
  
	I2S_InitStructure.I2S_Mode=I2S_Mode;//IISģʽ
	I2S_InitStructure.I2S_Standard=I2S_Standard;//IIS��׼
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat;//IIS���ݳ���
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;//��ʱ�������ֹ
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_Default;//IISƵ������
	I2S_InitStructure.I2S_CPOL=I2S_Clock_Polarity;//����״̬ʱ�ӵ�ƽ
	I2S_Init(SPI3,&I2S_InitStructure);//��ʼ��IIS

 
	SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);//SPI3 TX DMA����ʹ��.
  I2S_Cmd(SPI3,ENABLE);//SPI3 I2S ENʹ��.	
} 
//I2S2��ʼ��

//����I2S_Standard:  @ref SPI_I2S_Standard  I2S��׼,
//I2S_Standard_Phillips,�����ֱ�׼;
//I2S_Standard_MSB,MSB�����׼(�Ҷ���);
//I2S_Standard_LSB,LSB�����׼(�����);
//I2S_Standard_PCMShort,I2S_Standard_PCMLong:PCM��׼
//����I2S_Mode:  @ref SPI_I2S_Mode  I2S_Mode_SlaveTx:�ӻ�����;I2S_Mode_SlaveRx:�ӻ�����;I2S_Mode_MasterTx:��������;I2S_Mode_MasterRx:��������;
//����I2S_Clock_Polarity   @ref SPI_I2S_Clock_Polarity:  I2S_CPOL_Low,ʱ�ӵ͵�ƽ��Ч;I2S_CPOL_High,ʱ�Ӹߵ�ƽ��Ч
//����I2S_DataFormat�� @ref SPI_I2S_Data_Format :���ݳ���,I2S_DataFormat_16b,16λ��׼;I2S_DataFormat_16bextended,16λ��չ(frame=32bit);I2S_DataFormat_24b,24λ;I2S_DataFormat_32b,32λ.
void I2S2_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat)
{ 
  I2S_InitTypeDef I2S_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//ʹ��SPI2ʱ��
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE); //��λSPI2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//������λ
  
	I2S_InitStructure.I2S_Mode=I2S_Mode;//IISģʽ
	I2S_InitStructure.I2S_Standard=I2S_Standard;//IIS��׼
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat;//IIS���ݳ���
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;//��ʱ�������ֹ
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_Default;//IISƵ������
	I2S_InitStructure.I2S_CPOL=I2S_Clock_Polarity;//����״̬ʱ�ӵ�ƽ
	I2S_Init(SPI2,&I2S_InitStructure);//��ʼ��IIS

 
	SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);//SPI2 TX DMA����ʹ��.
  I2S_Cmd(SPI2,ENABLE);//SPI2 I2S ENʹ��.	
} 
//�����ʼ��㹫ʽ:Fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//һ��HSE=8Mhz 
//pllm:��Sys_Clock_Set���õ�ʱ��ȷ����һ����8
//PLLI2SN:һ����192~432 
//PLLI2SR:2~7
//I2SDIV:2~255
//ODD:0/1
//I2S��Ƶϵ����@pllm=8,HSE=8Mhz,��vco����Ƶ��Ϊ1Mhz
//���ʽ:������/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const u16 I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz������
	{1102,429,4,19,0},		//11.025Khz������ 
	{1600,213,2,13,0},		//16Khz������
	{2205,429,4, 9,1},		//22.05Khz������
	{3200,213,2, 6,1},		//32Khz������
	{4410,271,2, 6,0},		//44.1Khz������
	{4800,258,3, 3,1},		//48Khz������
	{8820,316,2, 3,1},		//88.2Khz������
	{9600,344,2, 3,1},  	//96Khz������
	{17640,361,2,2,0},  	//176.4Khz������ 
	{19200,393,2,2,0},  	//192Khz������
};  
//����IIS�Ĳ�����(@MCKEN)
//samplerate:������,��λ:Hz
//����ֵ:0,���óɹ�;1,�޷�����.
u8 I2S2_SampleRate_Set(u32 samplerate)
{ 
	u8 i=0; 
	u32 tempreg=0;
	samplerate/=10;//��С10��   
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//�����Ĳ������Ƿ����֧��
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
 
	RCC_PLLI2SCmd(DISABLE);//�ȹر�PLLI2S
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//�ѱ���Ҳ�Ҳ���
	RCC_PLLI2SConfig((u32)I2S_PSC_TBL[i][1],(u32)I2S_PSC_TBL[i][2]);//����I2SxCLK��Ƶ��(x=2)  ����PLLI2SN PLLI2SR
 
	RCC->CR|=1<<26;					//����I2Sʱ��
	while((RCC->CR&1<<27)==0);		//�ȴ�I2Sʱ�ӿ����ɹ�. 
	tempreg=I2S_PSC_TBL[i][3]<<0;	//����I2SDIV
	tempreg|=I2S_PSC_TBL[i][4]<<8;	//����ODDλ
	tempreg|=1<<9;					//ʹ��MCKOEλ,���MCK
	SPI2->I2SPR=tempreg;			//����I2SPR�Ĵ��� 
	return 0;
}  

//����IIS�Ĳ�����(@MCKEN)
//samplerate:������,��λ:Hz
//����ֵ:0,���óɹ�;1,�޷�����.
u8 I2S3_SampleRate_Set(u32 samplerate)
{ 
	u8 i=0; 
	u32 tempreg=0;
	samplerate/=10;//��С10��   
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//�����Ĳ������Ƿ����֧��
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
 
	RCC_PLLI2SCmd(DISABLE);//�ȹر�PLLI2S
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//�ѱ���Ҳ�Ҳ���
	RCC_PLLI2SConfig((u32)I2S_PSC_TBL[i][1],(u32)I2S_PSC_TBL[i][2]);//����I2SxCLK��Ƶ��(x=2)  ����PLLI2SN PLLI2SR
 
	RCC->CR|=1<<26;					//����I2Sʱ��
	while((RCC->CR&1<<27)==0);		//�ȴ�I2Sʱ�ӿ����ɹ�. 
	tempreg=I2S_PSC_TBL[i][3]<<0;	//����I2SDIV
	tempreg|=I2S_PSC_TBL[i][4]<<8;	//����ODDλ
	tempreg|=1<<9;					//ʹ��MCKOEλ,���MCK
	SPI3->I2SPR=tempreg;			//����I2SPR�Ĵ��� 
	return 0;
} 
//I2S2 TX DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
void I2S2_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num)
{  
	NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1ʱ��ʹ�� 
	
	DMA_DeInit(DMA1_Stream4);
	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}//�ȴ�DMA1_Stream1������ 
		
  /* ���� DMA Stream */

  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //ͨ��0 SPI2_TXͨ�� 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR;//�����ַΪ:(u32)&SPI2->DR
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�洢��������ģʽ
  DMA_InitStructure.DMA_BufferSize = num;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ��ȣ�16λ 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //��ʹ��FIFOģʽ        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//����ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//�洢��ͻ�����δ���
  DMA_Init(DMA1_Stream4, &DMA_InitStructure);//��ʼ��DMA Stream
	
	DMA_DoubleBufferModeConfig(DMA1_Stream4,(u32)buf1,DMA_Memory_0);//˫����ģʽ����
 
  DMA_DoubleBufferModeCmd(DMA1_Stream4,ENABLE);//˫����ģʽ����
 
  DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);//������������ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
  
} 

//I2S2 TX DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
void I2S3_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num)
{  
	#if  USE_IIS
	NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1ʱ��ʹ�� 
	
	DMA_DeInit(DMA1_Stream5);
	while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}//�ȴ�DMA1_Stream1������ 
		
  /* ���� DMA Stream */

  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //ͨ��0 SPI3_TXͨ�� 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI3->DR;//�����ַΪ:(u32)&SPI2->DR
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�洢��������ģʽ
  DMA_InitStructure.DMA_BufferSize = num;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ��ȣ�16λ 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //��ʹ��FIFOģʽ        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//����ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//�洢��ͻ�����δ���
  DMA_Init(DMA1_Stream5, &DMA_InitStructure);//��ʼ��DMA Stream
	
	DMA_DoubleBufferModeConfig(DMA1_Stream5,(u32)buf1,DMA_Memory_0);//˫����ģʽ����
 
  DMA_DoubleBufferModeCmd(DMA1_Stream5,ENABLE);//˫����ģʽ����
 
  DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);//������������ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	#elif USE_FSMC

		
  /* ���� DMA Stream */


 #endif
}
//I2S DMA�ص�����ָ��
void (*i2s_tx_callback)(void);	//TX�ص����� 
//DMA1_Stream4�жϷ�����


void DMA1_Stream5_IRQHandler(void)
{      
	if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5)==SET)////DMA1_Stream5,������ɱ�־
	{ 
		DMA_ClearITPendingBit(DMA1_Stream5,DMA_IT_TCIF5);
      	i2s_tx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
	}   											 
} 

void DMA1_Stream4_IRQHandler(void)
{      
	if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4)==SET)////DMA1_Stream4,������ɱ�־
	{ 
		DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
      	i2s_tx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
	}   											 
}

//I2S��ʼ����
void I2S_Play_Start(u8 mode)
{  
	switch(mode)
	{//dsfģʽ
		case 0x02:
			DMA_Cmd(DMA1_Stream5,ENABLE);//����DMA TX����,��ʼ����
			DMA_Cmd(DMA1_Stream4,ENABLE);//����DMA TX����,��ʼ���� 
		break;
		case 0x01:
			IIS_BEGIN=1;
			DMA1_Stream5->CR |= (uint32_t)DMA_SxCR_EN;
//			DMA_Cmd(DMA1_Stream5,ENABLE);//����DMA TX����,��ʼ����
		break;
		case 0x03:
			DMA_Cmd(DMA1_Stream5,ENABLE);//����DMA TX����,��ʼ����
		break;
		default:
		break;
	}
}
//�ر�I2S����
void I2S_Play_Stop(u8 mode)
{   
	switch(mode)
	{//dsfģʽ
		case 0x02:
			DMA_Cmd(DMA1_Stream5,DISABLE);//����DMA TX����,��ʼ����
			DMA_Cmd(DMA1_Stream4,DISABLE);//����DMA TX����,��ʼ���� 
		break;
		case 0x01:
			DMA1_Stream5->CR &= ~(uint32_t)DMA_SxCR_EN;
			IIS_BEGIN=0;
//			DMA_Cmd(DMA1_Stream5,DISABLE);//����DMA TX����,��ʼ����
		break;
		case 0x03:
			DMA_Cmd(DMA1_Stream5,DISABLE);//����DMA TX����,��ʼ����
		break;
		default:
		break;
	}
} 








