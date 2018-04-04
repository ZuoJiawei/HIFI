#ifndef __NOKIA5110_H_
#define __NOKIA5110_H_
#include "sys.h"
/*#define sce PBout(0)
#define res PBout(1)
#define dc PBout(2)
#define sdin PBout(3)
#define sclk PBout(4)*/
#define sce PEout(15)
#define Res PEout(14)
#define dc PEout(13)
#define sdin PEout(12)
#define sclk PEout(11)

void Nokia5110_WriteByte(u8 dt,u8 command );
void Nokia5110_GPIO_Init(void);
void Nokia5110_Init(void);
void Nokia5110_Set_XY(u8 X,u8 Y);
void Nokia5110_Clear(void);
void Nokia5110_Write_Shu(u8 row,u8 page,u8 c);
void Nokia5110_Write_Hanzi(u8 row,u8 page,u8 c);
void Nokia5110_int(void);
#endif
