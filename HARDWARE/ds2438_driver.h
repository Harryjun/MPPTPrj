#ifndef __DS2438_DRIVER_H
#define __DS2438_DRIVER_H


void ds2438_init(void);//初始化函数
void ReadOnePage(unsigned char PageX,unsigned char *p) ;//获取一页数据
void GetData(void) ;//获取数据
void ReadRomCode(void) ;
#endif

