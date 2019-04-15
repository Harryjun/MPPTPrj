/************************************************************** 

MCU:STC89C52,??:12MHz(??11.0592MHz),ds2438???? 

**************************************************************/ 
#include "stm32f10x.h"
#include "ds2438_driver.h"
//#include <intrins.h>
#include "delay.h"
/*ROM Function Commands*/ 
#define  	READ_ROM   0X33 
#define   MATCH_ROM  0X55 
#define   SKIP_ROM   0XCC 
#define   SEARCH_ROM  0XF0 
/*Memory Function Commands*/ 
#define   WRITE_SP   0X4E 
#define   READ_SP   0XBE 
#define   COPY_SP   0X48 
#define   RECALL_MEMORY 0XB8 
#define   CONVERT_T   0X44 
#define   CONVERT_V   0XB4 

//sbit DQ = P2^0;//??DS2438??DQ 
#define DQ1_ON   GPIO_SetBits(GPIOG,GPIO_Pin_2)
#define DQ1_OFF    GPIO_ResetBits(GPIOG,GPIO_Pin_2)
#define DQ  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2)
#define DQ2_ON   GPIO_SetBits(GPIOG,GPIO_Pin_3)
#define DQ2_OFF   GPIO_ResetBits(GPIOG,GPIO_Pin_3)

#define  page(x)   (unsigned char)x 

unsigned char RomCode[8] = {0}; 
/*????ds2438????family code(8 bits),serial number(48 bits),CRC(8 bits)*/ 
unsigned char Page0[9]   = {0}; 
/*????Memory Map??Page 0,????STATUS/CONFIGURATION, 
TEMPERATURE, VOLTAGE,CURRENT,THRESHOLD?CRC??*/ 
unsigned char Page1[9]   = {0}; 
/*????Memory Map??Page 1,????ETM,ICA,OFFSET?CRC??*/ 
void reset_presence(void) 
{ 

 do{ 

  DQ1_OFF; 

  delay_us(60);//6+8*60=486us 

  DQ1_ON; 

  delay_us(8);//6+8*8=70us 

 }while(DQ);//?????presence pulse???? 

 delay_us(51);//6+8*51=414us 

 DQ1_ON; 

} 

 
/*READ A BIT*/ 
/*读一个字*/
unsigned char ReadBit(void) 
{ //  6    9     55
	GPIO_InitTypeDef GPIO_InitStructure;	
	unsigned char Temp; 
	DQ1_OFF; 
	delay_us(22); 
	DQ1_ON; 
	delay_us(86); 

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOG0/1

	

	Temp=DQ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOG0/1
	delay_us(62);//6+8*7=62us 
	return Temp; 
} 
/*READ A BYTE*/ 
/*读一个字节*/
unsigned char ReadByte(void) 
{ 
	unsigned char i,dataTemp=0x00; 
	for(i=0;i<8;i++) 
	{ 
	dataTemp>>=1; 
	if(ReadBit())//???????????? 
	{ 
	 dataTemp|=0x80; 
	} 
	} 
	return dataTemp; 
} 
/*WRITE A BIT*/ 
/*写一个字*/
void WriteBit(unsigned char Bit) 
{ //6    64   60   16
	if(Bit)//Write Bit 1 
	{ 
		DQ1_OFF; 
		delay_us(22); 
		DQ1_ON; 
		delay_us(70);//6+8*8=70us 
	} 
	else 
	{ 
		DQ1_OFF; 
		delay_us(80);//6+8*8=70us 
		DQ1_ON; 
	} 
} 
/*WRITE A BYTE*/ 
/*写一个字*/
void WriteByte(unsigned char dataTemp) 
{ 
	unsigned char i; 
	for(i=0;i<8;i++) 
	{ 
		WriteBit(dataTemp&0x01); 
		dataTemp>>=1; 
	}  
	DQ1_ON; 
} 

/*Write to the scratchpad PageX of the DS2438*/ 
void WriteSP(unsigned char PageX) 
{ 
	reset_presence(); 
	WriteByte(SKIP_ROM); 
	WriteByte(WRITE_SP); 
	WriteByte(PageX); 
} 
/*Read the contents of the scratchpad PageX on the DS2438*/ 
void ReadSP(unsigned char PageX,unsigned char *p) 
{ 
	unsigned char i; 
	reset_presence(); 
	WriteByte(SKIP_ROM); 
	WriteByte(READ_SP); 
	WriteByte(PageX); 
	for(i=0;i<9;i++) 
	{ 
		*p = ReadByte(); 
		p++; 
	} 
} 

/*Copy the scratchpad PageX into the EEPROM/SRAM memory PageX of the DS2438*/ 
void CopySP(unsigned char PageX) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	reset_presence(); 
	WriteByte(SKIP_ROM); 
	WriteByte(COPY_SP); 
	WriteByte(PageX); 
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOG0/1
	while(!DQ);//???????? 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOG0/1

} 

/*Recall the stored values in EEPROM/SRAM PageX to the scratchpad PageX*/ 
void RecallMem(unsigned char PageX) 
{ 
	reset_presence();  
	WriteByte(SKIP_ROM); 
	WriteByte(RECALL_MEMORY); 
	WriteByte(PageX); 
} 
/*Begin a temperature conversion*/ 
void TempConv(void) 
{ GPIO_InitTypeDef GPIO_InitStructure;
	reset_presence(); 
	WriteByte(SKIP_ROM); 
	WriteByte(CONVERT_T); 
	
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOG0/1
	while(!DQ);//???????? 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOG0/1
} 

/*Instruct the DS2438 to initiate a voltage analog-to-digital conversion cycle*/ 
void VoltConv(void) 
{	GPIO_InitTypeDef GPIO_InitStructure;
	reset_presence(); 
	WriteByte(SKIP_ROM); 
	WriteByte(CONVERT_V); 
	

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOG0/1
	while(!DQ);//???????? 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOG0/1

} 
/*初始化函数*/
void ds2438_init(void) 
{ 
	/*初始化引脚端口*/

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);//使能PORTA,PORTE时钟
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOG0/1
	
	WriteSP(page(0)); 
	WriteByte(0X07); //启动电压电流测量
	CopySP(page(0)); //？
} 

/*Read ROM Code*/ 
void ReadRomCode(void) 
{ 
	unsigned char i; 
	reset_presence(); 
	WriteByte(READ_ROM);//??????? 
	for(i=0;i<8;i++) 
	{ 
		RomCode[i]=ReadByte(); 
	} 
} 

 

/*获取某页数据*/
void ReadOnePage(unsigned char PageX,unsigned char *p) 
{
 RecallMem(PageX); 
 ReadSP(PageX,p); 
} 

/*获取全部数据*/
void GetData(void) 
{ 
	TempConv(); 
	VoltConv(); 
	ReadOnePage(page(0),Page0); 
	ReadOnePage(page(1),Page1); 
} 
/***************************************************************************** 

CRC??,CRC=X8+X5+X4+1 

*****************************************************************************/ 

unsigned char Cal_CRC_Onebyte(unsigned char Byte) 
{     
	unsigned char i,crc_1byte=0; 
	for(i=0;i<8;i++) 
	{     
		if(((crc_1byte^Byte)&0x01))//CRC=X8+X5+X4+1 
		{ 
			crc_1byte^=0x18; 
			crc_1byte>>=1; 
			crc_1byte|=0x80; 
		} 
		else 
			crc_1byte>>=1; 
		Byte>>=1; 
	}
	return crc_1byte; 
}   

 


unsigned char Cal_CRC_Bytes(unsigned char *p,unsigned char len) 
{   
uint8_t CRC_c=0; 

while(len--)//len??????????  
{   

     CRC_c=Cal_CRC_Onebyte(CRC_c^*p++); 

 }   

 return CRC_c;//??????CRC?0???????? 

} 


 
 //?????   

unsigned char Read_Current(void)   
{       
	unsigned char c[9];    
	float n;      
	int j =0;

	reset_presence();  

	WriteByte(0xCC); // Skip ROM   
	WriteByte(0xB8); // Read Scratchpad   
	WriteByte(0x00); // Read 0th Page   

	reset_presence();  

	WriteByte(0xCC); // Skip ROM   
	WriteByte(0xBE); // Read Scratchpad   
	WriteByte(0x00); // Read 0th Page   
	for(j=0;j<9;j++)   
	{   
		c[j]=ReadByte();   
	}   
	n=((c[6]&0x03)*256+c[5])/(4096 * 1); //电阻修改   
	return (n);   
}   

 
uint16_t Read_Voltage(void)   
{      
	uint8_t b[9];   
int j = 0;	
	uint16_t m;   
	reset_presence();  
	WriteByte(0xCC); // Skip ROM        WriteByte(0xB4); //WRITE VOLTAGE CORTROL   
	delay_us(100);   
	reset_presence();  
	WriteByte(0xCC); // Skip ROM   
	WriteByte(0xB8); //READ SCRATCHPAD   
	WriteByte(0x00); //READ 0TH PAGE   

	reset_presence();  
	WriteByte(0xCC); // Skip ROM   
	WriteByte(0xBE); //READ SCRATCHPAD   
	WriteByte(0x00); //READ 0TH PAGE   
	for(j=0;j<9;j++)   
	{   
		b[j]=ReadByte();   
	}   
	m=((b[4]&0x03)*256+b[3])*0.01;   
	return (m);   
}   
 
 