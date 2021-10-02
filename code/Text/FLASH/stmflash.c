#include "stmflash.h"
#include "delay.h"
#include "usart.h"
#include "oled.h"

#define MaxPaperNum 170//校准时最多170张，测量时169张
unsigned char cThePointOfCalibrate[170]//校准点标志位
	={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};
unsigned long FDC2214ThresholdValue[MaxPaperNum]//各数量纸张对应点的中点值作为阈值
={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};
unsigned long FDC2214CalibrateValue[MaxPaperNum]//用于放校准的值
={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};

const unsigned long  longValueInROM[170]=  //存放在ROM中的阈值
{18164736,18887680,19426304,19871744,20256768,20636672,20975616,21270528,21553152,21801984,
22028288,22241280,22446080,22627328,22792192,22958080,23097344,23236608,23361536,23486464,
23588864,23698432,23803904,23884800,23964672,24062976,24129536,24206336,24273920,24342528,
24400896,24466432,24524800,24577024,24623104,24681472,24723712,24768512,24808448,24859648,
24893440,24932352,24972288,25003008,25040896,25071616,25106432,25134080,25163776,25189376,
25218048,25245696,25274368,25300992,25323776,25340928,25362432,25391104,25412608,25432064,
25455616,25472000,25496576,25512960,25526272,25549824,25565184,25583616,25597952,25614336,
25628672,25646080,25660416,25672704,25691136,25702400,25718784,25733120,25744384,25756672,
25768960,25783296,25796608,25809920,25821184,25830400,25839616,25849856,25862144,25874432,
25884672,25894912,25904128,25914368,25924864,25932800,25940992,25952256,25957376,25968640,
25977856,25986048,25993216,26000384,26008576,26017792,26024960,26031104,26040320,26046464,
26053632,26062848,26066944,26074112,26080256,26086648,26091520,26098688,26104832,26114048,
26120192,26124288,26131456,26136528,26141696,26148864,26153984,26161200,26165596,26169344,
26174464,26180608,26186752,26191872,26198016,26201088,26206208,26211400,26215424,26221568,
26228736,26231808,26237952,26241024,26244096,26248192,26254136,26257408,26262528,26265600,
26269720,26272768,26277888,26282708,26285056,26288076,26292320,26295696,26298368,26302464,
26305584,26310864,26314752,26317824,26319872,26324168,26330112,26333184,26336256,26342400};//采样后，改成校准用的数据
 
//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
const u16 flag[] = {0x31};
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	FLASH_Status status = FLASH_COMPLETE;
	for(i=0;i<NumToWrite;i++)
	{
    status = FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	  WriteAddr+=2;//地址增加2.
	}  
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	FLASH_Unlock();						//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6       64
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)     
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
 
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}
		else {STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);}//写已经擦除了的,直接写入扇区剩余区间.
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void STMFLASH_Write_Halfword(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个半字 
}

//longValueInROM中存放出厂校准点，
//第一次初始化时，从longValueInROM复制到EEPROM中（存高24位）,并算出阈值，存入FDC2214ThresholdValue
//非第一次初始化时，EEPROM中的校准点，算出阈值，存入FDC2214ThresholdValue
void FLASHInit(void)		//将校准数据从FLASH中调入到内存中
{
	unsigned char i=0;
   //u16 cTemp0=0,cTemp1=0,cTemp2=0;
	unsigned long longTemp0[1]={0};
	unsigned long longTemp1[1]={0};

	//以第1扇区最后一个字节作为标志位
	if(STMFLASH_ReadHalfWord(0x2FE+FLASH_SAVE_ADDR)!= 0x31)//下载程序后，第一次运行程序时，0x1FF地址EEPROM的值肯定不是1，正常应该是0xFF（下载程序会擦除不用的ROM区域）
	{

//		IapErase(0x200);//擦除第2扇区
		for(i=0;i<170;i++)//第一次运行程序，将出厂存储于EEPROM中第1个扇区，170个数据，每个3字节，一共510个字节，占第一个扇区
		{
//			OLED_ShowCN(16,4,3);	
			STMFLASH_Write(FLASH_SAVE_ADDR+i*4,(u16*)(longValueInROM+i),2);    //每次偏移4个字节		
			OLED_ShowNum(96,8,i,1);
     			
		
		}
	
		for(i=1;i<170;i++)//第一次运行程序，算出阈值，存入FDC2214ThresholdValue	
		FDC2214ThresholdValue[i-1]=(longValueInROM[i-1]+longValueInROM[i])/2;//只有前169个阈值数据有效		
		STMFLASH_Write(0x2FE+FLASH_SAVE_ADDR,(u16*)flag,1);//改变标志位
	}
	else//不是第一次运行，则根据第一个扇区的校准值计算阈值存到FDC2214ThresholdValue[i]
	{
//	OLED_ShowCN(0,6,2);
		STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)longTemp0,2);
    delay_ms(1000);
		for(i=1;i<170;i++)
		{//将当前读出的值与上一值到中点作为判断阈值
     //读取后一组出厂校准值
			STMFLASH_Read(FLASH_SAVE_ADDR+i*4,(u16*)longTemp1,2);	
			
			FDC2214ThresholdValue[i-1]=(longTemp0[0]+longTemp1[0])/2;//只有前169个数据有效	
			longTemp0[0]=longTemp1[0];
		}
	}	
}


//根据已校准的点据FDC2214CalibrateValue[]中不为0的点（EEPROMInit()中FDC2214CalibrateValue[]已初始化为0），
//及原始出厂数据，拟合出所有新的校准点，存入EEPROM，并算出新的阈值，存入FDC2214ThresholdValue
void FLASHSave(void)		//将校准数据从XDATA内存保存至EEPROM中
{//根据已校准的点（可能没有170个），及原始出厂数据，拟合出所有新的校准点，存入EEPROM，并算出新的阈值，存入FDC2214ThresholdValue
	unsigned char i=0,j=0,k=0,cTemp1=0,cTemp2=0;
	float fTemp1=0,fTemp2=0;
//	unsigned long longTemp0=0,longTemp1=0,longTemp2=0,longTemp3=0;
	cTemp1=0;
	for(i=1;i<170;i++)//校准时，至少测1张(即i=0)和另一个数量
	{
		if(FDC2214CalibrateValue[i]!=0)
		{//找出非0的校准点，根据此校准点和上一点校准点数值,及已有的测量曲线，推算出中间区域各点数值
			cTemp2=i;
			for(j=cTemp1+1;j<cTemp2;j++)
			{//INT(($B2-$B$1)*(H$21-H$1)/($B$21-$B$1)+H$1)
				fTemp1=(float)(FDC2214CalibrateValue[cTemp2]-FDC2214CalibrateValue[cTemp1]);
				fTemp2=(float)(longValueInROM[cTemp2]-longValueInROM[cTemp1]);
				fTemp1=fTemp1/fTemp2;
				fTemp2=(float)	(longValueInROM[j]-longValueInROM[cTemp1]);
				fTemp1=fTemp1*fTemp2;
				FDC2214CalibrateValue[j]= (unsigned long)fTemp1+FDC2214CalibrateValue[cTemp1];//
				
			}
			cTemp1=cTemp2;//向后递进
		}
	}	
	FLASH_Unlock();
	FLASH_ErasePage(FLASH_SAVE_ADDR);//擦除一个扇区
  FLASH_Lock();
  OLED_ShowStr(0,6,"                ",2);//		
	for(i=0;i<170;i++)//将校准数据存储于EEPROM中第1个扇区，170个数据，每个3字节，一共510个字节，占第一个扇区
	{
      STMFLASH_Write(FLASH_SAVE_ADDR+i*4,(u16*)(FDC2214CalibrateValue+i),2);    //每次偏移4个字节
			//OLED_ShowNum(96,8,i,1);
		
		 OLED_ShowStr((int)(i*104/170),6,">>>",2);
	}
		  
	OLED_ShowStr(0,2,"                ",2);
	OLED_ShowStr(0,6,"                ",2);
	
	OLED_ShowStr(40,0,"Saved!",2);
	delay_ms(1000);
	
	STMFLASH_Write(0x2FE+FLASH_SAVE_ADDR,(u16*)flag,1);//改变标志位
		
	for(i=1;i<170;i++)//算出阈值，存入FDC2214ThresholdValue	
		  FDC2214ThresholdValue[i-1]=(FDC2214CalibrateValue[i-1]+FDC2214CalibrateValue[i])/2;//只有前169个阈值数据有效		

	//处理完后，根据标志位，将线性拟合出的校准数据点清零，保留原始数据点
	for(i=0;i<170;i++)
	{
		if((cThePointOfCalibrate[i])==0)FDC2214CalibrateValue[i]=0;
	}		

}

void FLASHReset(void)		//恢复出厂EEPROM数据
{	
  FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
	FLASH_ErasePage(FLASH_SAVE_ADDR);//擦除扇区
  FLASH_Lock(); 
}











