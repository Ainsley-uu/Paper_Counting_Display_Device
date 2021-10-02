#include "key.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include "sys.h"
#include "led.h"
#include  "oled.h"
 u8 StartStopFlg; //����
 u8 ConfirmKey;    //��
 u8 KeyUpFlg;      //��
 u8 KeyDownFlg;    //��
 u8 KeyLeftFlg;    //��
 u8 KeyRightFlg;   //��
extern unsigned char cKeyPressNumber;//�洢5�򰴼�ֵ
extern u8 bFlagReflashFDC2214;//���ڵ�1����ͨ��ֵ��ʾˢ������

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
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, DISABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15|GPIO_Pin_5;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_11|GPIO_Pin_12;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

//int Key_Scan(void)
//{
//	if( key == 0) 
//	{		
//		  while(!key);    
//		  return key_press; 
//	}		//����  1
//	else if( key_up == 0) 
//	{		
//		  while(!key_up);    
//		  return key_up_press; 
//	}			//��    2
//	else  if(key_down==0) 
//	{
//			while(!key_down);    
//		  return key_down_press; 
//	}    //��    3
//	else 	if(key_left==0) 
//	{
//		while(!key_left); 
//		return key_left_press;
//	}     //��    4
//	else  if(key_right==0)
//	{
//		while(!key_right); 
//		return key_right_press;
//	}  //��    5
//  else	if(key_middle==0) 
//	{
//		while(!key_middle);
//		return key_middle_press;
//	}   //��    6	
//	else return 0; 
//}

//��������Լ�д��
void KeyscanINIT()	   //�˿ڳ�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SystemInit();					//ϵͳʱ�ӳ�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	      //��GPIOA��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//�ܽŸ��ù���ʱ�Ӵ�
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable , ENABLE);		 //Changes the mapping of the specified pin.
   
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	//GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�������

	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	//GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//��������

	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	
	 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
}

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

int keyscan()//�������ɨ��
{
	
	u16 value;
	u8 h1,h2,h3,h4,ans,keys;
	ans=99;
  
	if(key==0)   return key_press;
	//GPIO_Write(GPIOA,(u16)(0xfe<<8));  //�жϵ�һ���Ǹ���������
	GPIO_Write(GPIOA,(u16)(0xfe));
	//value=GPIO_ReadInputData(GPIOA);
	//h1=(u8)(value>>8);
	h1=(u8)(GPIO_ReadInputData(GPIOA)&0xfe);
	
	if(h1!=0xfe)		 
	{
		
		delay_ms(10);	  //����
		if(h1!=0xfe)
		{
			keys=h1&0xf0;
			switch(keys)		
			{
				case 0xe0: ans=0;break;
				case 0xd0: ans=1;break;
				case 0xb0: ans=2;break;
				case 0x70: ans=3;break;
			}
		}
		//	while(h1!=0xfe);
		//while( (u8)( GPIO_ReadInputData(GPIOA) >>8) !=0xfe);//��ֹ����
		while( (u8)( GPIO_ReadInputData(GPIOA)&0xfe) !=0xfe);//��ֹ����
	}

	//GPIO_Write(GPIOA,(u16)(0xfd<<8));  //�жϵ�2���Ǹ���������
	GPIO_Write(GPIOA,(u16)(0xfd));
	//value=GPIO_ReadInputData(GPIOA);
	//h2=(u8)(value>>8);
	h2=(u8)(GPIO_ReadInputData(GPIOA)&0xfd);
	if(h2!=0xfd)		 
	{
		delay_ms(10);
		if(h2!=0xfd)
		{
			keys=h2&0xf0;
			switch(keys)		
			{
				case 0xe0: ans=4;break;
				case 0xd0: ans=5;break;
				case 0xb0: ans=6;break;
				case 0x70: ans=7;break;
			}
		}
		//	while(h2!=0xfd);
		//while( (u8)( GPIO_ReadInputData(GPIOA) >>8) !=0xfd);//��ֹ����
		while( (u8)( GPIO_ReadInputData(GPIOA)&0xfd) !=0xfd);//��ֹ����
		
	}

	//GPIO_Write(GPIOA,(u16)(0xfb<<8));  //�жϵ�3���Ǹ���������
	GPIO_Write(GPIOA,(u16)(0xfb));
	//value=GPIO_ReadInputData(GPIOA);
	//h3=(u8)(value>>8);
	h3=(u8)(GPIO_ReadInputData(GPIOA)&0xfb);
	if(h3!=0xfb)		 
	{
		delay_ms(10);
		if(h3!=0xfb)
		{
			keys=h3&0xf0;
			switch(keys)		
			{
				case 0xe0: ans=8;break;
				case 0xd0: ans=9;break;
				case 0xb0: ans=10;break;
				case 0x70: ans=11;break;
			}
		}
		//while(h3!=0xfb);
		//while( (u8)( GPIO_ReadInputData(GPIOA) >>8) !=0xfb);//��ֹ����
		while( (u8)( GPIO_ReadInputData(GPIOA)&0xfb) !=0xfb);//��ֹ����
			
	}

	//	GPIO_Write(GPIOA,(u16)(0xf7<<8));  //�жϵ�4���Ǹ���������
	GPIO_Write(GPIOA,(u16)(0xf7));
	//	value=GPIO_ReadInputData(GPIOA);
	//	h4=(u8)(value>>8);
	h4=(u8)(GPIO_ReadInputData(GPIOA)&0xf7);
	if(h4!=0xf7)		 
	{
		
		delay_ms(10);
		if(h4!=0xf7)
		{
			keys=h4&0xf0;
			switch(keys)		
			{
				case 0xe0: ans=12;break;
				case 0xd0: ans=13;break;
				case 0xb0: ans=14;break;
				case 0x70: ans=15;break;
			}
		}
		//	while(h4!=0xf7);
		//while( (u8)( GPIO_ReadInputData(GPIOA) >>8) !=0xf7);//��ֹ����
		while( (u8)( GPIO_ReadInputData(GPIOA)&0xf7) !=0xf7);//��ֹ����
	}
	
	switch(ans)
	{
		case 1: return key_up_press;
		case 4: return key_left_press;
		case 5: return key_middle_press;
		case 6: return key_right_press;
		case 8: return key_left10_press;
		case 9: return key_down_press;
		case 10: return key_right10_press;
		case 15: return ourlist;

		default: return 0;
		
	}
}

void key_exti_test(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;
  	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��
    Key_Init();
	
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource13); //StartStop
  	EXTI_InitStructure.EXTI_Line=EXTI_Line13;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource11);//KEy_Middle
  	EXTI_InitStructure.EXTI_Line=EXTI_Line11;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
 
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6);//Key_Right
  	EXTI_InitStructure.EXTI_Line=EXTI_Line6;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource7);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line7;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
    
//   	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�1
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
//  	NVIC_Init(&NVIC_InitStructure); 
//    	
//   	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�1
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
//  	NVIC_Init(&NVIC_InitStructure); 
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}


void EXTI9_5_IRQHandler(void)
{
   if(EXTI_GetITStatus(EXTI_Line7))   //ʹ��ѧϰģʽ���ݲɼ�  B7 LEFT
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
