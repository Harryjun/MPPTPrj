#include "exti.h"

void exti_init()
{
	//声明两个结构体
//	GPIO_InitTypeDef IRQ;
 	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;//外部中断结构体
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//时钟配置
	
	//配置IO口
//	IRQ.GPIO_Mode = GPIO_Mode_IPD;//上啦输入
	//IRQ.GPIO_Pin = GPIO_Pin_5;
	//GPIO_Init(GPIOE,&IRQ);
	//配置NVIC
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource5);//选择中断线路GPIOE5
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//中断向量IRQ5-9
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//响应优先级
	NVIC_Init(&NVIC_InitStructure);
	//配置中断
	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器	
}


void EXTI9_5_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_1);
		EXTI_ClearITPendingBit(EXTI_Line5);//清中断标志位
	}
}
