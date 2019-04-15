#include "scrollbar.h"
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
//type:[bit7]:����,0,ˮƽ;1,��ֱ;bit[6:2]:����;[bit1:0]:��ť����,0,��׼������;1,û�ж˰�ť�Ĺ�����;2,3,����.
_scrollbar_obj * scrollbar_creat(u16 left,u16 top,u16 width,u16 height,u8 type)
{
 	_scrollbar_obj * scrollbar_crt;
	if((type&0x80)==SCROLLBAR_DIR_HOR&&(width<2*SCROLLBAR_PART_LEN||height<SCROLLBAR_MIN_THICK))return NULL;//�ߴ粻��̫С	
	if((type&0x80)==SCROLLBAR_DIR_VER&&(width<SCROLLBAR_MIN_THICK||height<2*SCROLLBAR_PART_LEN))return NULL;//�ߴ粻��̫С	  
	scrollbar_crt=(_scrollbar_obj*)gui_memin_malloc(sizeof(_scrollbar_obj));//�����ڴ�
	if(scrollbar_crt==NULL)return NULL;//�ڴ���䲻��.
	gui_memset((u8*)scrollbar_crt,0,sizeof(_scrollbar_obj));//��scrollbar_crt��ֵȫ������Ϊ0    
	scrollbar_crt->top=top;
	scrollbar_crt->left=left;
	scrollbar_crt->width=width;
	scrollbar_crt->height=height;
	scrollbar_crt->type=type; 
	scrollbar_crt->sta=0; 
	scrollbar_crt->id=0; 
	scrollbar_crt->totalitems=0; 
	scrollbar_crt->itemsperpage=0; 
	scrollbar_crt->topitem=0; 
	scrollbar_crt->scbbarlen=0; 

	scrollbar_crt->inbkcolor=SCLB_DFT_INBKCOLOR; 
	scrollbar_crt->btncolor=SCLB_DFT_BTNCOLOR; 
	scrollbar_crt->rimcolor=SCLB_DFT_RIMCOLOR; 	   	  
    return scrollbar_crt;
}
//ɾ��������
//btn_del:Ҫɾ���İ���
void scrollbar_delete(_scrollbar_obj *scrollbar_del)
{
 	gui_memin_free(scrollbar_del);
}
//���	
//scbx:������ָ��
//in_key:���밴��ָ��
//����ֵ:δ�õ�				   
u8 scrollbar_check(_scrollbar_obj * scbx,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
 	u8 signx;
	u16 x,y,btnw,btnh,b1xoff,b1yoff,b2xoff,b2yoff;
 	u16 alphacolor;
	u32 temp,temp1;	 
	u16 maxtopitem;//���Ķ�����Ŀ
	u8  scbplen;
	if((scbx->type&0X03)==SCROLLBAR_TYPE_NBT)scbplen=0;//�޶˰�ť�����	
	else scbplen=SCROLLBAR_PART_LEN;  		   	//�����а�ť
	if(scbx->totalitems>=scbx->itemsperpage)maxtopitem=scbx->totalitems-scbx->itemsperpage;
	else maxtopitem=0;
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//������������
			if((scbx->type&0x80)==SCROLLBAR_DIR_HOR)//ˮƽ������
			{
				temp=key->x;								//��ǰx����
				temp1=scbx->left+scbplen+scbx->scbbarlen/2;	//��������ʼλ��

				x=scbx->left;								//������x����
				y=scbx->top;								//������y����
 				btnw=scbplen;								//��ť���
				btnh=scbx->height;							//��ť�߶�
				b1xoff=scbplen;								//��һ����ť��x����ƫ��
				b1yoff=0;									//��һ����ť��y����ƫ��
				b2xoff=scbx->width-scbplen;					//�ڶ�����ť��x����ƫ��
				b2yoff=0;	 						  		//�ڶ�����ť��y����ƫ��
			}else
			{
				temp=key->y;//��ǰy����
				temp1=scbx->top+scbplen+scbx->scbbarlen/2;//��������ʼλ��

				x=scbx->left;						 		//������x����
				y=scbx->top;						  		//������y����
				btnw=scbx->width;					  		//��ť���
				btnh=scbplen;	 		   					//��ť�߶�
				b1xoff=0;							   		//��һ����ť��x����ƫ��
				b1yoff=scbplen;								//��һ����ť��y����ƫ��
				b2xoff=0;							   		//�ڶ�����ť��x����ƫ��
				b2yoff=scbx->height-scbplen;				//�ڶ�����ť��y����ƫ��	 
			}
			if((scbx->type&0x80)==SCROLLBAR_DIR_HOR)signx=SCROLLBAR_SIGN_LEFT;//ˮƽ 
			else signx=SCROLLBAR_SIGN_UP;//ˮƽ 
			//������/�϶˰�ť�ڲ�
			if(y<key->y&&key->y<(y+btnh)&&x<key->x&&key->x<(x+btnw))
			{
 				if(scbx->topitem)//�޸Ľ�������λ��.
				{
					if((scbx->sta&SCROLLBAR_BTN_LTP)==0)//ԭ������SCROLLBAR_BTN_LTP������
					{
						scbx->sta|=SCROLLBAR_BTN_LTP;//����
						alphacolor=gui_alpha_blend565(scbx->btncolor,GUI_COLOR_WHITE,16);
						//����ť
						scrollbar_draw_btn(x,y,btnw,btnh,alphacolor,scbx->rimcolor,SCROLLBAR_SIGN_PCOLOR,signx);  
	 				}
					scbx->topitem--;
					scrollbar_setpos(scbx);
 				}
 			}else if(scbx->sta&SCROLLBAR_BTN_LTP)//ԭ��״̬�ǰ��µ�
			{	
				scbx->sta&=~SCROLLBAR_BTN_LTP;						    
				scrollbar_draw_btn(x,y,btnw,btnh,scbx->btncolor,scbx->rimcolor,SCROLLBAR_SIGN_RCOLOR,signx);   
			}
			if((scbx->type&0x80)==SCROLLBAR_DIR_HOR)signx=SCROLLBAR_SIGN_RIGHT;//ˮƽ 
			else signx=SCROLLBAR_SIGN_DOWN;//ˮƽ   
			//������/�¶˰�ť�ڲ�
			if((y+b2yoff)<key->y&&key->y<(y+scbx->height)&&(x+b2xoff)<key->x&&key->x<(x+scbx->width))
			{			    
				if(maxtopitem&&scbx->topitem<maxtopitem)//�޸Ľ�������λ��.
				{
					if((scbx->sta&SCROLLBAR_BTN_RBP)==0)//ԭ������SCROLLBAR_BTN_RBP������
					{
						scbx->sta|=SCROLLBAR_BTN_RBP;//����
						alphacolor=gui_alpha_blend565(scbx->btncolor,GUI_COLOR_WHITE,16);
						//����ť
						scrollbar_draw_btn(x+b2xoff,y+b2yoff,btnw,btnh,alphacolor,scbx->rimcolor,SCROLLBAR_SIGN_PCOLOR,signx);   
					}
					scbx->topitem++;
					scrollbar_setpos(scbx);
				} 		   
			}else if(scbx->sta&SCROLLBAR_BTN_RBP)//ԭ��״̬�ǰ��µ�
			{	
				scbx->sta&=~SCROLLBAR_BTN_RBP;						    
				scrollbar_draw_btn(x+b2xoff,y+b2yoff,btnw,btnh,scbx->btncolor,scbx->rimcolor,SCROLLBAR_SIGN_RCOLOR,signx);   
			}
			//���м�
			if((y+b1yoff)<key->y&&key->y<(y+scbx->height-b1yoff)&&(x+b1xoff)<key->x&&key->x<(x+scbx->width-b1xoff))
			{
				if((scbx->sta&SCROLLBAR_BTN_MID)==0)scbx->sta=SCROLLBAR_BTN_MID;//ԭ������SCROLLBAR_BTN_MID������
  				if(temp>=temp1)temp-=temp1;
				else temp=0;
				temp=(temp*maxtopitem)/(b2xoff+b2yoff-scbplen-scbx->scbbarlen);
				if(temp>maxtopitem)temp=maxtopitem;
				if(scbx->topitem!=temp)//�õ���ǰ����
				{
					scbx->topitem=temp;
					scrollbar_setpos(scbx);
				}
			}else if(scbx->sta&SCROLLBAR_BTN_MID)scbx->sta&=~SCROLLBAR_BTN_MID;//ԭ��״̬�ǰ��µ�
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


//6�� 8*8��������־ͼ��
const u8 scrollsign[6][8]={
{0x00,0x00,0x10,0x38,0x7C,0xFE,0x00,0x00},		//��ͼ��  
{0x00,0x00,0xFE,0x7C,0x38,0x10,0x00,0x00},		//��ͼ��
{0x04,0x0C,0x1C,0x3C,0x1C,0x0C,0x04,0x00}, 		//��ͼ��
{0x20,0x30,0x38,0x3C,0x38,0x30,0x20,0x00},		//��ͼ��
{0x00,0xFE,0x00,0xFE,0x00,0xFE,0x00,0x00},		//��ֱͼ��
{0x00,0x54,0x54,0x54,0x54,0x54,0x54,0x54},		//ˮƽͼ��
};   													 
//������
//x,y:����
//signcolor:������ɫ
//signx:���ű��
//signx:0,up;1,down;2,left;3,right;4,horsign;5,versign;
void scrollbar_draw_sign(u16 x,u16 y,u16 signcolor,u8 signx)
{
	u8 i,j;
	u8 *signbuf;
	u8 temp;	  
	signbuf=(u8*)scrollsign[signx];//�õ�ͼ����������
	for(i=0;i<8;i++)
	{
		temp=signbuf[i];		  
		for(j=0;j<8;j++)
		{
			if(temp&0x80)gui_phy.draw_point(x+j,y+i,signcolor);	
			temp<<=1;		  
		}	
	}
}  
//���������İ���
//x,y,width,height:���꼰�ߴ�
//bkcolor:����ɫ
//rimcolor:�߿���ɫ
//signcolor:���ŵ���ɫ
//signx:���ű�� 0,��;1,��;2,��;3,��;4,��ֱ;5,ˮƽ;
void scrollbar_draw_btn(u16 x,u16 y,u16 width,u16 height,u16 bkcolor,u16 rimcolor,u16 signcolor,u8 signx)
{
	gui_fill_rectangle(x,y,width,height,bkcolor); //����ڲ���ɫ
	gui_draw_rectangle(x,y,width,height,rimcolor);//����߿�
	if(width>=8&&height>=8)//�㹻��ͼ��
	{
		scrollbar_draw_sign(x+width/2-4,y+height/2-4,signcolor,signx);
	}
}
//���ù�������λ��
//scbx:������ָ��
void scrollbar_setpos(_scrollbar_obj *scbx)
{
	u16 scbpos;
	u16 scblen;
	u16 x,y,xlen,ylen;
	u8 scbtype;
	u8 scbplen;
	if((scbx->type&0X03)==SCROLLBAR_TYPE_NBT)scbplen=0;//����û�а�ť
	else scbplen=SCROLLBAR_PART_LEN;			//�����а�ť
	if((scbx->type&0x80)==SCROLLBAR_DIR_VER)//��ֱ������ 
	{
		scblen=scbx->height-2*scbplen-scbx->scbbarlen;//�õ������������ռ�ĳ���
		x=scbx->left+1;
		y=scbx->top+scbplen+1;
		xlen=scbx->width-2;
		ylen=scblen+scbx->scbbarlen-2;
	}else  //ˮƽ������ 
	{
		scblen=scbx->width-2*scbplen-scbx->scbbarlen;
		x=scbx->left+scbplen+1;
		y=scbx->top+1;
		xlen=scblen+scbx->scbbarlen-2;
		ylen=scbx->height-2;
	}
	gui_fill_rectangle(x,y,xlen,ylen,scbx->inbkcolor);  	//����ϴε�λ��
    
	if(scbx->totalitems<=scbx->itemsperpage)//����Ŀ����һҳ��ʾ��
	{
		scbpos=scblen;
	}else if(scbx->topitem>=(scbx->totalitems-scbx->itemsperpage))//��ǰѡ������ڵ������һҳ�ĵ�һ��
	{
		scbx->topitem=scbx->totalitems-scbx->itemsperpage;//��Χ�޶������һҳ��һ��
		scbpos=scblen;	
	}else	 //�������һҳ
	{
		scbpos=(scbx->topitem*scblen)/(scbx->totalitems-scbx->itemsperpage);//���㵱ǰ��������λ��
	}
	if((scbx->type&0x80)==SCROLLBAR_DIR_VER)//��ֱ������ 
	{		 
		x=scbx->left;
		y=scbx->top+scbplen+scbpos;
		xlen=scbx->width;
		ylen=scbx->scbbarlen;	 
		scbtype=SCROLLBAR_SIGN_HOR;
    }else//ˮƽ������ 
	{
		x=scbx->left+scbplen+scbpos;
		y=scbx->top;
		xlen=scbx->scbbarlen;
		ylen=scbx->height;
		scbtype=SCROLLBAR_SIGN_VER;
	}
	scrollbar_draw_btn(x,y,xlen,ylen,scbx->btncolor,scbx->rimcolor,SCROLLBAR_SIGN_RCOLOR,scbtype);//�ػ�������
	scrollbar_hook(scbx);//ִ�й��Ӻ���
}
//��������
//scbx:������ָ��
void scrollbar_draw_scrollbar(_scrollbar_obj *scbx)
{
	u32 scbactlen=0;//���������Թ����Ŀռ䳤��
	if(scbx==NULL)return;//��Ч,ֱ���˳�
	gui_fill_rectangle(scbx->left,scbx->top,scbx->width,scbx->height,scbx->inbkcolor);	
	gui_draw_rectangle(scbx->left,scbx->top,scbx->width,scbx->height,scbx->rimcolor);	
 	if((scbx->type&0X03)==0)//��׼������
	{
		if((scbx->type&0x80)==0)//ˮƽ������
		{																		 
			scrollbar_draw_btn(scbx->left,scbx->top,SCROLLBAR_PART_LEN,scbx->height,scbx->btncolor,scbx->rimcolor,SCROLLBAR_SIGN_RCOLOR,SCROLLBAR_SIGN_LEFT);
			scrollbar_draw_btn(scbx->left+scbx->width-SCROLLBAR_PART_LEN,scbx->top,SCROLLBAR_PART_LEN,scbx->height,scbx->btncolor,scbx->rimcolor,SCROLLBAR_SIGN_RCOLOR,SCROLLBAR_SIGN_RIGHT);
			scbactlen=scbx->width-2*SCROLLBAR_PART_LEN;//�ɹ� �����������Ŀռ䷶Χ
 		}else //��ֱ������
		{
			scrollbar_draw_btn(scbx->left,scbx->top,scbx->width,SCROLLBAR_PART_LEN,scbx->btncolor,scbx->rimcolor,SCROLLBAR_SIGN_RCOLOR,SCROLLBAR_SIGN_UP);
			scrollbar_draw_btn(scbx->left,scbx->top+scbx->height-SCROLLBAR_PART_LEN,scbx->width,SCROLLBAR_PART_LEN,scbx->btncolor,scbx->rimcolor,SCROLLBAR_SIGN_RCOLOR,SCROLLBAR_SIGN_DOWN);
			scbactlen=scbx->height-2*SCROLLBAR_PART_LEN;//�ɹ� �����������Ŀռ䷶Χ
 		}
	}else//û�����˰�ť�Ĺ�����
	{
		if((scbx->type&0x80)==0)scbactlen=scbx->width;//ˮƽ������
		else scbactlen=scbx->height;//��ֱ������
	}	
	if(scbx->itemsperpage>scbx->totalitems)scbx->scbbarlen=scbactlen;
	else scbx->scbbarlen=(scbactlen*scbx->itemsperpage)/scbx->totalitems;//�õ��������ĳ���
	scrollbar_setpos(scbx);
}		

//�������Ĺ��Ӻ���
//scbx:������ָ��
void scrollbar_hook(_scrollbar_obj *scbx)
{
	//printf("scrollbar%d pos:%d\r\n",scbx->id,scbx->topitem);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//u16 itot,u16 ipg,
void test_scb(u16 x,u16 y,u16 width,u16 height,u8 type,u16 icur,u16 inbkcolor,u16 btncolor,u16 rimcolor)
{
	_scrollbar_obj* tscb;
	tscb=scrollbar_creat(y,x,width,height,type);//����������
	if(tscb==NULL)return;//����ʧ��.

	tscb->totalitems=50;
 	tscb->itemsperpage=8;
	tscb->topitem=icur;

	tscb->inbkcolor=inbkcolor;   
	tscb->btncolor=btncolor; 
	tscb->rimcolor=rimcolor; 
	 
	scrollbar_draw_scrollbar(tscb);//����ť	 
	scrollbar_delete(tscb);//ɾ����ť	 
}



















