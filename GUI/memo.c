#include "memo.h"
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

//����memo
_memo_obj * memo_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font,u32 textlen)
{
 	_memo_obj * memo_crt;
 	u8 sta=0;
	if(height<=SCROLLBAR_MIN_THICK*2)return NULL;	//�߶�С��,������scrollbar	    
	if(width<SCROLLBAR_PART_LEN+font/2)return NULL;	//����Ӧ�ù�scrollbar�Ŀ�ȼ��ϰ��font/2�Ŀ��.
	memo_crt=(_memo_obj*)gui_memin_malloc(sizeof(_memo_obj));	//�����ڴ�
	if(memo_crt==NULL)return NULL;					//�ڴ���䲻��.
	gui_memset(memo_crt,0,sizeof(_memo_obj));	 	//����
	memo_crt->scbv=scrollbar_creat(left+width-MEMO_SCB_WIDTH,top,MEMO_SCB_WIDTH,height,0x80);//����scrollbar.
	if(memo_crt->scbv==NULL)sta=1;	    
	else//����ɹ� 
	{
		memo_crt->scbv->itemsperpage=height/font;			//ÿҳ��ʾ����Ŀ��
		if((height%font)>2)memo_crt->scbv->itemsperpage++;	//ÿҳ��ʾ����Ŀ����1��.
	}
 	memo_crt->top=top;
	memo_crt->left=left;
	memo_crt->width=width;
	memo_crt->height=height;
	memo_crt->id=id;			
	memo_crt->type=type;				//���� 
 	memo_crt->sta=0;					//״̬Ϊ0
 	memo_crt->text=gui_memex_malloc(textlen);	//�����ַ��洢�ռ�
	if(memo_crt->text==NULL)sta=1;	    
	else gui_memset(memo_crt->text,0,textlen);		//����ɹ�,����ڴ�����

 	memo_crt->offsettbl=gui_memex_malloc(4);	//����4���ֽ�
	if(memo_crt->offsettbl==NULL)sta=1;	    
	else gui_memset(memo_crt->offsettbl,0,4);		//����ɹ�,����ڴ�����
	if(sta)//�з��䲻�ɹ������
	{
		memo_delete(memo_crt);//ɾ��
		return NULL;
	}
	memo_crt->textlen=textlen;				   
 	memo_crt->font=font;	        	//text��������

 	memo_crt->lin=0;					//������ 
	memo_crt->col=0;					//������
 	memo_crt->typos=0;					//��ʱy���� 
	memo_crt->txpos=0;					//��ʱx����
	memo_crt->memo_timer_old=0;			//��һ��ʱ��
													
  	memo_crt->textcolor=MEMO_DFT_TXTC;	//������ɫ
 	memo_crt->textbkcolor=MEMO_DFT_TBKC;//���ֱ���ɫ	  
 	return memo_crt;
}
//ɾ��
void memo_delete(_memo_obj * memo_del)
{
	if(memo_del==NULL)return;//�Ƿ��ĵ�ַ,ֱ���˳�
	gui_memex_free(memo_del->text);	 //�ڴ����ⲿ�����
	gui_memex_free(memo_del->offsettbl);//�ڴ����ⲿ�����
	scrollbar_delete(memo_del->scbv);//ɾ��������
	gui_memin_free(memo_del);
} 
void memo_cursor_set(_memo_obj * memox,u16 x);
//���	
//memox:�ı��༭��
//in_key:���밴��ָ��
//����ֵ:δ�õ�			 					   
u8 memo_check(_memo_obj * memox,void * in_key)	   
{
	_in_obj *key=(_in_obj*)in_key;
	u16 i;
	u16 temptopitem;
	u16 disitems=1;
	u16 temp;
   	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//������������
		  	memo_cursor_flash(memox);//ֻ������༭��ʱ�����Ҫ�����˸
			if(memox->top<key->y&&key->y<(memox->top+memox->height)&&(memox->left<key->x)&&key->x<(memox->left+memox->width))//��edit���ڲ�
			{
				memox->sta|=1<<7;//����а����¼� 	 
				if(key->x<(memox->left+memox->width-memox->scbv->width))//���ı�����
				{
					if(memox->sta&(1<<6))
					{
						if(gui_disabs(key->y,memox->typos)>10)//ƫ�Ƴ���10����λ��
						{
							memox->sta|=1<<5;	//��ǻ���
							if(memox->scbv->totalitems>memox->scbv->itemsperpage)//����Ҫ����һҳ�����ݲ��ܿ�ʼ����.
							{
								temptopitem=memox->scbv->topitem;
								disitems=gui_disabs(memox->typos,key->y)/memox->font;
								if(disitems==0)disitems=1;//��С��λΪ1;
								if(memox->typos>key->y)//topitem��Ҫ����
								{
									if(memox->scbv->topitem<(memox->scbv->totalitems-memox->scbv->itemsperpage))
									{
										temp=memox->scbv->totalitems-memox->scbv->itemsperpage-memox->scbv->topitem;
										if(temp>disitems)memox->scbv->topitem+=disitems;
										else memox->scbv->topitem+=temp;
									}
								}else//topitem��Ҫ��С
								{																				   
									if(memox->scbv->topitem>disitems)memox->scbv->topitem-=disitems;
									else memox->scbv->topitem=0;  
								}
								if(temptopitem!=memox->scbv->topitem)//��ֹÿ�ζ�����
								{
									memo_draw_text(memox);					//������ʾ����
									scrollbar_draw_scrollbar(memox->scbv);	//���»�scrollbar
								}
							}
							memox->typos=key->y;	//��¼�µ�λ��
						}		
					}
					else
					{
						memox->sta|=1<<6;
						memox->txpos=key->x;//������һ�ε�����ֵ
						memox->typos=key->y;

					}
				}else //�ڹ�������
				{
					temptopitem=memox->scbv->topitem;
					scrollbar_check(memox->scbv,in_key);//���������
					if(temptopitem!=memox->scbv->topitem)//topitemλ�÷����˱仯
					{
						memo_draw_text(memox);
					}					
				}
			}else if(memox->sta&(1<<7))//֮ǰ���а����¼���
			{
				if((memox->sta&0x60)==(1<<6))//�ǵ�����
				{
 					for(i=0;i<memox->scbv->itemsperpage;i++)//�õ�lin-memox->scbv->topitem�ı�� 
					{
						if(memox->typos<=(memox->top+(i+1)*memox->font)&&memox->typos>=(memox->top+i*memox->font))break;//�ڸ�����
					}
					if(memox->scbv->topitem+i<memox->scbv->totalitems)//���ܳ����������ĳ���
					{
						memo_show_cursor(memox,0);		//���ǰһ�ι���λ��	   
						memox->lin=memox->scbv->topitem+i;//�õ���ǰ��lin	 		  
						memo_cursor_set(memox,memox->txpos);
					}
				}
				scrollbar_check(memox->scbv,in_key);//����ټ��scrollbar
				memox->sta&=~(7<<5);//�������λ
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
//����x��������cursor��λ��
//memox:���±�
//x:x����
void memo_cursor_set(_memo_obj * memox,u16 x)
{
	u16 newpos;	    
	u16 strlen;
	u32 i;
	u16 maxpos;
	u8 tchr;

	strlen=(memox->width-MEMO_SCB_WIDTH)/(memox->font/2); 
	for(i=0;i<strlen;i++)
	{
	 	tchr=*(memox->text+memox->offsettbl[memox->lin]+i);
		if(tchr==0||tchr==0x0D)break;//������������   			 
	} 					    
	maxpos=i;										    
	if(x>memox->left+3)
	{
		x-=memox->left+3;
		newpos=x/(memox->font/2);
		if((x%(memox->font/2))>memox->font/2)newpos+=1;//����font/2,����Ϊ����һ��λ��
		
		if(newpos>maxpos)newpos=maxpos;	//���ܳ���text�ĳ���
		if(newpos>0)
		{
			maxpos=gui_string_forwardgbk_count(memox->text+memox->offsettbl[memox->lin],newpos-1);//�õ�֮ǰ�ĺ����������.
			if((maxpos%2)!=0)newpos++;//���ں����м�,��Ҫ����һ���ֽ�
		}
		memox->col=newpos;
		memo_show_cursor(memox,1);//������ʾ�µĹ��
	}					    
}
//�õ����ĵ�ǰx����
//memox:memoָ��
//����ֵ:����x����
u16 memo_get_cursorxpos(_memo_obj * memox)
{
	return memox->left+memox->col*(memox->font/2); 	//����X����;
}	
//�õ����ĵ�ǰy����
//memox:memoָ��
//����ֵ:����y����
u16 memo_get_cursorypos(_memo_obj * memox)
{
	return (memox->lin-memox->scbv->topitem)*memox->font+memox->top; //����Y����;
}
//�õ���ǰ�������λ����text�����λ��
u32 memo_get_cursortextpos(_memo_obj * memox)
{
	return memox->offsettbl[memox->lin]+memox->col;
}
//�������
//memox:memoָ��
//color:�����ɫ
void memo_draw_cursor(_memo_obj * memox,u16 color)
{
	u16 x,y;
	u8 i;
	x=memo_get_cursorxpos(memox);//����X����
	y=memo_get_cursorypos(memox);//����Y����
 	for(i=0;i<memox->font;i++)
	{	   
		if(i>(memox->top+memox->height-y-1))break;//ʣ�ಿ�ֲ���Ҫ��ʾ
		gui_phy.draw_point(x,y+i,color);
 		gui_phy.draw_point(x+1,y+i,color);					 
	}
}
//��ʾ���
//memox:memoָ��
//sta:0,����ʾ����;1,��ʾ����
void memo_show_cursor(_memo_obj * memox,u8 sta)
{
	u8 txt[3];
	u16 color;
	u32 textpos;
	u16 y;
	if((memox->type&0x01)==0)return;//������ʾ���	 
	if(sta==0)
	{
		color=memox->textbkcolor;//����Ҫ��ʾ,�ñ���ɫ
 		memox->sta&=~(1<<1);//����״̬Ϊδ��ʾ
	}else 
	{
		color=memox->textcolor;//��Ҫ��ʾ,������ɫ
		memox->sta|=1<<1;//����״̬Ϊ��ʾ
	}	 
	if((memox->lin<memox->scbv->topitem)||(memox->lin>=(memox->scbv->topitem+memox->scbv->itemsperpage)))return;//���lin<topitem,ֱ�ӾͲ���ʾ��.
	memo_draw_cursor(memox,color);//��ʾ���
	if(sta==0)//��Ҫ��ʾ���
	{
		textpos=memo_get_cursortextpos(memox);
		txt[0]=memox->text[textpos];
		if(txt[0]>0X80)
		{
			txt[1]=memox->text[textpos+1];
			txt[2]='\0';
		}else 																				   
		{
			if(txt[0]<' ')txt[0]='\0';
			txt[1]='\0';
		}
		if(txt[1]=='\0')y=memox->font/2;//���ַ�
		else y=memox->font;				//�Ǻ���
		if(memo_get_cursorxpos(memox)+y<=memox->left+memox->width-MEMO_SCB_WIDTH)//ֻ�в���������ߵĳ��ȵ�ʱ��,��������ʾ 
		{
			y=memo_get_cursorypos(memox);//�õ�y����
			gui_phy.back_color=memox->textbkcolor;//���ñ���ɫ 
			if(y<memox->top+memox->height)gui_show_ptstr(memo_get_cursorxpos(memox),y,memox->left+memox->width-MEMO_SCB_WIDTH,memox->top+memox->height-1,0,memox->textcolor,memox->font,txt,0);//
		}
	}
 
} 
//�����˸	 
//memox:�༭��
void memo_cursor_flash(_memo_obj *memox)
{
 	if(gui_disabs(memox->memo_timer_old,GUI_TIMER_10MS)>=50)//����ms��.
	{
		memox->memo_timer_old=GUI_TIMER_10MS;
		if(memox->sta&0x02)memo_show_cursor(memox,0);//֮ǰ����ʾ�����˵�
		else memo_show_cursor(memox,1);//֮ǰû����ʾ����	 
	}
}
	    					    
//��ʾ���ִ�
//memox:�༭��
void memo_draw_text(_memo_obj * memox)
{
 	gui_fill_rectangle(memox->left,memox->top,memox->width-MEMO_SCB_WIDTH,memox->height,memox->textbkcolor);//����ڲ�
  	gui_show_string(memox->text+memox->offsettbl[memox->scbv->topitem],memox->left,memox->top,memox->width-MEMO_SCB_WIDTH,memox->height,memox->font,memox->textcolor);
 	memo_show_cursor(memox,1); 	//��ʾ���	 
}
//����memo����Ϣ
//curpos:������ַ��������λ��
//����ֵ:0,ok;����,������.
u8 memo_info_update(_memo_obj * memox,u32 curpos)
{
	u16 xpos=0;
	u32 temp=0;
	u8 *str=memox->text;
	u16 linelenth=memox->width-MEMO_SCB_WIDTH;//ÿ�г���
	u32	lincnt=0;//������
	lincnt=gui_get_stringline(memox->text,linelenth,memox->font);
 	gui_memex_free(memox->offsettbl);//�ͷ�֮ǰ��
	memox->offsettbl=gui_memex_malloc(lincnt*4);//��������
	if(memox->offsettbl==NULL)return 1;
	memox->scbv->totalitems=lincnt;
	lincnt=0;
	memox->offsettbl[0]=0;
	//�õ��µ�offset��	   		    
	while(*str!='\0')
	{										   
		if(*str==0x0D&&(*(str+1)==0X0A))//�ǻس���
		{
			str+=2;	
			temp+=2; 
			lincnt++;//������1
			memox->offsettbl[lincnt]=temp;  
			xpos=0;
		}else if(*str>=0X81&&(*(str+1)>=0X40))//��gbk����
		{
			xpos+=memox->font;
			str+=2;
			temp+=2; 
			if(xpos>linelenth)//�Ѿ����ڱ���֮��
			{
				xpos=memox->font;
				lincnt++;//������1
				memox->offsettbl[lincnt]=temp-2;  
			}
		}else//���ַ�
		{
			xpos+=memox->font/2;
			str+=1;
			temp+=1; 
			if(xpos>linelenth)//�Ѿ����ڱ���֮��
			{
				xpos=memox->font/2;
				lincnt++;//������1
				memox->offsettbl[lincnt]=temp-1;  
			}				
		}		   
	}				  

	if(memox->offsettbl[memox->scbv->topitem]>curpos)//����һ��
	{
		while(memox->offsettbl[memox->scbv->topitem]>curpos)
		{
			if(memox->scbv->topitem)memox->scbv->topitem--;
			else 
			{
				memox->lin=0;
				memox->col=0;
				return 0;
			}
		}
 		memox->lin=memox->scbv->topitem;//������
		memox->col=curpos-memox->offsettbl[memox->scbv->topitem];
	}else 
	{
		temp=memox->scbv->topitem;
		while(1)
		{
			temp++;
			if(temp==memox->scbv->totalitems){temp--;break;}
			if(memox->offsettbl[temp]>curpos){temp--;break;}
		}
		if((temp-memox->scbv->topitem)>=memox->scbv->itemsperpage)
		{
			memox->scbv->topitem=temp-memox->scbv->itemsperpage+1;//����topitem
		}else if(memox->scbv->totalitems<memox->scbv->itemsperpage)memox->scbv->topitem=0;
		memox->lin=temp;
		memox->col=curpos-memox->offsettbl[temp];
	}
	return 0;	
}


//��memox��������ַ���
//memox:�༭��
//str:�ַ���
void memo_add_text(_memo_obj * memox,u8 * str)
{				  
	u8 *tempstr;
	u32 temp;
	u32 curpos=0;
 	u8 *laststr='\0';
	if((memox->type&0x01)==0)return;//���ǿɱ༭��״̬.�����������	 
	if(*str!=NULL)//������
	{
		curpos=memo_get_cursortextpos(memox);
		tempstr=memox->text+curpos;				//��������е��ַ�����ʼλ��
 		if(*str!=0x08)//�����˸�
		{
			temp=strlen((const char*)str)+strlen((const char*)memox->text);//�õ�������Ӻ�ĳ���
 			if(memox->textlen<temp)
			{
 				return;//̫����.ֱ�Ӳ�ִ�в���
			}
		}
		if(*tempstr!='\0')//���ǽ�����
		{
			temp=strlen((const char*)tempstr);//�õ�tempstr�ĳ���
			laststr=gui_memex_malloc(temp+1);//�����ڴ�
			if(laststr==NULL)return;//����ʧ��,ֱ���˳�.
			laststr[0]='\0';//������
			strcpy((char*)laststr,(const char*)tempstr);//����tempstr�����ݵ�laststr
		}
		if(*str==0x08)//�˸�
		{
			if(curpos>0)//�����˸�
			{
				curpos--;
				if(memox->text[curpos]>0x80)//�Ǻ���,Ҫ��2��
				{
					curpos--;
				}else if(curpos)//�����ַ�
				{
					if(memox->text[curpos]==0X0A&&memox->text[curpos-1]==0X0D)//�س�����
					{
						curpos--;
					}
				}
				memox->text[curpos]='\0';//��ӽ�����
				if(laststr)strcat((char*)memox->text,(const char*)laststr);//�ٽ�����ƴ����   	
			}  
		}else//���ַ���
		{
			memox->text[curpos]='\0';//��ӽ�����
			strcat((char*)memox->text,(const char*)str);	//������   	
			curpos+=strlen((const char*)str);				//���ƫ��
			if(laststr)strcat((char*)memox->text,(const char*)laststr);//�������ٴ�����   	
		}
		gui_memex_free(laststr);//�ͷ��ڴ�
		memo_info_update(memox,curpos);
		memo_draw_text(memox);
		scrollbar_draw_scrollbar(memox->scbv);
	}
}


//��memo
void memo_draw_memo(_memo_obj * memox)
{	   
	if(memox==NULL)return;//��Ч,ֱ���˳�
	gui_fill_rectangle(memox->left,memox->top,memox->width-MEMO_SCB_WIDTH,memox->height,MEMO_DFT_TBKC);//����ڲ�
	memo_info_update(memox,0);
	memo_draw_text(memox);
	scrollbar_draw_scrollbar(memox->scbv);
}


/////////////////////////////////////////////////////////////////////////////
void memo_test(u16 x,u16 y,u16 width,u16 height,u8 type,u8 sta,u16 textlen)
{
	_memo_obj* tmemo;
	tmemo=memo_creat(x,y,width,height,0,type,16,textlen);//������ť
	if(tmemo==NULL)return;//����ʧ��.
	//tmemo->sta=sta;
	//strcpy((char*)tmemo->text,(const char *)text);
	memo_draw_memo(tmemo);//����ť
 	memo_delete(tmemo);//ɾ����ť	 
} 




























