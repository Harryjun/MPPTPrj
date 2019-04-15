#include "window.h"
#include "touch.h"
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
 
//��ȡ����ɫ
//btnx:��ťָ��
void window_read_backcolor(_window_obj * windosx)
{
	u16 x,y,ccnt;
	ccnt=0;
	for(y=windosx->top;y<windosx->top+windosx->height;y++)
	{
		for(x=windosx->left;x<windosx->left+windosx->width;x++)
		{
			windosx->bkctbl[ccnt]=gui_phy.read_point(x,y);//��ȡ��ɫ
			ccnt++;
		}
	}

}
//�ָ�����ɫ
//btnx:��ťָ��
void window_recover_backcolor(_window_obj * windosx)
{
	u16 x,y,ccnt;
	ccnt=0;	    
	for(y=windosx->top;y<windosx->top+windosx->height;y++)
	{
		for(x=windosx->left;x<windosx->left+windosx->width;x++)
		{
			gui_phy.draw_point(x,y,windosx->bkctbl[ccnt]);//����	
			ccnt++;
		}
	}  
}
//��������
//top,left,width,height:���꼰�ߴ�.
//id:window��ID
//type:window����
//[7]:0,û�йرհ�ť.1,�йرհ�ť			   
//[6]:0,����ȡ����ɫ.1,��ȡ����ɫ.					 
//[5]:0,���⿿��.1,�������.					 
//[4:2]:����					 
//[1:0]:0,��׼�Ĵ���(��XP);1,Բ�ߴ���(��Android)
//font:����������������С
_window_obj * window_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font)
{
 	_window_obj * window_crt;
	if(width<(WIN_BTN_SIZE+20)||height<WIN_CAPTION_HEIGHT)return NULL;//�ߴ粻��̫С		  
	window_crt=(_window_obj*)gui_memin_malloc(sizeof(_window_obj));//�����ڴ�
	if(window_crt==NULL)return NULL;//�ڴ���䲻��.
	window_crt->top=top;
	window_crt->left=left;
	window_crt->width=width;
	window_crt->height=height;
	window_crt->id=id;
	window_crt->type=type;
	window_crt->sta=0;
	window_crt->caption=0;
	window_crt->captionheight=WIN_CAPTION_HEIGHT;	//Ĭ�ϸ߶�
	window_crt->font=font;							//���������С
	window_crt->arcwinr=6;							//Ĭ��Բ�ǵİ뾶


	window_crt->captionbkcu=WIN_CAPTION_UPC;	//Ĭ��caption�ϲ��ֱ���ɫ
	window_crt->captionbkcd=WIN_CAPTION_DOWNC;	//Ĭ��caption�²��ֱ���ɫ
	window_crt->captioncolor=WIN_CAPTION_COLOR;	//Ĭ��caption����ɫ
	window_crt->windowbkc=WIN_BODY_BKC;			//Ĭ�ϱ���ɫ

	if(type&(1<<7))//��Ҫ�رհ�ť(�����÷�XP����)
	{
		window_crt->closebtn=btn_creat(left+width-WIN_BTN_SIZE-WIN_BTN_OFFSIDE,top+(window_crt->captionheight-WIN_BTN_SIZE)/2,WIN_BTN_SIZE,WIN_BTN_SIZE,0,2);//�����߽ǰ�ť
		if(window_crt->closebtn==NULL)
		{
			window_delete(window_crt);		//�ͷ�֮ǰ������ڴ�
			return NULL;					//�ڴ���䲻��.
		}
		window_crt->closebtn->caption="��";				//�ر�ͼ��
 		window_crt->closebtn->bcfucolor=WIN_BTN_FUPC;	//Ĭ���ɿ�����ɫ
		window_crt->closebtn->bcfdcolor=WIN_BTN_FDOWNC;	//Ĭ�ϰ��µ���ɫ
		window_crt->closebtn->bkctbl[0]=WIN_BTN_RIMC;	//Բ�ǰ�ť�߿���ɫ
		window_crt->closebtn->bkctbl[1]=WIN_BTN_TP1C;	//��һ�е���ɫ
		window_crt->closebtn->bkctbl[2]=WIN_BTN_UPHC;	//�ϰ벿����ɫ
		window_crt->closebtn->bkctbl[3]=WIN_BTN_DNHC;	//�°벿����ɫ

	}else window_crt->closebtn=NULL;


	if(type&(1<<6))//��Ҫ��ȡ����ɫ
	{
		window_crt->bkctbl=(u16*)gui_memex_malloc(width*height*2);//Ҫ���������ı���ɫ��
		if(window_crt->bkctbl==NULL)
		{
			window_delete(window_crt);		//�ͷ�֮ǰ������ڴ�
			return NULL;					//�ڴ���䲻��.
		}
		window_read_backcolor(window_crt);	//��ȡ����ɫ	   
	}
    return window_crt;
}
//ɾ������
//windowx:����ָ��
void window_delete(_window_obj * window_del)
{
	if(window_del->type&(1<<6))window_recover_backcolor(window_del);//�����˱���ɫ,��ʱ�ָ�
 	btn_delete(window_del->closebtn);//ɾ���رհ�ť
  	gui_memex_free(window_del->bkctbl);
	gui_memin_free(window_del);
};
//��������
//windowx:����ָ��
void window_draw(_window_obj * windowx)
{
	u8 winclosebtnsize=0;	//�رհ�ť�ĳߴ�
	if(windowx==NULL)return;//��Ч,ֱ���˳�
	if(windowx->type&(1<<0))//Բ�Ǵ�,��android��ʱ��,��֧�ְ�ť(�����ѿ�)
	{
		gui_draw_arcrectangle(windowx->left,windowx->top,windowx->width,windowx->captionheight,windowx->arcwinr,1,windowx->captionbkcu,windowx->captionbkcd);									//����������
 		gui_fill_rectangle(windowx->left,windowx->top+windowx->captionheight-windowx->arcwinr,windowx->arcwinr,windowx->arcwinr,windowx->captionbkcd);											//���²��ֲ���Ҫ�İ�Բ�εĲ�������1
 		gui_fill_rectangle(windowx->left+windowx->width-windowx->arcwinr,windowx->top+windowx->captionheight-windowx->arcwinr,windowx->arcwinr,windowx->arcwinr,windowx->captionbkcd);			//���²��ֲ���Ҫ�İ�Բ�εĲ�������2
	
		gui_draw_arcrectangle(windowx->left,windowx->top+windowx->captionheight,windowx->width,windowx->height-windowx->captionheight,windowx->arcwinr,1,windowx->windowbkc,windowx->windowbkc);//���հײ���
  		gui_fill_rectangle(windowx->left,windowx->top+windowx->captionheight,windowx->arcwinr,windowx->arcwinr,windowx->windowbkc);																//���ϲ��ֲ���Ҫ�İ�Բ�εĲ�������1
 		gui_fill_rectangle(windowx->left+windowx->width-windowx->arcwinr,windowx->top+windowx->captionheight,windowx->arcwinr,windowx->arcwinr,windowx->windowbkc);								//���ϲ��ֲ���Ҫ�İ�Բ�εĲ�������2
		gui_draw_arcrectangle(windowx->left,windowx->top,windowx->width,windowx->height,windowx->arcwinr,0,ARC_WIN_RIM_COLOR,ARC_WIN_RIM_COLOR);//���߿�										//����������
 	}else	//���Ǵ�
	{
		//����ڲ�
 		gui_fill_rectangle(windowx->left,windowx->top,windowx->width,windowx->captionheight/2,windowx->captionbkcu);//����ϰ벿��
		gui_fill_rectangle(windowx->left,windowx->top+windowx->captionheight/2,windowx->width,windowx->captionheight/2,windowx->captionbkcd);//����°벿��
		gui_fill_rectangle(windowx->left,windowx->top+windowx->captionheight-1,windowx->width,windowx->height-windowx->captionheight,windowx->windowbkc);//����ڲ�
		//���߿�
		gui_draw_rectangle(windowx->left,windowx->top,windowx->width,windowx->height,STD_WIN_RIM_OUTC);//�����
		gui_draw_rectangle(windowx->left+1,windowx->top+1,windowx->width-2,windowx->height-2,STD_WIN_RIM_MIDC);//���м��
		gui_draw_rectangle(windowx->left+2,windowx->top+windowx->captionheight-1,windowx->width-4,windowx->height-windowx->captionheight-1,STD_WIN_RIM_INC); //���ڿ�
		if(windowx->type&(1<<7))
		{
			btn_draw(windowx->closebtn);//�а�ť��ʱ����Ҫ����ť.	
			winclosebtnsize=WIN_BTN_SIZE;
		}
	}
	if(windowx->type&(1<<5))gui_show_strmid(windowx->left,windowx->top,windowx->width,windowx->captionheight,windowx->captioncolor,windowx->font,windowx->caption);//��ʾ����
	else gui_show_ptstr(windowx->left+WIN_BTN_OFFSIDE,windowx->top+(windowx->captionheight-windowx->font)/2,windowx->left+windowx->width-2*WIN_BTN_OFFSIDE-winclosebtnsize,windowx->top+windowx->captionheight,0,windowx->captioncolor,windowx->font,windowx->caption,1);//������ʾcaption
}

//�˶�����main.c��������,�������Ҫtpad����,ֱ��ȥ��.
extern volatile u8 system_task_return;		//����ǿ�Ʒ��ر�־.


#define MSG_BOX_BTN1_WIDTH 			60
#define MSG_BOX_BTN2_WIDTH 			100
#define MSG_BOX_BTN_HEIGHT 			30
//���ƶ�λ����ʾһ��msg box
//x,y,width,height:����ߴ�
//str:�ַ���
//caption:��Ϣ��������
//font:�����С
//color:��ɫ
//mode:
//[7]:0,û�йرհ�ť.1,�йرհ�ť			   
//[6]:0,����ȡ����ɫ.1,��ȡ����ɫ.					 
//[5]:0,���⿿��.1,�������.					 
//[4:2]:����
//[1]:0,����ʾȡ������;1,��ʾȡ������.
//[0]:0,����ʾOK����;1,��ʾOK����.
//����ֵ:
//0,û���κΰ�������/�����˴���.
//1,ȷ�ϰ�ť������.
//2,ȡ����ť������.	   
u8 window_msg_box(u16 x,u16 y,u16 width,u16 height,u8 *str,u8 *caption,u8 font,u16 color,u8 mode)
{
	u8 rval=0,res;				 
	u16 offx=0,offy=0;	
	u16 temp,strheight=0;

 	_window_obj* twin=0;	//����
 	_btn_obj * okbtn=0;		//ȷ����ť
 	_btn_obj * cancelbtn=0; //ȡ����ť
														  
 	if(width<150||height<(WIN_CAPTION_HEIGHT+font+5))return 0;//�ߴ����
  	twin=window_creat(x,y,width,height,0,1|((7<<5)&mode),16);//��������
	if((mode&0X03)==0x03)	//��������ť
	{
		offy=MSG_BOX_BTN1_WIDTH;
		offx=(width-MSG_BOX_BTN1_WIDTH*2)/3;
	}else 		   			//ֻ��һ����ť
	{
		offy=MSG_BOX_BTN2_WIDTH;
		offx=(width-MSG_BOX_BTN2_WIDTH)/2; 
	}
	if(mode&(1<<0))//��Ҫ��ʾOK����
	{
 		okbtn=btn_creat(x+offx,y+height-MSG_BOX_BTN_HEIGHT-10,offy,MSG_BOX_BTN_HEIGHT,0,0x02);//����OK��ť
		if(okbtn==NULL)rval=1;
		else
		{
	 		okbtn->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];//ȷ��
			okbtn->bkctbl[0]=0X8452;//�߿���ɫ
			okbtn->bkctbl[1]=0XAD97;//��һ�е���ɫ				
			okbtn->bkctbl[2]=0XAD97;//�ϰ벿����ɫ
			okbtn->bkctbl[3]=0X8452;//�°벿����ɫ
		}
	}
  	if(mode&(1<<1))//��Ҫ��ʾȡ������
	{
		if(mode&(1<<0))cancelbtn=btn_creat(x+offx*2+MSG_BOX_BTN1_WIDTH,y+height-MSG_BOX_BTN_HEIGHT-10,offy,MSG_BOX_BTN_HEIGHT,0,0x02);//����cancel��ť
		else cancelbtn=btn_creat(x+offx,y+height-MSG_BOX_BTN_HEIGHT-10,offy,MSG_BOX_BTN_HEIGHT,0,0x02);//����cancel��ť
		if(cancelbtn==NULL)rval=1;
		else
		{
	 		cancelbtn->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];//ȷ��
			cancelbtn->bkctbl[0]=0X8452;//�߿���ɫ
			cancelbtn->bkctbl[1]=0XAD97;//��һ�е���ɫ				
			cancelbtn->bkctbl[2]=0XAD97;//�ϰ벿����ɫ
			cancelbtn->bkctbl[3]=0X8452;//�°벿����ɫ
		}
	}	
 	if(twin==NULL)rval=1;
	else
	{
		twin->caption=caption;	
 		window_draw(twin);			//��������
		btn_draw(okbtn);			//����ť
		btn_draw(cancelbtn);		//����ť
 		if((mode&0X03)==0)
		{
			rval=1;//����Ҫ����while	
			strheight=height-(WIN_CAPTION_HEIGHT+5);
		}else strheight=height-(WIN_CAPTION_HEIGHT+50+5);
 	
   		temp=strlen((const char*)str)*(font/2);	//�õ��ַ�������
		if(temp>=(width-10))offx=5;				//�õ�x��ƫ��
		else offx=(width-temp)/2;

		temp=gui_get_stringline(str,width-offx*2,font)*font;//�õ��ַ���Ҫռ�õ�����(����)
 		if(temp>=strheight)offy=5;				//�õ�y��ƫ��
		else offy=(strheight-temp)/2;
		gui_show_string(str,x+offx,y+WIN_CAPTION_HEIGHT+offy,width-offx*2,strheight,font,color);	//��ʾҪ��ʾ������
	}
	system_task_return=0;//ȡ��TPAD
	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
 		//������ϵͳ����,�û���������ȥ���˾�
 		if(system_task_return)//TPAD����
		{	
			rval=0XFE;//��Ϊȡ�������,�˳�
 			break;			
		}
		delay_ms(10);							//��ʱ10ms
		if(okbtn)
		{
			res=btn_check(okbtn,&in_obj);		//ȷ�ϰ�ť���
			if(res)
			{
				if((okbtn->sta&0X80)==0)		//����Ч����
				{
					rval=0XFF;
					break;//�˳�
				}
			}
		}	
 		if(cancelbtn)
		{
			res=btn_check(cancelbtn,&in_obj);	//���ذ�ť���
			if(res)
			{
				if((cancelbtn->sta&0X80)==0)	//����Ч����
				{
					rval=0XFE;
					break;//�˳�
				}
			}
		}
	}
 	btn_delete(cancelbtn);	//ɾ����ť	
  	btn_delete(okbtn);		//ɾ����ť
	window_delete(twin);	//ɾ������
	if(rval==0XFF)return 1;	//ȷ��
	if(rval==0XFE)return 2;	//ȡ��
	return 0;	    		//��������޲���
}					    



//////////////////////////////////////////////////////////////////////////////////////////
//����	  
void win_test(u16 x,u16 y,u16 width,u16 height,u8 type,u16 cup,u16 cdown,u8 *caption)
{
	_window_obj* twin;
	twin=window_creat(x,y,width,height,0,type,16);//��������
	if(twin==NULL)return;//����ʧ��.
	twin->caption=caption;
 	{
		twin->windowbkc=cup; 
 	}
	window_draw(twin);
	window_delete(twin);

}












