#ifndef __PWM_H
#define __PWM_H
#include"sys.h"

int PWM_init(int TIM_CH,int Period ,int Pulse);
int PWM_OUT(int TIM_CH,int Pulse);

#endif
