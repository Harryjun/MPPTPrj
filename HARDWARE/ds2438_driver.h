#ifndef __DS2438_DRIVER_H
#define __DS2438_DRIVER_H


void ds2438_init(void);//��ʼ������
void ReadOnePage(unsigned char PageX,unsigned char *p) ;//��ȡһҳ����
void GetData(void) ;//��ȡ����
void ReadRomCode(void) ;
#endif

