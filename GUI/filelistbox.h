#ifndef __FILELISTBOX_H
#define __FILELISTBOX_H 
#include "guix.h"  	 			  
#include "scrollbar.h"
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

//�ļ�ͼ�궨��
#define FICO_DISK				0  			//����
#define FICO_FOLDER		 		1			//�ļ���
#define FICO_UNKNOW				2			//δ֪�ļ�
#define FICO_BIN				3			//BIN�ļ�
#define FICO_LRC				4			//LRC�ļ�
#define FICO_NES				5			//NES�ļ�
#define FICO_TEXT				6			//�ı��ļ�
#define FICO_MUSIC				7			//�����ļ�
#define FICO_PICTURE			8			//ͼƬ�ļ�

//�ļ���������λ���ܶ���
#define FLBOX_FLT_BIN 			1<<0		//BIN�ļ�
#define FLBOX_FLT_LRC	 		1<<1		//LRC�ļ�
#define FLBOX_FLT_NES	 		1<<2		//NES�ļ�
#define FLBOX_FLT_TEXT	 		1<<3		//�ı��ļ�
#define FLBOX_FLT_MUSIC	 		1<<4		//�����ļ�
#define FLBOX_FLT_PICTURE 		1<<5		//ͼƬ�ļ�
#define FLBOX_FLT_UNKNOW		1<<15		//�޷�ʶ���һ���ļ�

//////filelistbox�ϵķ��ŵ�Ĭ����ɫ 
#define FLBOX_DFT_LBKCOLOR		0XFFFF		//�ڲ�����ɫ
#define FLBOX_DFT_LSELCOLOR		0XFFFF		//ѡ��list���������ɫ
#define FLBOX_DFT_LSELBKCOLOR	0X001F		//0X07E0		//ѡ��list��ı���ɫ
#define FLBOX_DFT_LNCOLOR		0X0000		//δѡ�е�list������ɫ
#define FLBOX_DFT_RIMCOLOR		0XF800		//�߿���ɫ
#define FLBOX_DFT_PTHINFBKCOLOR	0XCE7E		//·��/��Ϣ���ı�����ɫ


//filelistbox�ļ��洢��ȶ���,���������������,�����ټ�����һ����ѡ��Ŀ��λ��.
#define FLBOX_PATH_DEPTH		0X0A		//�ļ�·�����
//filelistbox�����ļ�����������Ŀ���ļ�������Ŀ
#define FLBOX_MAX_DFFILE 		2000		//ʹ��12*12������ 2K��,�������ļ��е�ʱ��,��Ҫռ��2K*2���ڴ�

//�̶���߶ȶ���
#define FLBOX_SCB_WIDTH 		0X0E		//���������Ϊ15������
#define FLBOX_ICO_HEIGHT		0X10		//icoͼ��ĸ߶�
#define FLBOX_ICO_WIDTH			0X10		//icoͼ��Ŀ��
#define FLBOX_PATH_HEIGHT		0X14		//��ʾ·��������߶�(�����)
#define FLBOX_ITEM_HEIGHT		0X14		//ÿ����Ŀ��ռ�߶�,Ĭ��20
#define FLBOX_FINFO_HEIGHT		0X14		//�ײ���Ϣ��ʾ���ĸ߶�,Ĭ��20


#define FLBOX_PTHINF_FONT 		0X0C		//ʹ��12*12������


//filelist�ṹ��.����ṹ
__packed typedef struct 
{
	void * prevlist;
	void * nextlist;
	u8 type;		//�ļ�����
	u8 *name;		//�ļ�����
}_filelistbox_list;

//filelistbox�ṹ�嶨��		  
__packed typedef struct 
{
	u16 top; 				  		//filelistbox��������
	u16 left;                       //filelistbox�������
	u16 width; 				  		//���
	u16 height;						//�߶� ����Ϊ12/16�ı���,����б߿�,�ڱ��������ϼ�2

	u8 type;						//���ͱ����
									//[bit7]:1,��Ҫ������������(������totalitems>itemsperpage);0,����Ҫ������.(��λ������Զ�����)
									//[bit6:0]:����					 
								
	u8 sta;							//filelistbox״̬,[bit7]:������־;[bit6]:�����Ч�ı�־;[bit5:0]:��һ�ΰ��µı��.	 
	u8 id;							//filelistbox ��id
	u8 dbclick;					   	//˫��,
									//[7]:0,û��˫��.1,��˫��.
									//[0]:0,��Ŀ���ļ�(���ļ���/����).1,Ŀ���ļ�(���ļ���/����)

	u8 font;						//�������� 12/16
	u16 selindex;					//ѡ�е����� 0<selindex<foldercnt+filecnt;

	u16 foldercnt;					//�ļ�����Ŀ
	u16 filecnt;					//�ļ���Ŀ
 
	u16 lbkcolor;					//�ڲ�������ɫ
 	u16 lnselcolor; 				//list name ѡ�к����ɫ
  	u16 lnselbkcolor; 				//list name ѡ�к�ı�����ɫ
	u16 lncolor; 					//list name δѡ�е���ɫ
	u16 rimcolor;					//�߿���ɫ
	u16 pthinfbkcolor;			    //·��/��Ϣ���ı�����ɫ

	u8* path;						//��Ŀ¼·��
	//u8  seltblindex;				//��ǰ��Ŀ¼����
	u16 seltbl[FLBOX_PATH_DEPTH];	//ѡ�е���Ŀ�б�,FLBOX_PATH_DEPTHΪ���
	u16 fliter;						//�ļ�������,�������
									//ˢѡ�������Ķ���
	u16 fliterex;					//�ļ���չ�Ĺ�����
									//����ѡ����������С��,���֧��16�ֲ�ͬ���ļ�.Ĭ��0XFFFF,������С�����ˢѡ.

//////////////////////
	u8 *fname;						//��ǰѡ�е�index������
	u16 namelen;					//name��ռ�ĵ���.
	u16 curnamepos;					//��ǰ��ƫ��
	u32 oldtime;					//��һ�θ���ʱ��

    //����ֻ��Ŀ���ļ����(�����ļ���)
 	u16 *findextbl;    				//Ŀ���ļ���������	

	_scrollbar_obj * scbv;			//��ֱ������
	_filelistbox_list *list; 		//����	    
}_filelistbox_obj;


_filelistbox_obj * filelistbox_creat(u16 left,u16 top,u16 width,u16 height,u8 type,u8 font);//����filelistbox
_filelistbox_list * filelist_search(_filelistbox_list *filelistx,u16 index);		//���ұ��Ϊindex��list,��ȡ����ϸ��Ϣ
void filelist_delete(_filelistbox_obj * filelistbox);						//ɾ��filelist����
void filelistbox_delete(_filelistbox_obj *filelistbox_del);	 				//ɾ��filelistbox
u8 filelistbox_rebuild_filelist(_filelistbox_obj * filelistbox);			//�ؽ�filelistbox
u8 filelistbox_check(_filelistbox_obj * filelistbox,void * in_key);			//���filelistbox�İ���״̬
u8 filelistbox_addlist(_filelistbox_obj * filelistbox,u8 *name,u8 type);	//����һ��filelist
void filelistbox_draw_list(_filelistbox_obj *filelistbox);					//��filelist
void filelistbox_draw_listbox(_filelistbox_obj *filelistbox);				//��filelistbox
void filelistbox_show_path(_filelistbox_obj *filelistbox); 					//��ʾ·��
void filelistbox_show_info(_filelistbox_obj *filelistbox);
u8   filelistbox_scan_filelist(_filelistbox_obj *filelistbox);
void filelistbox_check_filelist(_filelistbox_obj *filelistbox);
void filelistbox_back(_filelistbox_obj *filelistbox);
void filelistbox_2click_hook(_filelistbox_obj *filelistbox);				//˫�����Ӻ���
#endif





