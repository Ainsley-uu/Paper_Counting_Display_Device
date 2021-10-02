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
#define ShortCircuitValue 0x04000000//两极板短路的判断阈值
void yuyin(unsigned int cMesuare);
void BuzzerBeep(void);

//电容值存放数据
float temp0=0,temp1=0,temp2=0,temp3=0;//四路通道值存放的变量
float Total_Value;//四路通道总值
void GetFouChVal(void);//读取四路通道值的函数
//extern u8 bS1Rec;//串口接收到数据标志
u8 bFlag3s;//3s计时标志	
unsigned int  T5Counter3s;//3s计数器	
//extern unsigned char cS1Rec;//串口接收到字符
extern unsigned long  FDC2214ThresholdValue[];//各数量纸张对应点的中点值作为阈值
extern unsigned long  FDC2214CalibrateValue[];//用于放校准的值
extern unsigned char  cThePointOfCalibrate[];//校准点标志位	
//u8 bFlagReflashFDC2214;//用于第1屏各通道值显示刷新允许
unsigned char  cKeyPressNumber=0;//存储5向按键值
extern u8 AsciiBuff[14];
extern u8 AsciiBuff3[3];
unsigned long  FDC2214CH[4]={0,0,0,0};	//存放读取的FDC2214值，4通道
unsigned char  i=0,cCalibrateNum=1;//校准功能中显示的张数
unsigned int  intTemp=0,cMeasureNum=255;	//测量功能中显示的张数
u8 bFunctionIndex=0;//功能指示标志，0为校准，1为测试
u8 bIndicatorIcon=0;	//功能图标样式，0为?，1为OK，就是汉字表索引18+bIndicatorIcon
u8 bShowPaperNum=0;//显示测试纸张数量标志
u8 bReCalibrate=0;//在校准选项中按下过中间键确认，说明校准数据有改变，此位为1
u8 bCalibrateValueNoSaved=0;//用于当前校准数据保存标志
u8 bSelectSaveItOrNot=0;//离开校准选项时，显示确认是否要保存数据界面
u8 bSaveCalibrateData=0;//保存校准数据
u8 bCalibrateDataReset=0;//校准数据恢复出厂设置标志	
unsigned int JudgePaperNum(unsigned long longFDC2214Value);


int main()
{

		SystemInit();
	  delay_init();	    	                             //延时函数初始化	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置中断优先级分组2
    uart_init(115200);	                             //串口初始化为115200
    TIM2_init(79,7199);                              //10ms中断
    IIC_Start();                                     //IIC初始化
    Oled_Init();
    Beep_Init();	//oled初始化
    LED_Init();	                                     //led初始化
    Key_Init();                                      //初始化五向按键和开关按键，可使用Key_Scan（）函数得到返回值
  //key_exti_test();                                 //按键检测    
		FLASHInit();
		uart2_init(9600)  ;

		Usart_SendString(USART2,"AF:28");//调节音量
    delay_ms(3);                 
    //OLED_PaperNumTitle();   
		FDC_IIC_Init();	
//**********检测有没有连接好FDC2214***************
	FDC2214_Init();
	intTemp=FDC_Read(MANUFACTURER_ID);
//	if(intTemp!=0x5449)
//		OLED_Fill(0x00);

//		Usart_SendString(USART2,"B7:14");//纸张测试开始
  BuzzerBeep();

  OLED_Fill(0x00); 		
	if(intTemp!=0x5449)
	{	
		Usart_SendString(USART2,"A7:00019");//请连接好传感器
		while(intTemp!=0x5449)
		{
				
				OLED_ShowStr(0,0,"Have no FDC2214!",2);
				OLED_ShowStr(0,2,"Please connect",2);		
				OLED_ShowStr(0,4,"it correctly!",2);
				intTemp=FDC_Read(MANUFACTURER_ID);
		}
	}
//************************************************

	OLED_PaperNumTitle();    //标题
  delay_ms(5);  	
	Usart_SendString(USART2,"A7:00014");//测量前请校准纸张
  while(1)
	{
			cKeyPressNumber = Key_Scan();  //检测按键//读取按键值
			if(cKeyPressNumber)//有键按下，判断为哪个键按下，根据逻辑功能标识标志位
			{
				
				switch(cKeyPressNumber) //根据按键执行 
				{ 
					case 1:// KeyStartStop		
					{
						//按键超过3秒，恢复原始校准数据
						//要加3秒判断
						if(bFlag3s==0)bFlag3s=1;
						if(bFlag3s==1 && T5Counter3s>600)
						{
							bCalibrateDataReset=1;//恢复原始校准数据标志
							bFlag3s=0;
							T5Counter3s=0;
//							OLED_ShowStr(0,2,"                ",2);//擦除2,3,4行显示
//							OLED_ShowStr(0,4,"                ",2);//			
//							OLED_ShowStr(0,6,"                ",2);//		

//							OLED_ShowStr(32,2,"Cleared",2);//?
//							OLED_ShowStr(0,2,"                ",2);//擦除2,3,4行显示
						}
						
						//S1SendData(0x00);
						break;//串口发送	; 			
					}
					case 2:// KeyUp
					{
						//S1SendData(0x05);//串口发送	; 
						if(bFunctionIndex==1)	//当前在测量功能
						{
							//语音
							Usart_SendString(USART2,"A7:00017");//重新校准
							bFunctionIndex=0;
							bShowPaperNum=0;
						//	cCalibrateNum=1;//校准功能中数字归1
							bIndicatorIcon=0;//显示？，表示当前数据点未记录校准值							
						}
						else;//在校准功能中
						break;
					}
					case 3://KeyDown
					{
						
						if(bFunctionIndex==0)//在校准功能中，按向下键
						{
			
							if(bReCalibrate==1)//如果校准过数据，说明校准结束，跳出对话框，要求确认是否保存校准数据，
							{
							
								bSelectSaveItOrNot=1;//是否显示保存选择框标志
								
						//		cCalibrateNum=1;//校准功能中数字归1
							
							}
							else//如果没有校准过数据，就直接用以前的校准数据测量
							{
								bFunctionIndex=1;//光标移到测量选项
								Usart_SendString(USART2,"A7:00014");//重新校准
							}
						}
					//	S1SendData(0x01);//串口发送	; 

						break;
					}
					case 4:// KeyLeft
					{
						if(bFunctionIndex==0 & cCalibrateNum>1)//菜单选择为校准操作
						{
							cCalibrateNum--;
						}
	//					S1SendData(0x04);
						bIndicatorIcon=0;//显示？，表示当前数据点未记录校准值
						break;//串口发送	;  
						
					}
					case 5://	KeyRight
					{
						if(bFunctionIndex==0 & (cCalibrateNum<=170))//菜单选择为校准操作
						{
							cCalibrateNum++;
						}
	//					S1SendData(0x03);//串口发送	; 
						bIndicatorIcon=0;//显示？，表示当前数据点未记录校准值					
						break;

					}
					case 6:// KeyMiddle 
					{
						if(bFunctionIndex==0)//在校准功能中
						{
							bReCalibrate=1;//标识有数据被校准过
							bCalibrateValueNoSaved=1;//标识当前数据未被保存
							//语音
               Usart_SendString(USART2,"A7:00020");//已确认
							 BuzzerBeep();//蜂鸣器叫一声
						}
						else////菜单选择为计数操作
						{
							bShowPaperNum=1;
//							Usart_SendString(USART2,"A7:00016");//按键开始测量
						}
					
						bIndicatorIcon=1;//功能图标样式改为OK。如果在校准中，表示当前数据已校准；如果在测量中，表示已确认显示张数
						BuzzerBeep();	
						
						//S1SendData(0x02);//串口发送	; 
						break;
					}

					default:
					break;//
				}	
				cKeyPressNumber=0;//读键值结束后即默认按键抬起
			
			}
			
      if(!cKeyPressNumber)//没有按键按下时，刷新显示；有键按下时，OLED不刷新。这样处理是由于OLED刷新和读取FDC2214占用CPU时间，按键响应不流畅
			{
				
				if(bCalibrateDataReset==1)//恢复原始校准数据标志
				{
					bCalibrateDataReset=0;
					bIndicatorIcon=0;
					Usart_SendString(USART2,"A7:00018");//恢复出厂数据，要重新校准//语音	
				  BuzzerBeep();	BuzzerBeep();	
					FLASHReset();
				}
				if(bSelectSaveItOrNot==1)//显示保存数据选择界面，在校准选项中离开，需要保存校准数据
				{
					Usart_SendString(USART2,"A7:00015");//是否保存校准数据
					
					//语音			
					OLED_ShowStr(0,2,"                ",2);//擦除2,3,4行显示
					OLED_ShowStr(0,4,"                ",2);//			
					OLED_ShowStr(0,6,"                ",2);//		

					//调用保存校准数据的界面
					OLED_ShowCN(0,2,21);	//是			
					OLED_ShowCN(16,2,22);//否
					OLED_ShowCN(32,2,23);//保	
					OLED_ShowCN(48,2,24);//存	
					OLED_ShowCN(64,2,3);	//数
					OLED_ShowCN(80,2,25);//据	
					OLED_ShowStr(96,2,"?",2);//?
					OLED_ShowCN(16,6,21);						
					OLED_ShowCN(64,6,27);

						
					
					while(cKeyPressNumber!=0x06)//等待确认保存或不保存
					{
						cKeyPressNumber = Key_Scan();  //检测按键//读取按键值
						if(cKeyPressNumber==4)//向左键
						{
								OLED_ShowCN(16,6,26);						
								OLED_ShowCN(64,6,22);
								bSaveCalibrateData=1;//保存
						}
						else if(cKeyPressNumber==5)//向右键
						{
							
								OLED_ShowCN(16,6,21);						
								OLED_ShowCN(64,6,27);
								bSaveCalibrateData=0;//不保存
						}							
					}						

						
	
					if(bSaveCalibrateData==1)//选择保存
					{
//					cCalibrateNum=1;//显示的校准张数归0
						FLASHSave();			//	将校准数据保存到FLASH第一页面	
						bFunctionIndex=1;//光标移到测量选项
						bReCalibrate=0;	//清重校准标志								
					}
					else
					{
						Usart_SendString(USART2,"A7:00014");//测量前请校准纸张
						bFunctionIndex=0;//光标留在校准选项
					}
					
					bSelectSaveItOrNot=0;//不再显示保存对话框					
					cKeyPressNumber=0;//读键值结束后即默认按键抬起					
				
					OLED_PaperNumTitle();//恢复显示主界面
					bIndicatorIcon=0;//显示空圈		
					
				}
				else//显示正常主界面	
				{
					FDC2214CH[1]=FCD2214_ReadCH(1);
//						S1SendData(FDC2214CH[1]>>24);
//						S1SendData(FDC2214CH[1]>>16);						
//						S1SendData(FDC2214CH[1]>>8);	
//						S1SendData(FDC2214CH[1]);	
					if(FDC2214CH[1]>ShortCircuitValue)//FDC2214短路
					{	
						//语音S2SendString("A7:00013");				
						
						OLED_Fill(0x00);
						OLED_ShowCN(0,0,8);		//短路提示		
						OLED_ShowCN(16,0,9);	
						Usart_SendString(USART2,"A7:00013");//短路
						while(FDC2214CH[1]>ShortCircuitValue)
						{
							
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++						
							
							FDC2214CH[1]=FilteredFCD2214ReadCH(1);
							//FDC2214CH[1]=FCD2214ReadCH(1);
							BuzzerBeep();//蜂鸣器叫一声
							//S1SendData(0x01);
						}//程序停在这里直到短路解除
						 //Beep=1;
						//S1SendData(0x02);
						OLED_PaperNumTitle();//重新显示原来的界面							
					}
					else//FDC2214极板不短路
					{	
						Data2ASCII(FDC2214CH[1]);
						OLED_ShowStr(34,6,AsciiBuff,2);	//最后一行显示当前FDC2214结果，转换成8位十六进制显示
						if(bFunctionIndex==0 )//在校准功能中，
						{
								
							if(bCalibrateValueNoSaved==1)//且有数据被校准
							 {
									//S2SendString("A7:00017");	
									//+++++++++读取FDC2214值，以[cCalibrateNum-1]为下标，存入数组+++++++++++++			
									cThePointOfCalibrate[cCalibrateNum-1]=1;//改变标志位 
									FDC2214CalibrateValue[cCalibrateNum-1]=FilteredFCD2214ReadCH(1);//保存读入经过滤波的校准值								
									bCalibrateValueNoSaved=0;//当前数据已被保存										 
							 }							
							 Data2ASCII3Figure(cCalibrateNum);
//							 yuyin(cCalibrateNum);
							 OLED_ShowStr(48,2,AsciiBuff3,2);	//显示校准张数
							 
							 OLED_ShowCN(112,4,20);	//根据当前功能标志，擦除原来光标	
							 OLED_ShowCN(112,2,18+(bIndicatorIcon?1:0));	//根据bIndicatorIcon值，为1显示OK ，为0显示？
							 OLED_ShowStr(48,4,"   ",2);//擦除测量功能上显示的张数				 				 							 
						}
						else//在测量功能中，
						{
							
							if(bShowPaperNum==1)//显示测量张数
							{
								//++++++++++++++++
								//+++++++++++++++++
								//根据FDC2214CH[1]判断张数，
								
										
								cMeasureNum=JudgePaperNum(FilteredFCD2214ReadCH(1));
//								Usart_SendString(USART2,"B7:0102");
								//S2SendString(Num2Speaker(cMeasureNum));//语音报张数
								
								bIndicatorIcon=1;							
								//P55=0;
								OLED_ShowStr(48,4,"   ",2);//擦除显示的张数
							  OLED_ShowCN(112,4,18+(bIndicatorIcon?1:0));	//根据bIndicatorIcon值，为1显示OK，为0显示？								
								delay_ms(500);
								//+++++++++++++++显示计数张数，调用声音提示++++++++++++++++++
							if(cMeasureNum!=999)					yuyin(cMeasureNum);
								Data2ASCII3Figure(cMeasureNum);
								OLED_ShowStr(48,4,AsciiBuff3,2);		//显示测量张数	
								//P55=1;
								bIndicatorIcon=0;
								bShowPaperNum=0;
							}
							OLED_PaperNumTitle();//重新显示原来的界面									
							OLED_ShowCN(112,2,20);	//根据当前功能标志，擦除原来光标	
							OLED_ShowCN(112,4,18+(bIndicatorIcon?1:0));	//根据bIndicatorIcon值，为1显示OK，为0显示？																	
							OLED_ShowStr(48,2,"   ",2);//擦除校准功能上显示的张数						

						}
					}

				}
			}
}
}
/**
**定时器5中断函数 10ms
**在定时器中采集四路通道值，并计算总值
**/
void TIM2_IRQHandler(void)         
{
	 
	if(TIM2->SR&0X0001)//溢出中断
	{	
    //temp1= Cap_Calculate(1);
		if(bFlag3s==1) T5Counter3s++;
	}						   
  TIM2->SR&=~(1<<0);
}

//根据读入的FDC2214值，和FDC2214ThresholdValue[170]

unsigned int JudgePaperNum(unsigned long longFDC2214Value)
{
	extern unsigned long FDC2214ThresholdValue[170];//各数量纸张对应点的中点值作为阈值
	unsigned char i=0;
	if(longFDC2214Value < FDC2214ThresholdValue[0])
		return(1);
	else
		for(i=1;i<169;i++)
		{
			if((longFDC2214Value >= FDC2214ThresholdValue[i-1]) && (longFDC2214Value < FDC2214ThresholdValue[i]) )
				return(i+1);//随着纸张增加，传感器值增加，"<"
		}	
	Usart_SendString(USART2,"A7:00021");		
	return(999);//显示999表示超出范围了

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
//	yu[3]='1';//当前纸张数目为
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
			yu[num++]='1';yu[num++]='1';//百
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
			yu[num++]='1';yu[num++]='0';//十
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
