#ifndef __WINDOW_H
#define __WINDOW_H 			 
#include "guix.h"  
#include "button.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//GUI-���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/10/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//Ĭ�ϵĴ���caption����ɫcaption����ɫ.
//�Լ����屳��ɫ.
#define WIN_CAPTION_UPC	   	  			0X7D7C	//����CAPTION�ϲ������ɫ
#define WIN_CAPTION_DOWNC				0X5458	//����CAPTION�²������ɫ
#define WIN_CAPTION_COLOR				0XFFFF	//��������ɫΪ��ɫ
#define WIN_BODY_BKC					0XD699	//���ڵ�ɫ,0XEF5D

#define WIN_CAPTION_HEIGHT				0X20	//Ĭ�ϵĸ߶�

//�����ϵĹرհ�ť�����Ϣ����
#define WIN_BTN_SIZE					0X18	//�����γ� ��߳�(����С��WIN_CAPTION_HEIGHT)
#define WIN_BTN_OFFSIDE					0X06	//��ť/����ƫ��߿������

#define WIN_BTN_FUPC					0XFFFF	//��ť�ɿ�ʱ���ֵ���ɫ
#define WIN_BTN_FDOWNC					0XDCD0	//��ť����ʱ���ֵ���ɫ
#define WIN_BTN_RIMC					0XFE18	//�߿���ɫ
#define WIN_BTN_TP1C					0XF36D	//��һ�е���ɫ
#define WIN_BTN_UPHC					0XF36D	//�ϰ벿����ɫ,0X6D3C
#define WIN_BTN_DNHC					0XC800	//�°벿����ɫ,0XAE5C



//��׼����߿���ɫ����
#define STD_WIN_RIM_OUTC 	0X3317	    //������ɫ
#define STD_WIN_RIM_MIDC 	0X859B		//������ɫ
#define STD_WIN_RIM_INC 	0X2212		//������ɫ	    		   
//Բ�ߴ���߿���ɫ����
#define ARC_WIN_RIM_COLOR 	0XB595	    //������ɫ  


//window�ṹ�嶨��
__packed typedef struct 
{
	u16 top; 				  		//window��������
	u16 left;                       //window�������
	u16 width; 				  		//window���(�����������Ŀ��)
	u16 height;						//window�߶�

	u8 id;							//window
	u8 type;						//window����
									//[7]:0,û�йرհ�ť.1,�йرհ�ť			   
									//[6]:0,����ȡ����ɫ.1,��ȡ����ɫ.					 
									//[5]:0,���⿿��.1,�������.					 
									//[4:2]:����					 
									//[1:0]:0,��׼�Ĵ���(��XP);1,Բ�ߴ���(��Andriod)			   

	u8 sta;							//window״̬
 									//[7:0]:����

	u8 *caption;					//window����
	u8 captionheight;				//caption���ĸ߶�
	u8 font;						//window��������
	u8 arcwinr;						//Բ�Ǵ��ڵ�Բ�ǵİ뾶	
										
	u16 captionbkcu; 				//caption���ϰ벿�ֱ���ɫ
	u16 captionbkcd; 				//caption���°벿�ֱ���ɫ
 	u16 captioncolor; 				//caption����ɫ
	u16 windowbkc; 					//window�ı���ɫ

	u16 *bkctbl;					//����ɫ��(��Ҫ��ȡ���屳��ɫ��ʱ���õ�)
	_btn_obj* closebtn;				//���ڹرհ�ť
}_window_obj;

_window_obj * window_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font);
void window_delete(_window_obj * window_del);
void window_draw(_window_obj * windowx);
u8 window_msg_box(u16 x,u16 y,u16 width,u16 height,u8 *str,u8 *caption,u8 font,u16 color,u8 mode);





void win_test(u16 x,u16 y,u16 width,u16 height,u8 type,u16 cup,u16 cdown,u8 *caption);
#endif

















