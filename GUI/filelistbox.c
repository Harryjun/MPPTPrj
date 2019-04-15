#include "filelistbox.h"
#include "ff.h"
#include "exfuns.h"
#include "piclib.h"			 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//GUI-�ļ��б�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/10/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

												    
//����filelistbox
//top,left,width,height:����.���ߴ�. 
//height:�߶ȱ���Ϊ:FLBOX_ITEM_HEIGHT*N+FLBOX_PATH_HEIGHT+FLBOX_FINFO_HEIGHT,����Ĭ��Ϊ40+20��N�� 
//width:��ȱ�����ڵ���FLBOX_ITEM_HEIGHT(20)
//type:bit7,�����Ƿ���ʾ������(�Զ�����),����λ����.
//font:�������� 12/16��ʾ����Ϊ12*12����16*16.
_filelistbox_obj * filelistbox_creat(u16 left,u16 top,u16 width,u16 height,u8 type,u8 font)
{
 	_filelistbox_obj * filelistbox_crt;
	_scrollbar_obj * scb_crt;
	if(height<=FLBOX_PATH_HEIGHT+FLBOX_ITEM_HEIGHT+FLBOX_FINFO_HEIGHT)return NULL;//�߶�С��	    
	if((height-FLBOX_PATH_HEIGHT-FLBOX_FINFO_HEIGHT)%FLBOX_ITEM_HEIGHT)return NULL;//�ߴ粻�ϸ� 	 				    
	if(width<FLBOX_ITEM_HEIGHT)return NULL;			//�ߴ粻�ϸ�
	filelistbox_crt=(_filelistbox_obj*)gui_memin_malloc(sizeof(_filelistbox_obj));	//�����ڴ�
	if(filelistbox_crt==NULL)return NULL;			//�ڴ���䲻��.
	gui_memset((u8*)filelistbox_crt,0,sizeof(_filelistbox_obj));//��filelistbox_crt��ֵȫ������Ϊ0    
	scb_crt=scrollbar_creat(left+width-FLBOX_SCB_WIDTH,top+FLBOX_PATH_HEIGHT,FLBOX_SCB_WIDTH,height-FLBOX_PATH_HEIGHT-FLBOX_FINFO_HEIGHT,0x80);//����scrollbar.
	if(scb_crt==NULL)
	{
		gui_memin_free(filelistbox_crt);//�ͷ�֮ǰ������ڴ�
		return NULL;					//�ڴ���䲻��.
 	}
 	scb_crt->itemsperpage=(height-FLBOX_PATH_HEIGHT-FLBOX_FINFO_HEIGHT)/FLBOX_ITEM_HEIGHT;	//ÿҳ��ʾ����Ŀ��

 	filelistbox_crt->top=top;
	filelistbox_crt->left=left;
	filelistbox_crt->width=width;
	filelistbox_crt->height=height;
	filelistbox_crt->type=type;		//���� 
 	filelistbox_crt->sta=0;
	filelistbox_crt->id=0;
	filelistbox_crt->dbclick=0;		//û��˫��
	filelistbox_crt->font=font;		//�����С 
	filelistbox_crt->selindex=0;	//��ǰѡ�е�����
													
   	filelistbox_crt->lbkcolor=FLBOX_DFT_LBKCOLOR;			//�ڲ�����ɫ
 	filelistbox_crt->lnselcolor=FLBOX_DFT_LSELCOLOR;		//ѡ��list���������ɫ 
 	filelistbox_crt->lnselbkcolor=FLBOX_DFT_LSELBKCOLOR;	//ѡ��list��ı���ɫ
 	filelistbox_crt->lncolor=FLBOX_DFT_LNCOLOR;				//δѡ�е�list������ɫ
   	filelistbox_crt->rimcolor=FLBOX_DFT_RIMCOLOR;			//�߿���ɫ
	filelistbox_crt->pthinfbkcolor=FLBOX_DFT_PTHINFBKCOLOR;	//·��/��Ϣ�� ������ɫ
	
	
	filelistbox_crt->path=NULL;			    				//·��Ϊ��	 

	filelistbox_crt->fliter=0X0000;		    				//�����ļ�����(�������ļ���)
	filelistbox_crt->fliterex=0XFFFF;	    				//����С����й���(����С�������Ҫ��)

	filelistbox_crt->fname=NULL;			    	    	//����Ϊ��
	filelistbox_crt->namelen=0;			    	    		//��������
	filelistbox_crt->curnamepos=0;			    	    	//��ǰλ������
	filelistbox_crt->oldtime=0;			    	    		//ʱ������
	 

	filelistbox_crt->findextbl=NULL;			    		//�ļ�������Ϊ��

	filelistbox_crt->scbv=scb_crt;			    			//������
 	filelistbox_crt->list=NULL;								//������
 	return filelistbox_crt;
}
//���ұ��Ϊindex��flist
//flistx:��ǰ��flistָ��
//index:ҪѰ�ҵ�flist��� 0~totalitems-1;
//����ֵ:list�ṹ��
//����ҵ�,�򷵻�list�ṹ��,���û�ҵ�,����NULL.
_filelistbox_list * filelist_search(_filelistbox_list *filelistx,u16 index)
{
	u16 icnt=0;
 	_filelistbox_list * filelisttemp=filelistx;	
	if(filelistx==NULL)return NULL;
	while(filelisttemp->prevlist!=NULL)filelisttemp=filelisttemp->prevlist;//׷�ݵ��ʼ��list
	while(1)//Ѱ�ұ��Ϊindex��list
	{
		if(index==icnt)break;	 
		if(filelisttemp->nextlist!=NULL)
		{
			filelisttemp=filelisttemp->nextlist;    
			icnt++;//��������
		}else return NULL;//δ�ҵ����Ϊindex��list
	} 
	return filelisttemp;//�����ҵ���listָ��
}
//ɾ���������� 
//filelistdel,Ҫɾ���������е�ĳ����Ա
void filelist_delete(_filelistbox_obj * filelistbox)
{
 	_filelistbox_list * filelisttemp;
  	_filelistbox_list * filelistdel;
	if(filelistbox==NULL)return;//�Ƿ��ĵ�ַ,ֱ���˳�
 	if(filelistbox->list!=NULL)
	{
		filelisttemp=filelist_search(filelistbox->list,0);//�õ���һ��filelist�Ľṹ��
		do
		{
			filelistdel=filelisttemp;
			filelisttemp=filelisttemp->nextlist;
		    gui_memex_free(filelistdel->name);	//ɾ����Ŀ������
		    gui_memex_free(filelistdel);		//ɾ��һ����Ŀ
		}while(filelisttemp!=NULL);		//һ��ɾ�����е���Ŀ
	}
	filelistbox->list=NULL;//���
 	filelistbox->scbv->totalitems=0;//����Ŀ����
	filelistbox->scbv->topitem=0;	//������Ŀ����
}
//ɾ��filelistbox
//filelistbox_del:Ҫɾ����filelistbox
void filelistbox_delete(_filelistbox_obj *filelistbox_del)
{
 	filelist_delete(filelistbox_del);	 	//ɾ����Ŀ
	gui_memin_free(filelistbox_del->findextbl);//ɾ��Ŀ���ļ�������
	gui_memin_free(filelistbox_del->path);	//ɾ��·��
	gui_memin_free(filelistbox_del->scbv);	//ɾ��������
	gui_memin_free(filelistbox_del);	  	//ɾ��filelistbox
}
//�ؽ��ļ��б��
//����Ч�ļ�ʹ�����˳���ʹ��,����ɨ���ļ������ػ�
//filelistbox:listboxָ��
u8 filelistbox_rebuild_filelist(_filelistbox_obj * filelistbox)
{
	u8 res=0;
	if(filelistbox!=NULL)
	{
		res=filelistbox_scan_filelist(filelistbox);	//����ɨ���б�	  
		if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)filelistbox->scbv->topitem=filelistbox->selindex;
		filelistbox_draw_listbox(filelistbox);		//�ػ� 
 	}else res=2;
	return res;
}	 
//���	
//filelistbox:listboxָ��
//in_key:���밴��ָ��
//����ֵ:δ�õ�				   
u8 filelistbox_check(_filelistbox_obj * filelistbox,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
	_filelistbox_list * filelistx;
	u16 endx;//��Ŀ��ʾ����β��.	
	u16 tempindex;
	u16 i;	   
	u16 lastindex; 
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//������������
 			if(filelistbox!=NULL)//listbox�ǿ�
			{
				if(filelistbox->dbclick==0x81||filelistbox->list==NULL)//Ŀ���ļ�������˫���������б�Ϊ��
				{		 
					break;//����ִ��ԭ����ɨ��			
				} 
				endx=filelistbox->left+filelistbox->width-1;		  
 				if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)//�й�����
				{ 
					endx-=FLBOX_SCB_WIDTH;
					 //�ڹ�������Χ֮��.
					if((filelistbox->top+FLBOX_PATH_HEIGHT)<=key->y&&key->y<=(filelistbox->top+filelistbox->height-FLBOX_FINFO_HEIGHT)&&endx<=key->x&&key->x<=(endx+FLBOX_SCB_WIDTH))
					{
						if(filelistbox->sta&(1<<6))break;//�Ǵ�list����������,ֱ�Ӳ�ִ��
						tempindex=filelistbox->scbv->topitem;
			 			scrollbar_check(filelistbox->scbv,in_key);//���������
						if(tempindex!=filelistbox->scbv->topitem)filelistbox_draw_list(filelistbox);
						filelistbox->sta|=1<<7;
						break;
					}
					if((filelistbox->sta&0xc0)==(1<<7))//�ϴβ����������ڹ�����������
					{
						filelistbox->sta=0;
						scrollbar_check(filelistbox->scbv,in_key);//���������					
					}
 				}
				tempindex=filelistbox->sta&0x3f;//�õ���ǰsta�е�λ��
				if((filelistbox->top+FLBOX_PATH_HEIGHT)<=key->y&&key->y<=(filelistbox->top+filelistbox->height-FLBOX_FINFO_HEIGHT)&&filelistbox->left<=key->x&&key->x<(endx))//��items������
				{ 
					//itemperpage����ܳ���64!(��20һ��index���,����64��list����֧�ֵ�64*20=1280��ֱ���ص���)
					for(i=0;i<filelistbox->scbv->itemsperpage;i++)//�ҵ���ǰ�������µ�������listbox�ֵ�index
					{
						if(key->y<=(filelistbox->top+FLBOX_PATH_HEIGHT+(i+1)*FLBOX_ITEM_HEIGHT)&&key->y>=(filelistbox->top+FLBOX_PATH_HEIGHT+i*FLBOX_ITEM_HEIGHT))break;
					}
					if((filelistbox->sta&(1<<6))==0)//��Ż���Ч
					{
						filelistbox->sta|=1<<6;	//�����Ч
						filelistbox->sta|=i;	//��¼���
					}else if((filelistbox->sta&(1<<7))==0)//�����ǻ���
					{
						if(filelistbox->scbv->totalitems<=filelistbox->scbv->itemsperpage)break;//û������,������Ч
						if(gui_disabs(i,(filelistbox->sta&0x3f))>1)	filelistbox->sta|=1<<7;//�����������1����Ŀ�ļ�϶ ��ǻ���
					}
					if((filelistbox->sta&0xc0)==0xc0)//�ǻ���
					{
						lastindex=filelistbox->scbv->topitem; 
						if(tempindex>i)//������
						{
							filelistbox->sta&=0xc0;//����ϴε�
							filelistbox->sta|=i;//��¼�����index��	    
							filelistbox->scbv->topitem+=tempindex-i;    
							if(filelistbox->scbv->topitem>=(filelistbox->scbv->totalitems-filelistbox->scbv->itemsperpage))filelistbox->scbv->topitem=filelistbox->scbv->totalitems-filelistbox->scbv->itemsperpage;
						}else if(i>tempindex)//������
						{
							filelistbox->sta&=0xc0;//����ϴε�
							filelistbox->sta|=i;//��¼�����index��
							i-=tempindex;				  
							if(filelistbox->scbv->topitem>=i)filelistbox->scbv->topitem-=i;
							else filelistbox->scbv->topitem=0;						
 						}else break;
						if(filelistbox->scbv->topitem!=lastindex)filelistbox_draw_listbox(filelistbox);//�ػ�listbox
					}
					filelistbox->oldtime=GUI_TIMER_10MS;//��¼��ǰʱ��
					filelistbox->curnamepos=0;//����
				}else //�����ɿ���,���߻�����
				{
					if(filelistbox->sta&(1<<7))//����
					{
						filelistbox->sta=0;										 
					}else if(filelistbox->sta&(1<<6))//���㰴��
					{
						filelistbox->dbclick=0;	//˫���������
						if((filelistbox->scbv->topitem+tempindex)==filelistbox->selindex)
						{
							filelistbox->dbclick|=1<<7;							//���˫����.
							filelistbox_check_filelist(filelistbox);//�ļ���
							filelistbox_2click_hook(filelistbox);//ִ��˫�����Ӻ���
						}else if((filelistbox->scbv->topitem+tempindex)<filelistbox->scbv->totalitems) //����ѡ���µ�ѡ��
						{	  
							if((filelistbox->selindex<(filelistbox->scbv->topitem+filelistbox->scbv->itemsperpage))&&(filelistbox->selindex>=filelistbox->scbv->topitem))//�ϵ�selindex����Ļ��Χ��
							{
								i=filelistbox->selindex-filelistbox->scbv->topitem;//�ϱ��
								filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//�õ�listindex������
								gui_fill_rectangle(filelistbox->left,filelistbox->top+FLBOX_PATH_HEIGHT+i*FLBOX_ITEM_HEIGHT,endx-filelistbox->left+1,FLBOX_ITEM_HEIGHT,filelistbox->lbkcolor);//�ָ���ɫ
 								gui_draw_icos(filelistbox->left,filelistbox->top+FLBOX_PATH_HEIGHT+i*FLBOX_ITEM_HEIGHT+(FLBOX_ITEM_HEIGHT-FLBOX_ICO_HEIGHT)/2,filelistx->type);//��ICOͼ��																				    
								gui_show_ptstr(filelistbox->left+FLBOX_ICO_WIDTH+2,filelistbox->top+FLBOX_PATH_HEIGHT+i*FLBOX_ITEM_HEIGHT+(FLBOX_ITEM_HEIGHT-filelistbox->font)/2,endx,gui_phy.lcdheight,0,filelistbox->lncolor,filelistbox->font,filelistx->name,1);//��ʾ����
							}
							filelistbox->selindex=filelistbox->scbv->topitem+tempindex; 
							filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//�õ�listindex������
							gui_fill_rectangle(filelistbox->left+FLBOX_ICO_WIDTH,filelistbox->top+FLBOX_PATH_HEIGHT+tempindex*FLBOX_ITEM_HEIGHT,endx-filelistbox->left+1-FLBOX_ICO_WIDTH,FLBOX_ITEM_HEIGHT,filelistbox->lnselbkcolor);//���ѡ�к�ĵ�ɫ
							gui_draw_icos(filelistbox->left,filelistbox->top+FLBOX_PATH_HEIGHT+tempindex*FLBOX_ITEM_HEIGHT+(FLBOX_ITEM_HEIGHT-FLBOX_ICO_HEIGHT)/2,filelistx->type);//��ICOͼ��																				    
							gui_show_ptstr(filelistbox->left+FLBOX_ICO_WIDTH+2,filelistbox->top+FLBOX_PATH_HEIGHT+tempindex*FLBOX_ITEM_HEIGHT+(FLBOX_ITEM_HEIGHT-filelistbox->font)/2,endx,gui_phy.lcdheight,0,filelistbox->lnselcolor,filelistbox->font,filelistx->name,1);//��ʾ����
							
							filelistbox->fname=filelistx->name;//��õ�ǰѡ�е���Ŀ������
							filelistbox->namelen=strlen((const char*)filelistbox->fname)*filelistbox->font/2;//���ֵ��ܳ���
  							filelistbox->curnamepos=0;//��ǰ��λ��Ϊ0. 
  						}
						filelistbox->sta=0;	  
						filelistbox_show_info(filelistbox);
						//˫��Ŀ���ļ���.
						if(filelistbox->dbclick==0X81&&filelistbox->list!=NULL)filelist_delete(filelistbox);//ɾ��֮ǰ����Ŀ,�ͷ��ڴ�
 						filelistbox->oldtime=GUI_TIMER_10MS;//��¼��ǰʱ��
						filelistbox->curnamepos=0;
					}else //����״̬,��û�л���,Ҳû���㰴,ִ��������ʾ.
					{
						if(gui_disabs(filelistbox->oldtime,GUI_TIMER_10MS)>=10)//����100ms��.
						{	    
							if(filelistbox->selindex>=filelistbox->scbv->topitem&&filelistbox->selindex<(filelistbox->scbv->topitem+filelistbox->scbv->itemsperpage))//ѡ����Ŀ�ڵ�ǰ����ʾ��Χ��
	 						{	 
								i=endx-filelistbox->left-FLBOX_ICO_WIDTH-2;//�õ���ʾ��������ĳ���
								if(i<filelistbox->namelen)//С�����ֳ���,���ǲ��ܹ�һ����ʾ��
								{
 		 							tempindex=filelistbox->selindex-filelistbox->scbv->topitem;
									gui_fill_rectangle(filelistbox->left+FLBOX_ICO_WIDTH,filelistbox->top+FLBOX_PATH_HEIGHT+tempindex*FLBOX_ITEM_HEIGHT,endx-filelistbox->left+1-FLBOX_ICO_WIDTH,FLBOX_ITEM_HEIGHT,filelistbox->lnselbkcolor);//���ѡ�к�ĵ�ɫ
		 							gui_show_ptstr(filelistbox->left+FLBOX_ICO_WIDTH+2,filelistbox->top+FLBOX_PATH_HEIGHT+tempindex*FLBOX_ITEM_HEIGHT+(FLBOX_ITEM_HEIGHT-filelistbox->font)/2,endx,gui_phy.lcdheight,filelistbox->curnamepos,filelistbox->lnselcolor,filelistbox->font,filelistbox->fname,1);//��ʾ����
									filelistbox->curnamepos++;
									if(filelistbox->curnamepos+i>filelistbox->namelen+i/4)filelistbox->curnamepos=0;//ѭ����ʾ	
								}
							}
							filelistbox->oldtime=GUI_TIMER_10MS;//��¼��ǰʱ��
						}
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
//����һ��filelistbox����Ŀ
//filelistbox:
//0,���ӳɹ�;
//1,����ʧ��
u8 filelistbox_addlist(_filelistbox_obj * filelistbox,u8 *name,u8 type)
{
	_filelistbox_list * filelistx;
	_filelistbox_list * filelisttemp;

	filelistx=(_filelistbox_list*)gui_memex_malloc(sizeof(_filelistbox_list));//�����ڴ�(�ⲿSRAM)
	if(filelistx==NULL)return 1;//�ڴ���䲻��.
	gui_memset(filelistx,0,sizeof(_filelistbox_list));//ȫ������Ϊ0
	filelistx->name=gui_memex_malloc(strlen((const char*)name)+1);	//�����ڴ������name,���һ��������.
 	if(filelistx->name==NULL)
	{
		gui_memex_free(filelistx);//�ͷ��Ѿ����뵽���ڴ�
		return 2;//�ڴ���䲻��.
	}
 	filelistx->name=(u8*)strcpy((char *)filelistx->name,(const char*)name);	//����name�����ݵ�
	filelistx->type=type;								//����
 	filelistx->nextlist=NULL;
 	if(filelistbox->list==NULL)//��δ��������
	{
		filelistx->prevlist=NULL;
		filelistbox->list=filelistx;
	}else //�Ѿ�������
	{
		filelisttemp=filelistbox->list;
		while(filelisttemp->nextlist!=NULL)filelisttemp=filelisttemp->nextlist;//ƫ�Ƶ����һ��list	 
		filelistx->prevlist=filelisttemp;
		filelisttemp->nextlist=filelistx;
	}
	filelistbox->scbv->totalitems++;//����Ŀ������1��
	if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)filelistbox->type|=0x80;//��Ҫ��ʾ����
 	else filelistbox->type&=~0x80;//����Ҫ��ʾ����
	return 0;
}
//��filelist
//filelistbox:filelistboxָ��
void filelistbox_draw_list(_filelistbox_obj *filelistbox)
{
	u16 i;
	u16 endx=0;
	u16 tempcolor;	
  	_filelistbox_list * filelistx;		   
 	endx=filelistbox->left+filelistbox->width-1; //��Ŀ��ʾ����x����
	if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)
	{
		endx-=FLBOX_SCB_WIDTH;//��Ҫ��������.
	}else filelistbox->scbv->topitem=0;
 	gui_fill_rectangle(filelistbox->left,filelistbox->top+FLBOX_PATH_HEIGHT,endx-filelistbox->left+1,filelistbox->height-FLBOX_PATH_HEIGHT-FLBOX_FINFO_HEIGHT,filelistbox->lbkcolor);//���Ϊ��ɫ.	
 	filelistx=filelist_search(filelistbox->list,filelistbox->scbv->topitem);//�õ�����list��Ϣ(curitem��listbox�����ڼ�¼����index)
	if(filelistx==NULL)return ;//û���κ�list
	for(i=0;i<filelistbox->scbv->itemsperpage;i++)//��ʾ��Ŀ
	{	
		if((filelistbox->scbv->topitem+i)==filelistbox->selindex)//����Ϊѡ����Ŀ
		{
			gui_fill_rectangle(filelistbox->left+FLBOX_ICO_WIDTH,filelistbox->top+FLBOX_PATH_HEIGHT+i*FLBOX_ITEM_HEIGHT,endx-filelistbox->left+1-FLBOX_ICO_WIDTH,FLBOX_ITEM_HEIGHT,filelistbox->lnselbkcolor);//����ɫ
			tempcolor=filelistbox->lnselcolor;
			filelistbox->fname=filelistx->name;//��õ�ǰѡ�е���Ŀ������
			filelistbox->namelen=strlen((const char*)filelistbox->fname)*filelistbox->font/2;//���ֵ��ܳ���
			filelistbox->curnamepos=0;//��ǰ��λ��Ϊ0. 
		}else tempcolor=filelistbox->lncolor;		
		gui_draw_icos(filelistbox->left,filelistbox->top+FLBOX_PATH_HEIGHT+i*FLBOX_ITEM_HEIGHT+(FLBOX_ITEM_HEIGHT-FLBOX_ICO_HEIGHT)/2,filelistx->type);//��ICOͼ��																				    
		gui_show_ptstr(filelistbox->left+FLBOX_ICO_WIDTH+2,filelistbox->top+FLBOX_PATH_HEIGHT+i*FLBOX_ITEM_HEIGHT+(FLBOX_ITEM_HEIGHT-filelistbox->font)/2,endx,gui_phy.lcdheight,0,tempcolor,filelistbox->font,filelistx->name,1);//��ʾ����
		if(filelistx->nextlist==0)break;
		filelistx=filelistx->nextlist;
 	}
}
//��ʾfilelistbox��·�� 
//filelistbox:filelistboxָ��
void filelistbox_show_path(_filelistbox_obj *filelistbox)
{
	u16 plen;
	u8 *path=filelistbox->path;
	u8 offset;    
	const u8 chgchar[2]={0X5C,0X00};//ת��� ��Ч"\"
	if(strlen((const char*)filelistbox->path)>4)offset=1;//�ļ���
 	else offset=0;//����һ��offset
	gui_draw_icosalpha(filelistbox->left+5,filelistbox->top+(FLBOX_PATH_HEIGHT-16)/2,offset);//ƫ��5����λ,��ʾͼ��
	offset=0;
	if(path==NULL)return;
	plen=strlen((const char*)path);
	if(plen)//��Ҫ��ʾ
	{
		if(plen*(FLBOX_PTHINF_FONT/2)>(filelistbox->width-16-7))
		{
			offset=(FLBOX_PTHINF_FONT/2)*3;
			while(plen*(FLBOX_PTHINF_FONT/2)>(filelistbox->width-offset-16-7))
			{
 				path=(u8*)strstr((const char*)++path,(const char*)chgchar);	//ȥ��һ��
				plen=strlen((const char*)path);					//�õ��µĳ���
			}
 			gui_show_ptstr(filelistbox->left+16+7,filelistbox->top+(FLBOX_PATH_HEIGHT-FLBOX_PTHINF_FONT)/2,filelistbox->width+filelistbox->left,gui_phy.lcdheight,0,filelistbox->lncolor,FLBOX_PTHINF_FONT,"...",1);//��ʾ3��
		}   	
		gui_show_ptstr(filelistbox->left+offset+16+7,filelistbox->top+(FLBOX_PATH_HEIGHT-FLBOX_PTHINF_FONT)/2,filelistbox->width+filelistbox->left,gui_phy.lcdheight,0,filelistbox->lncolor,FLBOX_PTHINF_FONT,path,1);//��ʾ·��
	}
}	 

//���������ַ���
const u8* FLBOX_DISK_SIZE_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"��������:","�űP����:","Capacity:",	
};
//���̿��ÿռ��ַ���
const u8* FLBOX_DISK_FREE_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"���ÿռ�:","���ÿ��g:","Free:",	
};
//�ļ����ַ���
const u8* FLBOX_FOLDER_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"�ļ���:","�ļ��A:","Folder:",	
};
//���̿��ÿռ��ַ���
const u8* FLBOX_FILES_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"  �ļ�:","  �ļ�:","  Files:",	
};

//��ʾ��Ϣ  
//filelistbox:filelistboxָ��
//mode:0,�Ǵ��ļ���/���̵Ĳ���.1,���ļ��л����,��Ҫ����ȫ����Ϣ
void filelistbox_show_info(_filelistbox_obj *filelistbox)
{
	_filelistbox_list *filelistx;
	FATFS *fs1;
	u8 res;
    u32 freclust=0,fresec=0,totsec=0;
 	u8 * temp;
	u8 * temp1;
	u8 * mark;
	temp=gui_memin_malloc(60);	//���ڴ���ַ���
	if(temp==NULL)return;
 	temp1=gui_memin_malloc(30);	//���ڴ�����ִ�
	if(temp1==NULL)
	{
		gui_memin_free(temp);	
		return;  
	}
 	gui_memset((char *)temp,0,60);
  	gui_memset((char *)temp1,0,30);

	gui_fill_rectangle(filelistbox->left,filelistbox->top+filelistbox->height-FLBOX_FINFO_HEIGHT,filelistbox->width,FLBOX_FINFO_HEIGHT,filelistbox->pthinfbkcolor);//��ʾ�ļ���Ϣ����ɫ
	gui_draw_hline(filelistbox->left,filelistbox->top+filelistbox->height-FLBOX_FINFO_HEIGHT,filelistbox->width,0X000);//��ʾ��ɫ�ĺ���		
	filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//�õ�ѡ�е�list����ϸ��Ϣ
	if(filelistx->type==FICO_DISK)//��ǰѡ�е��Ǵ���
	{
		if(filelistbox->selindex==0)mark="0:";//����0
		else mark="1:";//����1
		res=f_getfree((const TCHAR*)mark,(DWORD*)&freclust,&fs1);//���ʣ������
	    if(res==0)
		{											   
		    totsec=(fs1->n_fatent-2)*fs1->csize;//�õ���������
		    fresec=freclust*fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512								//��������������512�ֽڴ�С�Ĵ���
			totsec*=fs1->ssize/512;
			fresec*=fs1->ssize/512;
#endif	  
		}
		if(totsec<20480)//������С��10M
		{
			totsec>>=1,fresec>>=1;
			mark="KB ";
		}else
		{
			totsec>>=11,fresec>>=11;
			mark="MB ";
 		}
		strcpy((char*)temp,(const char*)FLBOX_DISK_SIZE_CAPTION_TBL[gui_phy.language]);//��������	
		gui_num2str(temp1,totsec);				//������תΪ�ַ���
		strcat((char*)temp,(const char*)temp1);	
		strcat((char*)temp,(const char*)mark);	//��ӵ�λ
  		strcat((char*)temp,(const char*)FLBOX_DISK_FREE_CAPTION_TBL[gui_phy.language]);//���ÿռ�		
 		gui_num2str(temp1,fresec);				//���ÿռ�תΪ�ַ���
		strcat((char*)temp,(const char*)temp1);	
		strcat((char*)temp,(const char*)mark);	//��ӵ�λ
	}else	//ѡ�е����ļ�/�ļ���
	{
 		if(filelistbox->selindex<filelistbox->foldercnt)fresec=filelistbox->selindex+1;	//���ļ�������Ŀ����.
		else freclust=filelistbox->selindex-filelistbox->foldercnt+1;		//���ļ�����Ŀ����
		strcpy((char*)temp,(const char*)FLBOX_FOLDER_CAPTION_TBL[gui_phy.language]);//�ļ���	
		gui_num2str(temp1,fresec);					//��ǰ�ļ��б��תΪ�ַ���
		strcat((char*)temp,(const char*)temp1);	
 		strcat((char*)temp,(const char*)"/");		
		gui_num2str(temp1,filelistbox->foldercnt);	//�ļ�������תΪ�ַ���
 		strcat((char*)temp,(const char*)temp1);	
 		strcat((char*)temp,(const char*)FLBOX_FILES_CAPTION_TBL[gui_phy.language]);//�ļ�		
		gui_num2str(temp1,freclust);				//��ǰ�ļ����תΪ�ַ���
		strcat((char*)temp,(const char*)temp1);	
 		strcat((char*)temp,(const char*)"/");		
		gui_num2str(temp1,filelistbox->filecnt);	//�ļ�����תΪ�ַ���
 		strcat((char*)temp,(const char*)temp1);	
	}
	gui_show_ptstr(filelistbox->left,filelistbox->top+filelistbox->height-FLBOX_FINFO_HEIGHT+(FLBOX_FINFO_HEIGHT-FLBOX_PTHINF_FONT)/2,filelistbox->width+filelistbox->left,gui_phy.lcdheight,0,filelistbox->lncolor,FLBOX_PTHINF_FONT,temp,1);//��ʾ·��
	gui_memin_free(temp);
 	gui_memin_free(temp1);
}
//��filelistbox
//filelistbox:filelistboxָ��
void filelistbox_draw_listbox(_filelistbox_obj *filelistbox)
{															  
	if(filelistbox==NULL)return;//��Ч,ֱ���˳�
	gui_fill_rectangle(filelistbox->left,filelistbox->top,filelistbox->width,FLBOX_PATH_HEIGHT-1,filelistbox->pthinfbkcolor);//��ʾ·������ɫ
	gui_draw_hline(filelistbox->left,filelistbox->top+FLBOX_PATH_HEIGHT-1,filelistbox->width,0X000);//��ʾ��ɫ�ĺ���		
 	filelistbox_show_path(filelistbox);		//��ʾ·��
	if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)scrollbar_draw_scrollbar(filelistbox->scbv);//��Ҫʱ,��������
    filelistbox_draw_list(filelistbox);	//��list
	filelistbox_show_info(filelistbox);	//��ʾ��Ϣ
}

//�������ֱ�
const u8* FLIST_DISK_NAME_TBL[2][GUI_LANGUAGE_NUM]=
{
	{"SD��","SD��","SD Card",},
 	{"FLASH����","FLASH�űP","FLASH Disk",},
};
//ɨ�赱ǰ·���µ������ļ�
//filelistbox:filelistboxָ��
//����ֵ:0,����;
//    ����,�������̳��ִ���
u8 filelistbox_scan_filelist(_filelistbox_obj *filelistbox)
{
	u8 res=0; 
 	DIR flboxdir;		//flboxר��	  
	FILINFO flboxfinfo;	 	   
	u16 type=0XFFFF;
	u16 typeex=0;
	u16 findex;	  
    u8 *fn;   //This function is assuming non-Unicode cfg. 
	u8 mark=0;//0,�����ļ���;1,����Ŀ���ļ�;2

#if _USE_LFN						
 	flboxfinfo.lfsize = _MAX_LFN * 2 + 1;
	flboxfinfo.lfname = gui_memin_malloc(flboxfinfo.lfsize);
	if(flboxfinfo.lfname==NULL)return 0XFF;//�����ڴ�ʧ�� 
   	gui_memset((char *)flboxfinfo.lfname,0,flboxfinfo.lfsize);
#endif	 
	 
 	if(filelistbox->path)	//·����Ч
	{
		res=f_opendir(&flboxdir,(const TCHAR*)filelistbox->path); //��һ��Ŀ¼
		if(res==FR_OK)		//��Ŀ¼�ɹ���.
		{
			if(filelistbox->list!=NULL)filelist_delete(filelistbox);	//ɾ��֮ǰ����Ŀ
 			filelistbox->foldercnt=0;
			filelistbox->filecnt=0;
			gui_memin_free(filelistbox->findextbl);//�ͷ�֮ǰ������������ڴ�
			filelistbox->findextbl=(u16*)gui_memin_malloc(FLBOX_MAX_DFFILE*2);//��Ĭ������FLBOX_MAX_DFFILE��Ŀ���ļ������ڴ�
 		} 
		while(res==FR_OK)
		{
			findex=(u16)flboxdir.index;//��¼�´��ļ���indexֵ
			res=f_readdir(&flboxdir,&flboxfinfo);//��ȡ�ļ���Ϣ
			if(res!=FR_OK||flboxfinfo.fname[0]==0)
			{
				if(mark==0)	//�ļ��в�������
				{
					mark=1;	//��ǲ���Ŀ���ļ�
					res=f_opendir(&flboxdir,(const TCHAR*)filelistbox->path); //�ٴδ򿪸�Ŀ¼
 					continue;
				}
				break;		//������/��ĩβ��,�˳�   
			}
			if (flboxfinfo.fname[0]=='.'&&flboxfinfo.fname[1]==0)continue;//���Ա���Ŀ¼
			fn=(u8*)(*flboxfinfo.lfname?flboxfinfo.lfname:flboxfinfo.fname);
 			if(flboxfinfo.fattrib&(1<<4)&&mark==0)//��һ���ļ�����mark=0
			{
				type=FICO_FOLDER;	
				filelistbox->foldercnt++;
			}else if(flboxfinfo.fattrib&(1<<5)&&mark==1)//��һ���鵵�ļ���mark=1
			{
				type=f_typetell(fn);	//�������
				typeex=type&0XF;		//���С��ı��
				type>>=4;				//��ô���ı��
 				if(((1<<type)&filelistbox->fliter)&&((1<<typeex)&filelistbox->fliterex))//��С���Ͷ���ȷ
				{
					if(type<10)type+=3;	//�ļ�����(3~8)
					else type=2;		//δ֪�ļ�
					if(filelistbox->filecnt<FLBOX_MAX_DFFILE)filelistbox->findextbl[filelistbox->filecnt]=findex;//��¼���ļ�����
					filelistbox->filecnt++;//Ŀ���ļ�����
				}else continue;	//����Ҫ������	
			}else continue;		//��������һ�� 
			if(filelistbox_addlist(filelistbox,fn,type))break;//���ʧ����         
		}
		if(mark==1&&filelistbox->filecnt)
		{
			fn=gui_memin_realloc(filelistbox->findextbl,filelistbox->filecnt*2);//�ͷŶ�����ڴ�
			if(fn!=NULL)filelistbox->findextbl=(u16*)fn;//�������ɹ���,��ʹ���µĵ�ַ,����Դ��ַ����
		}   
	}else//û���κ���Ч·��,˵���ص�ѡ����̵�״̬��
	{
		filelist_delete(filelistbox);//ɾ��֮ǰ����Ŀ
		filelistbox->selindex=0;
		filelistbox->foldercnt=0;
		filelistbox->filecnt=0;
 		filelistbox_addlist(filelistbox,(u8*)FLIST_DISK_NAME_TBL[0][gui_phy.language],0);		//����0
		filelistbox_addlist(filelistbox,(u8*)FLIST_DISK_NAME_TBL[1][gui_phy.language],0);		//����1
 	}
	gui_memin_free(flboxfinfo.lfname);
	return res;
}
//���·����Ŀ¼���
//path:·��
u8 filelistbox_get_pathdepth(u8* path)
{
	u8 depth=0;
	while(*path!=0)
	{
		if(*path==0X5C)//�ҵ�'\'��
		{
			depth++;
		}
		path++;
	}
	return depth;
	
}
//ɨ��·���µ��ļ�
//filelistbox:filelistָ��
void filelistbox_check_filelist(_filelistbox_obj *filelistbox)
{
	u8 *str;
	u8 *oldpath;
	u16 temp;
	u8 res=0;
	_filelistbox_list *filelistx;
	u16 ttlen;//�ܳ���
	filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//�õ�ѡ�е�list����ϸ��Ϣ
 	if(filelistx->type==0||filelistx->type==1)//���ڴ���/�ļ���,���	
	{   
		temp=filelistbox_get_pathdepth(filelistbox->path);//�õ���ǰĿ¼�����
		if(filelistx->type==0)//�Ǵ���
		{	
			if(filelistbox->selindex==0)str="0:";//ѡ�д���0,SD��.
		   	else str="1:";//����1,flash��
			gui_memin_free(filelistbox->path);//�ͷ�֮ǰ������ڴ�
			filelistbox->path=(u8*)gui_memin_malloc(strlen((const char*)str)+1);//���������ڴ�
			if(filelistbox->path==NULL)return;//�����ڴ�ʧ��.
 			strcpy((char *)filelistbox->path,(const char*)str);//����str��path����
		}else //���ļ���
		{
			if(strcmp((const char*)filelistx->name,".."))//����..�ļ���
			{
				if(temp<FLBOX_PATH_DEPTH)filelistbox->seltbl[temp]=filelistbox->selindex;//��¼�µ�ǰĿ¼��ѡ�е�
			  	ttlen=strlen((const char*)filelistbox->path);//�õ�ԭ����·���ĳ���(������������)
				ttlen+=strlen((const char*)filelistx->name);	//�õ����ֵĳ���
				oldpath=(u8*)gui_memin_malloc(strlen((const char*)filelistbox->path)+1);//�����ڴ�
				if(oldpath==NULL)return;//�����ڴ�ʧ��
 				oldpath=(u8*)strcpy((char *)oldpath,(const char*)filelistbox->path);//����filelistbox->path��oldpath����
	 			gui_memin_free(filelistbox->path);//�ͷ�filelistbox->path֮ǰ������ڴ�
				filelistbox->path=(u8*)gui_memin_malloc(ttlen+2);//���������ڴ�
				if(filelistbox->path==NULL)
				{
					gui_memin_free(oldpath);
					return;//�����ڴ�ʧ��.
				}
 				filelistbox->path=gui_path_name(filelistbox->path,oldpath,filelistx->name);//·�����ļ������
				gui_memin_free(oldpath);//�ͷ�������ڴ�.
				temp=0;//������Ŀ¼,ѡ��0λ��.
			}else //..,������һ��Ŀ¼
			{
				filelistbox_back(filelistbox);//�˻���һ��Ŀ¼
				return ;  
 			}
		}
		res=filelistbox_scan_filelist(filelistbox);	//����ɨ���б�
		filelistbox->selindex=temp;				//���һ��ѡ�е���Ŀ
		if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)filelistbox->scbv->topitem=temp;
		if(res==0&&filelistx->type==0&&filelistbox->foldercnt==0&&filelistbox->filecnt==0)	//���̸�Ŀ¼ɨ������,�Ҹ�Ŀ¼��û���κ��ļ�/�ļ���
		{	    
 			filelistbox_addlist(filelistbox,"..",1);	//��ӷ�����һ��α�ļ��� 
		}
		filelistbox_draw_listbox(filelistbox);	//�ػ�
 	}else 
	{
		filelistbox->dbclick|=1<<0;//�����Ŀ���ļ�
	}	 
}

//����һ������.(���ز���)
//�˺������Ե�������,�Զ�������һ��.�Դ���ɨ�輰�ػ�����.
//filelistbox:filelistboxָ��
void filelistbox_back(_filelistbox_obj *filelistbox)
{
	u8 *oldpath;
	u16 temp=0; 
	oldpath=filelistbox->path;
	while(*oldpath!=0)//һֱƫ�Ƶ����.
	{
		oldpath++;
		temp++;
	}
	if(temp<4)
	{
		if(filelistbox->path)gui_memin_free(filelistbox->path);//�ͷ�֮ǰ���뵽���ڴ�
		filelistbox->path=NULL;//�˵����
		temp=0;
	}else	   
	{	  
		while(*oldpath!=0x5c)oldpath--;	//׷����������һ��"\"�� 
		*oldpath='\0';//�ý���������"\".�Ӷ��ص���һ��Ŀ¼
		temp=filelistbox_get_pathdepth(filelistbox->path);//�õ���Ŀ¼�����
		if(temp<FLBOX_PATH_DEPTH)temp=filelistbox->seltbl[temp];
		else temp=0;
	}   
	filelistbox_scan_filelist(filelistbox);	//����ɨ���б�
	filelistbox->selindex=temp;				//���һ��ѡ�е���Ŀ
	if(filelistbox->scbv->totalitems>filelistbox->scbv->itemsperpage)filelistbox->scbv->topitem=temp;
	filelistbox_draw_listbox(filelistbox);	//�ػ�
}

//��������Ŀ˫�����Ӻ���
//filelistbox:filelistboxָ��
void filelistbox_2click_hook(_filelistbox_obj *filelistbox)
{	 	
//	u8 *pname; 
//	u8 t=0;
//	_filelistbox_list *filelistx;
//	filelistx=filelist_search(filelistbox->list,filelistbox->selindex);//�õ�ѡ�е�list����ϸ��Ϣ
// 	if(filelistx->type==FICO_PICTURE)//����ͼƬ�ļ�
//	{
//		pname=gui_memin_malloc(strlen((const char*)filelistx->name)+strlen((const char*)filelistbox->path)+2);//�����ڴ�
//		if(pname==NULL)return;
//	    pname=gui_path_name(pname,filelistbox->path,filelistx->name);//�ļ�������·��  
//		pic_viewer(pname);//������ļ�.
//		filelistbox_draw_listbox(filelistbox);//�ػ�filelistbox
//		gui_memin_free(pname);
//	}		  
// 	if(filelistx->type==FICO_MUSIC)//���������ļ�
//	{
//		pname=gui_memin_malloc(strlen((const char*)filelistx->name)+strlen((const char*)filelistbox->path)+2);//�����ڴ�
//		if(pname==NULL)return;
//	    pname=gui_path_name(pname,filelistbox->path,filelistx->name);//�ļ�������·��  
//
//		mp3sta&=~(1<<0);//������ֹ����
//		while(mp3sta&0x80)delay_ms(10);//�ȴ�MP3ֹͣ����
//		OSMboxPost(mp3mbox,pname);//��������
//		mp3sta|=1<<0;	//������
//		while((mp3sta&0x80)==0&&t<200)//�ȴ�MP3��ʼ����
//		{
//			delay_ms(10);
//			t++;
//		}
//		filelistbox_draw_listbox(filelistbox);//�ػ�filelistbox
//		gui_memin_free(pname);
//	}
//	printf("listbox%d index %d 2click!\r\n",filelistbox->id,filelistbox->selindex);
} 

///////////////////////////////////////��������//////////////////////////
_filelistbox_obj * tfilelistbox;
//����
void tsflistbox_creat(u16 x,u16 y,u16 lenth,u16 height,u8 type,u8 fsize)
{
	tfilelistbox=filelistbox_creat(x,y,lenth,height,type,fsize);
}
//����·��
void tsflistbox_setpath(u8 *path)
{	 
	gui_memin_free(tfilelistbox->path);//�ͷ�֮ǰ��.		   
	tfilelistbox->path=(u8*)gui_memin_malloc(strlen((const char*)path)+1);//�����ڴ�
	if(tfilelistbox->path==NULL)return  ;//�ڴ���䲻��.	
	tfilelistbox->path=(u8*)strcpy((char *)tfilelistbox->path,(const char*)path);
}
//ɾ��
void tsflistbox_del(void)
{
	filelistbox_delete(tfilelistbox);
	tfilelistbox=NULL;
	LCD_Clear(0XFFFF);
}	  
//���1��listbox��Ŀ
u16 tsflistbox_addlist(u8 *name,u8 type)
{
	filelistbox_addlist(tfilelistbox,name,type);
	return tfilelistbox->scbv->totalitems; 
}
//����listbox
void tsflistbox_draw(void)
{
	filelistbox_draw_listbox(tfilelistbox);	 
}















