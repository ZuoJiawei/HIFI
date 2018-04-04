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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200 
	IIS_plugin();
	AK4595_Init();
 	KEY_Init();					//按键初始化  
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  res=f_mount(fs[0],"0:",1); 		//挂载SD卡  
	NOKIA5110_init(); //初始化液晶  
	NOKIA5110_clear();
	while(1)
	{
		 audio_play();
	} 
}







