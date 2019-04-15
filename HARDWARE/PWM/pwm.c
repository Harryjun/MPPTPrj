/******************************************************
�������ƣ�PWM���β���
���ߣ��������Ϸ
��дʱ�䣺2015.8.5
��ע��
TIM3��ʱ��
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
�������ƣ�PWM��ʼ��
��ڲ�������ʱ��ͨ��CH,����Periodռ�ձ�Pulse
���ڲ�����1��ʼ�����0��ʼ��ʧ��
����˵����PWM���β�������
ʹ��TIM3��ʱ������PWMģ��
CH1:PA6
CH2:PA7
Ƶ�ʣ�10KHZ   --- 7199
���ڣ�Period/10 ms   ---  7199
			Period    us   ---  71
ռ�ձȣ�Pulse 0-Period
ע��δ��Ƶ������
***************************************************/
int PWM_init(int TIM_CH,int Period ,int Pulse)
{
	TIM_TimeBaseInitTypeDef PWM_1;//ʱ��
	TIM_OCInitTypeDef PWM_OC;//���
	GPIO_InitTypeDef PWM;//PWM;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5    
 
  
	PWM.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; //TIM_CH2
	PWM.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	PWM.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &PWM);//��ʼ��GPIO
 
   //��ʼ��TIM3
	PWM_1.TIM_Period = Period; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	PWM_1.TIM_Prescaler = 1;//1;//71; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	PWM_1.TIM_ClockDivision = //0; //����ʱ�ӷָ�:TDTS = Tck_tim
														TIM_CKD_DIV1;
	PWM_1.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &PWM_1); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	PWM_OC.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	PWM_OC.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	PWM_OC.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	PWM_OC.TIM_Pulse = Pulse;


	switch(TIM_CH)
	{
		case 1:
					TIM_OC1Init(TIM3,&PWM_OC);//ʹ��ͨ��1
					TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��Ԥװ��
					break;
		case 2:
					TIM_OC2Init(TIM3,&PWM_OC);//ʹ��ͨ��1
					TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��Ԥװ��
					break;
		case 3:
					TIM_OC3Init(TIM3,&PWM_OC);//ʹ��ͨ��1
					TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��Ԥװ��
					break;
		case 4:
					TIM_OC4Init(TIM3,&PWM_OC);//ʹ��ͨ��1
					TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��Ԥװ��
					break;
		default:
					return 0;
	}
		
//	TIM_ARRPreloadConfig(TIM3,ENABLE);//�����ؼĴ���ʹ��
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܶ�ʱ��3	
	return 1;
}
/***************************************************
�������ƣ�PWM��ʼ��
��ڲ�������ʱ��ͨ��CH,ռ�ձ�Pulse
���ڲ�����1��ʼ�����0��ʼ��ʧ��
����˵����PWM�����������
ʹ��TIM3��ʱ������PWMģ��
***************************************************/
int PWM_OUT(int TIM_CH,int Pulse)
{
	TIM_OCInitTypeDef PWM_OC;
	PWM_OC.TIM_OCMode = TIM_OCMode_PWM1;//���ģʽ
	PWM_OC.TIM_OutputState = TIM_OutputState_Enable;//ʹ�ܻ��ֹ
	PWM_OC.TIM_Pulse = Pulse;//����ֵ
	PWM_OC.TIM_OCPolarity = TIM_OCPolarity_High;//��Ч��ƽ����
	switch(TIM_CH)
	{
		case 1:
					TIM_OC1Init(TIM3,&PWM_OC);//ʹ��ͨ��1
					TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��Ԥװ��
					break;
		case 2:
					TIM_OC2Init(TIM3,&PWM_OC);//ʹ��ͨ��1
					TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��Ԥװ��
					break;
		case 3:
					TIM_OC3Init(TIM3,&PWM_OC);//ʹ��ͨ��1
					TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��Ԥװ��
					break;
		case 4:
					TIM_OC4Init(TIM3,&PWM_OC);//ʹ��ͨ��1
					TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��Ԥװ��
					break;
		default:
					return 0;
	}
	return 1;
}
