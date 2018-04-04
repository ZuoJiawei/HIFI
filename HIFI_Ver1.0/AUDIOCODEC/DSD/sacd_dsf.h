/*
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

#ifndef _SACD_DSF_H_INCLUDED
#define _SACD_DSF_H_INCLUDED

#include "stm32f4xx.h"
#include <stdint.h>

#define SACD_SAMPLING_FREQUENCY        2822400
#define FALSE  0
#define TRUE   1

typedef __packed struct  {
  uint8_t   minutes;//D  S  D  /
	uint8_t  seconds;//u64     headsize     /
	uint8_t  frames;
}duration;

typedef enum
{
    _false=FALSE,
    _true=TRUE,
}_bool;

typedef __packed struct  {
  uint32_t   id;//D  S  D  /
	uint64_t  headsize;//u64     headsize     /
	uint64_t  file_size;//  u64 file_size      /
	uint64_t id3_offset;//ָ��ĵط���������Ŀ��Ϣ 
}_head;

typedef __packed struct {
	uint32_t  fmt_id;//f  m  t  /
	uint64_t fmt_size;// u64  fmt_size       /
	uint32_t format_version;//u32format_version/
	uint32_t format_id;//U32 format_id/
	uint32_t channel_type;//u32 channel_type /
	uint32_t channel_count;//u32 channel_count /
	uint32_t samplerate;//u32 samplerate/
	uint32_t bits_per_sample;//u32 bits_persample/
	uint64_t sample_count;//��������,��1bit����Ĳ�������,����������bit����*2��ת��Ϊ�ֽ�Ҫ/8
	uint32_t block_size;  //���С
	uint32_t reserved;//����
}_fmt;
typedef __packed struct  {
	uint32_t  data_id;
	uint64_t data_size;
}_data;

typedef struct DsfChunk   {
_head head;
_fmt  fmt;
_data data;
}DsfChunk;

typedef __packed struct
{ 
	u16 audioformat;			//��Ƶ��ʽ;0X01,��ʾ����PCM;0X11��ʾIMA ADPCM
	u16 nchannels;				//ͨ������;1,��ʾ������;2,��ʾ˫����;   
	u32 block_size;					//DSF���ݴ�С 

	u32 totsec ;					//���׸�ʱ��,��λ:��
	u32 cursec ;					//��ǰ����ʱ��

	u32 bitrate;	   			//������(λ��)
	u32 samplerate;				//������ 
	
	u32 datastart;				//����֡��ʼ��λ��(���ļ������ƫ��)
}__dsfctrl; 

u8 dsf_play_song(u8* fname);


#endif
