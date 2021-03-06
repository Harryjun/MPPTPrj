#include "main.h"
uint16_t AD_C[6];
//__IO uint16_t AD_C[2];
float AD_VALUE;
/************************************************

技术支持：正点原子
作者：刁钻的游戏 
备注：
包括代码段


触摸屏（按键控件、显示控件）
（多引脚）

ADC模块：
PC1---DMA1---ADC1-11---Uo
PC2---DMA1---ADC1-12---Io
PC3---DMA1---ADC1-13---Ui
PC4---DMA1---ADC1-14---Ii

PC5---outputI


定时器模块
TIM2定时器

PWM模块
PC7---PWM输出

MOS开关2个
PG0   输入侧MOS1
PG1   放电侧MOS2
PG2   DS2408信号端
PG3   DS2408信号端



定时器模块
1定时中断调控//PID模糊控制算法

2九键数字键盘
************************************************/

/***************************************************
参量表
***************************************************/
/*采样参数  采样系数*/

float K_voltage = 11.0;
float K_current = 2.67;//4.4;//2.7;//2.7;

/*全局变量*/
float Voltage = 0;
float Current = 0;
float Battery_All = 60;
float PWM_ctr = 65;//50.0;
int Flag_Mode = -1;
int Last_Mode=0;
int Mode_System = 0;
int OUT_B = 0;
/*调控模式*/
#define Voltage_Constant 1
#define Current_Constant 2
#define Power_Most 3
#define Debug 0



/*综合调控参数*/
float Current_MAX = 2.0;
float Voltage_MAX = 30;
int  OUT_V_MIN = 20;

/*恒压恒流调控参数*/
float V_C = 0.25;
float C_C = 0.8;


/*太阳能调控参数*/
float ctr_step = 0.2;//上一次升高的步长值
float P_l = 0;//上一次功率
int fuhao = -1;//扰动方向（太阳能最大功率）
float D_p = 0;//功率差


/*恒压恒流标准参数*/
float V_our = 30 ;   //20;
float I_our = 2.5 ;  //10;


extern unsigned char RomCode[8];
extern unsigned char Page0[9];

int main()
{
	int temp[4] = {0};
	int temp2[4] = {0};
//	float a[4] = {1.31,2.12,3,4};
//	int b[2] = {0,0};
//	int temp[4] = {0};
//	int temp2[4] = {0};
//	int len =0;
//	int number = 0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	uart_init(115200);	 	//串口初始化为9600
	delay_ms(1000);
	printf("AT+CWSAP=\"LiMing\",\"0123456789\",11,0\r\n");
	delay_ms(50);
	printf("AT+CIPSTART=\"UDP\",\"192.168.4.2\",5000,5000\r\n");
	delay_ms(50);
	
	MOS_Init();
	ds2438_init();
	
	LCD_Init();	
	KEY_Init();	
	ADC_init();//DMA_ADC初始化
	PWM_init(2,1500,PWM_ctr*1500/100);//PWM初始化
	
	//usmart_dev.init(72);//usmart初始化	
  //my_mem_init(SRAMIN);	//初始化内部内存池	
 	//exfuns_init();  					//为fatfs相关变量申请内存  
	//f_mount(fs[0],"0:",1);  		//挂载SD卡 
 	//f_mount(fs[1],"1:",1); 		//挂载FLASH.
	/*彩屏初始化*/
	tp_dev.init();
	font_init();
	POINT_COLOR=RED;   //画笔颜色   
	gui_init();
	/*屏幕界面初始化*/
	menu_button();
	menu_term();
	LCD_ShowString(260,120,200,16,24,"00.00V");//电压界面初始化
	LCD_ShowString(260,190,200,16,24,"00.00A");//电流界面初始化
	
	gui_fill_rectangle(240,380,198,68,WHITE);
	gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"DebugMode");
	//gui_show_str_center(260,240,200,70,0,RED,RED,16,"  保留  ");
	LCD_ShowString(260,330,200,16,24,"00%");//占空比界面初始化
	/*定时器初始化：调节采样周期*/
	TIMER_init(500);
	while(1)
	{
		delay_ms(1000);
		ReadRomCode(); 
	//发挥数据
		printf("%c",RomCode[0]);
		printf("%c",RomCode[1]);
		printf("%c",RomCode[2]);
		printf("%c",RomCode[3]);
		printf("%c",RomCode[4]);
		printf("%c",RomCode[5]);
		printf("%c",RomCode[6]);
		printf("%c",RomCode[7]);
		printf("%c",RomCode[8]);
		printf("00");
	}
	while(1)
	{		
		/*对手机接收数据进行判别*/
		if(USART_RX_STA == 12)//(USART_RX_STA&0x8000)
		{//接收到数据------------------------------------------------------------------------------------		
			if(USART_RX_BUF[0] == 'M')
			{//模式调整
				if(USART_RX_BUF[1] == '5')
				{
					Mode_System = 2;//////////////////////////////
					OUT_B = 0;
					Flag_Mode = 5;
				}
				else if(USART_RX_BUF[1] == '6')
				{
					Mode_System = 2;
					OUT_B = 1;
					Flag_Mode = 6;
				}
				else if(USART_RX_BUF[1] == '1')
				{
					Mode_System = 1;
					Flag_Mode = 0;
				}
				else if(USART_RX_BUF[1] == '2')
				{
					Mode_System = 1;
					Flag_Mode = 1;
				}
				else if(USART_RX_BUF[1] == '3')
				{
					Mode_System = 1;
					Flag_Mode = 2;
				}
				else if(USART_RX_BUF[1] == '4')
				{
					Mode_System = 1;
					Flag_Mode = 3;
				}
			}
			if(USART_RX_BUF[0] == 'Z')
			{
				//占空比模式
				if(USART_RX_BUF[1] == '2')
				{
					PWM_ctr++;
					if(PWM_ctr >= 99)
						PWM_ctr = 99;
				}
				else if(USART_RX_BUF[1] == '1')
				{
					PWM_ctr--;
					if(PWM_ctr <= 0)
						PWM_ctr = 0;
				}
			}
			/**/
			else if(USART_RX_BUF[0] == 'U')
			{//恒压模式
				Flag_Mode = 1;
				V_our = (USART_RX_BUF[1] - 48) *10 + USART_RX_BUF[2] - 48;
			}
			else if(USART_RX_BUF[0] == 'I')
			{//恒流模式
				Flag_Mode = 2;
				I_our = (USART_RX_BUF[1] - 48)  + (USART_RX_BUF[3] - 48) * 0.1;
			}
			else if(USART_RX_BUF[0] == 'Y')
			{//最大功率
				Flag_Mode = 3;
			}
			else if(USART_RX_BUF[0] == 'F')
			{//最大电压参数更改
				Voltage_MAX = (USART_RX_BUF[1] - 48) *10 + USART_RX_BUF[2] - 48;
			}
			else if(USART_RX_BUF[0] == 'S')
			{//最大电流参数更改
				Current_MAX = (USART_RX_BUF[1] - 48)  + (USART_RX_BUF[3] - 48) * 0.1;
			}
			
/*		b[0] =  (USART_RX_BUF[0] - 48) *10 + USART_RX_BUF[1] - 48 ;
				//b[0] = b[0] * 10 + USART_RX_BUF[8];
			//b[0] = (int)(USART_RX_BUF[7] - 48)*10 + USART_RX_BUF[8]-48;
			printf("AT+CIPSEND=1\r\n");
			delay_ms(10);//
			printf("A\r\n");
			delay_ms(10);//
			printf("AT+CIPSEND=4\r\n");
			delay_ms(10);
			printf("%d\r\n",b[0]);
				delay_ms(10);
	*/					
			/*
			for(int t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);//向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}*/
			//printf("\r\n\r\n");//插入换行
				/*对彩屏数据显示*/
		/**/
		//状态显示
		if(Mode_System == 1)
		{//调试模式
			gui_fill_rectangle(240,380,198,68,WHITE);
			gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"DebugMode");		
			switch(Flag_Mode)
					{
						case 0:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"PwmMode");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"PWM??�?");
							break;
						case 1:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantVoltage");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"o??1??�?");
							break;
						case 2:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantCurrent");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"o?狩3?�?");
							break;
						case 3:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"MaxPower");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"�?'�1|?�");
							PWM_ctr += 1;
							break;
					}
			
			
		}
		else if(Mode_System == 2)
		{//正常模式
			gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"NormalMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"?y3???�?");
					Flag_Mode = 0;
			if(OUT_B == 1)
					{//放电模式
						gui_fill_rectangle(240,240,198,68,WHITE);
						gui_show_str_center(240,240,200,70,0,RED,BLACK,16," OUT_Battery ");
						OUTPUT_ON;
						INPUT_OFF;
						Flag_Mode = 6;
					}
					else
					{
						gui_fill_rectangle(240,240,198,68,WHITE);
						gui_show_str_center(240,240,200,70,0,RED,BLACK,16," IN_Battery ");
						OUTPUT_OFF;
						INPUT_ON;
					}
		}
		USART_RX_STA=20;//还原状态位置

		}
		
		
		if(Mode_System == 1 || Mode_System == 2)
		{
			if(Flag_Mode != Last_Mode)
			{
				switch(Flag_Mode)
				{
					case 0:
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"PwmMode");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"PWM??�?");
					break;
					case 1:
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantVoltage");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"o??1??�?");
					break;
					case 2:
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantCurrent");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"o?狩3?�?");
					break;
					case 3:
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"MaxPower");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"�?'�1|?�");
					PWM_ctr += 1;
					break;
				}
				Last_Mode = Flag_Mode;
			}
		}
		
		tp_dev.scan(0);
		in_obj.intype=IN_TYPE_TOUCH;
		
		if(tp_dev.sta&0X80)in_obj.ksta|=0X01;	//触摸被按下
				else in_obj.ksta&=~(0X01);				//触摸松开了 
		{
			in_obj.x=tp_dev.x[0];		 			//得到触屏坐标
			in_obj.y=tp_dev.y[0];
			/*按键调控*/
			if(btn_check(button,&in_obj) == 1 && ((button->sta&0X80)==0))
			{//+号键
				if(Mode_System == 1)
				{
					PWM_ctr++;
					if(PWM_ctr >= 99)
						PWM_ctr = 99;
				}
				else if(Mode_System == 3)
				{//开发者模式调整参数
					switch(Flag_Mode)
					{
						case 0: V_C += 0.05;
										break;
						case 1: C_C += 0.05;
										break;
						case 2:
										break;
						case 3:
										break;
						case 4: 
							flash_read(0x08010000,&temp[0],4);
							V_C = temp[0]/100.0;
							C_C = temp[1]/100.0;
							
										break;
					}
					temp[0] = V_C*100;
					temp[1] = C_C*100;
					LCD_ShowxNum(100,600,V_C*100,3,24,0X80);
					LCD_ShowxNum(100,650,C_C*100,3,24,0X80);
					flash_write(0x08010000,&temp[0],4);
				//	WriteFlashOneWord(32,C_C*100);
					//WriteFlashOneWord(0,V_C*100);
				}
			}
			else if(btn_check(button1,&in_obj) == 1 && ((button1->sta&0X03) == BTN_PRESS))
			{//-号键
				if(Mode_System == 1)
				{
					PWM_ctr--;
					if(PWM_ctr <= 0)
						PWM_ctr = 0;
				}
				else if(Mode_System == 3)
				{/*开发者模式调整参数*/
					switch(Flag_Mode)
					{
						case 0: V_C -= 0.05;
										break;
						case 1: C_C -= 0.05;
										break;
						case 2:
										break;
						case 3:
										break;
						case 4: 
							flash_read(0x08010000,&temp[0],4);
							V_C = temp[0]/100.0;
							C_C = temp[1]/100.0;
							
										break;
					}
					temp[0] = V_C*100;
					temp[1] = C_C*100;
					LCD_ShowxNum(100,600,V_C*100,3,24,0X80);
					LCD_ShowxNum(100,650,C_C*100,3,24,0X80);
					flash_write(0x08010000,&temp[0],4);
				//	WriteFlashOneWord(32,C_C*100);
					//WriteFlashOneWord(0,V_C*100);
				}
				
			}
			else if(btn_check(button2,&in_obj) == 1 && ((button2->sta&0X80)==0))
			{//系统键
				Mode_System++;
				if(Mode_System >= 4)
						Mode_System = 0;
				if(Mode_System == 0)
				{
					gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"ProtectionMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"保护模式");
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"  Reserved ");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"  保留 ");
					Flag_Mode = 0;
				}
				else if(Mode_System == 1)
				{
					gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"DebugMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"调试模式");
					Flag_Mode = 0;
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"PwmMode");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"PWM模式");
				}
				else if(Mode_System == 2)
				{
					gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"NormalMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"正常模式");
					Flag_Mode = 0;
					OUT_B = 0;
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16," IN_Battery ");
				}
				else if(Mode_System == 3)
				{
					gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"DeveloperMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"开发者模式");
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"VoltageParameter");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"恒压调控比例系数");
					Flag_Mode = 0;
				}
				
			}
			else if(btn_check(button3,&in_obj) == 1 && ((button3->sta&0X80)==0))
			{//切换键
				
				if(Mode_System == 1)
				{/*调试模式*/
					Flag_Mode++;
					if(Flag_Mode > 3)
						Flag_Mode = 0;
					switch(Flag_Mode)
					{
						case 0:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"PwmMode");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"PWM模式");
							break;
						case 1:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantVoltage");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"恒压模式");
							break;
						case 2:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantCurrent");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"恒流充电");
							break;
						case 3:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"MaxPower");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"最大功率");
							PWM_ctr += 1;
							break;
					}
				}
				else if(Mode_System == 2)
				{/*正常模式*/
					if(OUT_B == 0)
					{//放电模式
						OUT_B = 1;
						gui_fill_rectangle(240,240,198,68,WHITE);
						gui_show_str_center(240,240,200,70,0,RED,BLACK,16," OUT_Battery ");
						OUTPUT_ON;
						INPUT_OFF;
						Flag_Mode = 4;
					}
					else
					{
						OUT_B = 0;
						gui_fill_rectangle(240,240,198,68,WHITE);
						gui_show_str_center(240,240,200,70,0,RED,BLACK,16," IN_Battery ");
						OUTPUT_OFF;
						INPUT_ON;
					}
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"  保留 ");
					
				}
				else if(Mode_System == 3)
				{/*开发者模式：参数调节模式*/
					Flag_Mode++;
					if(Flag_Mode > 3)
						Flag_Mode = 0;
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"  Reserved ");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"  保留 ");
				
					
					switch(Flag_Mode)
					{
						case 0:
							gui_fill_rectangle(240,240,198,68,WHITE);//VoltageParameter
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"VoltageParameter");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"恒压调控比例系数");
							break;
						case 1:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"CurrentParameter");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"恒流调控比例系数");
							break;
						case 2:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"Reserved");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"保留");
							break;
						case 3:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"Reserved");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"保留");
							break;
						case 4:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"UseLastPartner");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"载入上一次参数");
							break;
					}
				}
				else if(Mode_System == 0)
				{/*保护模式无任何操作*/
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"  Reserved ");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"  保留 ");
					Flag_Mode = 0;
				}
			}
		}
		
		////////////////////////////////////////////
		
	
		
		
	}
}
	


/**********************************************
函数名称：主界面函数
入口参数：无
出口参数：无
函数说明：主界面初始化
**********************************************/
void menu_term()
{
	int i=0;
	/*主标题显示*/
	gui_show_str_center(0,0,480,100,0,BLUE,BLACK,24,"ConditionBattery");
	//gui_show_str_center(0,0,480,100,0,BLUE,BLACK,24,"电池当前状况");
	for(;i<5;i++)
	{
		gui_draw_rectangle(40,100+70*i,200,70,BLACK);
		gui_draw_rectangle(240,100+70*i,200,70,BLACK);
	}
	/*参量显示*/
	gui_show_str_center(40,100,200,70,0,RED,BLACK,16,"Voltage");//
	//gui_show_str_center(40,100,200,70,0,RED,BLACK,24,"充电电压");
	gui_show_str_center(40,170,200,70,0,RED,BLACK,16,"Current");//
	//gui_show_str_center(40,170,200,70,0,RED,BLACK,24,"充电电流");
	gui_show_str_center(40,240,200,70,0,RED,BLACK,16,"Mode_Now");
	//gui_show_str_center(40,240,200,70,0,RED,BLACK,24,"当前充电状态");
	gui_show_str_center(40,310,200,70,0,RED,BLACK,16,"PWM");
	//gui_show_str_center(40,310,200,70,0,RED,BLACK,24,"PWM占空比");
}
/**********************************************
函数名称：采样函数
入口参数：无
出口参数：无
函数说明：采集电流电压
**********************************************/
void VI_collection()
{
	static int num_a = 0;//滤波计数
	static float temp=0,temp1=0;
	int V_z;
	if(num_a < 8)
	{
		temp += AD_C[0];
		temp1 += AD_C[1];
		
		num_a++;
	}
	else
	{
		temp = temp / 8;
		temp1 = temp1 / 8;
		temp=(float)K_voltage * temp *(3.3/4096);
		//temp=(float)temp *(3.3/4096);
		//
		Voltage = temp;
		V_z = temp;
		LCD_ShowxNum(260,120,V_z,2,24,0);//显示电压值
		temp-=V_z;
		temp*=100;
		LCD_ShowxNum(296,120,temp,2,24,0X80);//显示小数部分
		//电流采集
		//LCD_ShowxNum(100,600,temp1 * (3.3/4096)*100,3,24,0X80);
		//////
		temp1 = (temp1 * (3.3/4096));
		if(temp1 > 2.5)
			temp1=(float)K_current * (temp1 - 2.5);
		else
			temp1=(float)K_current * (2.5 - temp1);
		
		
		//temp1= (float) temp1 * (3.3/4096);
		Current = temp1;
		V_z=temp1;
		if(V_z > 10)
			LCD_ShowxNum(260,190,V_z,2,24,0);//显示电流值
		else
		{
			LCD_ShowxNum(260,190,0,1,24,0);//显示电压值
			LCD_ShowxNum(272,190,V_z,1,24,0);//显示电压值
		}
		temp1 -= V_z;
		temp1 *= 100;
		LCD_ShowxNum(296,190,temp1,2,24,0X80);//显示小数部分
		temp = 0;
		temp1 = 0;
		num_a = 0;
	}
}

/***************************************************
函数名称：VI_control
入口参数：num控制模式
出口参数：无
函数说明：
三种控制模式
（1）恒压充电模式
（2）恒流充电模式
（3）太阳能最大功率充电模式
***************************************************/
void VI_control(int num)
{
	float D_v = Voltage - V_our;//电压差值
	float D_c = Current - I_our;//电流差值
	float P_k = P_l;

	P_l = Current * Voltage;//本次功率
	D_p = P_l - P_k;//功率差当前减去上次
  /*show_data
	if(D_p < 0)
		LCD_ShowxNum(200,600,-D_p,3,24,0X80);
	else
		LCD_ShowxNum(200,600,D_p,3,24,0X80);*/
	LCD_ShowxNum(1,600,P_l,4,24,0X80);
	LCD_ShowxNum(1,650,P_k,4,24,0X80);
		
	if(num == 1)
	{/*恒压充电模式*/
	  PWM_ctr = PWM_ctr + V_C * D_v;
	}
	else if(num == 2)
	{/*恒流充电模式*/
		PWM_ctr = PWM_ctr + C_C * D_c;
		//变步长值
	}
	else if(num == 3)
	{/*太阳能最大功率充电模式*/
		if(D_p < 0.02)
		{
			PWM_ctr = PWM_ctr - 0.8;
			fuhao = -1;
		}
		else
		{
			//ctr_step = 0.6 * D_p / ctr_step;////阈值
			if(D_p < 0)
				fuhao = -fuhao;
			PWM_ctr = PWM_ctr + fuhao * ctr_step;//通过D_p变量差值去改变ctr_step来调节
		}
	}
	else
	{
	
	}
		
	/*防止溢出*/
	if(PWM_ctr >= 80)
		PWM_ctr = 80;
	else if(PWM_ctr <= 65) 
		PWM_ctr = 65;
}

void Control_System(void)
{
	/*仲裁器*/
	if(Voltage > Voltage_MAX)
	{
		Flag_Mode = 1;//恒压充电
	}
	else if(Current > Current_MAX)
	{
		Flag_Mode = 2;
	}
	else
	{
		Flag_Mode = 3;
	}

}

/***************************************************
函数名称：TIM2_IRQHandler
入口参数：TIM2中断函数
出口参数：无
函数说明：中断
***************************************************/
void TIM2_IRQHandler(void)   //TIM2中断
{
	static int change = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
//添加代码			
			VI_collection();
//数据处理				
			if(Mode_System == 1)
			{
				VI_control(Flag_Mode);
				//保护模块+数据调用
				if(PWM_ctr >= 80)
					PWM_ctr = 80;
				else if(PWM_ctr <= 65) 
					PWM_ctr = 65;
				PWM_OUT(2,PWM_ctr*15);
			}
			else if(Mode_System == 2)
			{
				if(OUT_B == 0)
				{
					VI_control(Flag_Mode);
					Control_System();
					//保护模块+数据调用
					if(PWM_ctr >= 80)
						PWM_ctr = 80;
					else if(PWM_ctr <= 65) 
						PWM_ctr = 65;
					PWM_OUT(2,PWM_ctr*15);
				}
				else
				{//放电
					if(Voltage < OUT_V_MIN)
					{//放电过深
					//MOS关闭
						OUTPUT_OFF;
						OUT_B = 0;
						gui_fill_rectangle(240,240,198,68,WHITE);
						gui_show_str_center(240,240,200,70,0,RED,BLACK,16," IN_Battery ");
					}
				}
			}

//数据返回手机端
			/*
			if(change == 0)
			{
				printf("AT+CIPSEND=1\r\n");
				delay_ms(1);//
				printf("U\r\n");
				delay_ms(1);//
				printf("AT+CIPSEND=4\r\n");
				delay_ms(1);
				printf("%f\r\n",Voltage);//);
				delay_ms(10);
				change = 1;
			}
			else if(change == 1)
			{
				printf("AT+CIPSEND=1\r\n");
				delay_ms(1);
				printf("I\r\n");
				delay_ms(1);
				printf("AT+CIPSEND=4\r\n");
				delay_ms(1);
				printf("%f\r\n",Current);//);
				delay_ms(10);
				change = 2;
			}
			else if(change == 2)
			{
				printf("AT+CIPSEND=1\r\n");
				delay_ms(1);	
				printf("A\r\n");
				delay_ms(1);
				printf("AT+CIPSEND=1\r\n");
				delay_ms(1);	
				printf("%d\r\n",Flag_Mode);
				delay_ms(1);
				change = 3;
			}
			else if(change == 3)
			{
				printf("AT+CIPSEND=1\r\n");
				delay_ms(1);
				printf("Z\r\n");
				delay_ms(1);
				printf("AT+CIPSEND=4\r\n");
				delay_ms(1);
				printf("%f\r\n",PWM_ctr);
				delay_ms(1);
				change = 4;
			}
			else if(change == 4)
			{
				printf("AT+CIPSEND=1\r\n");
				delay_ms(1);
				printf("D\r\n");
				delay_ms(1);
				printf("AT+CIPSEND=4\r\n");
				delay_ms(1);
				printf("%f\r\n",Battery_All);
				delay_ms(1);
				change = 5;
			}
			else
			{
				change = 0;
			}
			*/
			LCD_ShowxNum(260,330,PWM_ctr,2,24,0X80);//占空比
			LCD_ShowxNum(400,500,PWM_ctr*15,4,24,0X80);//显示脉冲数
		}
}


