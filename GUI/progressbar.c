#include "progressbar.h"
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

//����������
//top,left,width,height:����.���ߴ�. 
//type:�������� 
//[bit7]:����,0,ˮƽ;1,��ֱ;
//[bit6]:0,����ʾ����;1,��ʾ����(�ٷֽ�������ʱ��,��ʾ�ٷ���);
//[bit5]:0,����ʾ������,1,��ʾ������(��ʵ������ɫ�Ƿ����,�Ƿ���Ÿ��������ɫ.)
//[bit4:2]:����;
//[bit1:0]:0,��׼������;1,�ٷ���������;2,3,����.
_progressbar_obj * progressbar_creat(u16 left,u16 top,u16 width,u16 height,u8 type)
{
 	_progressbar_obj * progressbar_crt;
	if(width<2||height<2)return NULL;//�ߴ粻��̫С
	progressbar_crt=(_progressbar_obj*)gui_memin_malloc(sizeof(_progressbar_obj));//�����ڴ�
	if(progressbar_crt==NULL)return NULL;//�ڴ���䲻��.
	progressbar_crt->top=top;
	progressbar_crt->left=left;
	progressbar_crt->width=width;
	progressbar_crt->height=height;
	progressbar_crt->type=type; 
	progressbar_crt->sta=0; 
	progressbar_crt->id=0; 
	progressbar_crt->totallen=0; 
	progressbar_crt->curpos=0; 
	//��ʹ�ðٷ��������������ν�����ʱ,ȷ��prgbarlen����Ϊ0. Ҳ���ǲ�����ʾ���� 
 	if(type&PRGB_TYPE_TEXT||(type&PRGB_TYPE_FEN)==0)progressbar_crt->prgbarlen=0;//Ĭ�Ϲ�������Ϊ10
	else progressbar_crt->prgbarlen=10;						//Ĭ�Ϲ�������Ϊ10
  	progressbar_crt->inbkcolora=PRGB_DFT_BKACOLOR;			//Ĭ��ɫ
 	progressbar_crt->inbkcolorb=PRGB_DFT_BKBCOLOR;			//Ĭ��ɫ 
 	progressbar_crt->infcolora=PRGB_DFT_FILLACOLOR;			//Ĭ��ɫ
 	progressbar_crt->infcolorb=PRGB_DFT_FILLBCOLOR;			//Ĭ��ɫ 
  	progressbar_crt->btncolor=PRGB_DFT_BTNCOLOR;			//Ĭ��ɫ  
  	progressbar_crt->rimcolor=PRGB_DFT_RIMCOLOR;			//Ĭ��ɫ  
  	return progressbar_crt;
}
//ɾ��������
//btn_del:Ҫɾ���İ���
void progressbar_delete(_progressbar_obj *progressbar_del)
{
 	gui_memin_free(progressbar_del);
}
//���	
//prgbx:������ָ��
//in_key:���밴��ָ��
//����ֵ:0,û����Ч�Ĳ���
//       1,����Ч�Ĳ���			   
u8 progressbar_check(_progressbar_obj * prgbx,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
	u32 temp,temp1;	 
	u8  prgblen;
	u8 prgbok=0;
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//������������
			if(prgbx->type&PRGB_TYPE_VER)//��ֱ������
			{
				temp=prgbx->top+prgbx->height;	//�ײ�����

				if((temp-key->y)<prgbx->prgbarlen/2)temp1=temp;
				else temp1=key->y+prgbx->prgbarlen/2;//��ǰx����
 				prgblen=prgbx->height-prgbx->prgbarlen;
			}else						   	//ˮƽ������
			{
				if((key->x-prgbx->left)<prgbx->prgbarlen/2)temp=0;
				else temp=key->x-prgbx->prgbarlen/2;//��ǰx����
				temp1=prgbx->left;	//��������ʼλ��
				prgblen=prgbx->width-prgbx->prgbarlen;
			}
			//���м�������
			if(prgbx->top<=key->y&&key->y<=(prgbx->top+prgbx->height)&&prgbx->left<=key->x&&key->x<=(prgbx->left+prgbx->width))
			{
				if((prgbx->sta&PRGB_BTN_DOWN)==0)prgbx->sta|=PRGB_BTN_DOWN;//��Ǳ�����
  				if(temp>=temp1)temp-=temp1;
				else temp=0;   
				temp=((float)temp/prgblen)*prgbx->totallen;//�õ���ǰӦ��Ҫ����λ��(���ܳ��������λ��)
				if(temp>prgbx->totallen)temp=prgbx->totallen;
				if(prgbx->curpos!=temp)//�õ���ǰ����
				{
					prgbx->curpos=temp;
					progressbar_setpos(prgbx);
				}
				prgbok=1;
			}else if(prgbx->sta&PRGB_BTN_DOWN)
			{
				prgbx->sta&=~PRGB_BTN_DOWN;//����ɿ�
				prgbok=1;
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
	return prgbok;
}

//5*8��С��0~9��%�����
const u8 pasciitbl[11][5]=
{
{0x00,0x7C,0x82,0x82,0x7C},/*"0",0*/
{0x00,0x84,0xFE,0x80,0x00},/*"1",1*/
{0x00,0xC4,0xA2,0x92,0x8C},/*"2",2*/
{0x00,0x44,0x92,0x92,0x6C},/*"3",3*/
{0x00,0x70,0x4C,0xFE,0x40},/*"4",4*/
{0x00,0x9E,0x92,0x92,0x62},/*"5",5*/
{0x00,0x7C,0x92,0x92,0x64},/*"6",6*/
{0x00,0x06,0xE2,0x12,0x0E},/*"7",7*/
{0x00,0x6C,0x92,0x92,0x6C},/*"8",8*/
{0x00,0x4C,0x92,0x92,0x7C},/*"9",9*/
{0x00,0xCC,0x2C,0xD0,0xCC},/*"%",10*/
};
//��ʾһ���ַ�
//x,y:����
//dir:����.0,ˮƽ;1,��ֱ.
//num:0~10;����0~9:'0'~'9';10:%
//color:��ɫ   
void progressbar_show_char(u16 x,u16 y,u8 dir,u8 num,u16 color)
{
	u16 x0,y0;
	u8 t,i,temp;
	if(dir==0)
	{	 
		x0=x;
		for(t=0;t<5;t++)
		{
			y0=y;
			temp=pasciitbl[num][t];
			for(i=0;i<8;i++)
			{
				if(temp&0x01)gui_phy.draw_point(x0,y0,color);//ˮƽ
				y0++;	
				temp>>=1;
			}
			x0++;
		}
	}else//��ֱ
	{
		y0=y;
		for(t=0;t<5;t++)
		{
			x0=x;
			temp=pasciitbl[num][t];
			for(i=0;i<8;i++)
			{
				if(temp&0x01)gui_phy.draw_point(x0,y0,color);//ˮƽ
				x0++;	
				temp>>=1;
			}
			y0--;
		}
	}   
}	   
//��ʾһ���ٷ���
//x,y:����
//dir:����.0,ˮƽ;1,��ֱ.
//num:0~100;���� 0%~100%
//color:��ɫ   
void progressbar_show_percnt(u16 x,u16 y,u8 dir,u8 num,u16 color)
{
	u8 t,temp;
	u8 enshow=0;
	u8 len=1;
	if(num>99)len=3;
	else if(num>9)len=2;
	else len=1;
	for(t=0;t<len;t++)
	{
		temp=(num/gui_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)continue;
		    else enshow=1;  
		}
	 	if(dir==0)progressbar_show_char(x+5*t,y,dir,temp,color); 
		else progressbar_show_char(x,y-5*t,dir,temp,color); 
	}
 	if(dir==0)progressbar_show_char(x+5*t,y,dir,10,color);//��ʾ�ٷֺ� 
	else progressbar_show_char(x,y-5*t,dir,10,color); 
}
//�����ȸ���
//prgbx:������ָ��
//x,y:����
void progressbar_draw_sign(_progressbar_obj *prgbx,u16 x,u16 y)
{
	u16 width,height;
	u8 percnt;
	if((prgbx->type&0x03)==PRGB_TYPE_STD)//��׼������
	{
		if(prgbx->type&PRGB_TYPE_VER)//��ֱ��
		{
			width=prgbx->width-2;
			height=prgbx->prgbarlen;
			y=y-prgbx->prgbarlen;
			gui_fill_rectangle(x,y,width,height,prgbx->btncolor); //����ڲ���ɫ
			gui_draw_hline(x,y,width,prgbx->rimcolor);
			gui_draw_hline(x,y+prgbx->prgbarlen-1,width,prgbx->rimcolor);
		}else  //ˮƽ��
		{
			width=prgbx->prgbarlen;
			height=prgbx->height-2;
			gui_fill_rectangle(x,y,width,height,prgbx->btncolor); //����ڲ���ɫ
			gui_draw_vline(x,y,height,prgbx->rimcolor);
			gui_draw_vline(x+width-1,y,height,prgbx->rimcolor);
		}
	}else if((prgbx->type&0x03)==PRGB_TYPE_TEXT)//�ٷ���������
	{
		percnt=(100*prgbx->curpos)/prgbx->totallen;//�õ��ٷ���
		if(prgbx->type&PRGB_TYPE_VER)//��ֱ��
		{
			if(prgbx->width<8&&prgbx->height<20)return;//�޷���ʾ�����İٷ���,ֱ�Ӳ���ʾ.
			progressbar_show_percnt(prgbx->left+(prgbx->width-8)/2,prgbx->top+prgbx->height/2+10,1,percnt,prgbx->btncolor);
		}else  //ˮƽ��
		{
			if(prgbx->height<8&&prgbx->width<20)return;//�޷���ʾ�����İٷ���,ֱ�Ӳ���ʾ.
			progressbar_show_percnt(prgbx->left+prgbx->width/2-10,prgbx->top+(prgbx->height-8)/2,0,percnt,prgbx->btncolor);
 		}
	}
}

//���ý�������λ��
//prgbx:������ָ��
void progressbar_setpos(_progressbar_obj *prgbx)
{
 	u16 prgblen;//�������ĳ���
	u16 x,y,xlen,ylen;
	u8 prgbtlen=0;//���곤��

	if((prgbx->type&0x03)==PRGB_TYPE_TEXT)prgbtlen=0;//�ٷ���������
	else if((prgbx->type&0x03)==PRGB_TYPE_STD)prgbtlen=prgbx->prgbarlen;//
	
	x=prgbx->left+1;
	y=prgbx->top+1;
	xlen=prgbx->width-2;
	ylen=prgbx->height-2;

	if(prgbx->type&PRGB_TYPE_VER)//��ֱ������
	{
		prgblen=prgbx->height-2-prgbtlen;
		gui_fill_rectangle(x,y,xlen/2,ylen,prgbx->inbkcolora);//��ս�����
	 	gui_fill_rectangle(x+xlen/2,y,xlen-xlen/2,ylen,prgbx->inbkcolorb);//��ս�����
	}else  //ˮƽ������
	{		 
		prgblen=prgbx->width-2-prgbtlen;
		gui_fill_rectangle(x,y,xlen,ylen/2,prgbx->inbkcolora);//��ս�����
	 	gui_fill_rectangle(x,y+ylen/2,xlen,ylen-ylen/2,prgbx->inbkcolorb);//��ս�����
	}
	if(prgbx->curpos>prgbx->totallen)prgbx->curpos=prgbx->totallen;//��Χ�޶�	
	prgblen=((float)prgbx->curpos/prgbx->totallen)*prgblen;//���㵱ǰ�������ĳ���
	if(prgbx->type&PRGB_TYPE_VER)//��ֱ������ 
	{		 
		x=prgbx->left+1;
		y=prgbx->top+prgbx->height-1-prgblen;	 			 
		xlen=prgbx->width-2;
 		ylen=prgblen;   
		if(prgbx->type&PRGB_TYPE_PEN)//������ʾ������
		{
			gui_fill_rectangle(x,y,xlen/2,ylen,prgbx->infcolora);//
  			gui_fill_rectangle(x+xlen/2,y,xlen-xlen/2,ylen,prgbx->infcolorb);//
    	}
	}else//ˮƽ������ 
	{
		x=prgbx->left+1;
		y=prgbx->top+1;
		xlen=prgblen;
		ylen=prgbx->height-2;	 	 		  
		if(prgbx->type&PRGB_TYPE_PEN)//������ʾ������
		{
 			gui_fill_rectangle(x,y,xlen,ylen/2,prgbx->infcolora);//
  			gui_fill_rectangle(x,y+ylen/2,xlen,ylen-ylen/2,prgbx->infcolorb);//
		}
		x+=xlen;
	}
	if(prgbx->type&PRGB_TYPE_FEN)progressbar_draw_sign(prgbx,x,y);//��������/����
	progressbar_hook(prgbx);//ִ�й��Ӻ���
}
//��������
//prgbx:������ָ��
void progressbar_draw_progressbar(_progressbar_obj *prgbx)
{
	if(prgbx==NULL)return;//��Ч,ֱ���˳�
	gui_draw_rectangle(prgbx->left,prgbx->top,prgbx->width,prgbx->height,prgbx->rimcolor);	
 	progressbar_setpos(prgbx); 
}
//�������Ĺ��Ӻ���
//prgbx:������ָ��
void progressbar_hook(_progressbar_obj *prgbx)
{
	//printf("progressbar%d pos:%d\r\n",prgbx->id,prgbx->curpos);
}
//���Ժ���
void test_prgb(u16 x,u16 y,u16 width,u16 height,u8 type,u16 icur)
{
	_progressbar_obj* prgb;
	prgb=progressbar_creat(x,y,width,height,type);//����������
	if(prgb==NULL)return;//����ʧ��.
							  		
	prgb->totallen=100;
	prgb->curpos=icur;
		 
	progressbar_draw_progressbar(prgb);//����ť	 
	progressbar_delete(prgb);//ɾ����ť	 
}




































