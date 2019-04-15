#ifndef __MEMO_H
#define __MEMO_H 			 
#include "guix.h"  
#include "scrollbar.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//GUI-�ı��� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/10/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//memoĬ�ϵ�������ɫ������ɫ		 
#define MEMO_DFT_TBKC 	0XFFFF		//Ĭ�ϵ��ı�����ɫ
#define MEMO_DFT_TXTC 	0X0000		//Ĭ�ϵ��ı���ɫ

//memo�Ĺ�������ȶ���
#define MEMO_SCB_WIDTH 	0X0E		//���������Ϊ15������


//memo�ṹ�嶨��
__packed typedef struct 
{
	u16 top; 				  		//memo��������
	u16 left;                       //memo�������
	u16 width; 				  		//memo���(�����������Ŀ��)
	u16 height;						//memo�߶�

	u8 id;							//memoID
	u8 type;						//memo����
									//[7:1]:����					 
									//[0]:0,������༭;1,����༭			   

	u8 sta;							//memo״̬
									//[7]:0,memoû�м�⵽�����¼�;1,�а����¼�.
									//[6]:��ʱ������Ч��־
									//[5]:������־
									//[4:2]:����
									//[1]:�����ʾ״̬ 0,δ��ʾ.1,��ʾ.	  
									//[0]:memoѡ��״̬ 0,δѡ��.1,ѡ��.	  

	u32 textlen;					//memo�ַ���text�ĳ���
	u8 *text;						//memo������,��textlen�Ĵ�С������
	u32 *offsettbl;					//�ַ���ƫ������,��¼ÿ�п�ʼ���ַ�����text�����λ��,��СΪ:scbv->totalitems
	u8 font;						//text��������
											  
 	u16 lin;						//������ڵ���λ��(��ָ������)
	u16 col;						//���������λ��(����ȳ���(font/2)����)

	u16 typos;						//��ʱ��y����
	u16 txpos;						//��ʱ��x����

	u32 memo_timer_old;				//��һ��ʱ��

	_scrollbar_obj * scbv;			//��ֱ������
	u16 textcolor; 				  	//������ɫ
	u16 textbkcolor;				//���ֱ���ɫ
}_memo_obj;

_memo_obj * memo_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font,u32 textlen);
void memo_delete(_memo_obj * memo_del);
void memo_draw(_memo_obj * memox);


u8 memo_set_fontsize(_memo_obj * memox,u8 size);
u16 memo_get_cursorxpos(_memo_obj * memox);
void memo_read_cursorbkctbl(_memo_obj * memox);
void memo_draw_cursor(_memo_obj * memox,u16 color);
void memo_recover_cursorbkctbl(_memo_obj * memox);
void memo_show_cursor(_memo_obj * memox,u8 sta);
void memo_cursor_flash(_memo_obj *memox);
void memo_draw_text(_memo_obj * memox);	  
void memo_add_text(_memo_obj * memox,u8 * str);
void memo_draw_memo(_memo_obj * memox);
u8 memo_check(_memo_obj * memox,void * in_key);	 
 
//
void memo_test(u16 x,u16 y,u16 width,u16 height,u8 type,u8 sta,u16 textlen);

#endif

















