#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "key.h"    
#include "malloc.h"  
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "audioplay.h"	
#include "i2s.h"
#include "AK4595.h"
#include "nokia5110.h"
#include "delay.h"


int main(void)
{   
	FRESULT res;	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200 
	IIS_plugin();
	AK4595_Init();
 	KEY_Init();					//������ʼ��  
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
  res=f_mount(fs[0],"0:",1); 		//����SD��  
	NOKIA5110_init(); //��ʼ��Һ��  
	NOKIA5110_clear();
	while(1)
	{
		 audio_play();
	} 
}







