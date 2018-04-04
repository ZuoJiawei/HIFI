#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F407������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define PCM_DSD_CTRL    PFout(2) //PCM/DSDģʽ�л�   	   		   
//IO��������
//#if RED_DRAGONBOARD
//#define SDA_IN()  {GPIOF->MODER&=~(3<<(0*2));GPIOF->MODER|=0<<0*2;}	//PB9����ģʽ
//#define SDA_OUT() {GPIOF->MODER&=~(3<<(0*2));GPIOF->MODER|=1<<0*2;} //PB9���ģʽ

//#define IIC_SCL    PFout(1) //SCL
//#define IIC_SDA    PFout(0) //SDA	 
//#define READ_SDA   PFin(0)  //����SDA 
//#else 
#define SDA_IN()  {IIC_WR=0;GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9����ģʽ
#define SDA_OUT() {IIC_WR=1;GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9���ģʽ
#define IIC_SCL    PBout(8) //SCL
#define IIC_WR     PFout(0) //SCL
#define IIC_SDA    PBout(9) //SDA	 
#define READ_SDA   PBin(9)  //����SDA 
//#endif
//IO��������	 


//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif















