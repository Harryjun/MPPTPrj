#include "time.h"
#include "stm32f10x_tim.h"

/***************************************************
�������ƣ�TIMER_init
��ڲ�����Count����
���ڲ�������
����˵����ͨ�ö�ʱ����ʼ������
�ɸ���ʵ���������
Ĭ��ΪTIM2
��ƵΪ10khz0.0001S
***************************************************/
void TIMER_init(int Count)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1, ENABLE); //ʱ��ʹ��
	//���ö�ʱ��
	TIM_TimeBaseStructure.TIM_Period = Count-1;  //��װֵ��
	TIM_TimeBaseStructure.TIM_Prescaler = 7200-1; //��Ƶϵ����72M/7200=10KHz,������������
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);     //��������ֵд���Ӧ�Ĵ���
  //�ж�����
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);            //ʹ�ܻ���ʧ��ָ����TIM�ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;            //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                 //��������ֵд���Ӧ�Ĵ�

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����	
}


