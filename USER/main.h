
/**#include********************************/
#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "usmart.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "piclib.h"	
#include "string.h"		
#include "math.h"
#include "touch.h"
#include "gui.h"
#include "text.h"
#include "adc.h"
#include "pwm.h"
#include "time.h"
#include "flash_inside.h"
#include "ds2438_driver.h"
/**FUNCTION********************************/
void VI_collection(void);//采样函数
void menu_term(void);//菜单函数
void VI_control(int num);//调控函数
void Control_System(void);

