#ifndef __FLASH_INSIDE_H
#define __FLASH_INSIDE_H
#include"sys.h"

#include <stm32f10x.h>
#include "stm32f10x_flash.h"

#define PageSize 4
int flash_read(u32 StartAddr,u32 *p_data,u32 size);
u8 flash_write(u32 StartAddr,u32 *p_data,u32 size);

#endif
