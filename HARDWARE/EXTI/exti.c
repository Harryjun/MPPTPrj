#include "exti.h"

void exti_init()
{
	//���������ṹ��
//	GPIO_InitTypeDef IRQ;
 	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;//�ⲿ�жϽṹ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//ʱ������
	
	//����IO��
//	IRQ.GPIO_Mode = GPIO_Mode_IPD;//��������
	//IRQ.GPIO_Pin = GPIO_Pin_5;
	//GPIO_Init(GPIOE,&IRQ);
	//����NVIC
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource5);//ѡ���ж���·GPIOE5
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ж�����IRQ5-9
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
	//�����ж�
	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���	
}


void EXTI9_5_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_1);
		EXTI_ClearITPendingBit(EXTI_Line5);//���жϱ�־λ
	}
}
