#include "flash_inside.h"

#define  STARTADDR  0x08010000                   	 //STM32F103RB ????????,??? 


/*******************************************************************************
函数名称：flash_write
入口参数：
		u32 StartAddr	flash存储区首地址
		u32 *p_data	数据首地址
		u32 size	数据大小
出口参数：
		0：成功
*******************************************************************************/
u8 flash_write(u32 StartAddr,u32 *p_data,u32 size)
{	
	volatile FLASH_Status FLASHStatus;	
	u32 EndAddr=StartAddr+size*4;	
	vu32 NbrOfPage = 0;	
	u32 EraseCounter = 0x0, Address = 0x0;
	int i;
	int MemoryProgramStatus=1;
	
	FLASH_Unlock();          //去除锁定
	NbrOfPage=((EndAddr-StartAddr)>>10)+1;	
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	FLASHStatus=FLASH_COMPLETE;
	for(EraseCounter=0;(EraseCounter<NbrOfPage)&&(FLASHStatus==FLASH_COMPLETE);EraseCounter++)
	{		
		FLASHStatus = FLASH_ErasePage(StartAddr+(PageSize*EraseCounter));	
	}
	//
	Address = StartAddr;
	i=0;
	while((Address<EndAddr)&&(FLASHStatus==FLASH_COMPLETE))	
	{	FLASHStatus=FLASH_ProgramWord(Address,p_data[i++]);
		Address = Address+4;
		}
	Address = StartAddr;
	i=0;
	while((Address < EndAddr) && (MemoryProgramStatus != 0))
	{	
		if((*(vu32*) Address) != p_data[i++])
		{		MemoryProgramStatus = 0;
				return 1;
		}
		Address += 4;
	}
	return 0;
}

/*******************************************************************************
函数名称：flash_read
入口参数：
		u32 StartAddr	flash存储区首地址
		u32 *p_data	数据首地址
		u32 size	数据大小
出口参数：
		0：成功
*******************************************************************************/
int flash_read(u32 StartAddr,u32 *p_data,u32 size)
{
	u32 EndAddr=StartAddr+size*4;//结束地址
	int MemoryProgramStatus=1;
	u32 Address = 0x0;
	int i=0;
	Address = StartAddr;
	while((Address < EndAddr) && (MemoryProgramStatus != 0))
	{//不断读取
		p_data[i++]=(*(vu32*) Address);
		Address += 4;	
	}
	return 0;	
}
