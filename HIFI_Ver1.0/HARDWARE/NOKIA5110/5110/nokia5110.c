#include "nokia5110.h"
#include "delay.h"
#include "english_6x8_pixel.h"

void NOKIA5110_write_byte(unsigned char dat, unsigned char command)
{
	    unsigned char i;
	    ce = 0;	    //��Ƭѡ�ź�
	    if (command == 0)        // ��������
	     dc = 0;
	    else		        // ��������
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
	     ce = 1;			  //��ֹ5110
}
void NOKIA5110_init(void)
{   
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
		GPIO_Init(GPIOE,&GPIO_InitStructure);	
//		GPIO_ResetBits(GPIOE,GPIO_Pin_15);
		reset = 0;
		delay_us(1);
		reset = 1;			   // ����һ����LCD��λ�ĵ͵�ƽ����	
		NOKIA5110_write_byte(0x21, 0);	// ʹ����չ��������LCDģʽ
		NOKIA5110_write_byte(0xc8, 0);	// ����ƫ�õ�ѹ
		NOKIA5110_write_byte(0x06, 0);	// �¶�У��
		NOKIA5110_write_byte(0x13, 0);	// 1:48
		NOKIA5110_write_byte(0x20, 0);	// ʹ�û�������
		NOKIA5110_clear();	            // ����   
		NOKIA5110_write_byte(0x0c, 0);	// �趨��ʾģʽ��������ʾ
}
void NOKIA5110_clear(void)
{
		unsigned int i;
		NOKIA5110_write_byte(0x0c, 0);			
		NOKIA5110_write_byte(0x80, 0);			
		for (i=0; i<504; i++)
		NOKIA5110_write_byte(0, 1);	//1���� 0����		
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
/*��λ�ֽ�*/
void NOKIA5110_set_XY(unsigned char X, unsigned char Y)
{
    NOKIA5110_write_byte(0x40 | Y, 0);		// column
    NOKIA5110_write_byte(0x80 | X, 0);          	// row
}
/*��ʾ����*/
void NOKIA5110_clear_row(unsigned char num)
{
 unsigned char i;
 NOKIA5110_set_XY(0,num);//�����п�ʼ��
 for(i=0;i<84;i++)
 NOKIA5110_write_byte(0x00,1);
}
/*д��СΪ16*16����*/
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
/*д��СΪ14*16����*/
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
/* ��С��8*16 */
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
/*��ʾͼƬ 84*48*/
void NOKIA5110_picture(unsigned char  *img_dat)
{
	unsigned int i;

	for(i = 0; i < 504; i++) NOKIA5110_write_byte(img_dat[i],1);
}
/*ͼƬ��λ��ʾ*/
//��λ��ʾָ����СͼƬ
//ȡ�����:	��λ��ǰ, ����ɨ��, ����(1-��, 0-��)
//pag:	0 - 5			ҳ����
//col:  0 - 83			������
//x:	0 - (83-col)	ͼƬ��
//y:	0 - (47-pag*8)	ͼƬ��
void NOKIA5110_pos_picture(unsigned char col, unsigned char pag, unsigned char x, unsigned char y, unsigned char  *img_dat)
{
	unsigned char i, j;

	y = (y + 4) >> 3;			//��������
	for(j = 0; j < y; j++)
	{	
		for(i = 0; i < x; i++) 
		{
			NOKIA5110_set_XY(col + i, pag + j);		//����
			NOKIA5110_write_byte(img_dat[j * x + i],1);	//����
		}
	}
}

//��λ�������
//x: 0 - 13
//y: 0 - 5
//num: 0 - 65535	Ҫ��ʾ������
//num_bit: 0 - 5	���ֵ�λ��
void NOKIA5110_printn(unsigned char x, unsigned char y, unsigned int num, unsigned char num_bit)
{
	signed   char i;
	unsigned char ii;
	unsigned char dat[6];
	
	for(i = 0; i < 6; i++) dat[i] = 0; 	//��ʼ������
	i = 0;
	while(num / 10)						//��λ
	{
		dat[i] = num % 10;				//���λ
		num /= 10; i++;		
	}
	dat[i] = num;						//���λ
	ii = i;								//����dat��λ��
	for(; i >= 0; i--)	dat[i] += 48;	//ת����ASCII
	for(i = 0; i < num_bit; i++)
	{
	  NOKIA5110_set_XY(x+i,y);
	  NOKIA5110_write_char( ' ');			//����ʾ����
	}
	for(i = ii; i >= 0; i--)
	{
	  NOKIA5110_set_XY(x++,y);
	  NOKIA5110_write_char(dat[i]);			//�����ֵ
	}
}

