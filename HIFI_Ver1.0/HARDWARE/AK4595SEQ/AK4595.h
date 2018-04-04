#ifndef __AK4595_H
#define __AK4595_H
#include "sys.h"    									
/*




*/

#define CONTROL_1        0x00
        #define ACKS        7  //������/�Զ�����λ ��ACKS ����λΪ1ʱ,256fs /384fs �ᵼ��3db�仯 �������ʱ�� DFS2-0bit ��������  �������õ�����111                                   
        #define EXDF        6  //�ⲿ��չ�˲�������λ
        #define ECS         5  //��768K��������λ
        #define DIF2        3  //����I2S��ʽ����λ3
        #define DIF1        2  //����I2S��ʽ����λ2
        #define DIF0        1   //����I2S��ʽ����λ1
        #define RSTN        0   //ϵͳ�ڲ�ʱ��ʱ��λ

#define CONTROL_2        0x01
        #define DZFE        7  //��������0λ��⹦�ܿ���λ ���������ͨ��DZFB������DZF����������ƽ
        #define DZFM        6  //��������0λ��⹦�ܿ���
        #define SD          5  //PCM�˲�����λ1
        #define DFS1        4  //����ģʽ���ֶ�PCM��������λ2
        #define DFS0        3  //����ģʽ���ֶ�PCM��������λ1
        #define DEM1        2  //Ĭ��0����
        #define DEM0        1  //Ĭ��0����
        #define SMUTE        0  //PCM/DSD��������

#define CONTROL_3        0x02
        #define DP          7 //DSD��������λ
        #define DCKS        5 //DSDģʽ�µ�ϵͳʱ������λ  0ʱ 512fs��fs=44.1K��=22.5792  1ʱ 768fs ��fs=44.1K��=33.8688
        #define DCKB        4 //DSDģʽ�������������,0�½��� 1������ 
        #define MONO        3 //������mono����
        #define DZFB        2 //DZFM ��������0λ��⹦�ܿ��������ͨ��DZFB������DZF�ŵ������ƽ
        #define SELLR       1 //��������������ѡ�� 0λΪ������ 1λΪ������
        #define SLOW        0  //PCM�˲�����λ2

#define Lch_ATT                0x03  //��������������
        #define ATT7        7
        #define ATT6        6
        #define ATT5        5
        #define ATT4        4
        #define ATT3        3
        #define ATT2        2
        #define ATT1        1
        #define ATT0        0

#define Rch_ATT                0x04 //��������������
        #define ATT7        7
        #define ATT6        6
        #define ATT5        5
        #define ATT4        4
        #define ATT3        3
        #define ATT2        2
        #define ATT1        1
        #define ATT0        0

#define CONTROL_4        0x05
        #define INVL        7  //�������������λ
        #define INVR        6  //�������������λ
        #define DFS2        1  //����ģʽ���ֶ�����PCM����ģʽλ3
        #define SSLOW        0  //������PCM�˲�����λ

#define CONTROL_5        0x06
        #define DDM                7  //DSD�ڲ�������������λ
        #define DML                6  //DSDR/L �źż���־λ
        #define DMR                5  //DSDR/L �źż���־λ
        #define DMC                4  //��DDM ��������λΪ1ʱ,ͨ��DMC�������ͷ����ֶ������Զ� DCMΪ1���ֶ� Ϊ0���Զ�
        #define DMRE               3  //��DDM bit =��1�� & DMC bit =��1���������ܿ���ʱ,�ͷž���λ
        #define DSDD1              2  //DSDD bit ="1" ��ʱ�� ����DSDһ��������� 50/150K
				#define DSDD0              1
        #define DSDSEL0            0  //DSD�ٶ����� 1 : 2.8224(64fs)  5.6448(128fs)  11.2896(256fs)

#define CONTROL_6        0x07
        #define SYNCE        0  //��Ƭ����ϵͳͬ������

#define CONTROL_7        0x08
				#define SC2                2  //���ʵȼ�����λ2
        #define SC1                1  //���ʵȼ�����λ1
        #define SC0                0  // ���ʵȼ�����λ0

#define AK4595_ADDR				0X1A	//WM8978��������ַ,�̶�Ϊ0X1A 
 
u8 AK4595_Init(void);
u8 AK4595_SetLen(u8 len);
u8 AK4495_I2S_Cfg(u8 mode,u8 len,u32 Lrck);
#endif





















