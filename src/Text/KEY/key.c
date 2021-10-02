#include "key.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include "sys.h"
#include "led.h"
#include  "oled.h"
 u8 StartStopFlg; //侧向
 u8 ConfirmKey;    //中
 u8 KeyUpFlg;      //上
 u8 KeyDownFlg;    //下
 u8 KeyLeftFlg;    //左
 u8 KeyRightFlg;   //右
extern unsigned char cKeyPressNumber;//存储5向按键值
extern u8 bFlagReflashFDC2214;//用于第1屏各通道值显示刷新允许

void ClearFlag()
{
	  StartStopFlg = 0;
		KeyUpFlg = 0;
		KeyDownFlg = 0;
		KeyLeftFlg = 0;
		KeyRightFlg = 0;
}


void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, DISABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15|GPIO_Pin_5;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_11|GPIO_Pin_12;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	 GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

//int Key_Scan(void)
//{
//	if( key == 0) 
//	{		
//		  while(!key);    
//		  return key_press; 
//	}		//侧向  1
//	else if( key_up == 0) 
//	{		
//		  while(!key_up);    
//		  return key_up_press; 
//	}			//上    2
//	else  if(key_down==0) 
//	{
//			while(!key_down);    
//		  return key_down_press; 
//	}    //下    3
//	else 	if(key_left==0) 
//	{
//		while(!key_left); 
//		return key_left_press;
//	}     //左    4
//	else  if(key_right==0)
//	{
//		while(!key_right); 
//		return key_right_press;
//	}  //右    5
//  else	if(key_middle==0) 
//	{
//		while(!key_middle);
//		return key_middle_press;
//	}   //中    6	
//	else return 0; 
//}



int Key_Scan(void)
{
	if(key==0)              return key_press;
	else	if(key_up==0)     return key_up_press;
	else  if(key_down==0)   return key_down_press;
	else 	if(key_left==0)   { delay_ms(200); return key_left_press;}
	else  if(key_right==0)  { delay_ms(200); return key_right_press;}
  else	if(key_middle==0) return key_middle_press;
	
	else return 0; 
	
}


void key_exti_test(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;
  	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
    Key_Init();
	
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13); //StartStop
  	EXTI_InitStructure.EXTI_Line=EXTI_Line13;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource11);//KEy_Middle
  	EXTI_InitStructure.EXTI_Line=EXTI_Line11;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
 
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6);//Key_Right
  	EXTI_InitStructure.EXTI_Line=EXTI_Line6;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource7);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line7;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    
//   	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//  	NVIC_Init(&NVIC_InitStructure); 
//    	
//   	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//  	NVIC_Init(&NVIC_InitStructure); 
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}


void EXTI9_5_IRQHandler(void)
{
   if(EXTI_GetITStatus(EXTI_Line7))   //使能学习模式数据采集  B7 LEFT
   {
      delay_ms(10);
      if(key_left==0)
      {	while(!key_left);
//      KeyRightLeftFlg=-1;ConfirmKey=0;Interrupt_Flg=1;Train_Figer_Entry_Flg=0;Train_Fist_Entry_Flg=0;
      } 
     EXTI_ClearITPendingBit(EXTI_Line7);
   }


if(EXTI_GetITStatus(EXTI_Line5))   //B5 down
	{   
		delay_ms(10);
    if(key_down==0)
    {
				while(!key_down);
//			KeyUpDownFlg=-1;
//			Interrupt_Flg=1;
    } 
  EXTI_ClearITPendingBit(EXTI_Line5);
	}	
	
	if(EXTI_GetITStatus(EXTI_Line6))   //B6 RIGHT
	{   
		delay_ms(10);
    if(key_right==0)
    {
				while(!key_right);
//			KeyRightLeftFlg=1;              
//			Interrupt_Flg=1;
    } 
  EXTI_ClearITPendingBit(EXTI_Line6);
	}
	
}

//void EXTI1_IRQHandler(void)   //write flash   Middle
//{
//  delay_ms(10);
//    if(key_middle==0)
//    {
//			while(!key_middle);
//		  ConfirmKey=1;
//			Interrupt_Flg=1;
//    } 
//  EXTI_ClearITPendingBit(EXTI_Line1);
//     
//}

//void EXTI4_IRQHandler(void)   //PA4 ++         Right
//{
//	
//	  delay_ms(10);
//    if(key_right==0)
//    {
//			while(!key_right);
//			KeyRightLeftFlg=1;              
//			Interrupt_Flg=1;
//    }
//  EXTI_ClearITPendingBit(EXTI_Line4);
//   
//}



void EXTI15_10_IRQHandler(void)  // C12 up 
{
  if(EXTI_GetITStatus(EXTI_Line11))     //A11 MID
   {
       delay_ms(10);
      if(key_middle==0)
      {
				while(!key_middle);
				ConfirmKey=1;
//			Interrupt_Flg=1;;
      }  
     EXTI_ClearITPendingBit(EXTI_Line11);
  }
	 
	
	 if(EXTI_GetITStatus(EXTI_Line13))   // A12 START
	 {	
	  delay_ms(10);
    if(key==0)
    {
			while(!key){};
//     InterruptFlg=1;
    } 
   EXTI_ClearITPendingBit(EXTI_Line13);
   }
	 
 if(EXTI_GetITStatus(EXTI_Line15))   // A15 UP
	 {	
	  delay_ms(10);
    if(key_up==0)
    {
			while(!key_up){};
//     KeyUpDownFlg=1;
//				Interrupt_Flg=1;
    } 
   EXTI_ClearITPendingBit(EXTI_Line15);
   }

	 
	 
}
