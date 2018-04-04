#include "AK4595.h"
#include "myiic.h"
#include "delay.h"
#include "i2s.h"  


//AK4595寄存器值缓存区(总共58个寄存器,0~57),占用116字节内存
//因为AK4595的IIC操作不支持读操作,所以在本地保存所有寄存器值
//写AK4595寄存器时,同步更新到本地寄存器值,读寄存器时,直接返回本地保存的寄存器值.
//注意:AK4595的寄存器值是9位的,所以要用u16来存储. 
static u16 AK4595_REGVAL_TBL[]=
{
	0X0000
};



//AK4595写寄存器
//reg:寄存器地址
//val:要写入寄存器的值 
//返回值:0,成功;
//    其他,错误代码
u8 AK4595_Write_Reg(u8 reg,u16 val)
{ 
	IIC_Start(); 
	IIC_Send_Byte(0x20);//发送器件地址+写命令	 
	if(IIC_Wait_Ack())return 1;	//等待应答(成功?/失败?) 
    IIC_Send_Byte(reg&0xff);//写寄存器地址
	if(IIC_Wait_Ack())return 2;	//等待应答(成功?/失败?) 
	IIC_Send_Byte(val&0XFF);	//发送数据
	if(IIC_Wait_Ack())return 3;	//等待应答(成功?/失败?) 
    IIC_Stop();
	AK4595_REGVAL_TBL[reg]=val;	//保存寄存器值到本地
	return 0;	
}  

//设置I2S工作模式 数据长度
u8 AK4595_SetLen(u8 len)
{
	u8 res=0;
	switch(len)
	{
		case 0x10://左对齐 16bit
			res=AK4595_Write_Reg(CONTROL_1,0x01);// 0000 0001
		break;
		case 0x14://左对齐 20bit
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

//设置主时钟频率倍数
//Lrck:
u8 AK4595_SetLrck(u32 Lrck)
{	
	u8 res=0;
	if(Lrck<54100) res=AK4595_Write_Reg(CONTROL_2,0x22);// 0000 0001
	else if(Lrck>54100&&Lrck>108000) res=AK4595_Write_Reg(CONTROL_2,0x2a);// 0000 0011
	else if (Lrck>120000&&Lrck<216000) res=AK4595_Write_Reg(CONTROL_2,0x32);// 0000 1011
	return res;
}	

//设置DSD数据格式
u8 AK4595_SetPcmDsd(u8 mode)
{
	u8 res=0;
	if(mode==0x01) res=AK4595_Write_Reg(CONTROL_3,0x80);// 0000 0001
	else          res=AK4595_Write_Reg(CONTROL_3,0x00);// 0000 0001
	return res;
}

//设置静音
u8 AK4595_Reset(u8 len)
{
	u8 res=0;
	switch(len)
	{
		case 0x10://左对齐 16bit
			res=AK4595_Write_Reg(CONTROL_1,0x01&0xfe);// 0000 0001
		break;
		case 0x14://左对齐 20bit
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

//设置I2S工作模式
//fmt:0,LSB(右对齐);1,MSB(左对齐);2,飞利浦标准I2S;3,PCM/DSP;
//len:0,16位;1,20位;2,24位;3,32位;  
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

//AK4595初始化
//返回值:0,初始化正常
//    其他,错误代码
u8 AK4595_Init(void)
{
	IIC_Init();//初始化IIC接口
	AK4495_I2S_Cfg(0,16,44100);
	AK4595_Reset(16);
	return 0;
} 












