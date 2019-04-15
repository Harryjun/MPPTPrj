#include "guix.h"
#include "touch.h"
#include "text.h"	
#include "icos.h"												   
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

//�˴��������ⲿ����asc2_1206��asc2_1608;
//#include "font.h" 
extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
////////////////////////////GUIͨ���ַ�����//////////////////////////////
//ȷ��
const u8* GUI_OK_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"ȷ��","�_��","OK",
};
//ѡ��
const u8* GUI_OPTION_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"ѡ��","�x�","MENU",
};
//����
const u8* GUI_BACK_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"����","����","BACK",
};
//ȡ�� 
const u8* GUI_CANCEL_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"ȡ��","ȡ��","CANCEL",
};
//�˳�
const u8* GUI_QUIT_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
	"�˳�","�˳�","QUIT",		    
};
///////////////////////////////////////////////////////////////////////////
	   
//����ӿ�
_in_obj in_obj=
{
 	gui_get_key,		//��ȡ��ֵ����
	0,					//����
	0,			
	0,					//������ֵ
	IN_TYPE_ERR,		//��������,Ĭ�Ͼ��Ǵ����״̬
};
_gui_phy gui_phy;		//gui�����ӿ�
//////////////////////////ͳһ��׼����//////////////////////////////
//���㺯��
//x0,y0:����
//color:��ɫ
void _gui_draw_point(u16 x0,u16 y0,u16 color)
{
	u16 tempcolor=POINT_COLOR;
	POINT_COLOR=color;
	LCD_DrawPoint(x0,y0);
	POINT_COLOR=tempcolor;
}

////////////////////////////////////////////////////////////////////
//gui��ʼ��
void gui_init(void)
{
	gui_phy.read_point=LCD_ReadPoint;
	gui_phy.draw_point=LCD_Fast_DrawPoint;//���ٻ���
	gui_phy.fill=LCD_Fill;		 
	gui_phy.colorfill=LCD_Color_Fill;		 
	gui_phy.back_color=BACK_COLOR;		 
	gui_phy.lcdwidth=lcddev.width;
	gui_phy.lcdheight=lcddev.height; 
}   
//��ȡ��ֵ����
void gui_get_key(void* obj,u8 type)
{
	_m_tp_dev * tp_dev;		 
	switch(type)
	{
		case IN_TYPE_TOUCH:
			in_obj.intype=IN_TYPE_TOUCH;
			tp_dev=(_m_tp_dev*)obj;
		    in_obj.x=tp_dev->x[0];		 			//�õ���������
 		    in_obj.y=tp_dev->y[0];
			if(tp_dev->sta&0X80)in_obj.ksta|=0X01;	//����������
			else in_obj.ksta&=~(0X01);				//�����ɿ��� 
			break;
		case IN_TYPE_KEY:
			in_obj.keyval=(u32)obj;
			in_obj.intype=IN_TYPE_KEY;
			break;
		case IN_TYPE_JOYPAD:
			in_obj.intype=IN_TYPE_JOYPAD;
			//break;
		case IN_TYPE_MOUSE:
			in_obj.intype=IN_TYPE_MOUSE;
			//break;
		default:
			in_obj.intype=IN_TYPE_ERR;
			break;
		
	}	   
}
//��ɫת��
//rgb:32λ��ɫ
//����ֵ:16λɫ.
u16 gui_color_chg(u32 rgb)
{
	u16 r,g,b;
	r=rgb>>19;
	g=(rgb>>10)&0x3f;
	b=(rgb>>3)&0x1f;
	return ((r<<11)|(g<<5)|b);   
}
//��rgb332��ɫת��Ϊrgb565.
//rgb332:rgb332��ʽ����
//����ֵ:rgb565��ʽ����.
u16 gui_rgb332torgb565(u16 rgb332)
{
	u16 rgb565=0x18E7;
	rgb565|=(rgb332&0x03)<<3;
	rgb565|=(rgb332&0x1c)<<6;
	rgb565|=(rgb332&0xe0)<<8;
	return rgb565;
}
//m^n����
long long gui_pow(u8 m,u8 n)
{
	long long result=1;	 
	while(n--)result*=m;    
	return result;
}
//�������Ĳ�ľ���ֵ
u32 gui_disabs(u32 x1,u32 x2)
{
	return x1>x2 ? x1-x2:x2-x1;	    
}   
//��ָ���������alphablend
//x,y,width,height:��������
//color:alphablend����ɫ
//aval:͸����(0~32)
void gui_alphablend_area(u16 x,u16 y,u16 width,u16 height,u16 color,u8 aval)
{
	u16 i,j;
	u16 tempcolor;
	for(i=0;i<width;i++)
	{
		for(j=0;j<height;j++)
		{
			tempcolor=gui_phy.read_point(x+i,y+j);
			tempcolor=gui_alpha_blend565(tempcolor,color,aval);
			gui_phy.draw_point(x+i,y+j,tempcolor);
		}
	}
}

//����㺯��
//x0,y0:����
//color:��ɫ
//��(x0,y0)Ϊ����,��һ��9����Ĵ��
void gui_draw_bigpoint(u16 x0,u16 y0,u16 color)
{
	u16 i,j;
	u16 x,y;				    
	if(x0>=1)x=x0-1;
	else x=x0;
	if(y0>=1)y=y0-1;
	else y=y0;
	for(i=y;i<y0+2;i++)
	{
		for(j=x;j<x0+2;j++)gui_phy.draw_point(j,i,color);
	}  						   
}
//��������
//x0,y0:��� x1,y1:�յ�
//color:��ɫ
void gui_draw_line(u16 x0,u16 y0,u16 x1,u16 y1,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x1-x0; //������������ 
	delta_y=y1-y0; 
	uRow=x0; 
	uCol=y0; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		gui_phy.draw_point(uRow,uCol,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	} 				 
}
//��һ������
//(x1,y1),(x2,y2):��������ֹ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void gui_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return;
	if(size==0){gui_phy.draw_point(x1,y1,color);return;} 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		gui_fill_circle(uRow,uCol,size,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance){xerr-=distance;uRow+=incx;}  
		if(yerr>distance){yerr-=distance;uCol+=incy;}
	}  
} 
//����ֱ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_vline(u16 x0,u16 y0,u16 len,u16 color)
{
	if((len==0)||(x0>gui_phy.lcdwidth)||(y0>gui_phy.lcdheight))return;
	gui_phy.fill(x0,y0,x0,y0+len-1,color);	
}
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if((len==0)||(x0>gui_phy.lcdwidth)||(y0>gui_phy.lcdheight))return;
	gui_phy.fill(x0,y0,x0+len-1,y0,color);	
}
//���һ��ɫ��.
//x0,y0,width,height:���꼰���ߴ�
//ctbl:��ɫ��,�������ʱwidth,�������ʱ��СΪheight
//mode:���ģʽ(����:������,���ϵ������;����:���ϵ���,���������)
//[7:1]:����
//[bit0]:0,����;1,����
void gui_fill_colorblock(u16 x0,u16 y0,u16 width,u16 height,u16* ctbl,u8 mode)
{
	u16 i;
	if(height==0||width==0)return;
	if(mode&0x01)//����
	{
		for(i=0;i<width;i++)
		{
			gui_draw_vline(x0+i,y0,height-1,ctbl[i]);	
		}
	}else
	{
		for(i=0;i<height;i++)
		{
			gui_draw_hline(x0,y0+i,width-1,ctbl[i]);	
		}
	}
}   

//��һ����ɫƽ�����ɵ�����һ����ɫ
//srgb,ergb:��ʼ��ɫ����ֹ��ɫ.
//cbuf:��ɫ������(��len)
//len:���ɼ���(�����������ɫֵ����)
void gui_smooth_color(u32 srgb,u32 ergb,u16*cbuf,u16 len)
{
	u8 sr,sg,sb;
	short dr,dg,db;
	u16 r,g,b;	  
	u16 i=0;
 	sr=srgb>>16;
	sg=(srgb>>8)&0XFF;
	sb=srgb&0XFF;
	dr=(ergb>>16)-sr;
	dg=((ergb>>8)&0XFF)-sg;
	db=(ergb&0XFF)-sb;
 	for(i=0;i<len;i++)
	{
	 	r=sr+(dr*i)/len;
 	 	g=sg+(dg*i)/len;
	 	b=sb+(db*i)/len;
		r>>=3;
		g>>=2;
		b>>=3;
	    cbuf[i]=((r<<11)|(g<<5)|b);		   
	}
}
//��һ��ƽ�����ɵĲ�ɫ����(����)
//x,y,width,height:���ε�����ߴ�
//srgb,ergb:��ʼ����ֹ����ɫ
void gui_draw_smooth_rectangle(u16 x,u16 y,u16 width,u16 height,u32 srgb,u32 ergb)
{
	u16 i,j;
	u16 *colortbl=NULL;
	colortbl=(u16*)gui_memin_malloc(width*2);//�����ڴ�
	if(colortbl==NULL)return ;//�ڴ�����ʧ��
	gui_smooth_color(srgb,ergb,colortbl,width);//�����ɫ��
 	for(i=0;i<width;i++)
	{	 
		for(j=0;j<height;j++)
		{
			gui_phy.draw_point(x+i,y+j,colortbl[i]);//����
		}
	}
	gui_memin_free(colortbl);
}
//������
//x0,y0:���ε����Ͻ�����
//width,height:���εĳߴ�
//color:��ɫ
void gui_draw_rectangle(u16 x0,u16 y0,u16 width,u16 height,u16 color)
{
	gui_draw_hline(x0,y0,width,color);
	gui_draw_hline(x0,y0+height-1,width,color);
	gui_draw_vline(x0,y0,height,color);
	gui_draw_vline(x0+width-1,y0,height,color);
}
//��Բ�Ǿ���/���Բ�Ǿ���
//x,y,width,height:Բ�Ǿ��ε�λ�úͳߴ�
//r:Բ�ǵİ뾶.
//mode:0,�����ο�;1,������.
//upcolor:�ϰ벿����ɫ
//downcolor:�°벿����ɫ
void gui_draw_arcrectangle(u16 x,u16 y,u16 width,u16 height,u8 r,u8 mode,u16 upcolor,u16 downcolor)
{
	u16 btnxh=0;
	if(height%2)btnxh=height+1;//��ż������
	else btnxh=height;
	if(mode)//���
	{
 		gui_fill_rectangle(x+r,y,width-2*r,btnxh/2,upcolor);			//����
  		gui_fill_rectangle(x+r,y+btnxh/2,width-2*r,btnxh/2,downcolor);	//����
		gui_fill_rectangle(x,y+r,r,btnxh/2-r,upcolor);					//����
		gui_fill_rectangle(x,y+btnxh/2,r,btnxh/2-r,downcolor);			//����
		gui_fill_rectangle(x+width-r,y+r,r,btnxh/2-r,upcolor);			//����
		gui_fill_rectangle(x+width-r,y+btnxh/2,r,btnxh/2-r,downcolor);	//����
	}else
	{
		gui_draw_hline (x+r,y,width-2*r,upcolor);					//��		   
		gui_draw_hline (x+r,y+btnxh-1,width-2*r,downcolor);			//��		   
		gui_draw_vline (x,y+r,btnxh/2-r,upcolor);					//����		   
		gui_draw_vline (x,y+btnxh/2,btnxh/2-r,downcolor);			//����		   
		gui_draw_vline (x+width-1,y+r,btnxh/2-r,upcolor);			//����
		gui_draw_vline (x+width-1,y+btnxh/2,btnxh/2-r,downcolor);	//����				   
	}
	gui_draw_arc(x,y,x+r,y+r,x+r,y+r,r,upcolor,mode);//����	
	gui_draw_arc(x,y+btnxh-r,x+r,y+btnxh-1,x+r,y+btnxh-r-1,r,downcolor,mode);//����	
	gui_draw_arc(x+width-r,y,x+width,y+r,x+width-r-1,y+r,r,upcolor,mode);	 //����
	gui_draw_arc(x+width-r,y+btnxh-r,x+width,y+btnxh-1,x+width-r-1,y+btnxh-r-1,r,downcolor,mode);//����
}
//��ico
//x0,y0:���ε����Ͻ�����
//index:icos���
void gui_draw_icos(u16 x,u16 y,u8 index)
{       
	u16 *colorbuf;
	u16 i=0;
	colorbuf=gui_memin_malloc(2*256);//�����ڴ�
	if(colorbuf)
	{
		for(i=0;i<256;i++)colorbuf[i]=gui_rgb332torgb565(icostbl[index][i]);//RGB332ת��ΪRGB565;
 		gui_phy.colorfill(x,y,x+15,y+15,colorbuf);//����ICOS
 	} 
	gui_memin_free(colorbuf);//�ͷ��ڴ�  	 
	  	   
} 
//������͸����ICO,����16*16��С��.
//x0,y0:���ε����Ͻ�����
//index:icos���
void gui_draw_icosalpha(u16 x,u16 y,u8 index)
{          
	u16 i,j;
	u8 *icoaddr=(u8*)pathico[index];
	u16 color;
	u8 alphabend;
	for(i=y;i<y+16;i++)
	{
		for(j=x;j<x+16;j++)
		{
			color=(*icoaddr++)>>3;		   		 	//B
			color+=((u16)(*icoaddr++)<<3)&0X07E0;	//G
			color+=(((u16)*icoaddr++)<<8)&0XF800;	//R
			alphabend=*icoaddr++;					//ALPHAͨ��
			if(alphabend==0)//ֻ��͸������ɫ���л�ͼ
			{
				if(color)gui_phy.draw_point(j,i,color);
 			}
		}
 	}   
}
//������
//x0,y0:���ε����Ͻ�����
//width,height:���εĳߴ�
//color:��ɫ
void gui_fill_rectangle(u16 x0,u16 y0,u16 width,u16 height,u16 color)
{	  							   
	if(width==0||height==0)return;//�Ƿ�.	 
	gui_phy.fill(x0,y0,x0+width-1,y0+height-1,color);	   	   
}
//��ʵ��Բ
//x0,y0:����
//r�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax) 
		{
			// draw lines from outside  
			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}
//�������ڻ���
//(sx,sy),(ex,ey):�趨����ʾ��Χ
//(x,y):������
//color:��ɫ
void gui_draw_expoint(u16 sx,u16 sy,u16 ex,u16 ey,u16 x,u16 y,u16 color)	    
{
	u16 tempcolor;
	if(x<=ex&&x>=sx&&y<=ey&&y>=sy)
	{
		tempcolor=POINT_COLOR;
		POINT_COLOR=color;
		LCD_DrawPoint(x,y);	    
		POINT_COLOR=tempcolor;
	}
}
//��8��(Bresenham�㷨)		  
//(sx,sy),(ex,ey):�趨����ʾ��Χ
//(rx,ry,a,b):����
//color:��ɫ
void gui_draw_circle8(u16 sx,u16 sy,u16 ex,u16 ey,u16 rx,u16 ry,int a,int b,u16 color)
{
	gui_draw_expoint(sx,sy,ex,ey,rx+a,ry-b,color);              
	gui_draw_expoint(sx,sy,ex,ey,rx+b,ry-a,color);                       
	gui_draw_expoint(sx,sy,ex,ey,rx+b,ry+a,color);                           
	gui_draw_expoint(sx,sy,ex,ey,rx+a,ry+b,color);             
	gui_draw_expoint(sx,sy,ex,ey,rx-a,ry+b,color);                  
	gui_draw_expoint(sx,sy,ex,ey,rx-b,ry+a,color);               
	gui_draw_expoint(sx,sy,ex,ey,rx-b,ry-a,color);                      
	gui_draw_expoint(sx,sy,ex,ey,rx-a,ry-b,color);                  
}	    
//��ָ��λ�û�һ��ָ����С��Բ
//(rx,ry):Բ��
//(sx,sy),(ex,ey):�趨����ʾ��Χ
//r    :�뾶
//color:��ɫ
//mode :0,�����;1,���
void gui_draw_arc(u16 sx,u16 sy,u16 ex,u16 ey,u16 rx,u16 ry,u16 r,u16 color,u8 mode)
{
	int a,b,c;
	int di;	  
	a=0;b=r;	  
	di=3-(r<<1);	//�ж��¸���λ�õı�־
	while(a<=b)
	{
		if(mode)for(c=a;c<=b;c++)gui_draw_circle8(sx,sy,ex,ey,rx,ry,a,c,color);//��ʵ��Բ
 		else gui_draw_circle8(sx,sy,ex,ey,rx,ry,a,b,color);					   //������Բ
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 							  
	}
}
//����Բ
//x0,y0:����
//rx:x����뾶
//ry:y����뾶
//color:��Բ����ɫ
void gui_draw_ellipse(u16 x0, u16 y0, u16 rx, u16 ry,u16 color) 
{
	u32 OutConst, Sum, SumY;
	u16 x,y;
	u16 xOld;
	u32 _rx = rx;
	u32 _ry = ry;
	if(rx>x0||ry>y0)return;//�Ƿ�.
	OutConst = _rx*_rx*_ry*_ry+(_rx*_rx*_ry>>1);  // Constant as explaint above 
	// To compensate for rounding  
	xOld = x = rx;
	for (y=0; y<=ry; y++) 
	{
		if (y==ry)x=0; 
		else 
		{
			SumY =((u32)(rx*rx))*((u32)(y*y)); // Does not change in loop  
			while(Sum = SumY + ((u32)(ry*ry))*((u32)(x*x)),(x>0) && (Sum>OutConst)) x--;
		}
		// Since we draw lines, we can not draw on the first iteration		    
		if (y) 
		{
			gui_draw_line(x0-xOld,y0-y+1,x0-x,y0-y,color);
			gui_draw_line(x0-xOld,y0+y-1,x0-x,y0+y,color);
			gui_draw_line(x0+xOld,y0-y+1,x0+x,y0-y,color);
			gui_draw_line(x0+xOld,y0+y-1,x0+x,y0+y,color);
		}
		xOld = x;
	}
}
//�����Բ
//x0,y0:����
//rx:x����뾶
//ry:y����뾶
//color:��Բ����ɫ
void gui_fill_ellipse(u16 x0,u16 y0,u16 rx,u16 ry,u16 color) 
{
	u32 OutConst, Sum, SumY;
	u16 x,y;
	u32 _rx = rx;
	u32 _ry = ry;
	OutConst = _rx*_rx*_ry*_ry+(_rx*_rx*_ry>>1);  // Constant as explaint above  
	// To compensate for rounding 
	x = rx;
	for (y=0; y<=ry; y++) 
	{
		SumY =((u32)(rx*rx))*((u32)(y*y)); // Does not change in loop  
		while (Sum = SumY + ((u32)(ry*ry))*((u32)(x*x)),(x>0) && (Sum>OutConst))x--;    
		gui_draw_hline(x0-x,y0+y,2*x,color);
		if(y)gui_draw_hline(x0-x,y0-y,2*x,color);    
	}
}
//����ALPHA BLENDING�㷨.
//src:Դ��ɫ
//dst:Ŀ����ɫ
//alpha:͸���̶�(0~32)
//����ֵ:��Ϻ����ɫ.
u16 gui_alpha_blend565(u16 src,u16 dst,u8 alpha)
{
	u32 src2;
	u32 dst2;	 
	//Convert to 32bit |-----GGGGGG-----RRRRR------BBBBB|
	src2=((src<<16)|src)&0x07E0F81F;
	dst2=((dst<<16)|dst)&0x07E0F81F;   
	//Perform blending R:G:B with alpha in range 0..32
	//Note that the reason that alpha may not exceed 32 is that there are only
	//5bits of space between each R:G:B value, any higher value will overflow
	//into the next component and deliver ugly result.
	dst2=((((dst2-src2)*alpha)>>5)+src2)&0x07E0F81F;
	return (dst2>>16)|dst2;  
}  	  
/////////////////////////////////////////////////////////////////////////////////////////////
 
//���м�λ����ʾһ���ַ���
//x,y,width,height:��ʾ��������.
//color:������ɫ.
//size:���ִ�С
//str:�ַ���
void gui_show_strmid(u16 x,u16 y,u16 width,u16 height,u16 color,u8 size,u8 *str)
{
	u16 xoff=0,yoff=0;
	u16 strlenth;
	u16 strwidth;
	if(str==NULL)return;
   	strlenth=strlen((const char*)str);	//�õ��ַ�������
	strwidth=strlenth*size/2;		//�ַ�����ʾռ�ÿ��
	if(height>size)yoff=(height-size)/2;
	if(strwidth<=width)//�ַ���û�������
 	{
		xoff=(width-strwidth)/2;	  
	}
	gui_show_ptstr(x+xoff,y+yoff,x+width-1,y+height-1,0,color,size,str,1);  
}

//��ָ����ַ��ʼ��ʾһ��ASCII�ַ�
//x,y:��ʾ��ʼ����.
//xend,yend:x,y ������յ�����
//offset:��ʼ��ʾ��ƫ��
//color:������ɫ.
//size:���ִ�С
//chr:�ַ�
//mode:0,�ǵ�����ʾ;1,������ʾ,2,������(ֻ�����ڵ���ģʽ)
void gui_show_ptchar(u16 x,u16 y,u16 xend,u16 yend,u8 offset,u16 color,u16 size,u8 chr,u8 mode)
{
    u8 temp;
    u8 pos,t;
	u8 tempoff;
	u16 y0=y;					     
	if(chr>' ')chr=chr-' ';//�õ�ƫ�ƺ��ֵ
	else chr=0;//С�ڿո��һ���ÿո����,����TAB��(��ֵΪ9)
	if(mode==0) //�ǵ��ӷ�ʽ
	{  
	    for(pos=0;pos<size;pos++)
	    {   
			if(size==12)temp=asc2_1206[chr][pos];  //����1206����
			else if(size==24)  temp=asc2_2412[chr][pos];
			else temp=asc2_1608[chr][pos];		 	//����1608����
				tempoff=offset;
			if(x>xend)return;						//�������� 	                          
	        for(t=0;t<8;t++)
			{			    
		        if(tempoff==0)
				{
					if(temp&0x80)gui_phy.draw_point(x,y,color);
					else gui_phy.draw_point(x,y,gui_phy.back_color);		 
				} 
				temp<<=1;
				y++;
				if((y-y0)==size||(y>yend))
				{
					y=y0;
					x++;  
					if(tempoff)tempoff--;
					break;
				}
			}  	   
		}	
	}else//���ӷ�ʽ(1,��ͨ����,2,������)
	{		   
	    for(pos=0;pos<size;pos++)
	    {   
			if(size==12)temp=asc2_1206[chr][pos];  //����1206����
			else if(size==16) temp=asc2_1608[chr][pos];		 	//����1608����
			else  temp=asc2_2412[chr][pos];
			tempoff=offset;
			if(x>xend)return;						//�������� 	                          
	        for(t=0;t<8;t++)
			{			    
		        if(tempoff==0)
				{
					if(temp&0x80)
					{
						if(mode==0X01)gui_phy.draw_point(x,y,color);
						else if(mode==0x02)gui_draw_bigpoint(x,y,color);	 
					}	 
				} 
				temp<<=1;
				y++;
				if((y-y0)==size||(y>yend))
				{
					y=y0;
					x++;  
					if(tempoff)tempoff--;
					break;
				}
			}  	   
		}	
	}						     
}
//��ʾlen������
//x,y :�������	 
//len :���ֵ�λ��
//color:��ɫ
//size:�����С
//num:��ֵ(0~2^64);
//mode:ģʽ.
//[7]:0,�����;1,���0.
//[3:0]:0,�ǵ�����ʾ;1,������ʾ.2,������(ֻ�����ڵ���ģʽ)
void gui_show_num(u16 x,u16 y,u8 len,u16 color,u8 size,long long num,u8 mode)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/gui_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)gui_show_ptchar(x+(size/2)*t,y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,size,'0',mode&0xf);//���0
				else gui_show_ptchar(x+(size/2)*t,y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,size,' ',mode&0xf);
 				continue;
			}else enshow=1; 
		 	 
		}
		gui_show_ptchar(x+(size/2)*t,y,gui_phy.lcdwidth,gui_phy.lcdheight,0,color,size,temp+'0',mode&0xf);
	}
}
//������תΪ�ַ���
//str:�ַ�����ŵ�ַ.
//num:���� 
u8* gui_num2str(u8*str,u32 num)
{
	u8 t,temp;
	u8 enrec=0;						   
	for(t=0;t<10;t++)
	{
		temp=(num/gui_pow(10,10-t-1))%10;
		if(enrec==0&&t<9)
		{
			if(temp==0)continue;//������Ҫ��¼
			else enrec=1; 		//��Ҫ��¼��.
		}
		*str=temp+'0';
		str++;
	}
	*str='\0';//���������.
	return str;
}


//��ָ����ַ��ʼ��ʾһ������
//x,y:��ʾ��ʼ����.
//xend,yend:x,y ������յ�����
//offset:��ʼ��ʾ��ƫ��
//color:������ɫ.
//size:���ִ�С
//chr:��������(���ֽ���ǰ,���ֽ��ں�)
//mode:0,�ǵ�����ʾ;1,������ʾ,2,������(ֻ�����ڵ���ģʽ)
void gui_show_ptfont(u16 x,u16 y,u16 xend,u16 yend,u8 offset,u16 color,u16 size,u8* chr,u8 mode)
{	 
    u8 temp;
    u8 pos,t;
	u8 tempoff;
	u16 y0=y;					   
	u8 dzk[72];
u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
	if(size!=12&&size!=16&&size!=24)return;	//��֧�ֵ�size
	Get_HzMat(chr,dzk,size);		//�õ���Ӧ��С�ĵ�������
	tempoff=offset;
	if(mode==0) 	//�ǵ��ӷ�ʽ
	{
		for(pos=0;pos<csize;pos++)
		{	
			if(x>xend)break;//�����յ�����
		    temp=dzk[pos];	//�õ���������                          
			if(tempoff==0)	//ƫ�Ƶ�ַ����
			{
		        for(t=0;t<8;t++)
				{		    
					if(y<=yend)
					{
						if(temp&0x80)gui_phy.draw_point(x,y,color); 
			 			else gui_phy.draw_point(x,y,gui_phy.back_color); 
					} 
					temp<<=1;
					y++;
					if((y-y0)==size)
					{
						y=y0;
						x++;
						break;
					}			    
				} 
			}else
			{
				y+=8;
				if((y-y0)>=size)//����һ���ֵĸ߶���
				{
					y=y0;		//y�������
 					tempoff--;   
				}
			}   
		}	
	}else		//���ӷ�ʽ
	{
		for(pos=0;pos<csize;pos++)
		{	
			if(x>xend)break;//�����յ�����
		    temp=dzk[pos];	//�õ���������                          
			if(tempoff==0)	//ƫ�Ƶ�ַ����
			{
		        for(t=0;t<8;t++)
				{		    
					if((y<=yend)&&(temp&0x80))
					{
						if(mode==0X01)gui_phy.draw_point(x,y,color);
						else if(mode==0x02)gui_draw_bigpoint(x,y,color);	 
					} 
					temp<<=1;
					y++;
					if((y-y0)==size)
					{
						y=y0;
						x++;
						break;
					}			    
				} 
			}else
			{
				y+=8;
				if((y-y0)>=size)//����һ���ֵĸ߶���
				{
					y=y0;		//y�������
 					tempoff--;   
				}
			}   
		}
	}						     
}   
//��ָ��λ�ÿ�ʼ,��ʾָ�����ȷ�Χ���ַ���.
//x,y:��ʾ��ʼ����.
//xend:x�������ֹ����
//offset:��ʼ��ʾ��ƫ��
//color:������ɫ.
//size:���ִ�С
//str:�ַ���
//mode:0,�ǵ�����ʾ;1,������ʾ,2,������(ֻ�����ڵ���ģʽ)
void gui_show_ptstr(u16 x,u16 y,u16 xend,u16 yend,u16 offset,u16 color,u8 size,u8 *str,u8 mode)
{
 	u8 bHz=0;     	//�ַ���������  
	u8 EnDisp=0;  	//ʹ����ʾ	    				    				  	  
    while(*str!=0)	//����δ����
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;	//���� 
	        else              	//�ַ�
	        {      	   
				if(EnDisp==0)	//��δʹ����ʾ
				{
					if(offset>=size/2)	//������һ���ַ�
					{
						offset-=size/2;	//����һ���ַ���ƫ��
					}else				//δ����һ���ַ���ƫ��
					{
						offset=offset%(size/2);	//�õ��ַ�ƫ����
						EnDisp=1;				//���Կ�ʼ��ʾ��
					}
				}
				if(EnDisp==1)	//ʹ����ʾ
				{
	 				gui_show_ptchar(x,y,xend,yend,offset,color,size,*str,mode);//��ʾһ���ַ�	 
			        if((xend-x)>size/2)x+=size/2;	//�ַ�,Ϊȫ�ֵ�һ��
					else x+=xend-x;					//δ��ȫ��ʾ
					if(offset)
					{
						x-=offset;
						offset=0;//���ƫ�� 
			        } 
				}
				if(x>=xend)return;//������,�˳� 
				str++; 
	        }
        }else//���� 
        {     
            bHz=0;//�к��ֿ� 
			if(EnDisp==0)//��δʹ��
			{
				if(offset>=size)//������һ����
				{
					offset-=size;//����
				}else
				{
					offset=offset%(size);//�õ��ַ�ƫ��
					EnDisp=1;//���Կ�ʼ��ʾ��
				}
			}	 	   
			if(EnDisp)
			{
 				gui_show_ptfont(x,y,xend,yend,offset,color,size,str,mode); //��ʾ�������,������ʾ 	 
				if((xend-x)>size)x+=size; //�ַ�,Ϊȫ�ֵ�һ��
				else x+=xend-x;//δ��ȫ��ʾ				
				if(offset)
				{
					x-=offset;
					offset=0;//���ƫ�� 
		        }   
			}  
			if(x>=xend)return;//������,�˳�  
	        str+=2;  
        }						 
    }    
}
//��ָ��λ�ÿ�ʼ,��ʾָ�����ȷ�Χ���ַ���.(������Ч��ʾ)
//x,y:��ʾ��ʼ����.
//width:�����ʾ���
//offset:��ʼ��ʾ��ƫ��
//color:������ɫ.
//rimcolor:���ֱߵ���ɫ
//size:���ִ�С
//str:�ַ���
void gui_show_ptstrwhiterim(u16 x,u16 y,u16 xend,u16 yend,u16 offset,u16 color,u16 rimcolor,u8 size,u8 *str)
{
	gui_show_ptstr(x,y,xend,yend,offset,rimcolor,size,str,2);	//�Ȼ���(�ô�ŵ㻭)
	gui_show_ptstr(x,y,xend,yend,offset,color,size,str,1);		//����(��С�㻭)
}
/*************************************************************************
�������ƣ�������Ч��
���ܽ��ܣ����ض�ѡ���м�����ּ�
���ߣ��������Ϸ
��дʱ�䣺2015.10.8
**************************************************************************/
void gui_show_str_center(u16 x,u16 y,u16 width,u16 height,u16 offset,u16 color,u16 rimcolor,u8 size,u8 *str)
{
	u16 xoff=0,yoff=0;
	u16 strlenth;
	u16 strwidth;
	if(str==NULL)return;
   	strlenth=strlen((const char*)str);	//�õ��ַ�������
	strwidth=strlenth*size/2;		//�ַ�����ʾռ�ÿ��
	if(height>size)yoff=(height-size)/2;
	if(strwidth<=width)//�ַ���û�������
 	{
		xoff=(width-strwidth)/2;	  
	}
	gui_show_ptstr(x+xoff,y+yoff,x+width,y+height,offset,rimcolor,size,str,2);	//�Ȼ���(�ô�ŵ㻭)
	gui_show_ptstr(x+xoff,y+yoff,x+width,y+height,offset,color,size,str,1);		//����(��С�㻭)
}

//��name���뵽path����.�Զ����\�ͽ�����.����Ҫȷ��pname���ڴ�ռ��㹻,������ܵ�������.
//pname:��·�����ļ���.path���ڴ��С����Ϊ:strlen(path)+strlen(name)+2!
//path:·��
//name:����
//����ֵ:��·�����ļ���
u8* gui_path_name(u8 *pname,u8* path,u8 *name)
{
	const u8 chgchar[2]={0X5C,0X00};//ת��� ��Ч"\"    
	strcpy((char *)pname,(const char*)path);	//����path��pname����
	strcat((char*)pname,(const char*)chgchar);	//���ת���
	strcat((char*)pname,(const char*)name);		//�������������
	return pname;
}
/////////////////////////////////////////////////////////////////////////////////////////
//�õ��ַ�����ռ������
//str:�ַ���ָ��;
//linelenth:ÿ�еĳ���
//font:�����С
//����ֵ:�ַ�����ռ������
u32 gui_get_stringline(u8*str,u16 linelenth,u8 font)
{
	u16 xpos=0;
	u32	lincnt=1;//���پ���1��.
	if(linelenth<font/2)return 0XFFFFFFFF;//�޷�ͳ�����
	while(*str!='\0'&&lincnt!=0xffffffff)
	{										   
		if(*str==0x0D&&(*(str+1)==0X0A))//�ǻس���
		{
			str+=2;	 
			lincnt++;//������1
			xpos=0;
		}else if(*str>=0X81&&(*(str+1)>=0X40))//��gbk����
		{
			xpos+=font;
			str+=2;
			if(xpos>linelenth)//�Ѿ����ڱ���֮��
			{
				xpos=font;
				lincnt++;//������1
			}
		}else//���ַ�
		{
			xpos+=font/2;
			str+=1;
			if(xpos>linelenth)//�Ѿ����ڱ���֮��
			{
				xpos=font/2;
				lincnt++;//������1
			}				
		}	
	}
	return lincnt;
}
//���趨��������ʾ�ַ���,�������������ʾ
//str:�ַ���ָ��;
//x,y,width,height:��ʾ����
//font:�����С
//fcolor:������ɫ,�Ե��ӷ�ʽд��.
void gui_show_string(u8*str,u16 x,u16 y,u16 width,u16 height,u8 font,u16 fcolor)
{
	u16 xpos=x;
	u16 ypos=y;
	u16 endx=x+width-1;
	u16 endy=y+height-1;
 	if(width<font/2)return ;//�޷���ʾ���
	while(*str!='\0')//δ����
	{										   
		if(*str==0x0D&&(*(str+1)==0X0A))//�ǻس���
		{
			str+=2;	 
 			xpos=x;
			ypos+=font;//y����16
		}else if(*str>=0X81&&(*(str+1)>=0X40))//��gbk����
		{
			if((xpos+font)>(endx+1))//�Ѿ����ڱ���֮��
			{
				xpos=x;
				ypos+=font;
 			}
			gui_show_ptfont(xpos,ypos,endx,endy,0,fcolor,font,str,1);
			xpos+=font;//ƫ��
			str+=2;

		}else//���ַ�
		{
			if((xpos+font/2)>(endx+1))
			{
				xpos=x;
				ypos+=font;
			}	
			gui_show_ptchar(xpos,ypos,endx,endy,0,fcolor,font,*str,1);		   
			xpos+=font/2;
			str+=1;	 			
		}
		if(ypos>endy)break;//��������ʾ������.	
	}	  
}	
//�ɴ˴���ʼ��ǰѰ��gbk��(������0x80���ֽ�)�ĸ���
//str:�ַ���
//pos:��ʼ���ҵĵ�ַ
//����ֵ:gbk�������
u16 gui_string_forwardgbk_count(u8 *str,u16 pos)
{		   
	u16 t=0;
	while(str[pos]>0x80)
	{
		t++;
		if(pos==0)break;
		pos--;
	}
	return t;
}
/////////////////////////////////////////////////////////////////////////////////////////
//�ڴ渴�ƺ���(��Ϊ��ϵͳmemset�����ᵼ��Ī��������,��ȫ�������������)
//*p:�ڴ��׵�ַ
//c:Ҫͳһ���õ�ֵ
//len:���ó���
void gui_memset(void *p,u8 c,u32 len)
{
	u8 *pt=(u8*)p;
	while(len)
	{
		*pt=c;
		pt++;
		len--;		
	}
}
//�ڲ��ڴ���� 
//size:Ҫ������ڴ��С(u8Ϊ��λ)
//����ֵ:NULL:����ʧ��;����,�ڴ��ַ
void *gui_memin_malloc(u32 size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//�ڲ��ڴ��ͷ�
void gui_memin_free(void* ptr)		 
{
	myfree(SRAMIN,ptr);
}
//�ⲿ�ڴ���� 
//size:Ҫ������ڴ��С(u8Ϊ��λ)
//����ֵ:NULL:����ʧ��;����,�ڴ��ַ
void *gui_memex_malloc(u32 size)			
{
	return (void*)mymalloc(SRAMEX,size);
}  
//�ⲿ�ڴ��ͷ�
void gui_memex_free(void* ptr)		 
{
	myfree(SRAMEX,ptr);
}
//copy ptr��ǰsize���ֽڵ��µ�ַ,�������µ�ֵַ
//ptr:Ҫcopy��Դ��ַ
//size:�µ�ַ�ڴ�Ĵ�С
//����ֵ:Ŀ�ĵ�ַ
void *gui_memin_realloc(void *ptr,u32 size)
{
	return myrealloc(SRAMIN,ptr,size);  
}		 





















