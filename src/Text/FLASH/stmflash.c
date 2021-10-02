#include "stmflash.h"
#include "delay.h"
#include "usart.h"
#include "oled.h"

#define MaxPaperNum 170//У׼ʱ���170�ţ�����ʱ169��
unsigned char cThePointOfCalibrate[170]//У׼���־λ
	={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};
unsigned long FDC2214ThresholdValue[MaxPaperNum]//������ֽ�Ŷ�Ӧ����е�ֵ��Ϊ��ֵ
={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};
unsigned long FDC2214CalibrateValue[MaxPaperNum]//���ڷ�У׼��ֵ
={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};

const unsigned long  longValueInROM[170]=  //�����ROM�е���ֵ
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
26305584,26310864,26314752,26317824,26319872,26324168,26330112,26333184,26336256,26342400};//�����󣬸ĳ�У׼�õ�����
 
//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
const u16 flag[] = {0x31};
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	FLASH_Status status = FLASH_COMPLETE;
	for(i=0;i<NumToWrite;i++)
	{
    status = FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	  WriteAddr+=2;//��ַ����2.
	}  
} 
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	FLASH_Unlock();						//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6       64
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)     
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
 
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}
		else {STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);}//д�Ѿ������˵�,ֱ��д������ʣ������.
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	FLASH_Lock();//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void STMFLASH_Write_Halfword(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ������ 
}

//longValueInROM�д�ų���У׼�㣬
//��һ�γ�ʼ��ʱ����longValueInROM���Ƶ�EEPROM�У����24λ��,�������ֵ������FDC2214ThresholdValue
//�ǵ�һ�γ�ʼ��ʱ��EEPROM�е�У׼�㣬�����ֵ������FDC2214ThresholdValue
void FLASHInit(void)		//��У׼���ݴ�FLASH�е��뵽�ڴ���
{
	unsigned char i=0;
   //u16 cTemp0=0,cTemp1=0,cTemp2=0;
	unsigned long longTemp0[1]={0};
	unsigned long longTemp1[1]={0};

	//�Ե�1�������һ���ֽ���Ϊ��־λ
	if(STMFLASH_ReadHalfWord(0x2FE+FLASH_SAVE_ADDR)!= 0x31)//���س���󣬵�һ�����г���ʱ��0x1FF��ַEEPROM��ֵ�϶�����1������Ӧ����0xFF�����س����������õ�ROM����
	{

//		IapErase(0x200);//������2����
		for(i=0;i<170;i++)//��һ�����г��򣬽������洢��EEPROM�е�1��������170�����ݣ�ÿ��3�ֽڣ�һ��510���ֽڣ�ռ��һ������
		{
//			OLED_ShowCN(16,4,3);	
			STMFLASH_Write(FLASH_SAVE_ADDR+i*4,(u16*)(longValueInROM+i),2);    //ÿ��ƫ��4���ֽ�		
			OLED_ShowNum(96,8,i,1);		
		
		}
	
		for(i=1;i<170;i++)//��һ�����г��������ֵ������FDC2214ThresholdValue	
		FDC2214ThresholdValue[i-1]=(longValueInROM[i-1]+longValueInROM[i])/2;//ֻ��ǰ169����ֵ������Ч		
		STMFLASH_Write(0x2FE+FLASH_SAVE_ADDR,(u16*)flag,1);//�ı��־λ
	}
	else//���ǵ�һ�����У�����ݵ�һ��������У׼ֵ������ֵ�浽FDC2214ThresholdValue[i]
	{
//	OLED_ShowCN(0,6,2);
		STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)longTemp0,2);
    delay_ms(1000);
		for(i=1;i<170;i++)
		{//����ǰ������ֵ����һֵ���е���Ϊ�ж���ֵ
     //��ȡ��һ�����У׼ֵ
			STMFLASH_Read(FLASH_SAVE_ADDR+i*4,(u16*)longTemp1,2);	
			
			FDC2214ThresholdValue[i-1]=(longTemp0[0]+longTemp1[0])/2;//ֻ��ǰ169��������Ч	
			longTemp0[0]=longTemp1[0];
		}
	}	
}


//������У׼�ĵ��FDC2214CalibrateValue[]�в�Ϊ0�ĵ㣨EEPROMInit()��FDC2214CalibrateValue[]�ѳ�ʼ��Ϊ0����
//��ԭʼ�������ݣ���ϳ������µ�У׼�㣬����EEPROM��������µ���ֵ������FDC2214ThresholdValue
void FLASHSave(void)		//��У׼���ݴ�XDATA�ڴ汣����EEPROM��
{//������У׼�ĵ㣨����û��170��������ԭʼ�������ݣ���ϳ������µ�У׼�㣬����EEPROM��������µ���ֵ������FDC2214ThresholdValue
	unsigned char i=0,j=0,cTemp1=0,cTemp2=0;
	float fTemp1=0,fTemp2=0;
//	unsigned long longTemp0=0,longTemp1=0,longTemp2=0,longTemp3=0;
	cTemp1=0;
	for(i=1;i<170;i++)//У׼ʱ�����ٲ�1��(��i=0)����һ������
	{
		if(FDC2214CalibrateValue[i]!=0)
		{//�ҳ���0��У׼�㣬���ݴ�У׼�����һ��У׼����ֵ,�����еĲ������ߣ�������м����������ֵ
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
			cTemp1=cTemp2;//���ݽ�
		}
	}	
	FLASH_Unlock();
	FLASH_ErasePage(FLASH_SAVE_ADDR);//����һ������
  FLASH_Lock(); 
	for(i=0;i<170;i++)//��У׼���ݴ洢��EEPROM�е�1��������170�����ݣ�ÿ��3�ֽڣ�һ��510���ֽڣ�ռ��һ������
	{
      STMFLASH_Write(FLASH_SAVE_ADDR+i*4,(u16*)(FDC2214CalibrateValue+i),2);    //ÿ��ƫ��4���ֽ�
			OLED_ShowNum(96,8,i,1);
	}
		  STMFLASH_Write(0x2FE+FLASH_SAVE_ADDR,(u16*)flag,1);//�ı��־λ
		
	for(i=1;i<170;i++)//�����ֵ������FDC2214ThresholdValue	
		  FDC2214ThresholdValue[i-1]=(FDC2214CalibrateValue[i-1]+FDC2214CalibrateValue[i])/2;//ֻ��ǰ169����ֵ������Ч		

	//������󣬸��ݱ�־λ����������ϳ���У׼���ݵ����㣬����ԭʼ���ݵ�
	for(i=0;i<170;i++)
	{
		if((cThePointOfCalibrate[i])==0)FDC2214CalibrateValue[i]=0;
	}		

}

void FLASHReset(void)		//�ָ�����EEPROM����
{	
  FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
	FLASH_ErasePage(FLASH_SAVE_ADDR);//��������
  FLASH_Lock(); 
}











