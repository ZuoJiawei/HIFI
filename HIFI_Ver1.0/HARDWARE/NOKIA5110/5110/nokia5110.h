#ifndef __NOKIA5110_H
#define __NOKIA5110_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 

////////////////////////////////////////////////////////////////////////////////// 	  
#define ce PEout(15)
#define reset PEout(14)
#define dc PEout(13)
#define sdin PEout(12)
#define sclk PEout(11)
//#define res    PAout(4)
//#define ce     PAout(5)				 
//#define dc     PAout(6)
//#define sdin   PAout(7)
//#define sclk   PBout(7)
void NOKIA5110_clear(void);
void NOKIA5110_init(void);
void NOKIA5110_set_XY(unsigned char X, unsigned char Y);
void NOKIA5110_write_english_string(unsigned char X,unsigned char Y,unsigned  char *s);
void NOKIA5110_write_char(unsigned char c);
void NOKIA5110_write_byte(unsigned char dat, unsigned char command);
void NOKIA5110_clear_row(unsigned char num);
void NOKIA5110_write_32(unsigned char x,unsigned char y,unsigned char c[32]);
void NOKIA5110_write_16(unsigned char x,unsigned char y,unsigned char c[16]);
void NOKIA5110_picture(unsigned char  *img_dat);
void NOKIA5110_pos_picture(unsigned char col, unsigned char pag, unsigned char x, unsigned char y, unsigned char  *img_dat);
void NOKIA5110_printn(unsigned char x, unsigned char y, unsigned int num, unsigned char num_bit);
void NOKIA5110_write_28(unsigned char x,unsigned char y,unsigned char c[32]);
#endif
