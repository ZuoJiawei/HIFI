#ifndef __AK4595_H
#define __AK4595_H
#include "sys.h"    									
/*




*/

#define CONTROL_1        0x00
        #define ACKS        7  //设置手/自动设置位 当ACKS 设置位为1时,256fs /384fs 会导致3db变化 而且这个时候 DFS2-0bit 无需设置  所以设置到保留111                                   
        #define EXDF        6  //外部扩展滤波器设置位
        #define ECS         5  //打开768K采样设置位
        #define DIF2        3  //接受I2S格式设置位3
        #define DIF1        2  //接受I2S格式设置位2
        #define DIF0        1   //接受I2S格式设置位1
        #define RSTN        0   //系统内部时钟时序复位

#define CONTROL_2        0x01
        #define DZFE        7  //输入数据0位检测功能开启位 开启后可以通过DZFB来控制DZF输出脚输出电平
        #define DZFM        6  //输入数据0位检测功能开启
        #define SD          5  //PCM滤波设置位1
        #define DFS1        4  //串行模式下手动PCM采样设置位2
        #define DFS0        3  //串行模式下手动PCM采样设置位1
        #define DEM1        2  //默认0即可
        #define DEM0        1  //默认0即可
        #define SMUTE        0  //PCM/DSD静音控制

#define CONTROL_3        0x02
        #define DP          7 //DSD开启设置位
        #define DCKS        5 //DSD模式下的系统时钟设置位  0时 512fs（fs=44.1K）=22.5792  1时 768fs （fs=44.1K）=33.8688
        #define DCKB        4 //DSD模式下输出极性设置,0下降沿 1上升沿 
        #define MONO        3 //单声道mono设置
        #define DZFB        2 //DZFM 输入数据0位检测功能开启后可以通过DZFB来控制DZF脚的输出电平
        #define SELLR       1 //单声道左右声道选择 0位为左声道 1位为右声道
        #define SLOW        0  //PCM滤波设置位2

#define Lch_ATT                0x03  //左声道音量控制
        #define ATT7        7
        #define ATT6        6
        #define ATT5        5
        #define ATT4        4
        #define ATT3        3
        #define ATT2        2
        #define ATT1        1
        #define ATT0        0

#define Rch_ATT                0x04 //右声道音量控制
        #define ATT7        7
        #define ATT6        6
        #define ATT5        5
        #define ATT4        4
        #define ATT3        3
        #define ATT2        2
        #define ATT1        1
        #define ATT0        0

#define CONTROL_4        0x05
        #define INVL        7  //左声道输出反向位
        #define INVR        6  //右声道输出反向位
        #define DFS2        1  //串行模式下手动设置PCM采样模式位3
        #define SSLOW        0  //超缓慢PCM滤波设置位

#define CONTROL_5        0x06
        #define DDM                7  //DSD内部静音功能设置位
        #define DML                6  //DSDR/L 信号检测标志位
        #define DMR                5  //DSDR/L 信号检测标志位
        #define DMC                4  //当DDM 开启设置位为1时,通过DMC来控制释放是手动还是自动 DCM为1：手动 为0：自动
        #define DMRE               3  //当DDM bit =“1” & DMC bit =“1“静音功能开启时,释放静音位
        #define DSDD1              2  //DSDD bit ="1" 的时候 搭配DSD一起过滤特性 50/150K
				#define DSDD0              1
        #define DSDSEL0            0  //DSD速度设置 1 : 2.8224(64fs)  5.6448(128fs)  11.2896(256fs)

#define CONTROL_6        0x07
        #define SYNCE        0  //多片并联系统同步设置

#define CONTROL_7        0x08
				#define SC2                2  //音质等级设置位2
        #define SC1                1  //音质等级设置位1
        #define SC0                0  // 音质等级设置位0

#define AK4595_ADDR				0X1A	//WM8978的器件地址,固定为0X1A 
 
u8 AK4595_Init(void);
u8 AK4595_SetLen(u8 len);
u8 AK4495_I2S_Cfg(u8 mode,u8 len,u32 Lrck);
#endif





















