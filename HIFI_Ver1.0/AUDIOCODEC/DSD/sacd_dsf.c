//WAV解析初始化
//fname:文件路径+文件名
//wavx:wav 信息存放结构体指针
//返回值:0,成功;1,打开文件失败;2,非WAV文件;3,DATA区域未找到.
/*用ultraedit 打开的文件头,电脑是大端模式,所以所有的数据都是倒着存的 LSB在最高位
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
	u16 audioformat;			//音频格式;0X01,表示线性PCM;0X11表示IMA ADPCM
	u16 nchannels;				//通道数量;1,表示单声道;2,表示双声道;   
	u32 datasize;					//DSF数据大小 

	u32 totsec ;					//整首歌时长,单位:秒
	u32 cursec ;					//当前播放时长

	u32 bitrate;	   			//比特率(位速)
	u32 samplerate;				//采样率 
	
	u32 datastart;				//数据帧开始的位置(在文件里面的偏移)

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
vu8 dsftransferend=0;	//i2s传输完成标志
vu8 dsfwitchbuf_left=0;		//i2sbufx指示标志
vu8 dsfwitchbuf_right=0;		//i2sbufx指示标志


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
	if(ftemp&&buf)	//内存申请成功
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//打开文件
		if(res==FR_OK)
		{
			f_read(ftemp,buf,512,&br);	//读取512字节在数据
			head=(_head *)buf;		//获取头块
			if(head->id==0x20445344)//是DSD文件
			{
//				da=hton64((uint64_t)28);//把数据小端转大端
				if(head->headsize==28)
				{
					fmt=(_fmt *)(buf+28);//文件头占28bit	
					if(fmt->fmt_id==0x20746D66)//获取FMT块 	
					{
						if(fmt->format_id==0)
						{
							dsfx->audioformat=fmt->channel_type;		//音频格式
							dsfx->nchannels=fmt->channel_count;		//通道数
							dsfx->samplerate=fmt->samplerate;		//采样率
							dsfx->block_size=fmt->block_size;//4096Byte			//数据块大小
							printf("dsfx->audioformat:%d\r\n",dsfx->audioformat);
							printf("dsfx->nchannels:%d\r\n",dsfx->nchannels);
							printf("dsfx->samplerate:%d\r\n",dsfx->samplerate);
							printf("dsfx->block_size:%d\r\n",dsfx->block_size);
							data=(_data*)(buf+28+52);
							if(data->data_id==0X61746174)//解析成功! 'data'
							{
								dsfx->datastart=28+52+12;
								printf("dsfx->datastart:%d\r\n",dsfx->datastart);
							}else res=5;//data区域未找到.
						}else  res=4;//fmt->id !=0;
					}else res=3;//没有找到fmt块
				}else res=7;//头块数据长度不对
			}else res=2;//非dsd文件
		}else res=1;//打开文件错误
	}else res=6;//内存申请失败
	f_close(ftemp);
	myfree(SRAMIN,ftemp);//释放内存
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
//填充buf
//buf:数据区
//size:填充数据量
//返回值:读到的数据个数
u32 dsf_buffill_left(u8 *buf,u16 size)
{
	u32 bread;
	u16 i;
	f_read(audiodev.file,buf,size,(UINT*)&bread);
	if(bread<size)//不够数据了,补充0
	{
		for(i=bread;i<size-bread;i++)buf[i]=0; 
	}
	return bread;
}

//填充buf
//buf:数据区
//size:填充数据量
//返回值:读到的数据个数
u32 dsf_buffill_right(u8 *buf,u16 size)
{
	u32 bread;
	u16 i;
	u8 *p;
	f_read(audiodev.file,audiodev.tbuf,size,(UINT*)&bread);
//	p=audiodev.tbuf;			
//	for(i=0;i<size;)
//	{
//		buf[i++]=p[2];//低位
//		buf[i++]=p[3]; //次低位
//		buf[i++]=p[1];//次高位
//		buf[i++]=p[0];//高位
//		p+=4;
//	} 
	bread=bread;		//填充后的大小.
	if(bread<size)//不够数据了,补充0
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
			for(i=0;i<DSF_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				audiodev.i2sbuf1[i]=0;//填充0
			}
		}
	}
	else 
	{
		dsfwitchbuf_left=1;
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<DSF_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				audiodev.i2sbuf2[i]=0;//填充0
			}
		}
	}
	if(DMA1_Stream4->CR&(1<<19))
	{
		dsfwitchbuf_right=0;
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<DSF_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				audiodev.i2sbuf3[i]=0;//填充0
			}
		}
	}
	else 
	{
		dsfwitchbuf_right=1;
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<DSF_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				audiodev.i2sbuf4[i]=0;//填充0
			}
		}
	}
	dsftransferend=1;
}

//播放某个WAV文件
//fname:wav文件路径.
//返回值:
//KEY0_PRES:下一曲
//KEY1_PRES:上一曲
//其他:错误
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
		res=dsf_decode_init(fname,&dsfctrl);//得到文件的信息
		if(res==0)//解析文件成功
		{
				AK4495_I2S_Cfg(0x01,16,0);	//飞利浦标准,24位数据长度
				I2S3_Init(I2S_Standard_LSB,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);
				I2S2_Init(I2S_Standard_LSB,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);		//飞利浦标准,主机发送,时钟低电平有效,16位扩展帧长度
		}	
		I2S3_SampleRate_Set(88200);//设置采样率88.2KHZ
		I2S3_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,DSF_I2S_TX_DMA_BUFSIZE/2); //配置TX DMA
		I2S2_SampleRate_Set(88200);//设置采样率88.2KHZ
		I2S2_TX_DMA_Init(audiodev.i2sbuf3,audiodev.i2sbuf4,DSF_I2S_TX_DMA_BUFSIZE/2); //配置TX DMA
		i2s_tx_callback=dsf_i2s_dma_tx_callback;			//回调函数指wav_i2s_dma_callback
		audio_stop(0x02);
		res=f_open(audiodev.file,(TCHAR*)fname,FA_READ);	//打开文件
		if(res==0)
		{
			f_lseek(audiodev.file, dsfctrl.datastart);		//跳过文件头
			fillnum=dsf_buffill_left(audiodev.i2sbuf1,DSF_I2S_TX_DMA_BUFSIZE);
			fillnum=dsf_buffill_left(audiodev.i2sbuf3,DSF_I2S_TX_DMA_BUFSIZE);
			fillnum=dsf_buffill_right(audiodev.i2sbuf2,DSF_I2S_TX_DMA_BUFSIZE);
			fillnum=dsf_buffill_right(audiodev.i2sbuf4,DSF_I2S_TX_DMA_BUFSIZE);
			
			audio_start(0x02); 
			printf("playing^_^\r\n");				
			while(res==0)
			{ 
				while(dsftransferend==0);//等待wav传输完成; 
				dsftransferend=0;
				if(fillnum!=DSF_I2S_TX_DMA_BUFSIZE)//播放结束?
				{
					res=KEY0_PRES;
					break;
				} 
				if(dsfwitchbuf_left&&dsfwitchbuf_right)
				{
					fillnum=dsf_buffill_left(audiodev.i2sbuf2,DSF_I2S_TX_DMA_BUFSIZE);//填充buf2
					fillnum=dsf_buffill_right(audiodev.i2sbuf4,DSF_I2S_TX_DMA_BUFSIZE);//填充buf4
				}
				else if (dsfwitchbuf_left==0&&dsfwitchbuf_right==0)
				{
					fillnum=dsf_buffill_left(audiodev.i2sbuf1,DSF_I2S_TX_DMA_BUFSIZE);//填充buf1
					fillnum=dsf_buffill_right(audiodev.i2sbuf3,DSF_I2S_TX_DMA_BUFSIZE);//填充buf3
				}
				while(1)
				{
					key=KEY_Scan(0); 
					if(key==WKUP_PRES)//暂停
					{
						if(audiodev.status&0X01)audiodev.status&=~(1<<0);
						else audiodev.status|=0X01;  
					}
					if(key==KEY2_PRES||key==KEY0_PRES)//下一曲/上一曲
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
	myfree(SRAMIN,audiodev.tbuf);	//释放内存
	myfree(SRAMIN,audiodev.i2sbuf1);//释放内存
	myfree(SRAMIN,audiodev.i2sbuf2);//释放内存 
	myfree(SRAMIN,audiodev.file);	//释放内存 
	return res;
} 
