#include "time.h"
#include "stm32f10x_tim.h"

/***************************************************
函数名称：TIMER_init
入口参数：Count计数
出口参数：无
函数说明：通用定时器初始化函数
可根据实际需求更改
默认为TIM2
分频为10khz0.0001S
***************************************************/
void TIMER_init(int Count)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1, ENABLE); //时钟使能
	//配置定时器
	TIM_TimeBaseStructure.TIM_Period = Count-1;  //重装值，
	TIM_TimeBaseStructure.TIM_Prescaler = 7200-1; //分频系数，72M/7200=10KHz,其他依此类推
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);     //把上述数值写入对应寄存器
  //中断配置
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);            //使能或者失能指定的TIM中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;            //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                 //把上述数值写入对应寄存

	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设	
}


