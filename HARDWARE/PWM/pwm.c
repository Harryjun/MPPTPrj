/******************************************************
例程名称：PWM波形产生
作者：刁钻的游戏
编写时间：2015.8.5
备注：
TIM3定时器
******************************************************/
#include "pwm.h"
#include "stm32f10x_tim.h"
/***************
PA1-TIM2_CH2
PA2-TIM2_CH3
PA3-TIM2_CH4

PA6-TIM3_CH1     --pc6
PA7-TIM3_CH2     --pc7
PB0-TIM3_CH3     --pc8
PB1-TIM3_CH4     --pc9

PA8-TIM1_CH1
PA9-TIM1_CH2
PA10-TIM1_CH3
PA11-TIM1_CH4

PB6-TIM4_CH1
PB7-TIM4_CH2
PB13-TIM1_CH1N
PB14-TIM1_CH2N
PB15-TIM1_CH3N
***************/
/***************************************************
函数名称：PWM初始化
入口参数：定时器通道CH,周期Period占空比Pulse
出口参数：1初始化完成0初始化失败
函数说明：PWM波形产生函数
使用TIM3定时器配置PWM模块
CH1:PA6
CH2:PA7
频率：10KHZ   --- 7199
周期：Period/10 ms   ---  7199
			Period    us   ---  71
占空比：Pulse 0-Period
注意未超频！！！
***************************************************/
int PWM_init(int TIM_CH,int Period ,int Pulse)
{
	TIM_TimeBaseInitTypeDef PWM_1;//时基
	TIM_OCInitTypeDef PWM_OC;//输出
	GPIO_InitTypeDef PWM;//PWM;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
 
  
	PWM.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; //TIM_CH2
	PWM.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	PWM.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &PWM);//初始化GPIO
 
   //初始化TIM3
	PWM_1.TIM_Period = Period; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	PWM_1.TIM_Prescaler = 1;//1;//71; //设置用来作为TIMx时钟频率除数的预分频值 
	PWM_1.TIM_ClockDivision = //0; //设置时钟分割:TDTS = Tck_tim
														TIM_CKD_DIV1;
	PWM_1.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &PWM_1); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	PWM_OC.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	PWM_OC.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	PWM_OC.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	PWM_OC.TIM_Pulse = Pulse;


	switch(TIM_CH)
	{
		case 1:
					TIM_OC1Init(TIM3,&PWM_OC);//使能通道1
					TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能预装载
					break;
		case 2:
					TIM_OC2Init(TIM3,&PWM_OC);//使能通道1
					TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能预装载
					break;
		case 3:
					TIM_OC3Init(TIM3,&PWM_OC);//使能通道1
					TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能预装载
					break;
		case 4:
					TIM_OC4Init(TIM3,&PWM_OC);//使能通道1
					TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能预装载
					break;
		default:
					return 0;
	}
		
//	TIM_ARRPreloadConfig(TIM3,ENABLE);//把重载寄存器使能
	TIM_Cmd(TIM3,ENABLE);//使能定时器3	
	return 1;
}
/***************************************************
函数名称：PWM初始化
入口参数：定时器通道CH,占空比Pulse
出口参数：1初始化完成0初始化失败
函数说明：PWM波形输出函数
使用TIM3定时器配置PWM模块
***************************************************/
int PWM_OUT(int TIM_CH,int Pulse)
{
	TIM_OCInitTypeDef PWM_OC;
	PWM_OC.TIM_OCMode = TIM_OCMode_PWM1;//输出模式
	PWM_OC.TIM_OutputState = TIM_OutputState_Enable;//使能或禁止
	PWM_OC.TIM_Pulse = Pulse;//跳变值
	PWM_OC.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平极性
	switch(TIM_CH)
	{
		case 1:
					TIM_OC1Init(TIM3,&PWM_OC);//使能通道1
					TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能预装载
					break;
		case 2:
					TIM_OC2Init(TIM3,&PWM_OC);//使能通道1
					TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能预装载
					break;
		case 3:
					TIM_OC3Init(TIM3,&PWM_OC);//使能通道1
					TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能预装载
					break;
		case 4:
					TIM_OC4Init(TIM3,&PWM_OC);//使能通道1
					TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能预装载
					break;
		default:
					return 0;
	}
	return 1;
}
