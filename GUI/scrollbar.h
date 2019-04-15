#ifndef __SCROLLBAR_H
#define __SCROLLBAR_H 		 
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

//�������ϵķ��ŵ���ɫ
#define SCROLLBAR_SIGN_RCOLOR 	0X0000	    //�����ɿ���ɫΪ��ɫ
#define SCROLLBAR_SIGN_PCOLOR 	0XFFFF	    //���Ű�����ɫΪ��ɫ

//�������ߴ��趨
#define SCROLLBAR_PART_LEN 		20			//���˰�ť�ĳ���(�����)
#define SCROLLBAR_MIN_THICK		10			//��С�ĺ��(���߶�)

//������
#define SCROLLBAR_DIR_HOR		0X00		//ˮƽscrollbar
#define SCROLLBAR_DIR_VER		0X80		//��ֱscrollbar
//ͼ����
#define SCROLLBAR_SIGN_UP	 	0X00	    //��ͼ��
#define SCROLLBAR_SIGN_DOWN	 	0X01	    //��ͼ��
#define SCROLLBAR_SIGN_LEFT	 	0X02	    //��ͼ��
#define SCROLLBAR_SIGN_RIGHT 	0X03	    //��ͼ��
#define SCROLLBAR_SIGN_HOR	 	0X04	    //ˮƽͼ��
#define SCROLLBAR_SIGN_VER 		0X05	    //��ֱͼ��
//��ť����
#define SCROLLBAR_BTN_LTP 		0X01	    //���/������ť������
#define SCROLLBAR_BTN_MID 		0X02	    //�м䰴��
#define SCROLLBAR_BTN_RBP 		0X04	    //�Ҷ�/�ײ���ť������
//���������Ͷ���
#define SCROLLBAR_TYPE_STD	 	0X00
#define SCROLLBAR_TYPE_NBT   	0X01

//������Ĭ����ɫ 
#define SCLB_DFT_INBKCOLOR		0XFFFF		//���ɫA��ɫ
#define SCLB_DFT_BTNCOLOR		0X2DDA //0X031F		//���ɫB��ɫ
#define SCLB_DFT_RIMCOLOR		0X4A49		//�߿���ɫ

//�������ṹ�嶨��
__packed typedef struct 
{
	u16 top; 				  		//��ť��������
	u16 left;                       //��ť�������
	u16 width; 				  		//���(���ٴ���40)
	u16 height;						//�߶�(���ٴ���2)

	u8 type;						//���ͱ���ֽ� [bit7]:����,0,ˮƽ;1,��ֱ;bit[6:2]:����;[bit1:0]:��ť����,0,��׼������;1,û�ж˰�ť�Ĺ�����;2,3,����.
	u8 sta;							//��ť״̬ bit0:���/������ť;bit1:�м䰴ť;bit2:�Ҷ�/�ײ���ť.	 
	u8 id;							//������id

 	u16 totalitems;					//����Ŀ��
	u16 itemsperpage;				//ÿҳ��ʾ����Ŀ��
	u16 topitem;					//�/��˵���Ŀ 
	u16 scbbarlen;					//��������
 
	u16 inbkcolor; 					//�ڲ���ɫ
	u16 btncolor;					//���Ű�ť���ɫ
	u16 rimcolor;					//�߿���ɫ
}_scrollbar_obj;


_scrollbar_obj * scrollbar_creat(u16 left,u16 top,u16 width,u16 height,u8 type);
void scrollbar_delete(_scrollbar_obj *scrollbar_del);	 
void scrollbar_draw_sign(u16 x,u16 y,u16 signcolor,u8 signx);
void scrollbar_draw_btn(u16 x,u16 y,u16 width,u16 height,u16 bkcolor,u16 outcolor,u16 signcolor,u8 signx);
void scrollbar_setpos(_scrollbar_obj *scbx);				//���û�����λ��
void scrollbar_draw_scrollbar(_scrollbar_obj *scbx);		//��������
u8 scrollbar_check(_scrollbar_obj * scbx,void * in_key);	//����������������
void scrollbar_hook(_scrollbar_obj *scbx);					//���Ӻ���

void test_scb(u16 x,u16 y,u16 width,u16 height,u8 type,u16 icur,u16 incolor,u16 btncolor,u16 outcolor);
#endif





