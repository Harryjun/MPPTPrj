#include "edit.h"
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

//�����༭��
//top,left,width,height:����.
//id:�༭��ID
//type:�༭������
//[7:2]:����
//[1]:0,����ʾ���;1,��ʾ���;
//[0]:
//0,��׼edit,��֮������д��.
//1,�Զ����edit,��֮���Զ����,�ص�ԭλ.
//font:�༭������
_edit_obj * edit_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font)
{
 	_edit_obj * edit_crt;
	u16 temp;
	if(width<6+6||height<12+6)return NULL;//�ߴ粻��̫С		  
	edit_crt=(_edit_obj*)gui_memin_malloc(sizeof(_edit_obj));//�����ڴ�
	if(edit_crt==NULL)return NULL;//�ڴ���䲻��.
	edit_crt->top=top;
	edit_crt->left=left;
	edit_crt->width=width;
	edit_crt->height=height;
	edit_crt->id=id;
	edit_crt->type=type;
	edit_crt->sta=0;	 
	edit_crt->text=NULL;  
 	edit_crt->font=font;			//����
	temp=(edit_crt->width-6)/(edit_crt->font/2)+1;		
	edit_crt->text=gui_memin_malloc(temp);//Ϊedit�����ַ����洢�ռ�
	if(edit_crt->text==NULL)		//����ʧ��
	{
		edit_delete(edit_crt);
		return NULL;
	}
	gui_memset(edit_crt->text,0,temp);			//�����ڴ������
	edit_crt->cursorpos=0;					//���λ��Ϊԭ��
	edit_crt->edit_timer_old=0;			 	//ʱ��Ϊ0	 
	edit_crt->textcolor=EDIT_DFT_TXTC;
	edit_crt->textbkcolor=EDIT_DFT_TBKC;	 
    return edit_crt;
}
//ɾ���༭��
//edit_del:Ҫɾ���ı༭��
void edit_delete(_edit_obj * edit_del)
{
	gui_memin_free(edit_del->text);
	gui_memin_free(edit_del);
}	  
//�õ����ĵ�ǰx����
//editx:�༭��
u16 edit_get_cursorxpos(_edit_obj * editx)
{
	return editx->left+3+editx->cursorpos*(editx->font/2);//����X����;
}	  
//�������
//editx:�༭��
//color:�����ɫ
void edit_draw_cursor(_edit_obj * editx,u16 color)
{
	u16 x,y;
	u8 i;
	x=edit_get_cursorxpos(editx);//����X����
	y=editx->top+3;
 	for(i=0;i<editx->font;i++)
	{
		if(i>(editx->top+editx->height-3-y))break;//ʣ�ಿ�ֲ���Ҫ��ʾ
		gui_phy.draw_point(x,y+i,color);
 		gui_phy.draw_point(x+1,y+i,color);					 
	}
}		 
//��ʾ���
//editx:�༭��
//sta:0,����ʾ����;1,��ʾ����
void edit_show_cursor(_edit_obj * editx,u8 sta)
{
	u8 txt[3];
	u16 color;
	if((editx->type&0x02)==0)//������ʾ���
	{
		if((editx->sta&0x02)==0)sta=0;//֮ǰ������ʾ����,��������
		else return;
	}
	if(sta==0)
	{
		color=editx->textbkcolor;//����Ҫ��ʾ,�ñ���ɫ
 		editx->sta&=~(1<<1);//����״̬Ϊδ��ʾ
	}else 
	{
		color=editx->textcolor;//��Ҫ��ʾ,������ɫ
		editx->sta|=1<<1;//����״̬Ϊ��ʾ
	}	 
	edit_draw_cursor(editx,color);//��ʾ���
	if(sta==0)//��Ҫ��ʾ���
	{
		txt[0]=editx->text[editx->cursorpos];
		if(txt[0]>0X80)
		{
			txt[1]=editx->text[editx->cursorpos+1];
			txt[2]='\0';
		}else txt[1]='\0';
		gui_show_ptstr(edit_get_cursorxpos(editx),editx->top+3,editx->left+editx->width-3,editx->top+editx->height-3,0,editx->textcolor,editx->font,txt,0);//-4����Ϊ�߿�ռ��3������,�����һ������
	}
 
}	
//�����˸	 
//editx:�༭��
void edit_cursor_flash(_edit_obj *editx)
{
 	if(gui_disabs(editx->edit_timer_old,GUI_TIMER_10MS)>=50)//����ms��.
	{
		editx->edit_timer_old=GUI_TIMER_10MS;
		if(editx->sta&0x02)edit_show_cursor(editx,0);//֮ǰ����ʾ�����˵�
		else edit_show_cursor(editx,1);//֮ǰû����ʾ����	 
	}
}
//��ʾ���ִ�
//editx:�༭��
void edit_draw_text(_edit_obj * editx)
{
	gui_fill_rectangle(editx->left+2,editx->top+2,editx->width-3,editx->height-3,EDIT_BACK_COLOR);//����ڲ�
	BACK_COLOR=EDIT_BACK_COLOR;
 	gui_show_ptstr(editx->left+3,editx->top+3,editx->left+editx->width-3,editx->top+editx->height-3,0,editx->textcolor,editx->font,editx->text,0);
	editx->sta&=~(1<<1);		//���ù��״̬Ϊδ��ʾ
	edit_show_cursor(editx,1); 	//��ʾ���
}

//��edit��������ַ���
//editx:�༭��
//str:�ַ���
void edit_add_text(_edit_obj * editx,u8 * str)
{				  
	u8 *tempstr;
	u16 temp;
	u16 temp2;
	u8 *laststr='\0';
	if(*str!=NULL)//������
	{
		tempstr=editx->text+editx->cursorpos;
		if(*str!=0x08)//�����˸�
		{
			temp=strlen((const char*)str)+strlen((const char*)editx->text);//�õ�������Ӻ�ĳ���
			temp2=(editx->width-6)/(editx->font/2);//�õ��༭����֧�ֵ���󳤶�
			if(temp2<temp)
			{
				if(editx->type&0x01)//�Զ������edit,������Χֱ�����
				{
					editx->cursorpos=0;//����0
					tempstr='\0';	
				}else return;//̫����.ֱ�Ӳ�ִ�в���
			}
		}
		if(tempstr!='\0')//���ǽ�����
		{
			temp=strlen((const char*)tempstr);//�õ�tempstr�ĳ���
			laststr=gui_memin_malloc(temp+1);//�����ڴ�
			if(laststr==NULL)return;//����ʧ��,ֱ���˳�.
			laststr[0]='\0';//������
			strcpy((char*)laststr,(const char*)tempstr);//����tempstr�����ݵ�laststr
		}
		if(*str==0x08)//�˸�
		{
			if(editx->cursorpos>0)//�����˸�
			{
				editx->cursorpos--;
				if(editx->text[editx->cursorpos]>0x80)editx->cursorpos--;//�Ǻ���,Ҫ��2��
				editx->text[editx->cursorpos]='\0';//��ӽ�����
				strcat((char*)editx->text,(const char*)laststr);//�ٽ�����ƴ����   	
			} 	
		}else if(*str>=' ')//���ַ���
		{
			editx->text[editx->cursorpos]='\0';//��ӽ�����
			strcat((char*)editx->text,(const char*)str);//������   	
			editx->cursorpos+=strlen((const char*)str);//���ƫ��
			strcat((char*)editx->text,(const char*)laststr);//�������ٴ�����   	
		}
		gui_memin_free(laststr);//�ͷ��ڴ�
		edit_draw_text(editx);//������ʾ�ַ���
	}
}	 	  
//���༭��
//editx:�༭��
void edit_draw(_edit_obj * editx)
{
	u8 sta;
	if(editx==NULL)return;//��Ч,ֱ���˳�
	sta=editx->sta&0x03;
	gui_draw_vline(editx->left,editx->top,editx->height,EDIT_RIM_LTOC);			//������
 	gui_draw_hline(editx->left,editx->top,editx->width,EDIT_RIM_LTOC);			//������
	gui_draw_vline(editx->left+1,editx->top+1,editx->height-2,EDIT_RIM_LTIC);	//������
 	gui_draw_hline(editx->left+1,editx->top+1,editx->width-2,EDIT_RIM_LTIC);	//������
	gui_draw_vline(editx->left+editx->width-1,editx->top,editx->height,EDIT_RIM_RBOC);		//������
 	gui_draw_hline(editx->left,editx->top+editx->height-1,editx->width,EDIT_RIM_RBOC);		//������
	gui_draw_vline(editx->left+editx->width-2,editx->top+1,editx->height-2,EDIT_RIM_RBIC);	//������
 	gui_draw_hline(editx->left+1,editx->top+editx->height-2,editx->width-2,EDIT_RIM_RBIC);	//������
	edit_draw_text(editx); 

	if(sta&0x80)//�༭��ѡ����Ч
	{
	}else
	{
	}	 	    		
}

		 
//����x��������cursor��λ��
//editx:�༭��
//x:x����
void edit_cursor_set(_edit_obj * editx,u16 x)
{
	u16 newpos;
	u16 maxpos=strlen((const char *)editx->text);
	if(x>editx->left+3)
	{
		x-=editx->left+3;
		newpos=x/(editx->font/2);
		if((x%(editx->font/2))>editx->font/2)newpos+=1;//����font/2,����Ϊ����һ��λ��
		edit_show_cursor(editx,0);		//���ǰһ�ι���λ��
		if(newpos>maxpos)newpos=maxpos;	//���ܳ���text�ĳ���
		if(newpos>0)
		{
			maxpos=gui_string_forwardgbk_count(editx->text,newpos-1);//�õ�֮ǰ�ĺ����������.
			if((maxpos%2)!=0)newpos++;//���ں����м�,��Ҫ����һ���ֽ�
		}
		editx->cursorpos=newpos;
		edit_show_cursor(editx,1);//������ʾ�µĹ��
	} 	
}

//���	
//editx:�༭��
//in_key:���밴��ָ��
//����ֵ:δ�õ�			 					   
u8 edit_check(_edit_obj * editx,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
   	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//������������
		 	edit_cursor_flash(editx);
			if(editx->top<key->y&&key->y<(editx->top+editx->height)&&(editx->left<key->x)&&key->x<(editx->left+editx->width))//��edit���ڲ�
			{
				edit_cursor_set(editx,key->x);//�õ���ǰ����x����
					 
			}	    
 			break;
		case IN_TYPE_KEY:	//��������
			break;
		case IN_TYPE_JOYPAD://�ֱ�����
			break;
		case IN_TYPE_MOUSE:	//�������
			break;
		default:
			break;
	}								  
	return 0;
}


//////////////////////////////////////////////////////////////////////////
void edit_test(u16 x,u16 y,u16 width,u16 height,u8 type,u8 sta,u8 *text)
{
	_edit_obj* tedit;
	tedit=edit_creat(x,y,width,height,0,type,16);//������ť
	if(tedit==NULL)return;//����ʧ��.
	tedit->sta=sta;
	strcpy((char*)tedit->text,(const char *)text);
	edit_draw(tedit);//����ť
 	edit_delete(tedit);//ɾ����ť	 
} 






















