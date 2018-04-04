//WAV������ʼ��
//fname:�ļ�·��+�ļ���
//wavx:wav ��Ϣ��Žṹ��ָ��
//����ֵ:0,�ɹ�;1,���ļ�ʧ��;2,��WAV�ļ�;3,DATA����δ�ҵ�.
/*��ultraedit �򿪵��ļ�ͷ,�����Ǵ��ģʽ,�������е����ݶ��ǵ��Ŵ�� LSB�����λ
/D  S  D  /  /u64     headsize     / /  u64 file_size      /
44 53 44 20  1c 00 00 00 00 00 00 00 D5 C0 33 0C 00 00 00 00
/  u64 id3_offset    /     /f  m  t  /
5C C0 33 0C 00 00 00 00    66 6D 74 20
/ u64  fmt_size       / 		/u32format_version/ /U32 format_id/
34 00 00 00 00 00 00 00			01   00   00    00	00  00  00  00   
/u32 channel_type / /u32 channel_count /    /u32 samplerate/ /u32 bits_persample/
02 00 00 00 		    02    00    00   00			00  01    00 00	 00    00    00   00
/u64  samplecount     /  /u32block_size/  /reserved /
00 00 CF 30 00 00 00 00  00  10  00   00  00 00 00 00
/d  a  t a/ 		/u64   data_size      /
64 61 74 61 		0C C0 33 0C 00 00 00 00

DSD 
*/
/*
	u16 audioformat;			//��Ƶ��ʽ;0X01,��ʾ����PCM;0X11��ʾIMA ADPCM
	u16 nchannels;				//ͨ������;1,��ʾ������;2,��ʾ˫����;   
	u32 datasize;					//DSF���ݴ�С 

	u32 totsec ;					//���׸�ʱ��,��λ:��
	u32 cursec ;					//��ǰ����ʱ��

	u32 bitrate;	   			//������(λ��)
	u32 samplerate;				//������ 
	
	u32 datastart;				//����֡��ʼ��λ��(���ļ������ƫ��)

*/
#include "delay.h"
#include "led.h"
#include "key.h"
#include "endianess.h"
#include "audioplay.h"
#include "usart.h" 
#include "malloc.h"
#include "ff.h"
#include "i2s.h"
#include  "sacd_dsf.h"
#include "AK4595.h"
#include "myiic.h"
__dsfctrl dsfctrl;
vu8 dsftransferend=0;	//i2s������ɱ�־
vu8 dsfwitchbuf_left=0;		//i2sbufxָʾ��־
vu8 dsfwitchbuf_right=0;		//i2sbufxָʾ��־


u8 dsf_decode_init(u8* fname,__dsfctrl* dsfx)
{
//	uint64_t da;
	FIL*ftemp;
	u8 *buf; 
	u32 br=0;
	u8 res=0;
	_data *data;
	_fmt *fmt;
	_head *head;
	
	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	buf=mymalloc(SRAMIN,512);
	if(ftemp&&buf)	//�ڴ�����ɹ�
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//���ļ�
		if(res==FR_OK)
		{
			f_read(ftemp,buf,512,&br);	//��ȡ512�ֽ�������
			head=(_head *)buf;		//��ȡͷ��
			if(head->id==0x20445344)//��DSD�ļ�
			{
//				da=hton64((uint64_t)28);//������С��ת���
				if(head->headsize==28)
				{
					fmt=(_fmt *)(buf+28);//�ļ�ͷռ28bit	
					if(fmt->fmt_id==0x20746D66)//��ȡFMT�� 	
					{
						if(fmt->format_id==0)
						{
							dsfx->audioformat=fmt->channel_type;		//��Ƶ��ʽ
							dsfx->nchannels=fmt->channel_count;		//ͨ����
							dsfx->samplerate=fmt->samplerate;		//������
							dsfx->block_size=fmt->block_size;//4096Byte			//���ݿ��С
							printf("dsfx->audioformat:%d\r\n",dsfx->audioformat);
							printf("dsfx->nchannels:%d\r\n",dsfx->nchannels);
							printf("dsfx->samplerate:%d\r\n",dsfx->samplerate);
							printf("dsfx->block_size:%d\r\n",dsfx->block_size);
							data=(_data*)(buf+28+52);
							if(data->data_id==0X61746174)//�����ɹ�! 'data'
							{
								dsfx->datastart=28+52+12;
								printf("dsfx->datastart:%d\r\n",dsfx->datastart);
							}else res=5;//data����δ�ҵ�.
						}else  res=4;//fmt->id !=0;
					}else res=3;//û���ҵ�fmt��
				}else res=7;//ͷ�����ݳ��Ȳ���
			}else res=2;//��dsd�ļ�
		}else res=1;//���ļ�����
	}else res=6;//�ڴ�����ʧ��
	f_close(ftemp);
	myfree(SRAMIN,ftemp);//�ͷ��ڴ�
	myfree(SRAMIN,buf); 
	return 0;
}

int get_channelcount(_fmt * format)
{
    return 2; 
}

int get_samplerate() 
{
	return SACD_SAMPLING_FREQUENCY;
}

double get_duration(duration *dura) 
{
		return dura->minutes *60.0 + dura->seconds * 1.0;
}
//���buf
//buf:������
//size:���������
//����ֵ:���������ݸ���
u32 dsf_buffill_left(u8 *buf,u16 size)
{
	u32 bread;
	u16 i;
	f_read(audiodev.file,buf,size,(UINT*)&bread);
	if(bread<size)//����������,����0
	{
		for(i=bread;i<size-bread;i++)buf[i]=0; 
	}
	return bread;
}

//���buf
//buf:������
//size:���������
//����ֵ:���������ݸ���
u32 dsf_buffill_right(u8 *buf,u16 size)
{
	u32 bread;
	u16 i;
	u8 *p;
	f_read(audiodev.file,audiodev.tbuf,size,(UINT*)&bread);
//	p=audiodev.tbuf;			
//	for(i=0;i<size;)
//	{
//		buf[i++]=p[2];//��λ
//		buf[i++]=p[3]; //�ε�λ
//		buf[i++]=p[1];//�θ�λ
//		buf[i++]=p[0];//��λ
//		p+=4;
//	} 
	bread=bread;		//����Ĵ�С.
	if(bread<size)//����������,����0
	{
		for(i=bread;i<size-bread;i++)buf[i]=0; 
	}
	return bread;
}

void dsf_i2s_dma_tx_callback(void) 
{   
	u16 i;
	if(DMA1_Stream5->CR&(1<<19))
	{
		dsfwitchbuf_left=0;
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<DSF_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				audiodev.i2sbuf1[i]=0;//���0
			}
		}
	}
	else 
	{
		dsfwitchbuf_left=1;
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<DSF_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				audiodev.i2sbuf2[i]=0;//���0
			}
		}
	}
	if(DMA1_Stream4->CR&(1<<19))
	{
		dsfwitchbuf_right=0;
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<DSF_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				audiodev.i2sbuf3[i]=0;//���0
			}
		}
	}
	else 
	{
		dsfwitchbuf_right=1;
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<DSF_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				audiodev.i2sbuf4[i]=0;//���0
			}
		}
	}
	dsftransferend=1;
}

//����ĳ��WAV�ļ�
//fname:wav�ļ�·��.
//����ֵ:
//KEY0_PRES:��һ��
//KEY1_PRES:��һ��
//����:����
u8 dsf_play_song(u8* fname)
{
	u8 key;
	u8 t=0; 
	u8 res;  
	u32 fillnum; 
	PCM_DSD_CTRL=1;
	audiodev.file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	audiodev.i2sbuf1=mymalloc(SRAMIN,DSF_I2S_TX_DMA_BUFSIZE);
	audiodev.i2sbuf2=mymalloc(SRAMIN,DSF_I2S_TX_DMA_BUFSIZE);
	audiodev.i2sbuf3=mymalloc(SRAMIN,DSF_I2S_TX_DMA_BUFSIZE);
	audiodev.i2sbuf4=mymalloc(SRAMIN,DSF_I2S_TX_DMA_BUFSIZE);
	audiodev.tbuf=mymalloc(SRAMIN,DSF_I2S_TX_DMA_BUFSIZE);
	if(audiodev.file&&audiodev.i2sbuf1&&audiodev.i2sbuf2&&audiodev.i2sbuf3&&audiodev.i2sbuf4&&audiodev.tbuf)
	{ 
		res=dsf_decode_init(fname,&dsfctrl);//�õ��ļ�����Ϣ
		if(res==0)//�����ļ��ɹ�
		{
				AK4495_I2S_Cfg(0x01,16,0);	//�����ֱ�׼,24λ���ݳ���
				I2S3_Init(I2S_Standard_LSB,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);
				I2S2_Init(I2S_Standard_LSB,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ��չ֡����
		}	
		I2S3_SampleRate_Set(88200);//���ò�����88.2KHZ
		I2S3_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,DSF_I2S_TX_DMA_BUFSIZE/2); //����TX DMA
		I2S2_SampleRate_Set(88200);//���ò�����88.2KHZ
		I2S2_TX_DMA_Init(audiodev.i2sbuf3,audiodev.i2sbuf4,DSF_I2S_TX_DMA_BUFSIZE/2); //����TX DMA
		i2s_tx_callback=dsf_i2s_dma_tx_callback;			//�ص�����ָwav_i2s_dma_callback
		audio_stop(0x02);
		res=f_open(audiodev.file,(TCHAR*)fname,FA_READ);	//���ļ�
		if(res==0)
		{
			f_lseek(audiodev.file, dsfctrl.datastart);		//�����ļ�ͷ
			fillnum=dsf_buffill_left(audiodev.i2sbuf1,DSF_I2S_TX_DMA_BUFSIZE);
			fillnum=dsf_buffill_left(audiodev.i2sbuf3,DSF_I2S_TX_DMA_BUFSIZE);
			fillnum=dsf_buffill_right(audiodev.i2sbuf2,DSF_I2S_TX_DMA_BUFSIZE);
			fillnum=dsf_buffill_right(audiodev.i2sbuf4,DSF_I2S_TX_DMA_BUFSIZE);
			
			audio_start(0x02); 
			printf("playing^_^\r\n");				
			while(res==0)
			{ 
				while(dsftransferend==0);//�ȴ�wav�������; 
				dsftransferend=0;
				if(fillnum!=DSF_I2S_TX_DMA_BUFSIZE)//���Ž���?
				{
					res=KEY0_PRES;
					break;
				} 
				if(dsfwitchbuf_left&&dsfwitchbuf_right)
				{
					fillnum=dsf_buffill_left(audiodev.i2sbuf2,DSF_I2S_TX_DMA_BUFSIZE);//���buf2
					fillnum=dsf_buffill_right(audiodev.i2sbuf4,DSF_I2S_TX_DMA_BUFSIZE);//���buf4
				}
				else if (dsfwitchbuf_left==0&&dsfwitchbuf_right==0)
				{
					fillnum=dsf_buffill_left(audiodev.i2sbuf1,DSF_I2S_TX_DMA_BUFSIZE);//���buf1
					fillnum=dsf_buffill_right(audiodev.i2sbuf3,DSF_I2S_TX_DMA_BUFSIZE);//���buf3
				}
				while(1)
				{
					key=KEY_Scan(0); 
					if(key==WKUP_PRES)//��ͣ
					{
						if(audiodev.status&0X01)audiodev.status&=~(1<<0);
						else audiodev.status|=0X01;  
					}
					if(key==KEY2_PRES||key==KEY0_PRES)//��һ��/��һ��
					{
						res=key;
						break; 
					}
					t++;
					if(t==20)
					{
						t=0;
						LED0=!LED0;
					}
					if((audiodev.status&0X01)==0)delay_ms(10);
					else break;
				}
			}
			audio_stop(0x02); 
		}else res=0XFF; 
	}else res=0XFF; 
	myfree(SRAMIN,audiodev.tbuf);	//�ͷ��ڴ�
	myfree(SRAMIN,audiodev.i2sbuf1);//�ͷ��ڴ�
	myfree(SRAMIN,audiodev.i2sbuf2);//�ͷ��ڴ� 
	myfree(SRAMIN,audiodev.file);	//�ͷ��ڴ� 
	return res;
} 
