#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

#define ADC1_DR_adress ((u32)0x40012400+0x4c)
int ADC_init(void);
 
#endif 
