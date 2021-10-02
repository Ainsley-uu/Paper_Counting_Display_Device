#include "stm32f10x.h"
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "stdio.h"
#include "usart.h"
#include "key.h"
#include <fdc2214.h>
#include <oled.h>
#include <niming.h>
#include <SortArr.h>
#include "time5.h"
#include "stmflash.h"

//#define Buzzer 	PAout(8)
#define ShortCircuitValue 0x04000000//�������·���ж���ֵ
void yuyin(unsigned int cMesuare);
void BuzzerBeep(void);

//����ֵ�������
float temp0=0,temp1=0,temp2=0,temp3=0;//��·ͨ��ֵ��ŵı���
float Total_Value;//��·ͨ����ֵ
void GetFouChVal(void);//��ȡ��·ͨ��ֵ�ĺ���
//extern u8 bS1Rec;//���ڽ��յ����ݱ�־
u8 bFlag3s;//3s��ʱ��־	
unsigned int  T5Counter3s;//3s������	
//extern unsigned char cS1Rec;//���ڽ��յ��ַ�
extern unsigned long  FDC2214ThresholdValue[];//������ֽ�Ŷ�Ӧ����е�ֵ��Ϊ��ֵ
extern unsigned long  FDC2214CalibrateValue[];//���ڷ�У׼��ֵ
extern unsigned char  cThePointOfCalibrate[];//У׼���־λ	
//u8 bFlagReflashFDC2214;//���ڵ�1����ͨ��ֵ��ʾˢ������
unsigned char  cKeyPressNumber=0;//�洢5�򰴼�ֵ
extern u8 AsciiBuff[14];
extern u8 AsciiBuff3[3];
unsigned long  FDC2214CH[4]={0,0,0,0};	//��Ŷ�ȡ��FDC2214ֵ��4ͨ��
unsigned char  i=0,cCalibrateNum=1;//У׼��������ʾ������
unsigned int  intTemp=0,cMeasureNum=255;	//������������ʾ������
u8 bFunctionIndex=0;//����ָʾ��־��0ΪУ׼��1Ϊ����
u8 bIndicatorIcon=0;	//����ͼ����ʽ��0Ϊ?��1ΪOK�����Ǻ��ֱ�����18+bIndicatorIcon
u8 bShowPaperNum=0;//��ʾ����ֽ��������־
u8 bReCalibrate=0;//��У׼ѡ���а��¹��м��ȷ�ϣ�˵��У׼�����иı䣬��λΪ1
u8 bCalibrateValueNoSaved=0;//���ڵ�ǰУ׼���ݱ����־
u8 bSelectSaveItOrNot=0;//�뿪У׼ѡ��ʱ����ʾȷ���Ƿ�Ҫ�������ݽ���
u8 bSaveCalibrateData=0;//����У׼����
u8 bCalibrateDataReset=0;//У׼���ݻָ��������ñ�־	
unsigned int JudgePaperNum(unsigned long longFDC2214Value);


int main()
{

		SystemInit();
	  delay_init();	    	                             //��ʱ������ʼ��	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�����ж����ȼ�����2
    uart_init(115200);	                             //���ڳ�ʼ��Ϊ115200
    TIM2_init(79,7199);                              //10ms�ж�
    IIC_Start();                                     //IIC��ʼ��
    Oled_Init();
    Beep_Init();	//oled��ʼ��
    LED_Init();	                                     //led��ʼ��
    Key_Init();                                      //��ʼ�����򰴼��Ϳ��ذ�������ʹ��Key_Scan���������õ�����ֵ
  //key_exti_test();                                 //�������    
		FLASHInit();
		uart2_init(9600)  ;

		Usart_SendString(USART2,"AF:28");//��������
    delay_ms(3);                 
    //OLED_PaperNumTitle();   
		FDC_IIC_Init();	
//**********�����û�����Ӻ�FDC2214***************
	FDC2214_Init();
	intTemp=FDC_Read(MANUFACTURER_ID);
//	if(intTemp!=0x5449)
//		OLED_Fill(0x00);

//		Usart_SendString(USART2,"B7:14");//ֽ�Ų��Կ�ʼ
  BuzzerBeep();

  OLED_Fill(0x00); 		
	if(intTemp!=0x5449)
	{	
		Usart_SendString(USART2,"A7:00019");//�����Ӻô�����
		while(intTemp!=0x5449)
		{
				
				OLED_ShowStr(0,0,"Have no FDC2214!",2);
				OLED_ShowStr(0,2,"Please connect",2);		
				OLED_ShowStr(0,4,"it correctly!",2);
				intTemp=FDC_Read(MANUFACTURER_ID);
		}
	}
//************************************************

	OLED_PaperNumTitle();    //����
  delay_ms(5);  	
	Usart_SendString(USART2,"A7:00014");//����ǰ��У׼ֽ��
  while(1)
	{
			cKeyPressNumber = Key_Scan();  //��ⰴ��//��ȡ����ֵ
			if(cKeyPressNumber)//�м����£��ж�Ϊ�ĸ������£������߼����ܱ�ʶ��־λ
			{
				
				switch(cKeyPressNumber) //���ݰ���ִ�� 
				{ 
					case 1:// KeyStartStop		
					{
						//��������3�룬�ָ�ԭʼУ׼����
						//Ҫ��3���ж�
						if(bFlag3s==0)bFlag3s=1;
						if(bFlag3s==1 && T5Counter3s>600)
						{
							bCalibrateDataReset=1;//�ָ�ԭʼУ׼���ݱ�־
							bFlag3s=0;
							T5Counter3s=0;
//							OLED_ShowStr(0,2,"                ",2);//����2,3,4����ʾ
//							OLED_ShowStr(0,4,"                ",2);//			
//							OLED_ShowStr(0,6,"                ",2);//		

//							OLED_ShowStr(32,2,"Cleared",2);//?
//							OLED_ShowStr(0,2,"                ",2);//����2,3,4����ʾ
						}
						
						//S1SendData(0x00);
						break;//���ڷ���	; 			
					}
					case 2:// KeyUp
					{
						//S1SendData(0x05);//���ڷ���	; 
						if(bFunctionIndex==1)	//��ǰ�ڲ�������
						{
							//����
							Usart_SendString(USART2,"A7:00017");//����У׼
							bFunctionIndex=0;
							bShowPaperNum=0;
						//	cCalibrateNum=1;//У׼���������ֹ�1
							bIndicatorIcon=0;//��ʾ������ʾ��ǰ���ݵ�δ��¼У׼ֵ							
						}
						else;//��У׼������
						break;
					}
					case 3://KeyDown
					{
						
						if(bFunctionIndex==0)//��У׼�����У������¼�
						{
			
							if(bReCalibrate==1)//���У׼�����ݣ�˵��У׼�����������Ի���Ҫ��ȷ���Ƿ񱣴�У׼���ݣ�
							{
							
								bSelectSaveItOrNot=1;//�Ƿ���ʾ����ѡ����־
								
						//		cCalibrateNum=1;//У׼���������ֹ�1
							
							}
							else//���û��У׼�����ݣ���ֱ������ǰ��У׼���ݲ���
							{
								bFunctionIndex=1;//����Ƶ�����ѡ��
								Usart_SendString(USART2,"A7:00014");//����У׼
							}
						}
					//	S1SendData(0x01);//���ڷ���	; 

						break;
					}
					case 4:// KeyLeft
					{
						if(bFunctionIndex==0 & cCalibrateNum>1)//�˵�ѡ��ΪУ׼����
						{
							cCalibrateNum--;
						}
	//					S1SendData(0x04);
						bIndicatorIcon=0;//��ʾ������ʾ��ǰ���ݵ�δ��¼У׼ֵ
						break;//���ڷ���	;  
						
					}
					case 5://	KeyRight
					{
						if(bFunctionIndex==0 & (cCalibrateNum<=170))//�˵�ѡ��ΪУ׼����
						{
							cCalibrateNum++;
						}
	//					S1SendData(0x03);//���ڷ���	; 
						bIndicatorIcon=0;//��ʾ������ʾ��ǰ���ݵ�δ��¼У׼ֵ					
						break;

					}
					case 6:// KeyMiddle 
					{
						if(bFunctionIndex==0)//��У׼������
						{
							bReCalibrate=1;//��ʶ�����ݱ�У׼��
							bCalibrateValueNoSaved=1;//��ʶ��ǰ����δ������
							//����
               Usart_SendString(USART2,"A7:00020");//��ȷ��
							 BuzzerBeep();//��������һ��
						}
						else////�˵�ѡ��Ϊ��������
						{
							bShowPaperNum=1;
//							Usart_SendString(USART2,"A7:00016");//������ʼ����
						}
					
						bIndicatorIcon=1;//����ͼ����ʽ��ΪOK�������У׼�У���ʾ��ǰ������У׼������ڲ����У���ʾ��ȷ����ʾ����
						BuzzerBeep();	
						
						//S1SendData(0x02);//���ڷ���	; 
						break;
					}

					default:
					break;//
				}	
				cKeyPressNumber=0;//����ֵ������Ĭ�ϰ���̧��
			
			}
			
      if(!cKeyPressNumber)//û�а�������ʱ��ˢ����ʾ���м�����ʱ��OLED��ˢ�¡���������������OLEDˢ�ºͶ�ȡFDC2214ռ��CPUʱ�䣬������Ӧ������
			{
				
				if(bCalibrateDataReset==1)//�ָ�ԭʼУ׼���ݱ�־
				{
					bCalibrateDataReset=0;
					bIndicatorIcon=0;
					Usart_SendString(USART2,"A7:00018");//�ָ��������ݣ�Ҫ����У׼//����	
				  BuzzerBeep();	BuzzerBeep();	
					FLASHReset();
				}
				if(bSelectSaveItOrNot==1)//��ʾ��������ѡ����棬��У׼ѡ�����뿪����Ҫ����У׼����
				{
					Usart_SendString(USART2,"A7:00015");//�Ƿ񱣴�У׼����
					
					//����			
					OLED_ShowStr(0,2,"                ",2);//����2,3,4����ʾ
					OLED_ShowStr(0,4,"                ",2);//			
					OLED_ShowStr(0,6,"                ",2);//		

					//���ñ���У׼���ݵĽ���
					OLED_ShowCN(0,2,21);	//��			
					OLED_ShowCN(16,2,22);//��
					OLED_ShowCN(32,2,23);//��	
					OLED_ShowCN(48,2,24);//��	
					OLED_ShowCN(64,2,3);	//��
					OLED_ShowCN(80,2,25);//��	
					OLED_ShowStr(96,2,"?",2);//?
					OLED_ShowCN(16,6,21);						
					OLED_ShowCN(64,6,27);

						
					
					while(cKeyPressNumber!=0x06)//�ȴ�ȷ�ϱ���򲻱���
					{
						cKeyPressNumber = Key_Scan();  //��ⰴ��//��ȡ����ֵ
						if(cKeyPressNumber==4)//�����
						{
								OLED_ShowCN(16,6,26);						
								OLED_ShowCN(64,6,22);
								bSaveCalibrateData=1;//����
						}
						else if(cKeyPressNumber==5)//���Ҽ�
						{
							
								OLED_ShowCN(16,6,21);						
								OLED_ShowCN(64,6,27);
								bSaveCalibrateData=0;//������
						}							
					}						

						
	
					if(bSaveCalibrateData==1)//ѡ�񱣴�
					{
//					cCalibrateNum=1;//��ʾ��У׼������0
						FLASHSave();			//	��У׼���ݱ��浽FLASH��һҳ��	
						bFunctionIndex=1;//����Ƶ�����ѡ��
						bReCalibrate=0;	//����У׼��־								
					}
					else
					{
						Usart_SendString(USART2,"A7:00014");//����ǰ��У׼ֽ��
						bFunctionIndex=0;//�������У׼ѡ��
					}
					
					bSelectSaveItOrNot=0;//������ʾ����Ի���					
					cKeyPressNumber=0;//����ֵ������Ĭ�ϰ���̧��					
				
					OLED_PaperNumTitle();//�ָ���ʾ������
					bIndicatorIcon=0;//��ʾ��Ȧ		
					
				}
				else//��ʾ����������	
				{
					FDC2214CH[1]=FCD2214_ReadCH(1);
//						S1SendData(FDC2214CH[1]>>24);
//						S1SendData(FDC2214CH[1]>>16);						
//						S1SendData(FDC2214CH[1]>>8);	
//						S1SendData(FDC2214CH[1]);	
					if(FDC2214CH[1]>ShortCircuitValue)//FDC2214��·
					{	
						//����S2SendString("A7:00013");				
						
						OLED_Fill(0x00);
						OLED_ShowCN(0,0,8);		//��·��ʾ		
						OLED_ShowCN(16,0,9);	
						Usart_SendString(USART2,"A7:00013");//��·
						while(FDC2214CH[1]>ShortCircuitValue)
						{
							
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++						
							
							FDC2214CH[1]=FilteredFCD2214ReadCH(1);
							//FDC2214CH[1]=FCD2214ReadCH(1);
							BuzzerBeep();//��������һ��
							//S1SendData(0x01);
						}//����ͣ������ֱ����·���
						 //Beep=1;
						//S1SendData(0x02);
						OLED_PaperNumTitle();//������ʾԭ���Ľ���							
					}
					else//FDC2214���岻��·
					{	
						Data2ASCII(FDC2214CH[1]);
						OLED_ShowStr(34,6,AsciiBuff,2);	//���һ����ʾ��ǰFDC2214�����ת����8λʮ��������ʾ
						if(bFunctionIndex==0 )//��У׼�����У�
						{
								
							if(bCalibrateValueNoSaved==1)//�������ݱ�У׼
							 {
									//S2SendString("A7:00017");	
									//+++++++++��ȡFDC2214ֵ����[cCalibrateNum-1]Ϊ�±꣬��������+++++++++++++			
									cThePointOfCalibrate[cCalibrateNum-1]=1;//�ı��־λ 
									FDC2214CalibrateValue[cCalibrateNum-1]=FilteredFCD2214ReadCH(1);//������뾭���˲���У׼ֵ								
									bCalibrateValueNoSaved=0;//��ǰ�����ѱ�����										 
							 }							
							 Data2ASCII3Figure(cCalibrateNum);
//							 yuyin(cCalibrateNum);
							 OLED_ShowStr(48,2,AsciiBuff3,2);	//��ʾУ׼����
							 
							 OLED_ShowCN(112,4,20);	//���ݵ�ǰ���ܱ�־������ԭ�����	
							 OLED_ShowCN(112,2,18+(bIndicatorIcon?1:0));	//����bIndicatorIconֵ��Ϊ1��ʾOK ��Ϊ0��ʾ��
							 OLED_ShowStr(48,4,"   ",2);//����������������ʾ������				 				 							 
						}
						else//�ڲ��������У�
						{
							
							if(bShowPaperNum==1)//��ʾ��������
							{
								//++++++++++++++++
								//+++++++++++++++++
								//����FDC2214CH[1]�ж�������
								
										
								cMeasureNum=JudgePaperNum(FilteredFCD2214ReadCH(1));
//								Usart_SendString(USART2,"B7:0102");
								//S2SendString(Num2Speaker(cMeasureNum));//����������
								
								bIndicatorIcon=1;							
								//P55=0;
								OLED_ShowStr(48,4,"   ",2);//������ʾ������
							  OLED_ShowCN(112,4,18+(bIndicatorIcon?1:0));	//����bIndicatorIconֵ��Ϊ1��ʾOK��Ϊ0��ʾ��								
								delay_ms(500);
								//+++++++++++++++��ʾ��������������������ʾ++++++++++++++++++
							if(cMeasureNum!=999)					yuyin(cMeasureNum);
								Data2ASCII3Figure(cMeasureNum);
								OLED_ShowStr(48,4,AsciiBuff3,2);		//��ʾ��������	
								//P55=1;
								bIndicatorIcon=0;
								bShowPaperNum=0;
							}
							OLED_PaperNumTitle();//������ʾԭ���Ľ���									
							OLED_ShowCN(112,2,20);	//���ݵ�ǰ���ܱ�־������ԭ�����	
							OLED_ShowCN(112,4,18+(bIndicatorIcon?1:0));	//����bIndicatorIconֵ��Ϊ1��ʾOK��Ϊ0��ʾ��																	
							OLED_ShowStr(48,2,"   ",2);//����У׼��������ʾ������						

						}
					}

				}
			}
}
}
/**
**��ʱ��5�жϺ��� 10ms
**�ڶ�ʱ���вɼ���·ͨ��ֵ����������ֵ
**/
void TIM2_IRQHandler(void)         
{
	 
	if(TIM2->SR&0X0001)//����ж�
	{	
    //temp1= Cap_Calculate(1);
		if(bFlag3s==1) T5Counter3s++;
	}						   
  TIM2->SR&=~(1<<0);
}

//���ݶ����FDC2214ֵ����FDC2214ThresholdValue[170]

unsigned int JudgePaperNum(unsigned long longFDC2214Value)
{
	extern unsigned long FDC2214ThresholdValue[170];//������ֽ�Ŷ�Ӧ����е�ֵ��Ϊ��ֵ
	unsigned char i=0;
	if(longFDC2214Value < FDC2214ThresholdValue[0])
		return(1);
	else
		for(i=1;i<169;i++)
		{
			if((longFDC2214Value >= FDC2214ThresholdValue[i-1]) && (longFDC2214Value < FDC2214ThresholdValue[i]) )
				return(i+1);//����ֽ�����ӣ�������ֵ���ӣ�"<"
		}	
	Usart_SendString(USART2,"A7:00021");		
	return(999);//��ʾ999��ʾ������Χ��

}

void yuyin(unsigned int cMesuare)
{
	int shiFlag=0;
	int baiFlag=0;
	char yu[20];
//	int BigFlag=0;
	int num=3;
	yu[0]='B';
	yu[1]='7';
	yu[2]=':';
//	yu[3]='1';//��ǰֽ����ĿΪ
//	yu[4]='5';
	
	
	
	
	if((( u8 )(cMesuare/100)%10)) baiFlag=1;
	if((( u8 )(cMesuare/10)%10)) shiFlag=1;
	
	
		switch((( u8 )(cMesuare/100)%10))
		{
//			case 0:{yu[num++]='1';yu[num++]='3';break;}
			case 1:{yu[num++]='0';yu[num++]='1';break;}
			case 2:{yu[num++]='0';yu[num++]='2';break;}
			case 3:{yu[num++]='0';yu[num++]='3';break;}
			case 4:{yu[num++]='0';yu[num++]='4';break;}
			case 5:{yu[num++]='0';yu[num++]='5';break;}
			case 6:{yu[num++]='0';yu[num++]='6';break;}
			case 7:{yu[num++]='0';yu[num++]='7';break;}
			case 8:{yu[num++]='0';yu[num++]='8';break;}
			case 9:{yu[num++]='0';yu[num++]='9';break;}

		}
		
		if(baiFlag){
			yu[num++]='1';yu[num++]='1';//��
		}
			switch((( u8 )(cMesuare/10)%10))
		{
//			case 0:{yu[num++]='1';yu[num++]='3';break;}
			case 1:{yu[num++]='0';yu[num++]='1';break;}
			case 2:{yu[num++]='0';yu[num++]='2';break;}
			case 3:{yu[num++]='0';yu[num++]='3';break;}
			case 4:{yu[num++]='0';yu[num++]='4';break;}
			case 5:{yu[num++]='0';yu[num++]='5';break;}
			case 6:{yu[num++]='0';yu[num++]='6';break;}
			case 7:{yu[num++]='0';yu[num++]='7';break;}
			case 8:{yu[num++]='0';yu[num++]='8';break;}
			case 9:{yu[num++]='0';yu[num++]='9';break;}
		}
		
		if(shiFlag)
		{
			yu[num++]='1';yu[num++]='0';//ʮ
		}
			
		
			switch((( u8 )(cMesuare)%10))
		{
//			case 0:{yu[num++]='1';yu[num++]='3';break;}
			case 1:{yu[num++]='0';yu[num++]='1';break;}
			case 2:{yu[num++]='0';yu[num++]='2';break;}
			case 3:{yu[num++]='0';yu[num++]='3';break;}
			case 4:{yu[num++]='0';yu[num++]='4';break;}
			case 5:{yu[num++]='0';yu[num++]='5';break;}
			case 6:{yu[num++]='0';yu[num++]='6';break;}
			case 7:{yu[num++]='0';yu[num++]='7';break;}
			case 8:{yu[num++]='0';yu[num++]='8';break;}
			case 9:{yu[num++]='0';yu[num++]='9';break;}
		}
	yu[num++]='1';yu[num++]='2';
	
				Usart_SendString(USART2,yu);
	
}

void BuzzerBeep(void)
{
		Beep=0;
		delay_ms(500);
		Beep=1;
	
}
