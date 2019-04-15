#include "main.h"
uint16_t AD_C[6];
//__IO uint16_t AD_C[2];
float AD_VALUE;
/************************************************

¼¼ÊõÖ§³Ö£ºÕıµãÔ­×Ó
×÷Õß£ºµó×êµÄÓÎÏ· 
±¸×¢£º
°üÀ¨´úÂë¶Î


´¥ÃşÆÁ£¨°´¼ü¿Ø¼ş¡¢ÏÔÊ¾¿Ø¼ş£©
£¨¶àÒı½Å£©

ADCÄ£¿é£º
PC1---DMA1---ADC1-11---Uo
PC2---DMA1---ADC1-12---Io
PC3---DMA1---ADC1-13---Ui
PC4---DMA1---ADC1-14---Ii

PC5---outputI


¶¨Ê±Æ÷Ä£¿é
TIM2¶¨Ê±Æ÷

PWMÄ£¿é
PC7---PWMÊä³ö

MOS¿ª¹Ø2¸ö
PG0   ÊäÈë²àMOS1
PG1   ·Åµç²àMOS2
PG2   DS2408ĞÅºÅ¶Ë
PG3   DS2408ĞÅºÅ¶Ë



¶¨Ê±Æ÷Ä£¿é
1¶¨Ê±ÖĞ¶Ïµ÷¿Ø//PIDÄ£ºı¿ØÖÆËã·¨

2¾Å¼üÊı×Ö¼üÅÌ
************************************************/

/***************************************************
²ÎÁ¿±í
***************************************************/
/*²ÉÑù²ÎÊı  ²ÉÑùÏµÊı*/

float K_voltage = 11.0;
float K_current = 2.67;//4.4;//2.7;//2.7;

/*È«¾Ö±äÁ¿*/
float Voltage = 0;
float Current = 0;
float Battery_All = 60;
float PWM_ctr = 65;//50.0;
int Flag_Mode = -1;
int Last_Mode=0;
int Mode_System = 0;
int OUT_B = 0;
/*µ÷¿ØÄ£Ê½*/
#define Voltage_Constant 1
#define Current_Constant 2
#define Power_Most 3
#define Debug 0



/*×ÛºÏµ÷¿Ø²ÎÊı*/
float Current_MAX = 2.0;
float Voltage_MAX = 30;
int  OUT_V_MIN = 20;

/*ºãÑ¹ºãÁ÷µ÷¿Ø²ÎÊı*/
float V_C = 0.25;
float C_C = 0.8;


/*Ì«ÑôÄÜµ÷¿Ø²ÎÊı*/
float ctr_step = 0.2;//ÉÏÒ»´ÎÉı¸ßµÄ²½³¤Öµ
float P_l = 0;//ÉÏÒ»´Î¹¦ÂÊ
int fuhao = -1;//ÈÅ¶¯·½Ïò£¨Ì«ÑôÄÜ×î´ó¹¦ÂÊ£©
float D_p = 0;//¹¦ÂÊ²î


/*ºãÑ¹ºãÁ÷±ê×¼²ÎÊı*/
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
	delay_init();	    	 //ÑÓÊ±º¯Êı³õÊ¼»¯	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //ÉèÖÃNVICÖĞ¶Ï·Ö×é2:2Î»ÇÀÕ¼ÓÅÏÈ¼¶£¬2Î»ÏìÓ¦ÓÅÏÈ¼¶
	
	uart_init(115200);	 	//´®¿Ú³õÊ¼»¯Îª9600
	delay_ms(1000);
	printf("AT+CWSAP=\"LiMing\",\"0123456789\",11,0\r\n");
	delay_ms(50);
	printf("AT+CIPSTART=\"UDP\",\"192.168.4.2\",5000,5000\r\n");
	delay_ms(50);
	
	MOS_Init();
	ds2438_init();
	
	LCD_Init();	
	KEY_Init();	
	ADC_init();//DMA_ADC³õÊ¼»¯
	PWM_init(2,1500,PWM_ctr*1500/100);//PWM³õÊ¼»¯
	
	//usmart_dev.init(72);//usmart³õÊ¼»¯	
  //my_mem_init(SRAMIN);	//³õÊ¼»¯ÄÚ²¿ÄÚ´æ³Ø	
 	//exfuns_init();  					//ÎªfatfsÏà¹Ø±äÁ¿ÉêÇëÄÚ´æ  
	//f_mount(fs[0],"0:",1);  		//¹ÒÔØSD¿¨ 
 	//f_mount(fs[1],"1:",1); 		//¹ÒÔØFLASH.
	/*²ÊÆÁ³õÊ¼»¯*/
	tp_dev.init();
	font_init();
	POINT_COLOR=RED;   //»­±ÊÑÕÉ«   
	gui_init();
	/*ÆÁÄ»½çÃæ³õÊ¼»¯*/
	menu_button();
	menu_term();
	LCD_ShowString(260,120,200,16,24,"00.00V");//µçÑ¹½çÃæ³õÊ¼»¯
	LCD_ShowString(260,190,200,16,24,"00.00A");//µçÁ÷½çÃæ³õÊ¼»¯
	
	gui_fill_rectangle(240,380,198,68,WHITE);
	gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"DebugMode");
	//gui_show_str_center(260,240,200,70,0,RED,RED,16,"  ±£Áô  ");
	LCD_ShowString(260,330,200,16,24,"00%");//Õ¼¿Õ±È½çÃæ³õÊ¼»¯
	/*¶¨Ê±Æ÷³õÊ¼»¯£ºµ÷½Ú²ÉÑùÖÜÆÚ*/
	TIMER_init(500);
	while(1)
	{
		delay_ms(1000);
		ReadRomCode(); 
	//·¢»ÓÊı¾İ
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
		/*¶ÔÊÖ»ú½ÓÊÕÊı¾İ½øĞĞÅĞ±ğ*/
		if(USART_RX_STA == 12)//(USART_RX_STA&0x8000)
		{//½ÓÊÕµ½Êı¾İ------------------------------------------------------------------------------------		
			if(USART_RX_BUF[0] == 'M')
			{//Ä£Ê½µ÷Õû
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
				//Õ¼¿Õ±ÈÄ£Ê½
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
			{//ºãÑ¹Ä£Ê½
				Flag_Mode = 1;
				V_our = (USART_RX_BUF[1] - 48) *10 + USART_RX_BUF[2] - 48;
			}
			else if(USART_RX_BUF[0] == 'I')
			{//ºãÁ÷Ä£Ê½
				Flag_Mode = 2;
				I_our = (USART_RX_BUF[1] - 48)  + (USART_RX_BUF[3] - 48) * 0.1;
			}
			else if(USART_RX_BUF[0] == 'Y')
			{//×î´ó¹¦ÂÊ
				Flag_Mode = 3;
			}
			else if(USART_RX_BUF[0] == 'F')
			{//×î´óµçÑ¹²ÎÊı¸ü¸Ä
				Voltage_MAX = (USART_RX_BUF[1] - 48) *10 + USART_RX_BUF[2] - 48;
			}
			else if(USART_RX_BUF[0] == 'S')
			{//×î´óµçÁ÷²ÎÊı¸ü¸Ä
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
				USART_SendData(USART1, USART_RX_BUF[t]);//Ïò´®¿Ú1·¢ËÍÊı¾İ
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ı·¢ËÍ½áÊø
			}*/
			//printf("\r\n\r\n");//²åÈë»»ĞĞ
				/*¶Ô²ÊÆÁÊı¾İÏÔÊ¾*/
		/**/
		//×´Ì¬ÏÔÊ¾
		if(Mode_System == 1)
		{//µ÷ÊÔÄ£Ê½
			gui_fill_rectangle(240,380,198,68,WHITE);
			gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"DebugMode");		
			switch(Flag_Mode)
					{
						case 0:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"PwmMode");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"PWM??ê?");
							break;
						case 1:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantVoltage");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"o??1??ê?");
							break;
						case 2:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantCurrent");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"o?á÷3?µ?");
							break;
						case 3:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"MaxPower");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"×?'ó1|?ê");
							PWM_ctr += 1;
							break;
					}
			
			
		}
		else if(Mode_System == 2)
		{//Õı³£Ä£Ê½
			gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"NormalMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"?y3???ê?");
					Flag_Mode = 0;
			if(OUT_B == 1)
					{//·ÅµçÄ£Ê½
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
		USART_RX_STA=20;//»¹Ô­×´Ì¬Î»ÖÃ

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
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"PWM??ê?");
					break;
					case 1:
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantVoltage");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"o??1??ê?");
					break;
					case 2:
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantCurrent");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"o?á÷3?µ?");
					break;
					case 3:
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"MaxPower");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"×?'ó1|?ê");
					PWM_ctr += 1;
					break;
				}
				Last_Mode = Flag_Mode;
			}
		}
		
		tp_dev.scan(0);
		in_obj.intype=IN_TYPE_TOUCH;
		
		if(tp_dev.sta&0X80)in_obj.ksta|=0X01;	//´¥Ãş±»°´ÏÂ
				else in_obj.ksta&=~(0X01);				//´¥ÃşËÉ¿ªÁË 
		{
			in_obj.x=tp_dev.x[0];		 			//µÃµ½´¥ÆÁ×ø±ê
			in_obj.y=tp_dev.y[0];
			/*°´¼üµ÷¿Ø*/
			if(btn_check(button,&in_obj) == 1 && ((button->sta&0X80)==0))
			{//+ºÅ¼ü
				if(Mode_System == 1)
				{
					PWM_ctr++;
					if(PWM_ctr >= 99)
						PWM_ctr = 99;
				}
				else if(Mode_System == 3)
				{//¿ª·¢ÕßÄ£Ê½µ÷Õû²ÎÊı
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
			{//-ºÅ¼ü
				if(Mode_System == 1)
				{
					PWM_ctr--;
					if(PWM_ctr <= 0)
						PWM_ctr = 0;
				}
				else if(Mode_System == 3)
				{/*¿ª·¢ÕßÄ£Ê½µ÷Õû²ÎÊı*/
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
			{//ÏµÍ³¼ü
				Mode_System++;
				if(Mode_System >= 4)
						Mode_System = 0;
				if(Mode_System == 0)
				{
					gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"ProtectionMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"±£»¤Ä£Ê½");
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"  Reserved ");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"  ±£Áô ");
					Flag_Mode = 0;
				}
				else if(Mode_System == 1)
				{
					gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"DebugMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"µ÷ÊÔÄ£Ê½");
					Flag_Mode = 0;
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"PwmMode");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"PWMÄ£Ê½");
				}
				else if(Mode_System == 2)
				{
					gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"NormalMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"Õı³£Ä£Ê½");
					Flag_Mode = 0;
					OUT_B = 0;
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16," IN_Battery ");
				}
				else if(Mode_System == 3)
				{
					gui_fill_rectangle(240,380,198,68,WHITE);
					gui_show_str_center(240,380,200,70,0,RED,BLACK,16,"DeveloperMode");
					//gui_show_str_center(240,380,200,70,0,RED,BLACK,24,"¿ª·¢ÕßÄ£Ê½");
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"VoltageParameter");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"ºãÑ¹µ÷¿Ø±ÈÀıÏµÊı");
					Flag_Mode = 0;
				}
				
			}
			else if(btn_check(button3,&in_obj) == 1 && ((button3->sta&0X80)==0))
			{//ÇĞ»»¼ü
				
				if(Mode_System == 1)
				{/*µ÷ÊÔÄ£Ê½*/
					Flag_Mode++;
					if(Flag_Mode > 3)
						Flag_Mode = 0;
					switch(Flag_Mode)
					{
						case 0:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"PwmMode");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"PWMÄ£Ê½");
							break;
						case 1:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantVoltage");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"ºãÑ¹Ä£Ê½");
							break;
						case 2:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"ConstantCurrent");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"ºãÁ÷³äµç");
							break;
						case 3:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"MaxPower");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"×î´ó¹¦ÂÊ");
							PWM_ctr += 1;
							break;
					}
				}
				else if(Mode_System == 2)
				{/*Õı³£Ä£Ê½*/
					if(OUT_B == 0)
					{//·ÅµçÄ£Ê½
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
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"  ±£Áô ");
					
				}
				else if(Mode_System == 3)
				{/*¿ª·¢ÕßÄ£Ê½£º²ÎÊıµ÷½ÚÄ£Ê½*/
					Flag_Mode++;
					if(Flag_Mode > 3)
						Flag_Mode = 0;
					gui_fill_rectangle(240,240,198,68,WHITE);
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"  Reserved ");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"  ±£Áô ");
				
					
					switch(Flag_Mode)
					{
						case 0:
							gui_fill_rectangle(240,240,198,68,WHITE);//VoltageParameter
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"VoltageParameter");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"ºãÑ¹µ÷¿Ø±ÈÀıÏµÊı");
							break;
						case 1:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"CurrentParameter");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"ºãÁ÷µ÷¿Ø±ÈÀıÏµÊı");
							break;
						case 2:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"Reserved");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"±£Áô");
							break;
						case 3:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"Reserved");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"±£Áô");
							break;
						case 4:
							gui_fill_rectangle(240,240,198,68,WHITE);
							gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"UseLastPartner");
							//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"ÔØÈëÉÏÒ»´Î²ÎÊı");
							break;
					}
				}
				else if(Mode_System == 0)
				{/*±£»¤Ä£Ê½ÎŞÈÎºÎ²Ù×÷*/
					gui_show_str_center(240,240,200,70,0,RED,BLACK,16,"  Reserved ");
					//gui_show_str_center(240,240,200,70,0,RED,BLACK,24,"  ±£Áô ");
					Flag_Mode = 0;
				}
			}
		}
		
		////////////////////////////////////////////
		
	
		
		
	}
}
	


/**********************************************
º¯ÊıÃû³Æ£ºÖ÷½çÃæº¯Êı
Èë¿Ú²ÎÊı£ºÎŞ
³ö¿Ú²ÎÊı£ºÎŞ
º¯ÊıËµÃ÷£ºÖ÷½çÃæ³õÊ¼»¯
**********************************************/
void menu_term()
{
	int i=0;
	/*Ö÷±êÌâÏÔÊ¾*/
	gui_show_str_center(0,0,480,100,0,BLUE,BLACK,24,"ConditionBattery");
	//gui_show_str_center(0,0,480,100,0,BLUE,BLACK,24,"µç³Øµ±Ç°×´¿ö");
	for(;i<5;i++)
	{
		gui_draw_rectangle(40,100+70*i,200,70,BLACK);
		gui_draw_rectangle(240,100+70*i,200,70,BLACK);
	}
	/*²ÎÁ¿ÏÔÊ¾*/
	gui_show_str_center(40,100,200,70,0,RED,BLACK,16,"Voltage");//
	//gui_show_str_center(40,100,200,70,0,RED,BLACK,24,"³äµçµçÑ¹");
	gui_show_str_center(40,170,200,70,0,RED,BLACK,16,"Current");//
	//gui_show_str_center(40,170,200,70,0,RED,BLACK,24,"³äµçµçÁ÷");
	gui_show_str_center(40,240,200,70,0,RED,BLACK,16,"Mode_Now");
	//gui_show_str_center(40,240,200,70,0,RED,BLACK,24,"µ±Ç°³äµç×´Ì¬");
	gui_show_str_center(40,310,200,70,0,RED,BLACK,16,"PWM");
	//gui_show_str_center(40,310,200,70,0,RED,BLACK,24,"PWMÕ¼¿Õ±È");
}
/**********************************************
º¯ÊıÃû³Æ£º²ÉÑùº¯Êı
Èë¿Ú²ÎÊı£ºÎŞ
³ö¿Ú²ÎÊı£ºÎŞ
º¯ÊıËµÃ÷£º²É¼¯µçÁ÷µçÑ¹
**********************************************/
void VI_collection()
{
	static int num_a = 0;//ÂË²¨¼ÆÊı
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
		LCD_ShowxNum(260,120,V_z,2,24,0);//ÏÔÊ¾µçÑ¹Öµ
		temp-=V_z;
		temp*=100;
		LCD_ShowxNum(296,120,temp,2,24,0X80);//ÏÔÊ¾Ğ¡Êı²¿·Ö
		//µçÁ÷²É¼¯
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
			LCD_ShowxNum(260,190,V_z,2,24,0);//ÏÔÊ¾µçÁ÷Öµ
		else
		{
			LCD_ShowxNum(260,190,0,1,24,0);//ÏÔÊ¾µçÑ¹Öµ
			LCD_ShowxNum(272,190,V_z,1,24,0);//ÏÔÊ¾µçÑ¹Öµ
		}
		temp1 -= V_z;
		temp1 *= 100;
		LCD_ShowxNum(296,190,temp1,2,24,0X80);//ÏÔÊ¾Ğ¡Êı²¿·Ö
		temp = 0;
		temp1 = 0;
		num_a = 0;
	}
}

/***************************************************
º¯ÊıÃû³Æ£ºVI_control
Èë¿Ú²ÎÊı£ºnum¿ØÖÆÄ£Ê½
³ö¿Ú²ÎÊı£ºÎŞ
º¯ÊıËµÃ÷£º
ÈıÖÖ¿ØÖÆÄ£Ê½
£¨1£©ºãÑ¹³äµçÄ£Ê½
£¨2£©ºãÁ÷³äµçÄ£Ê½
£¨3£©Ì«ÑôÄÜ×î´ó¹¦ÂÊ³äµçÄ£Ê½
***************************************************/
void VI_control(int num)
{
	float D_v = Voltage - V_our;//µçÑ¹²îÖµ
	float D_c = Current - I_our;//µçÁ÷²îÖµ
	float P_k = P_l;

	P_l = Current * Voltage;//±¾´Î¹¦ÂÊ
	D_p = P_l - P_k;//¹¦ÂÊ²îµ±Ç°¼õÈ¥ÉÏ´Î
  /*show_data
	if(D_p < 0)
		LCD_ShowxNum(200,600,-D_p,3,24,0X80);
	else
		LCD_ShowxNum(200,600,D_p,3,24,0X80);*/
	LCD_ShowxNum(1,600,P_l,4,24,0X80);
	LCD_ShowxNum(1,650,P_k,4,24,0X80);
		
	if(num == 1)
	{/*ºãÑ¹³äµçÄ£Ê½*/
	  PWM_ctr = PWM_ctr + V_C * D_v;
	}
	else if(num == 2)
	{/*ºãÁ÷³äµçÄ£Ê½*/
		PWM_ctr = PWM_ctr + C_C * D_c;
		//±ä²½³¤Öµ
	}
	else if(num == 3)
	{/*Ì«ÑôÄÜ×î´ó¹¦ÂÊ³äµçÄ£Ê½*/
		if(D_p < 0.02)
		{
			PWM_ctr = PWM_ctr - 0.8;
			fuhao = -1;
		}
		else
		{
			//ctr_step = 0.6 * D_p / ctr_step;////ãĞÖµ
			if(D_p < 0)
				fuhao = -fuhao;
			PWM_ctr = PWM_ctr + fuhao * ctr_step;//Í¨¹ıD_p±äÁ¿²îÖµÈ¥¸Ä±äctr_stepÀ´µ÷½Ú
		}
	}
	else
	{
	
	}
		
	/*·ÀÖ¹Òç³ö*/
	if(PWM_ctr >= 80)
		PWM_ctr = 80;
	else if(PWM_ctr <= 65) 
		PWM_ctr = 65;
}

void Control_System(void)
{
	/*ÖÙ²ÃÆ÷*/
	if(Voltage > Voltage_MAX)
	{
		Flag_Mode = 1;//ºãÑ¹³äµç
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
º¯ÊıÃû³Æ£ºTIM2_IRQHandler
Èë¿Ú²ÎÊı£ºTIM2ÖĞ¶Ïº¯Êı
³ö¿Ú²ÎÊı£ºÎŞ
º¯ÊıËµÃ÷£ºÖĞ¶Ï
***************************************************/
void TIM2_IRQHandler(void)   //TIM2ÖĞ¶Ï
{
	static int change = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //¼ì²éÖ¸¶¨µÄTIMÖĞ¶Ï·¢ÉúÓë·ñ:TIM ÖĞ¶ÏÔ´ 
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //Çå³ıTIMxµÄÖĞ¶Ï´ı´¦ÀíÎ»:TIM ÖĞ¶ÏÔ´ 
//Ìí¼Ó´úÂë			
			VI_collection();
//Êı¾İ´¦Àí				
			if(Mode_System == 1)
			{
				VI_control(Flag_Mode);
				//±£»¤Ä£¿é+Êı¾İµ÷ÓÃ
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
					//±£»¤Ä£¿é+Êı¾İµ÷ÓÃ
					if(PWM_ctr >= 80)
						PWM_ctr = 80;
					else if(PWM_ctr <= 65) 
						PWM_ctr = 65;
					PWM_OUT(2,PWM_ctr*15);
				}
				else
				{//·Åµç
					if(Voltage < OUT_V_MIN)
					{//·Åµç¹ıÉî
					//MOS¹Ø±Õ
						OUTPUT_OFF;
						OUT_B = 0;
						gui_fill_rectangle(240,240,198,68,WHITE);
						gui_show_str_center(240,240,200,70,0,RED,BLACK,16," IN_Battery ");
					}
				}
			}

//Êı¾İ·µ»ØÊÖ»ú¶Ë
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
			LCD_ShowxNum(260,330,PWM_ctr,2,24,0X80);//Õ¼¿Õ±È
			LCD_ShowxNum(400,500,PWM_ctr*15,4,24,0X80);//ÏÔÊ¾Âö³åÊı
		}
}


