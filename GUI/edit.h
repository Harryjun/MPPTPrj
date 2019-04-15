#ifndef __EDIT_H
#define __EDIT_H 			 
#include "guix.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//GUI-�༭�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/10/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//�༭��߿���ɫ����
#define EDIT_RIM_LTOC 	0XA535	    //����������ɫ
#define EDIT_RIM_LTIC 	0X8431		//����������ɫ
#define EDIT_RIM_RBOC 	0XFFFF		//����������ɫ
#define EDIT_RIM_RBIC 	0XDEFC		//����������ɫ
#define EDIT_BACK_COLOR	0XFFFF		//�༭�򱳾�ɫ

//�༭���ı���ɫ����
#define EDIT_DFT_TBKC 	0XFFFF		//Ĭ�ϵ��ı�����ɫ
#define EDIT_DFT_TXTC 	0X0000		//Ĭ�ϵ��ı���ɫ


//EDIT�ṹ�嶨��
__packed typedef struct 
{
	u16 top; 				  		//�༭�򶥶�����
	u16 left;                       //�༭���������
	u16 width; 				  		//���(���ַ����ĳ��ȶ�6����)
	u16 height;						//�߶�(������߶ȸ�6����)

	u8 id;							//�༭��ID
	u8 type;						//�༭������
									//[7:2]:����
									//[1]:0,����ʾ���;1,��ʾ���;
									//[0]:
									//0,��׼edit,��֮������д��.
									//1,�Զ����edit,��֮���Զ����,�ص�ԭλ.

	u8 sta;							//�༭��״̬
									//[7:1]:����
									//[1]:�����ʾ״̬ 0,δ��ʾ.1,��ʾ.	  
									//[0]:�༭��ѡ��״̬ 0,δѡ��.1,ѡ��.	  

	u8 *text;						//�༭�������,����<=width-6
	u8 font;						//text��������
	u16 cursorpos;					//����λ��((font/2)*x����)   
	u32 edit_timer_old;				//��һ��ʱ��

	u16 textcolor; 				  	//������ɫ
	u16 textbkcolor;				//���ֱ���ɫ
}_edit_obj;

_edit_obj * edit_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font);
void edit_delete(_edit_obj * edit_del);
u16 edit_get_cursorxpos(_edit_obj * editx);
void edit_draw(_edit_obj * editx);
void edit_read_cursorbkctbl(_edit_obj * editx);
void edit_draw_cursor(_edit_obj * editx,u16 color);
void edit_recover_cursorbkctbl(_edit_obj * editx);
void edit_show_cursor(_edit_obj * editx,u8 sta);
void edit_cursor_flash(_edit_obj *editx);
void edit_draw_text(_edit_obj * editx);	  
void edit_add_text(_edit_obj * editx,u8 * str);
void edit_draw(_edit_obj * editx);
u8 edit_check(_edit_obj * editx,void * in_key);	 
 
//
void edit_test(u16 x,u16 y,u16 width,u16 height,u8 type,u8 sta,u8 *text);
#endif

















