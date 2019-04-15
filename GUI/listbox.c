#include "listbox.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//GUI-�б�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/10/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//����listbox
//top,left,width,height:����.���ߴ�.
//width�ķ�Χ:������ڵ���LBOX_ITEM_HEIGHT
//height�ķ�Χ:������LBOX_ITEM_HEIGHT��������.		 
//type:bit7,�����Ƿ���ʾ������,����λ����.
//font:�������� 12/16��ʾ����Ϊ12*12����16*16.
_listbox_obj * listbox_creat(u16 left,u16 top,u16 width,u16 height,u8 type,u8 font)
{
 	_listbox_obj * listbox_crt;
	_scrollbar_obj * scb_crt;	    
	if(height%LBOX_ITEM_HEIGHT)return NULL;//�ߴ粻�ϸ� 	 				    
	if(height<LBOX_ITEM_HEIGHT||width<LBOX_ITEM_HEIGHT)return NULL;	//�ߴ粻�ϸ�
	listbox_crt=(_listbox_obj*)gui_memin_malloc(sizeof(_listbox_obj));//�����ڴ�
	if(listbox_crt==NULL)return NULL;			//�ڴ���䲻��.
	scb_crt=scrollbar_creat(left+width-LBOX_SCB_WIDTH,top,LBOX_SCB_WIDTH,height,0x80);//����scrollbar.
	if(scb_crt==NULL)//�ڴ���䲻��.
	{
		gui_memin_free(listbox_crt);
		return NULL;				
	}
 	scb_crt->itemsperpage=height/LBOX_ITEM_HEIGHT;	//ÿҳ��ʾ����Ŀ��

 	listbox_crt->top=top;
	listbox_crt->left=left;
	listbox_crt->width=width;
	listbox_crt->height=height;
	listbox_crt->type=type;		//���� 
 	listbox_crt->sta=0;
	listbox_crt->id=0;
	listbox_crt->dbclick=0;		//˫����־����
	listbox_crt->font=font;		//�����С 
	listbox_crt->selindex=0;	//��ǰѡ�е�����
													
   	listbox_crt->lbkcolor=LBOX_DFT_LBKCOLOR;			//�ڲ�����ɫ
 	listbox_crt->lnselcolor=LBOX_DFT_LSELCOLOR;			//ѡ��list���������ɫ 
 	listbox_crt->lnselbkcolor=LBOX_DFT_LSELBKCOLOR;		//ѡ��list��ı���ɫ
 	listbox_crt->lncolor=LBOX_DFT_LNCOLOR;				//δѡ�е�list������ɫ
   	listbox_crt->rimcolor=LBOX_DFT_RIMCOLOR;			//�߿���ɫ
 
	listbox_crt->scbv=scb_crt;			    			//������
 	listbox_crt->list=NULL;								//������
 	return listbox_crt;
}
//���ұ��Ϊindex��list
//listx:��ǰ��list
//index:ҪѰ�ҵ�list��� 0~totalitems-1;
//����ֵ:list�ṹ��
//����ҵ�,�򷵻�list�ṹ��,���û�ҵ�,����NULL.
_listbox_list * list_search(_listbox_list *listx,u16 index)
{
	u16 icnt=0;
 	_listbox_list * listtemp=listx;	
	while(listtemp->prevlist!=NULL)listtemp=listtemp->prevlist;//׷�ݵ��ʼ��list
	while(1)//Ѱ�ұ��Ϊindex��list
	{
		if(index==icnt)break;	 
		if(listtemp->nextlist!=NULL)
		{
			listtemp=listtemp->nextlist;    
			icnt++;//��������
		}else return NULL;//δ�ҵ����Ϊindex��list
	} 
	return listtemp;//�����ҵ���listָ��
} 
//ɾ��listbox
//btn_del:Ҫɾ���İ���
void listbox_delete(_listbox_obj *listbox_del)
{
 	_listbox_list * listtemp;
 	_listbox_list * listdel;
	if(listbox_del->list!=NULL)
	{				    	  
		listtemp=list_search(listbox_del->list,0);//�õ���һ��list�Ľṹ��
 		do
		{
			listdel=listtemp;
			listtemp=listtemp->nextlist;
		    gui_memin_free(listdel);	//ɾ��һ����Ŀ
		}while(listtemp!=NULL);	//һ��ɾ�����е���Ŀ
	}
	gui_memin_free(listbox_del->scbv);//ɾ��������
 	gui_memin_free(listbox_del);
}
//���	
//listbox:listboxָ��
//in_key:���밴��ָ��
//����ֵ:δ�õ�				   
u8 listbox_check(_listbox_obj * listbox,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
	_listbox_list * listx;
	u16 endx;//��Ŀ��ʾ����β��.	
	u16 tempindex;
	u16 i;	   
	u16 lastindex; 
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//������������
 			if(listbox!=NULL)//listbox�ǿ�
			{
				endx=listbox->left+listbox->width;		  
 				if(listbox->scbv->totalitems>listbox->scbv->itemsperpage)//�й�����
				{ 
					endx-=LBOX_SCB_WIDTH+1;
					 //�ڹ�������Χ֮��.
					if(listbox->top<=key->y&&key->y<=(listbox->top+listbox->height)&&endx<=key->x&&key->x<=(endx+LBOX_SCB_WIDTH))
					{
						if(listbox->sta&(1<<6))break;//�Ǵ�list����������,ֱ�Ӳ�ִ��
						tempindex=listbox->scbv->topitem;
			 			scrollbar_check(listbox->scbv,in_key);//���������
						if(tempindex!=listbox->scbv->topitem)listbox_draw_list(listbox);
						listbox->sta|=1<<7;
						break;
					}
					if((listbox->sta&0xc0)==(1<<7))//�ϴβ����������ڹ�����������
					{
						listbox->sta=0;
						scrollbar_check(listbox->scbv,in_key);//���������					
					}
					
				}
				tempindex=listbox->sta&0x3f;//�õ���ǰsta�ֵ�λ��
				if(listbox->top<=key->y&&key->y<=(listbox->top+listbox->height)&&listbox->left<=key->x&&key->x<(endx))//��items������
				{
					//itemperpage����ܳ���64!(��20һ��index���,����64��list����֧�ֵ�64*20=1280��ֱ���ص���)
					for(i=0;i<listbox->scbv->itemsperpage;i++)//�ҵ���ǰ�������µ�������listbox�ֵ�index
					{
						if(key->y<=(listbox->top+(i+1)*LBOX_ITEM_HEIGHT)&&key->y>=(listbox->top+i*LBOX_ITEM_HEIGHT))break;
					}
					if((listbox->sta&(1<<6))==0)//��Ż���Ч
					{
						listbox->sta|=1<<6;	//�����Ч
						listbox->sta|=i;	//��¼���
					}else if((listbox->sta&(1<<7))==0)//�����ǻ���
					{
						if(listbox->scbv->totalitems<=listbox->scbv->itemsperpage)break;//û������,������Ч
						if(gui_disabs(i,(listbox->sta&0x3f))>1)	listbox->sta|=1<<7;//�����������1����Ŀ�ļ�϶ ��ǻ���
					}
					if((listbox->sta&0xc0)==0xc0)//�ǻ���
					{
						lastindex=listbox->scbv->topitem; 
						if(tempindex>i)//������
						{
							listbox->sta&=0xc0;//����ϴε�
							listbox->sta|=i;//��¼�����index��	    
							listbox->scbv->topitem+=tempindex-i;    
							if(listbox->scbv->topitem>=(listbox->scbv->totalitems-listbox->scbv->itemsperpage))listbox->scbv->topitem=listbox->scbv->totalitems-listbox->scbv->itemsperpage;
						}else if(i>tempindex)//������
						{
							listbox->sta&=0xc0;//����ϴε�
							listbox->sta|=i;//��¼�����index��
							i-=tempindex;				  
							if(listbox->scbv->topitem>=i)listbox->scbv->topitem-=i;
							else listbox->scbv->topitem=0;						
 						}else break;
						if(listbox->scbv->topitem!=lastindex)listbox_draw_listbox(listbox);//�ػ�listbox
					}
				}else //�����ɿ���,���߻�����
				{
					if(listbox->sta&(1<<7))//����
					{
						listbox->sta=0;										 
					}else if(listbox->sta&(1<<6))//���㰴��
					{
						listbox->dbclick=0;			//˫���������
						if((listbox->scbv->topitem+tempindex)==listbox->selindex)
						{
							listbox->dbclick|=1<<7;	//���˫����
							listbox_2click_hook(listbox);//ִ��˫�����Ӻ���
						}else if((listbox->scbv->topitem+tempindex)<listbox->scbv->totalitems) //����ѡ���µ�ѡ��
						{		 
							if((listbox->selindex<(listbox->scbv->topitem+listbox->scbv->itemsperpage))&&(listbox->selindex>=listbox->scbv->topitem))//�ϵ�selindex����Ļ��Χ��
							{
								i=listbox->selindex-listbox->scbv->topitem;//�ϱ��
								listx=list_search(listbox->list,listbox->selindex);//�õ�listindex������
								gui_fill_rectangle(listbox->left,listbox->top+i*LBOX_ITEM_HEIGHT,endx-listbox->left+1,LBOX_ITEM_HEIGHT,listbox->lbkcolor);//�ָ���ɫ
								gui_show_ptstr(listbox->left,listbox->top+i*LBOX_ITEM_HEIGHT+(LBOX_ITEM_HEIGHT-listbox->font)/2,endx,gui_phy.lcdheight,0,listbox->lncolor,listbox->font,listx->name,1);//��ʾ����
							}
							listbox->selindex=listbox->scbv->topitem+tempindex; 
							listx=list_search(listbox->list,listbox->selindex);//�õ�listindex������
							gui_fill_rectangle(listbox->left,listbox->top+tempindex*LBOX_ITEM_HEIGHT,endx-listbox->left+1,LBOX_ITEM_HEIGHT,listbox->lnselbkcolor);//���ѡ�к�ĵ�ɫ
							gui_show_ptstr(listbox->left,listbox->top+tempindex*LBOX_ITEM_HEIGHT+(LBOX_ITEM_HEIGHT-listbox->font)/2,endx,gui_phy.lcdheight,0,listbox->lnselcolor,listbox->font,listx->name,1);//��ʾ����
						}
						listbox->sta=0;	  
					}
				}
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
//����һ��listbox����Ŀ
//0,���ӳɹ�;
//1,����ʧ��
u8 listbox_addlist(_listbox_obj * listbox,u8 *name)
{
	_listbox_list * listx;
	_listbox_list * listtemp;

	listx=(_listbox_list*)gui_memin_malloc(sizeof(_listbox_list));//�����ڴ�
	if(listx==NULL)return 1;//�ڴ���䲻��.
	listx->name=name;//�õ�����
	listx->nextlist=NULL;
 	if(listbox->list==NULL)//��δ��������
	{
		listx->prevlist=NULL;
		listbox->list=listx;
	}else //�Ѿ�������
	{
		listtemp=listbox->list;
		while(listtemp->nextlist!=NULL)listtemp=listtemp->nextlist;	 
		listx->prevlist=listtemp;
		listtemp->nextlist=listx;
	}
	listbox->scbv->totalitems++;//����Ŀ������1��
	if(listbox->scbv->totalitems>listbox->scbv->itemsperpage)listbox->type|=0x80;//��Ҫ��ʾ����
 	else listbox->type&=~0x80;//����Ҫ��ʾ����
	return 0;
}
//��list
//listbox:listboxָ��
void listbox_draw_list(_listbox_obj *listbox)
{
	u16 i;
	u16 endx=0;
	u16 tempcolor;	  
 	_listbox_list * listx;		   
 	endx=listbox->left+listbox->width; //��Ŀ��ʾ����x����
	if(listbox->scbv->totalitems>listbox->scbv->itemsperpage)endx-=LBOX_SCB_WIDTH+1;//��Ҫ��������.
	gui_fill_rectangle(listbox->left,listbox->top,endx-listbox->left+1,listbox->height,listbox->lbkcolor);//���Ϊ��ɫ.	
	listx=list_search(listbox->list,listbox->scbv->topitem);//�õ�����list��Ϣ(curitem��listbox�����ڼ�¼����index)
	for(i=0;i<listbox->scbv->itemsperpage;i++)//��ʾ��Ŀ
	{	
		if((listbox->scbv->topitem+i)==listbox->selindex)//����Ϊѡ����Ŀ
		{
			gui_fill_rectangle(listbox->left,listbox->top+i*LBOX_ITEM_HEIGHT,endx-listbox->left+1,LBOX_ITEM_HEIGHT,listbox->lnselbkcolor);//����ɫ
			tempcolor=listbox->lnselcolor;
		}else tempcolor=listbox->lncolor;																						    
		gui_show_ptstr(listbox->left,listbox->top+i*LBOX_ITEM_HEIGHT+(LBOX_ITEM_HEIGHT-listbox->font)/2,endx,gui_phy.lcdheight,0,tempcolor,listbox->font,listx->name,1);
		if(listx->nextlist==0)break;
		listx=listx->nextlist;
 	}
}
//��listbox
//listbox:listboxָ��
void listbox_draw_listbox(_listbox_obj *listbox)
{															  
	if(listbox==NULL)return;//��Ч,ֱ���˳�
	if(listbox->scbv->totalitems>listbox->scbv->itemsperpage)scrollbar_draw_scrollbar(listbox->scbv);//��Ҫʱ,��������
    listbox_draw_list(listbox);//��list
}
//��������Ŀ˫�����Ӻ���
//listbox:listboxָ��
void listbox_2click_hook(_listbox_obj *listbox)
{
	//printf("listbox%d index %d 2click!\r\n",listbox->id,listbox->selindex);
}

/////////////////////////////////////////////////////����Ϊ���Ժ���//////////////////////////////////////////////////
_listbox_obj * tlistbox;
//����
void tslistbox_creat(u16 x,u16 y,u16 lenth,u16 height,u8 type,u8 fsize)
{
	tlistbox=listbox_creat(x,y,lenth,height,type,fsize);
}
//ɾ��
void tslistbox_del(void)
{
	listbox_delete(tlistbox);
	tlistbox=NULL;
	LCD_Clear(0XFFFF);
}

const u8 *lntbl[6]=
{
"��������",
"�㳤���ϸ���,�ܲ�����������?",
"Settings",
"������յ�,fuck off me!", 
"����������ٸ���һ�λ���,�һ����˵'��Զ��'!",
"����ɫ����kdj����",
};
//���1��listbox��Ŀ
u16 tslistbox_addlist(u8 nameindex)
{
	listbox_addlist(tlistbox,(u8*)lntbl[nameindex]);
	return tlistbox->scbv->totalitems; 
}
//����listbox
void tslistbox_draw(void)
{
	listbox_draw_listbox(tlistbox);	 
}

void testlistadd(void)
{
	u8 t;
	t=mem_perused(SRAMIN);
	tlistbox=listbox_creat(0,0,100,120,0,12);
	t=listbox_addlist(tlistbox,"test");	 
	while(t==0)
	{
		t=listbox_addlist(tlistbox,"ab5cd");
	}
    listbox_draw_listbox(tlistbox);
	listbox_delete(tlistbox);
	t=mem_perused(SRAMIN);
	t=0;
	if(t==0)
	t=mem_perused(SRAMIN);
	if(t)t=0;
}













































