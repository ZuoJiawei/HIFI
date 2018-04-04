#include "nokia5110.h"
#include "delay.h"
#include "english_6x8_pixel.h"

void NOKIA5110_write_byte(unsigned char dat, unsigned char command)
{
	    unsigned char i;
	    ce = 0;	    //打开片选信号
	    if (command == 0)        // 传送命令
	     dc = 0;
	    else		        // 传送数据
	     dc = 1;
			for(i=0;i<8;i++)
			{
				if(dat&0x80)  sdin = 1;
				else          sdin = 0;
				sclk  = 0;
				dat <<= 1;
				delay_us(1);
				sclk  = 1;

			}
	     ce = 1;			  //禁止5110
}
void NOKIA5110_init(void)
{   
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//普通输出
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽输出
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//上拉
		GPIO_Init(GPIOE,&GPIO_InitStructure);	
//		GPIO_ResetBits(GPIOE,GPIO_Pin_15);
		reset = 0;
		delay_us(1);
		reset = 1;			   // 产生一个让LCD复位的低电平脉冲	
		NOKIA5110_write_byte(0x21, 0);	// 使用扩展命令设置LCD模式
		NOKIA5110_write_byte(0xc8, 0);	// 设置偏置电压
		NOKIA5110_write_byte(0x06, 0);	// 温度校正
		NOKIA5110_write_byte(0x13, 0);	// 1:48
		NOKIA5110_write_byte(0x20, 0);	// 使用基本命令
		NOKIA5110_clear();	            // 清屏   
		NOKIA5110_write_byte(0x0c, 0);	// 设定显示模式，正常显示
}
void NOKIA5110_clear(void)
{
		unsigned int i;
		NOKIA5110_write_byte(0x0c, 0);			
		NOKIA5110_write_byte(0x80, 0);			
		for (i=0; i<504; i++)
		NOKIA5110_write_byte(0, 1);	//1数据 0命令		
}
void NOKIA5110_write_char(unsigned char c)
{
    unsigned char line;
    c -= 32;
    for (line=0; line<6; line++)
    NOKIA5110_write_byte(font6x8[c][line], 1);
}
void NOKIA5110_write_english_string(unsigned char X,unsigned char Y,unsigned  char *s)
{
	NOKIA5110_set_XY(X,Y);
	while (*s) 
	{
		NOKIA5110_write_char(*s);
		s++;
	}
}
/*定位字节*/
void NOKIA5110_set_XY(unsigned char X, unsigned char Y)
{
    NOKIA5110_write_byte(0x40 | Y, 0);		// column
    NOKIA5110_write_byte(0x80 | X, 0);          	// row
}
/*显示清行*/
void NOKIA5110_clear_row(unsigned char num)
{
 unsigned char i;
 NOKIA5110_set_XY(0,num);//从哪行开始清
 for(i=0;i<84;i++)
 NOKIA5110_write_byte(0x00,1);
}
/*写大小为16*16汉字*/
void NOKIA5110_write_32(unsigned char x,unsigned char y,unsigned char c[32])
{
 unsigned char i;
 NOKIA5110_set_XY(x,y);
 for(i=0;i<16;i++)
 {
   NOKIA5110_write_byte(c[i],1);
 }
 NOKIA5110_set_XY(x,y+1);
 for(i=16;i<32;i++)
 {
   NOKIA5110_write_byte(c[i],1);
 }
}
/*写大小为14*16汉字*/
void NOKIA5110_write_28(unsigned char x,unsigned char y,unsigned char c[32])
{
 unsigned char i;
 NOKIA5110_set_XY(x,y);
 for(i=0;i<14;i++)
 {
   NOKIA5110_write_byte(c[i],1);
 }
 NOKIA5110_set_XY(x,y+1);
 for(i=14;i<28;i++)
 {
   NOKIA5110_write_byte(c[i],1);
 }
}
/* 大小：8*16 */
void NOKIA5110_write_16(unsigned char x,unsigned char y,unsigned char c[16])
{
 unsigned char i;
 NOKIA5110_set_XY(x,y);
 for(i=0;i<8;i++)
 {
   NOKIA5110_write_byte(c[i],1);
 }
 NOKIA5110_set_XY(x,y+1);
 for(i=8;i<16;i++)
 {
   NOKIA5110_write_byte(c[i],1);
 }  
}
/*显示图片 84*48*/
void NOKIA5110_picture(unsigned char  *img_dat)
{
	unsigned int i;

	for(i = 0; i < 504; i++) NOKIA5110_write_byte(img_dat[i],1);
}
/*图片定位显示*/
//定位显示指定大小图片
//取码规则:	低位在前, 列行扫描, 阴码(1-亮, 0-灭)
//pag:	0 - 5			页坐标
//col:  0 - 83			列坐标
//x:	0 - (83-col)	图片宽
//y:	0 - (47-pag*8)	图片高
void NOKIA5110_pos_picture(unsigned char col, unsigned char pag, unsigned char x, unsigned char y, unsigned char  *img_dat)
{
	unsigned char i, j;

	y = (y + 4) >> 3;			//四舍五入
	for(j = 0; j < y; j++)
	{	
		for(i = 0; i < x; i++) 
		{
			NOKIA5110_set_XY(col + i, pag + j);		//坐标
			NOKIA5110_write_byte(img_dat[j * x + i],1);	//数据
		}
	}
}

//定位输出数字
//x: 0 - 13
//y: 0 - 5
//num: 0 - 65535	要显示的数字
//num_bit: 0 - 5	数字的位数
void NOKIA5110_printn(unsigned char x, unsigned char y, unsigned int num, unsigned char num_bit)
{
	signed   char i;
	unsigned char ii;
	unsigned char dat[6];
	
	for(i = 0; i < 6; i++) dat[i] = 0; 	//初始化数据
	i = 0;
	while(num / 10)						//拆位
	{
		dat[i] = num % 10;				//最低位
		num /= 10; i++;		
	}
	dat[i] = num;						//最高位
	ii = i;								//保存dat的位数
	for(; i >= 0; i--)	dat[i] += 48;	//转化成ASCII
	for(i = 0; i < num_bit; i++)
	{
	  NOKIA5110_set_XY(x+i,y);
	  NOKIA5110_write_char( ' ');			//清显示区域
	}
	for(i = ii; i >= 0; i--)
	{
	  NOKIA5110_set_XY(x++,y);
	  NOKIA5110_write_char(dat[i]);			//输出数值
	}
}

