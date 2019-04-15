/*****************************************************
例程名称：触控屏幕按键
作者：刁钻的游戏
时间：2015.10.7
操作方法：
1创建按键控件在文件首部
2在文件底部对按键属性设置menu_button函数里面
属性选择包括几种模式
3获取键值函数

******************************************************/

#include "button.h"
#include "piclib.h"

_btn_obj* button;
_btn_obj* button1;
_btn_obj* button2;
_btn_obj* button3;

//读取背景色
//btnx:按钮指针
void btn_read_backcolor(_btn_obj * btnx)
{
	u16 x,y,ccnt;
	ccnt=0;
	for(y=btnx->top;y<btnx->top+btnx->height;y++)
	{
		for(x=btnx->left;x<btnx->left+btnx->width;x++)
		{
			btnx->bkctbl[ccnt]=gui_phy.read_point(x,y);//读取颜色
			ccnt++;
		}
	}

}
//恢复背景色
//btnx:按钮指针
void btn_recover_backcolor(_btn_obj * btnx)
{
	u16 x,y,ccnt;
	ccnt=0;	    
	for(y=btnx->top;y<btnx->top+btnx->height;y++)
	{
		for(x=btnx->left;x<btnx->left+btnx->width;x++)
		{
			gui_phy.draw_point(x,y,btnx->bkctbl[ccnt]);//画点	
			ccnt++;
		}
	}  
}

//创建按钮
//top,left,width,height:坐标.
//id:按键ID
//type:按键类型
//[7]:0,模式A,按下是一种状态,松开是一种状态.
//	  1,模式B,每按下一次,状态改变一次.按一下按下,再按一下弹起.
//[6:4]:保留1000 0000
//[3:0]:0,标准按钮;1,图片按钮;2,边角按钮;3,文字按钮(背景透明);4,文字按钮(背景单一)
_btn_obj * btn_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type)
{
 	_btn_obj * btn_crt;
	if(width<6||height<6)return NULL;//尺寸不能太小		  
	btn_crt=(_btn_obj*)gui_memin_malloc(sizeof(_btn_obj));//分配内存
	if(btn_crt==NULL)return NULL;//内存分配不够.
	btn_crt->top=top;
	btn_crt->left=left;
	btn_crt->width=width;
	btn_crt->height=height;
	btn_crt->id=id;
	btn_crt->type=type;
	btn_crt->sta=0;
	btn_crt->caption=0;
	btn_crt->font=16;					//默认为16字体
	btn_crt->arcbtnr=3;					//默认圆角的半径
	btn_crt->bcfucolor=BTN_DFU_BCFUC;	//默认松开的颜色
	btn_crt->bcfdcolor=BTN_DFU_BCFDC;	//默认按下的颜色
	btn_crt->picbtnpathu=NULL;//默认路径为空
	btn_crt->picbtnpathd=NULL;//默认路径为空   
	if((type&0X0f)>=BTN_TYPE_ANG)//边角按钮/文字按钮
	{
		switch(type&0X0f)
		{
			case BTN_TYPE_ANG://边角按钮
				btn_crt->bkctbl=(u16*)gui_memin_malloc(4*2);//分配8个字节
		 		if(btn_crt->bkctbl==NULL)
				{
					btn_delete(btn_crt);//释放之前申请的内存
					return NULL;		//内存分配不够.
				}
				btn_crt->bkctbl[0]=ARC_BTN_RIMC;//边框颜色
				btn_crt->bkctbl[1]=ARC_BTN_TP1C;//第一行的颜色				
				btn_crt->bkctbl[2]=ARC_BTN_UPHC;//上半部分颜色
				btn_crt->bkctbl[3]=ARC_BTN_DNHC;//下半部分颜色				
				break;
			case BTN_TYPE_TEXTA://背景透明的文字按钮
				btn_crt->bkctbl=(u16*)gui_memin_malloc(width*height*2);//要分配完整的背景色表
		 		if(btn_crt->bkctbl==NULL)
				{
					btn_delete(btn_crt);//释放之前申请的内存
					return NULL;		//内存分配不够.
				}
				btn_read_backcolor(btn_crt);//读取全部背景色
				break;
			case BTN_TYPE_TEXTB://背景单一的文字按钮
				btn_crt->bkctbl=(u16*)gui_memin_malloc(2*2);//分配4个字节
		 		if(btn_crt->bkctbl==NULL)
				{
					btn_delete(btn_crt);//释放之前申请的内存
					return NULL;		//内存分配不够.
				}
				btn_crt->bkctbl[0]=0XFFFF;//默认松开为白色
				btn_crt->bkctbl[1]=0X0000;//默认按下为黑色				
				break;
		} 
	}else btn_crt->bkctbl=NULL;//对标准按钮和图片按钮这个无效.
    return btn_crt;
}
//删除按钮
//btn_del:要删除的按键
void btn_delete(_btn_obj * btn_del)
{
	if(btn_del==NULL)return;//非法的地址,直接退出
	if((btn_del->type&0X0f)==BTN_TYPE_TEXTA)btn_recover_backcolor(btn_del);//恢复背景色
	gui_memin_free(btn_del->bkctbl);
	gui_memin_free(btn_del);
}
//检查	
//btnx:按键指针
//in_key:输入按键指针
//返回值:
//0,没有一次有效的操作	
//1,有一次有效操作		 	
u8 btn_check(_btn_obj * btnx,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
	u8 sta;
	u8 btnok=0;
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//??????
			if(btnx->top<key->y&&key->y<(btnx->top+btnx->height)&&btnx->left<key->x&&key->x<(btnx->left+btnx->width))//?????
			{
				if((btnx->sta&0X80)==0)	//?????????
				{
					btnx->sta|=1<<6;	//????????
				 	if(btnx->type&0X80)	//??B
					{
						sta=btnx->sta&0X03;
						btnx->sta&=~(0X03); 
						if(sta==BTN_PRESS)btnx->sta|=BTN_RELEASE;//??
						else btnx->sta|=BTN_PRESS;//??	  
					}else  				//??A
					{
						btnx->sta&=~(0X03);
						btnx->sta|=BTN_PRESS;//??
					}
					btn_draw(btnx);//???  
					btnok=1;
					delay_ms(2000);
 				}
				
				btnx->sta|=0X80;//????		   
			}else 
			{
				
				btnx->sta&=0X7F;//????
				if((btnx->sta&0x03)==BTN_PRESS)		//????????
				{
					if(key->ksta&0x01)				//??????????,??!!
					{
	 					btnx->sta=0;					
						btn_draw(btnx);				//???? 
					}else if((btnx->type&0X80)==0)	//??????
					{
						btnx->sta&=~(0X03);
						btnx->sta|=BTN_RELEASE;	//??
						btn_draw(btnx);			//??? 
						btnok=1;  
					}
				}
			}
			break;
		case IN_TYPE_KEY:	//????
			break;
		case IN_TYPE_JOYPAD://????
			break;
		case IN_TYPE_MOUSE:	//????
			break;
		default:
			break;
	}
	return btnok;
}
/*
u8 btn_check(_btn_obj * btnx,void * in_key)
{
	_in_obj *key=(_in_obj*)in_key;
	u8 sta;
	u8 btnok=0;
	switch(key->intype)
	{
		case IN_TYPE_TOUCH:	//触摸屏按下了
			if(btnx->top<key->y&&key->y<(btnx->top+btnx->height)&&btnx->left<key->x&&key->x<(btnx->left+btnx->width))//在按键内部
			{
				if((btnx->sta&0X80)==0)	//上次输入状态为松开
				{
					btnx->sta|=1<<6;	//标记此次按键有效
				 	if(btnx->type&0X80)	//模式B
					{
						sta=btnx->sta&0X03;
						btnx->sta&=~(0X03); 
						if(sta==BTN_PRESS)btnx->sta|=BTN_RELEASE;//松开
						else btnx->sta|=BTN_PRESS;//按下	  
					}else  				//模式A
					{
						btnx->sta&=~(0X03);
						//btnx->sta|=BTN_PRESS;//按下
					}
					btn_draw(btnx);//画按钮  
					btnok=1;
					delay_ms(200);
 				}
				else
					btnx->sta|=0X80;//标记按下		   
				//btn_draw(btnx);//画按钮  
				//	btnok=1;
				//delay_ms(200);
			}else 
			{
				//btnx->sta&=0X7F;//标记松开
				
				if((btnx->sta&0x03)==BTN_PRESS)		//之前有按键按下了
				{
					if(key->ksta&0x01)				//此时按键还是按下状态,无效!!
					{
	 					btnx->sta=0;					
						btn_draw(btnx);				//恢复按钮 
					}else if((btnx->type&0X80)==0)	//按键正常松开
					{
						btnx->sta&=~(0X03);
						btnx->sta|=BTN_RELEASE;	//松开
						btn_draw(btnx);			//画按钮 
						btnok=1;  
					}
				}
			}
			break;
		case IN_TYPE_KEY:	//按键数据
			break;
		case IN_TYPE_JOYPAD://手柄数据
			break;
		case IN_TYPE_MOUSE:	//鼠标数据
			break;
		default:
			break;
	}
	return btnok;
}
*/
//画标准按钮
//btnx:按钮
void btn_draw_stdbtn(_btn_obj * btnx)
{
	u8 sta;
	sta=btnx->sta&0x03;
	gui_fill_rectangle(btnx->left+2,btnx->top+2,btnx->width-4,btnx->height-4,STD_BTN_INSC);//填充内部
	switch(sta)
	{
		case BTN_RELEASE://松开
		case BTN_INACTIVE://未激活
			gui_draw_vline(btnx->left,btnx->top,btnx->height,STD_BTN_LTOC);			//左外线
		 	gui_draw_hline(btnx->left,btnx->top,btnx->width,STD_BTN_LTOC);			//上外线
			gui_draw_vline(btnx->left+1,btnx->top+1,btnx->height-2,STD_BTN_LTIC);	//左内线
		 	gui_draw_hline(btnx->left+1,btnx->top+1,btnx->width-2,STD_BTN_LTIC);	//上内线
		
			gui_draw_vline(btnx->left+btnx->width-1,btnx->top,btnx->height,STD_BTN_RBOC);		//右外线
		 	gui_draw_hline(btnx->left,btnx->top+btnx->height-1,btnx->width,STD_BTN_RBOC);		//下外线
			gui_draw_vline(btnx->left+btnx->width-2,btnx->top+1,btnx->height-2,STD_BTN_RBIC);	//右内线
		 	gui_draw_hline(btnx->left+1,btnx->top+btnx->height-2,btnx->width-2,STD_BTN_RBIC);	//右内线
 			if(sta==0)gui_show_strmid(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bcfucolor,btnx->font,btnx->caption);//显示caption
			else//未激活
			{
				gui_show_strmid(btnx->left+1,btnx->top+1,btnx->width,btnx->height,STD_BTN_DFRC,btnx->font,btnx->caption);
				gui_show_strmid(btnx->left,btnx->top,btnx->width,btnx->height,STD_BTN_DFLC,btnx->font,btnx->caption);
 			}
			break;
		case BTN_PRESS://按下
		  	gui_draw_rectangle(btnx->left,btnx->top,btnx->width,btnx->height,GUI_COLOR_BLACK);		//外框
		  	gui_draw_rectangle(btnx->left+1,btnx->top+1,btnx->width-2,btnx->height-2,STD_BTN_RBOC);	//内框
 			gui_show_strmid(btnx->left+1,btnx->top+1,btnx->width,btnx->height,btnx->bcfdcolor,btnx->font,btnx->caption);//显示caption
			break;
	}	    		
} 
//画图片按钮
//btnx:按钮
void btn_draw_picbtn(_btn_obj * btnx)
{
	u8 sta;
 	sta=btnx->sta&0x03;
 	switch(sta)
	{
		case BTN_RELEASE://松开
		case BTN_INACTIVE://未激活	
 			minibmp_decode(btnx->picbtnpathu,btnx->left,btnx->top,btnx->width,btnx->height,0,0);
 			break;
		case BTN_PRESS://按下,当存在按下图片时,显示按下的图片.当不存在时,在松开图片上做alphablending
			if(btnx->picbtnpathd!=NULL)
			{
 				minibmp_decode(btnx->picbtnpathd,btnx->left,btnx->top,btnx->width,btnx->height,0,0);
 			}else 
			{
				minibmp_decode(btnx->picbtnpathu,btnx->left,btnx->top,btnx->width,btnx->height,0xffff,0x90);
			}
			break;
	}	    		
}  
//画圆角按钮
//btnx:按钮
void btn_draw_arcbtn(_btn_obj * btnx)
{
	u16 angcolor=0,upcolor=0,downcolor=0;	
	u8 sta;
 	if(btnx->arcbtnr>btnx->width/2||btnx->arcbtnr>btnx->height/2)return;//超过范围了
	sta=btnx->sta&0x03;											 			
	switch(sta)
	{
		case BTN_RELEASE://正常(松开)
			angcolor=btnx->bkctbl[1];
			upcolor=btnx->bkctbl[2];
			downcolor=btnx->bkctbl[3];
			break;
		case BTN_PRESS://按下
	  		angcolor=gui_alpha_blend565(btnx->bkctbl[2],GUI_COLOR_WHITE,16);
	 		upcolor=angcolor; 
 			downcolor=gui_alpha_blend565(btnx->bkctbl[3],GUI_COLOR_WHITE,16); 
			break;
		case BTN_INACTIVE://此种按钮没有未激活状态
			return;
	} 
  	gui_draw_arcrectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->arcbtnr,1,upcolor,downcolor);//填充圆角按钮
   	gui_draw_arcrectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->arcbtnr,0,btnx->bkctbl[0],btnx->bkctbl[0]);//画圆角边框
	gui_draw_hline (btnx->left+btnx->arcbtnr,btnx->top+1,btnx->width-2*btnx->arcbtnr,angcolor);//第一行		   
 	//显示caption
	if(sta==0)gui_show_strmid(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bcfucolor,btnx->font,btnx->caption);
	else gui_show_strmid(btnx->left+1,btnx->top+1,btnx->width,btnx->height,btnx->bcfdcolor,btnx->font,btnx->caption);		   
}
//画背景透明文字按钮
//btnx:按钮
void btn_draw_textabtn(_btn_obj * btnx)
{
	u8 sta;
	sta=btnx->sta&0x03;	  
	switch(sta)
	{
		case BTN_RELEASE://正常(松开)
    		btn_recover_backcolor(btnx);//画底色
		    gui_show_strmid(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bcfucolor,btnx->font,btnx->caption);
			break;
		case BTN_PRESS://按下   
    		btn_recover_backcolor(btnx);//画底色
		    gui_show_strmid(btnx->left+1,btnx->top+1,btnx->width,btnx->height,btnx->bcfdcolor,btnx->font,btnx->caption);
			break;
		case BTN_INACTIVE://此种按钮没有未激活状态
			return;
	} 
}

//画背景单一文字按钮
//btnx:按钮
void btn_draw_textbbtn(_btn_obj * btnx)
{
	u8 sta;
	sta=btnx->sta&0x03;	  
	switch(sta)
	{
		case BTN_RELEASE://正常(松开)						 
			gui_fill_rectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bkctbl[0]);//填充 
 		    gui_show_strmid(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bcfucolor,btnx->font,btnx->caption);
			break;
		case BTN_PRESS://按下   
			gui_fill_rectangle(btnx->left,btnx->top,btnx->width,btnx->height,btnx->bkctbl[1]);//填充 
		    gui_show_strmid(btnx->left+1,btnx->top+1,btnx->width,btnx->height,btnx->bcfdcolor,btnx->font,btnx->caption);
			break;
		case BTN_INACTIVE://此种按钮没有未激活状态
			return;
	} 
}
//画按钮
//btnx:按钮
void btn_draw(_btn_obj * btnx) 	
{
	if(btnx==NULL)return;//无效,直接退出
	switch(btnx->type&0x0f)
	{
		case BTN_TYPE_STD://标准按钮
			btn_draw_stdbtn(btnx);
			break;
		case BTN_TYPE_PIC://图片按钮
			btn_draw_picbtn(btnx);
			break;
		case BTN_TYPE_ANG://边角按钮
			btn_draw_arcbtn(btnx);
			break;
		case BTN_TYPE_TEXTA://文字按钮(背景透明)
			btn_draw_textabtn(btnx);
			break;
		case BTN_TYPE_TEXTB://文字按钮(背景单一)
			btn_draw_textbbtn(btnx);
			break;
 	}
	if(btnx->sta&(1<<6))
	{
		if((btnx->sta&0x03)==BTN_PRESS)btn_press_hook(btnx);//按键按下的钩子函数
		else if((btnx->sta&0x03)==BTN_RELEASE)btn_release_hook(btnx);//按键松开时的钩子函数 
	}	
}	

//按钮松开钩子函数
//btnx:按钮指针
void btn_release_hook(_btn_obj *btnx)
{
	//printf("button release %d status:%d\r\n",btnx->id,btnx->sta);
}
//按钮按下钩子函数
//btnx:按钮指针
void btn_press_hook(_btn_obj *btnx)
{
	//printf("button press %d status:%d\r\n",btnx->id,btnx->sta);
}
/*************************************************************************
函数名称：触摸屏按键初始化函数
功能介绍：
作者：刁钻的游戏
编写时间：2015.10.6
**************************************************************************/
void menu_button()
{


	//_btn_obj* tbtn1,tbtn2,tbtn3;
	button=btn_creat(350,500,100,50,1,0X02);//创建按钮
	button->sta=0;
	button->caption = "P+";//名字
	button->font=16;//字体
	button->bcfdcolor = BLACK;
	button->bcfucolor = RED;
	btn_draw(button);//画按钮
	
	button1=btn_creat(350,550,100,50,2,0X02);//创建按钮
	button1->sta=0;
	button1->caption = "P-";//名字
	button1->font=16;//字体
	button1->bcfdcolor = BLACK;
	button1->bcfucolor = RED;
	
	btn_draw(button1);//画按钮
	
	button2=btn_creat(350,600,100,50,3,0X02);//创建按钮
	button2->sta=1;
	button2->caption = "System";
	//button2->caption = "系统";//名字
	button2->font=16;//字体
	button2->bcfdcolor = BLACK;
	button2->bcfucolor = RED;
	btn_draw(button2);//画按钮
	
	button3=btn_creat(350,650,100,50,4,0X02);//创建按钮
	button3->sta=1;
	button3->caption = "Change";
	//button3->caption = "切换";//名字
	button3->font=16;//字体
	button3->bcfdcolor = BLACK;
	button3->bcfucolor = RED;
	btn_draw(button3);//画按钮
	/*
	button2=btn_creat(350,600,100,50,3,0X82);//创建按钮
	button2->sta=1;
	button2->caption = "确认";//名字
	button2->font=16;//字体
	button2->bcfdcolor = BLACK;
	button2->bcfucolor = RED;
	btn_draw(button2);//画按钮
	*/
}
 
void check()
{
	
}














