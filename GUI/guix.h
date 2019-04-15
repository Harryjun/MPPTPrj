#ifndef __GUIX_H
#define __GUIX_H 	
#include <stm32f10x.h>
#include "malloc.h"
#include "lcd.h"
#include "usart.h"
#include "delay.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//GUI-�ײ�ʵ�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/2/18
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//V1.1 20140218
//����gui_draw_bline����
//////////////////////////////////////////////////////////////////////////////////



 
//�õ�ucos�ĵδ�ʱ��
typedef unsigned int   INT32U;	 
extern volatile  INT32U  OSTime;
//guiʱ�������,��Ҫ���ⲿʵ��
//gui��ʱ��,ÿ10ms����1��
#define GUI_TIMER_10MS	OSTime/2 	//ucos�ĵ���ʱ������Ϊ5ms 

#define GUI_VERSION		101			//gui�汾(������100��),100��ʾV1.00
//////////////////////////////////////////////////////////////////////////////////////////////
//GUI֧�ֵ�����������Ŀ
//ϵͳ����������
#define GUI_LANGUAGE_NUM	   	3			//����������
											//0,��������
											//1,��������
											//2,Ӣ��

extern const u8* GUI_OK_CAPTION_TBL[GUI_LANGUAGE_NUM]; 			//ȷ�ϰ�ť
extern const u8* GUI_OPTION_CAPTION_TBL[GUI_LANGUAGE_NUM];		//ѡ�ť
extern const u8* GUI_BACK_CAPTION_TBL[GUI_LANGUAGE_NUM];		//ȡ����ť
extern const u8* GUI_CANCEL_CAPTION_TBL[GUI_LANGUAGE_NUM];		//ȡ����ť
extern const u8* GUI_QUIT_CAPTION_TBL[GUI_LANGUAGE_NUM];		//�˳���ť	 
////////////////////////////////////////////////////////////////////////////////////////////////////////
//gui��ɫ����
#define GUI_COLOR_BLACK 0X0000
#define GUI_COLOR_WHITE 0XFFFF


//�����¼�����
#define	IN_TYPE_TOUCH	0X00
#define IN_TYPE_KEY		0X01
#define IN_TYPE_JOYPAD  0X02
#define IN_TYPE_MOUSE	0X03
#define IN_TYPE_ERR	 	0XFF

//gui����ӿ�
__packed typedef struct 
{
	void (*get_key)(void*,u8);	//��ȡ��ֵ����
	u16 x;						//����
	u16 y;			
	u32 keyval;					//������ֵ
	u8  intype;					//��������
	u8 	ksta;					//�����豸(����)��״̬ [7:1],����;[0],0,�ɿ�;1,����.
}_in_obj;
extern _in_obj in_obj;

//gui�ײ�ӿ�
//Ҳ�����ⲿ��Ҫʵ�ֵĺ���!
//����������Ҫ�ⲿ�ṩ4������
//1,���㺯��;
//2,���㺯��;
//3,��ɫ��亯��;
//4,��ɫ��亯��
__packed typedef struct 
{
	u8 language;							//GUI��ǰ����,ȡֵ��Χ0~GUI_LANGUAGE_NUM-1;
	u16 back_color;							//����ɫ		   
	u16(*read_point)(u16,u16);				//���㺯��
	void(*draw_point)(u16,u16,u16);			//���㺯��
 	void(*fill)(u16,u16,u16,u16,u16);		//��ɫ��亯�� 	  
 	void(*colorfill)(u16,u16,u16,u16,u16*);	//��ɫ��亯�� 	 
	u16 lcdwidth;							//LCD�Ŀ��
	u16 lcdheight;							//LCD�ĸ߶�
}_gui_phy;
extern _gui_phy gui_phy;

u16 gui_alpha_blend565(u16 src,u16 dst,u8 alpha);						//alpha blending
u16 gui_color_chg(u32 rgb);												//��ɫת��.��32λɫתΪ16λɫ
u16 gui_rgb332torgb565(u16 rgb332);										//��rgb332תΪrgb565
long long gui_pow(u8 m,u8 n);											//�õ�m^n
u8* gui_path_name(u8 *pname,u8* path,u8 *name);

void gui_memset(void *p,u8 c,u32 len);
void *gui_memin_malloc(u32 size);
void gui_memin_free(void* ptr);
void *gui_memex_malloc(u32 size);			
void gui_memex_free(void* ptr);		 
void *gui_memin_realloc(void *ptr,u32 size);

void gui_init(void);
void gui_get_key(void* obj,u8 type);									 
u32 gui_disabs(u32 x1,u32 x2);											//��ľ���ֵ
void gui_alphablend_area(u16 x,u16 y,u16 width,u16 height,u16 color,u8 aval);
void gui_draw_bigpoint(u16 x0,u16 y0,u16 color);						//�����
void gui_draw_line(u16 x0,u16 y0,u16 x1,u16 y1,u16 color);				//��������
void gui_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color);	//��һ������
void gui_draw_rectangle(u16 x0,u16 y0,u16 width,u16 height,u16 color);	//������
void gui_draw_arcrectangle(u16 x,u16 y,u16 width,u16 height,u8 r,u8 mode,u16 upcolor,u16 downcolor);//��Բ�Ǿ���
void gui_draw_vline(u16 x0,u16 y0,u16 len,u16 color);								//����ֱ��
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color);								//��ˮƽ��
void gui_fill_colorblock(u16 x0,u16 y0,u16 width,u16 height,u16* ctbl,u8 mode);		//�����ɫ��
void gui_smooth_color(u32 srgb,u32 ergb,u16*cbuf,u16 len);							//��ɫƽ������
void gui_draw_smooth_rectangle(u16 x,u16 y,u16 width,u16 height,u32 srgb,u32 ergb);	//����ɫƽ�����ɵľ���/����
void gui_fill_rectangle(u16 x0,u16 y0,u16 width,u16 height,u16 color);				//�������
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color);
void gui_draw_ellipse(u16 x0, u16 y0, u16 rx, u16 ry,u16 color);
void gui_fill_ellipse(u16 x0,u16 y0,u16 rx,u16 ry,u16 color);
void gui_draw_argrec(u16 x0,u16 y0,u16 width,u16 height,u16 color);
void gui_show_strmid(u16 x,u16 y,u16 width,u16 height,u16 color,u8 size,u8 *str);
void gui_show_ptchar(u16 x,u16 y,u16 xend,u16 yend,u8 offset,u16 color,u16 size,u8 chr,u8 mode);
void gui_show_ptfont(u16 x,u16 y,u16 xend,u16 yend,u8 offset,u16 color,u16 size,u8* chr,u8 mode);
void gui_show_ptstr(u16 x,u16 y,u16 xend,u16 yend,u16 offset,u16 color,u8 size,u8 *str,u8 mode);
void gui_show_ptstrwhiterim(u16 x,u16 y,u16 xend,u16 yend,u16 offset,u16 color,u16 rimcolor,u8 size,u8 *str);//��Ч��ʾ�ַ���
void gui_draw_icos(u16 x,u16 y,u8 index);
void gui_draw_icosalpha(u16 x,u16 y,u8 index);
void gui_show_num(u16 x,u16 y,u8 len,u16 color,u8 size,long long num,u8 mode);
u8* gui_num2str(u8*str,u32 num);//������תΪ�ַ���
void gui_draw_arc(u16 sx,u16 sy,u16 ex,u16 ey,u16 rx,u16 ry,u16 r,u16 color,u8 mode);

u32 gui_get_stringline(u8*str,u16 linelenth,u8 font);
void gui_show_string(u8*str,u16 x,u16 y,u16 width,u16 height,u8 font,u16 fcolor);
u16 gui_string_forwardgbk_count(u8 *str,u16 pos);


void gui_show_str_center(u16 x,u16 y,u16 width,u16 height,u16 offset,u16 color,u16 rimcolor,u8 size,u8 *str);
//�޸ĺ��


#endif



























