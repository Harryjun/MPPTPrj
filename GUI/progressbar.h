#ifndef __PROGRESSBAR_H
#define __PROGRESSBAR_H 	 
#include "guix.h" 	
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//GUI-������ ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/10/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

////�������ϵķ��ŵ�Ĭ����ɫ 
#define PRGB_DFT_BKACOLOR		0XEF7D		//����ɫA��ɫ
#define PRGB_DFT_BKBCOLOR		0XE73C		//����ɫB��ɫ
#define PRGB_DFT_FILLACOLOR		0X0FBB		//���ɫA��ɫ
#define PRGB_DFT_FILLBCOLOR		0X071F		//���ɫB��ɫ
#define PRGB_DFT_RIMCOLOR		0X4A49		//�߿���ɫ
#define PRGB_DFT_BTNCOLOR		0X0000		//������/������ɫ,��ɫ

//������type
#define PRGB_TYPE_HOR 			0X00		//ˮƽ��
#define PRGB_TYPE_VER			0X80		//��ֱ��
#define PRGB_TYPE_FEN			0X40  		//�����Ƿ���ʾ(�ٷֽ�������ʱ��,��ʾ�ٷ����Ƿ���ʾ)
#define PRGB_TYPE_PEN			0X20  		//�������Ƿ���ʾ
#define PRGB_TYPE_STD			0X00  		//��׼������
#define PRGB_TYPE_TEXT			0X01  		//�ٷ���������

#define PRGB_BTN_DOWN			0X01  		//�м䰴ť��������


//�������ṹ�嶨��
__packed typedef struct 
{
	u16 top; 				  		//��������������
	u16 left;                       //�������������
	u16 width; 				  		//���(���ٴ���40)
	u16 height;						//�߶�(���ٴ���2)

	u8 type;						//���ͱ���� 
									//[bit7]:����,0,ˮƽ;1,��ֱ;
									//[bit6]:0,����ʾ����;1,��ʾ����;
									//[bit5]:0,����ʾ������,1,��ʾ������(��ʵ������ɫ�Ƿ����,�Ƿ���Ÿ��������ɫ.)
									//[bit4:2]:����;
									//[bit1:0]:0,��׼������;1,�ٷ���������;2,3,����.
	u8 sta;							//������״̬ bit0:������������.	 
	u8 id;							//������id

 	u32 totallen;					//�ܳ���		    
	u32 curpos;						//��ǰλ��
	u16 prgbarlen;					//�������� ���û��趨.���ڱ�׼������ʱ��Ч
 
	u16 inbkcolora; 				//�ڲ�������ɫ
	u16 inbkcolorb; 				//�ڲ�������ɫ
	u16 infcolora; 					//�ڲ������ɫa
	u16 infcolorb; 					//�ڲ������ɫb
 	u16 btncolor;					//�������ɫ(��׼prgʱΪ�ڲ����������ɫ,�ٷ���prgʱ,Ϊ������ɫ,��չʱ,��Ч)
	u16 rimcolor;					//�߿���ɫ
}_progressbar_obj;


_progressbar_obj * progressbar_creat(u16 left,u16 top,u16 width,u16 height,u8 type);
u8 progressbar_check(_progressbar_obj * prgbx,void * in_key);
void progressbar_delete(_progressbar_obj *progressbar_del);	 
void progressbar_hook(_progressbar_obj *prgbx);
void progressbar_draw_progressbar(_progressbar_obj *prgbx);
void progressbar_setpos(_progressbar_obj *prgbx);
void progressbar_draw_sign(_progressbar_obj *prgbx,u16 x,u16 y);
void progressbar_show_percnt(u16 x,u16 y,u8 dir,u8 num,u16 color);

#endif





