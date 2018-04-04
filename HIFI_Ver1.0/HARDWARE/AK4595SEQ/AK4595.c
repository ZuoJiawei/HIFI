#include "AK4595.h"
#include "myiic.h"
#include "delay.h"
#include "i2s.h"  


//AK4595�Ĵ���ֵ������(�ܹ�58���Ĵ���,0~57),ռ��116�ֽ��ڴ�
//��ΪAK4595��IIC������֧�ֶ�����,�����ڱ��ر������мĴ���ֵ
//дAK4595�Ĵ���ʱ,ͬ�����µ����ؼĴ���ֵ,���Ĵ���ʱ,ֱ�ӷ��ر��ر���ļĴ���ֵ.
//ע��:AK4595�ļĴ���ֵ��9λ��,����Ҫ��u16���洢. 
static u16 AK4595_REGVAL_TBL[]=
{
	0X0000
};



//AK4595д�Ĵ���
//reg:�Ĵ�����ַ
//val:Ҫд��Ĵ�����ֵ 
//����ֵ:0,�ɹ�;
//    ����,�������
u8 AK4595_Write_Reg(u8 reg,u16 val)
{ 
	IIC_Start(); 
	IIC_Send_Byte(0x20);//����������ַ+д����	 
	if(IIC_Wait_Ack())return 1;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    IIC_Send_Byte(reg&0xff);//д�Ĵ�����ַ
	if(IIC_Wait_Ack())return 2;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
	IIC_Send_Byte(val&0XFF);	//��������
	if(IIC_Wait_Ack())return 3;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    IIC_Stop();
	AK4595_REGVAL_TBL[reg]=val;	//����Ĵ���ֵ������
	return 0;	
}  

//����I2S����ģʽ ���ݳ���
u8 AK4595_SetLen(u8 len)
{
	u8 res=0;
	switch(len)
	{
		case 0x10://����� 16bit
			res=AK4595_Write_Reg(CONTROL_1,0x01);// 0000 0001
		break;
		case 0x14://����� 20bit
			res=AK4595_Write_Reg(CONTROL_1,0x03);// 0000 0011
		break;
		case 0x18://flp 24bit
			res=AK4595_Write_Reg(CONTROL_1,0x07);// 0000 1001
		break;
		case 0x20://LSB 32bit
			res=AK4595_Write_Reg(CONTROL_1,0x0B);// 0000 1011
		break;
	}
	return res;
}

//������ʱ��Ƶ�ʱ���
//Lrck:
u8 AK4595_SetLrck(u32 Lrck)
{	
	u8 res=0;
	if(Lrck<54100) res=AK4595_Write_Reg(CONTROL_2,0x22);// 0000 0001
	else if(Lrck>54100&&Lrck>108000) res=AK4595_Write_Reg(CONTROL_2,0x2a);// 0000 0011
	else if (Lrck>120000&&Lrck<216000) res=AK4595_Write_Reg(CONTROL_2,0x32);// 0000 1011
	return res;
}	

//����DSD���ݸ�ʽ
u8 AK4595_SetPcmDsd(u8 mode)
{
	u8 res=0;
	if(mode==0x01) res=AK4595_Write_Reg(CONTROL_3,0x80);// 0000 0001
	else          res=AK4595_Write_Reg(CONTROL_3,0x00);// 0000 0001
	return res;
}

//���þ���
u8 AK4595_Reset(u8 len)
{
	u8 res=0;
	switch(len)
	{
		case 0x10://����� 16bit
			res=AK4595_Write_Reg(CONTROL_1,0x01&0xfe);// 0000 0001
		break;
		case 0x14://����� 20bit
			res=AK4595_Write_Reg(CONTROL_1,0x03&0xfe);// 0000 0011
		break;
		case 0x18://flp 24bit
			res=AK4595_Write_Reg(CONTROL_1,0x07&0xfe);// 0000 1001
		break;
		case 0x20://flp 32bit
			res=AK4595_Write_Reg(CONTROL_1,0x0B&0xfe);// 0000 1011
		break;
//		default :
//			break;
	}
	return res;
}	

//����I2S����ģʽ
//fmt:0,LSB(�Ҷ���);1,MSB(�����);2,�����ֱ�׼I2S;3,PCM/DSP;
//len:0,16λ;1,20λ;2,24λ;3,32λ;  
u8 AK4495_I2S_Cfg(u8 mode,u8 len,u32 Lrck)
{
	u8 res=0;u8 i=0;
	static u8 lentgh0,lentgh1; 
	static u8 mode0,mode1;
	lentgh0=len;
	mode0=mode;
	res=AK4595_SetPcmDsd(mode);
	res=AK4595_SetLen(len);
	res=AK4595_SetLrck(Lrck);
	if(lentgh0!=lentgh1||mode0!=mode1) 
	{
		lentgh1=lentgh0;
		res=AK4595_Reset(lentgh1);
	}
		return res;
}	

//AK4595��ʼ��
//����ֵ:0,��ʼ������
//    ����,�������
u8 AK4595_Init(void)
{
	IIC_Init();//��ʼ��IIC�ӿ�
	AK4495_I2S_Cfg(0,16,44100);
	AK4595_Reset(16);
	return 0;
} 












