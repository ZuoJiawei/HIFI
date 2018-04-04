#include "audioplay.h"
#include "ff.h"
#include "malloc.h"
#include "usart.h"
#include "wm8978.h"
#include "i2s.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
#include "key.h"
#include "exfuns.h"  
#include "text.h"
#include "string.h"  
#include <stdio.h>
#include "sacd_dsf.h"
#include "mp3play.h"
#include "apeplay.h"
#include "flacplay.h"
#include "nokia5110.h"

 
u8 sound_device_ctrl(void);
//���ֲ��ſ�����
__audiodev audiodev;	  



//��ʼ��Ƶ����
void audio_start(u8 mode)
{
	audiodev.status=3<<0;//��ʼ����+����ͣ
	
	I2S_Play_Start(mode);
} 
//�ر���Ƶ����
void audio_stop(u8 mode)
{
	audiodev.status=0;
	I2S_Play_Stop(mode);
}  
//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 audio_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO tfileinfo;	//��ʱ�ļ���Ϣ		
	u8 *fn; 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); //��Ŀ¼
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X40)//ȡ����λ,�����ǲ��������ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	} 
	myfree(SRAMIN,tfileinfo.lfname);
	return rval;
}
//��ʾ��Ŀ����
//index:��ǰ����
//total:���ļ���
void audio_index_show(u16 index,u16 total)
{
	//��ʾ��ǰ��Ŀ������,������Ŀ��
	printf("%d/%d\r\n",index,total);			  	  
}
 
//��ʾ����ʱ��,������ ��Ϣ  
//totsec;��Ƶ�ļ���ʱ�䳤��
//cursec:��ǰ����ʱ��
//bitrate:������(λ��)
void audio_msg_show(u32 totsec,u32 cursec,u32 bitrate)
{	
	static u16 playtime=0XFFFF;//����ʱ����	      
	if(playtime!=cursec)					//��Ҫ������ʾʱ��
	{
		playtime=cursec;
		//��ʾ����ʱ��	
		printf("%d minute:%d second\r\n",playtime/60,playtime%60);
		//��ʾλ��		
		printf("%d Kbps\r\n",bitrate/1000);
	} 		 
}
//��������
void audio_play(void)
{
	u8 res;
 	DIR wavdir;	 		//Ŀ¼
	FILINFO wavfileinfo;//�ļ���Ϣ
	u8 *fn;   			//���ļ���
	u8 *pname;			//��·�����ļ���
	u16 totwavnum; 		//�����ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ		  
 	u16 temp;
	u16 *wavindextbl;	//����������
 	while(f_opendir(&wavdir,"0:/MUSIC"))//�������ļ���
 	{
			printf("CAN'T OPEN MUCIC FILE FOLDER\r\n");						  
	} 									  
	totwavnum=audio_get_tnum("0:/MUSIC"); //�õ�����Ч�ļ���
  while(totwavnum==NULL)//�����ļ�����Ϊ0		
 	{	    
			printf("NO MUSIC\r\n");
	}										   
  wavfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	wavfileinfo.lfname=mymalloc(SRAMIN,wavfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
 	pname=mymalloc(SRAMIN,wavfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�
 	wavindextbl=mymalloc(SRAMIN,2*totwavnum);				//����2*totwavnum���ֽڵ��ڴ�,���ڴ�������ļ�����
 	while(wavfileinfo.lfname==NULL||pname==NULL||wavindextbl==NULL)//�ڴ�������
 	{	    
			printf("STORAGE ALOCATE WRONG\r\n");	  
	}  	 
 	//��¼����
  res=f_opendir(&wavdir,"0:/MUSIC"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=wavdir.index;								//��¼��ǰindex
			res=f_readdir(&wavdir,&wavfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
			if(res!=FR_OK||wavfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
			fn=(u8*)(*wavfileinfo.lfname?wavfileinfo.lfname:wavfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X40)//ȡ����λ,�����ǲ��������ļ�	
			{
				wavindextbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	}   
	curindex=0;											//��0��ʼ��ʾ
	res=f_opendir(&wavdir,(const TCHAR*)"0:/MUSIC"); 	//��Ŀ¼
	while(res==FR_OK)//�򿪳ɹ�
	{	
		dir_sdi(&wavdir,wavindextbl[curindex]);			//�ı䵱ǰĿ¼����	   
		res=f_readdir(&wavdir,&wavfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
		if(res!=FR_OK||wavfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
		fn=(u8*)(*wavfileinfo.lfname?wavfileinfo.lfname:wavfileinfo.fname);			 
		strcpy((char*)pname,"0:/MUSIC/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)fn);  			//���ļ������ں���
		printf("%s\r\n",fn);//��ʾ�������� 
		
		audio_index_show(curindex+1,totwavnum);
		key=audio_play_song(pname); 			 		//���������Ƶ�ļ�
		if(key==KEY2_PRES)		                      //��һ��
		{
			if(curindex)curindex--;
			else curindex=totwavnum-1;
 		}else if(key==KEY0_PRES)//��һ��
		{
			curindex++;		   	
			if(curindex>=totwavnum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
 		}else break;	//�����˴��� 	 
	} 											  
	myfree(SRAMIN,wavfileinfo.lfname);	//�ͷ��ڴ�			    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,wavindextbl);			//�ͷ��ڴ�	 
} 
//����ĳ����Ƶ�ļ�
u8 audio_play_song(u8* fname)
{	
	u8 res;  
	res=f_typetell(fname); 
	switch(res)
	{
		case T_WAV:
			res=wav_play_song(fname);
		  NOKIA5110_write_english_string(0,0,fname);
			break;
		case T_DSF :
			res=dsf_play_song(fname);
			break;
		case T_MP3:
			res=mp3_play_song(fname);	//����MP3�ļ�
		break;
		case T_APE:
			res=ape_play_song(fname);
			break;
		case T_FLAC:
			res=flac_play_song(fname);
		break;
		default://�����ļ�,�Զ���ת����һ��
			printf("can't play:%s\r\n",fname);
			res=KEY0_PRES;
			break;
	}
	return res;
}


























