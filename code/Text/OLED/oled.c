#include  "oled.h"
#include  "codetab.h"
#include  "delay.h"

u8 AsciiBuff[14];
u8 AsciiBuff3[4];
extern u8 bFunctionIndex;
// --------------OLED屏----------------------------------------------
// IO口模拟I2C通信
// SCL接PC^8
// SDA接PC^9
// ------------------------------------------------------------
void OLED_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	  GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_9); 	//
    //PBout(8)=1;
    //PBout(9)=1;
}

/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{   
	 SDA = high;
	 delay_us(1);
   SCL = high;
   delay_us(1);
   SDA = low;
	 delay_us(1);
   SCL = low;
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{ 
	 SDA = low;
	 delay_us(1);
   SCL = low;
   delay_us(1);
   SDA = high;
	 delay_us(1);
   SCL = high;
}

/**********************************************
// 通过I2C总线写一个字节
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
  for(i=0;i<8;i++)
	{
		if(IIC_Byte & 0x80)
			SDA=high;
		else
			SDA=low;
		    delay_us(1);
		SCL=high;
        delay_us(1);  //必须有保持SCL脉冲的延时
		SCL=low;
		    delay_us(1);
		IIC_Byte<<=1;
	}
    //原程序这里有一个拉高SDA，根据OLED的DATASHEET，此句必须去掉。
	SCL=1;
    delay_us(1);
	SCL=0;
}

/*********************OLED写数据************************************/ 
void OLED_WrDat(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);
	Write_IIC_Byte(0x40);			//write data
	Write_IIC_Byte(IIC_Data);
	IIC_Stop();
}
/*********************OLED写命令************************************/
void OLED_WrCmd(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);          //Slave address,SA0=0
	Write_IIC_Byte(0x00);			     //write command
	Write_IIC_Byte(IIC_Command);
	IIC_Stop();
}
/*********************OLED 设置坐标************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WrCmd(0xb0+y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);
	OLED_WrCmd((x&0x0f)|0x01);
} 
/*********************OLED全屏************************************/
void OLED_Fill(unsigned char bmp_dat) 
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		OLED_WrDat(bmp_dat);
	}
}
/*********************OLED复位************************************/
void OLED_CLS(void)
{
   OLED_Fill(0x00);
}
//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
	OLED_WrCmd(0X8D);  //设置电荷泵
	OLED_WrCmd(0X14);  //开启电荷泵
	OLED_WrCmd(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	OLED_WrCmd(0X8D);  //设置电荷泵
	OLED_WrCmd(0X10);  //关闭电荷泵
	OLED_WrCmd(0XAE);  //OLED休眠
}
/*********************OLED初始化************************************/
void Oled_Init(void)
{
	OLED_GPIO_Init();
	delay_ms(100);//初始化之前的延时很重要！
	OLED_WrCmd(0xae);//--turn off oled panel
	OLED_WrCmd(0x00);//---set low column address
	OLED_WrCmd(0x10);//---set high column address
	OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WrCmd(0x81);//--set contrast control register
	OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
	OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WrCmd(0xa6);//--set normal display
	
	
	
	OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3f);//--1/64 duty
	OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00);//-not offset
	OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9);//--set pre-charge period
	OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda);//--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb);//--set vcomh
	OLED_WrCmd(0x40);//Set VCOM Deselect Level
	OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x02);//
	OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WrCmd(0x14);//--set(0x10) disable
	OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WrCmd(0xaf);//--turn on oled panel
	OLED_Fill(0x00); //初始清屏
	OLED_Set_Pos(0,0);
} 
/***************功能描述：显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7****************/
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		OLED_WrDat(F6x8[c][i]);
		x+=6;
		j++;
	}
}
// x,y -- 起始点坐标(x:0~127, y:0~7); 
//ch[] -- 要显示的字符串; 
//TextSize -- 字符大小(1:6*8 ; 2:8*16)
//显示codetab.h中的ASCII字符,有6*8和8*16可选择
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
					OLED_WrDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_Set_Pos(x,y);
				for(i=0;i<8;i++)
					OLED_WrDat(F8X16[c*16+i]);
				OLED_Set_Pos(x,y+1);
				for(i=0;i<8;i++)
					OLED_WrDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}
/*******************功能描述：显示8*16一组标准ASCII字符串	 显示的坐标（x,y），y为页范围0～7****************/
void OLED_P8x16Str(unsigned char x,unsigned  char y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i]);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}
/**********功能描述：显示 6*8 点阵 显示的坐标（x,y），y为页范围0～7************/
void OLED_Show6x8(unsigned char x,unsigned char y,unsigned N)
{
	unsigned char i = 0;
	unsigned int  adder = 6*N;
  if(x>126)
     x=0,y++;
	OLED_Set_Pos(x,y);
	for(i=0;i < 6;i++)
	 OLED_WrDat(My6x8[adder++]);
	
}


/**********功能描述：显示 8*16 点阵 显示的坐标（x,y），y为页范围0～7***********/
void OLED_Show8x16(unsigned char x,unsigned char y,unsigned N)
{
	unsigned char i = 0;
	unsigned int  adder = 6*N;
  if(x>120)
     x=0,y++;
	OLED_Set_Pos(x,y);
	for(i=0;i < 8;i++)
	 OLED_WrDat(My8x16[adder++]);
	OLED_Set_Pos(x,y+1);
	for(i=0;i < 8;i++)
	 OLED_WrDat(My8x16[adder++]);
}
/**********功能描述：显示16*16点阵  显示的坐标（x,y），y为页范围0～7***********/
void OLED_Show16x16(unsigned char x,unsigned char y,unsigned int N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	} 	  	
}
void OLED_keyboard(unsigned char x,unsigned char y,unsigned int N)
{
  unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(keyboard[adder]);
		adder += 1;
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(keyboard[adder]);
		adder += 1;
	} 	  	 	
}



/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WrDat(BMP[j++]);
	    }
	}
}

void Oled_Show_X(unsigned char x,unsigned y,unsigned int temp,unsigned char TextSize)
{
		unsigned char i = 4;
	  unsigned char n;
		unsigned char a[6];
      while(temp>=10)
			{
				a[i--] = temp%10+'0';
				temp /= 10;
			}
			a[i]=temp+'0';
			n = i;
			a[5]='\0';
		  for(i=0;i<n;i++)
			a[i] = ' ';
			switch(TextSize)
			{
			case 0:
				OLED_P6x8Str(x,y,a);
				break;
			case 1:
        OLED_P8x16Str(x,y,a);
				break;
			default:
				break;
		}
}

void Oled_Show_3(unsigned char x,unsigned y,unsigned int temp,unsigned char TextSize)
{
		unsigned char i = 2;
	  unsigned char n;
		unsigned char a[4];
      while(temp>=10)
			{
				a[i--] = temp%10+'0';
				temp /= 10;
			}
			a[i]=temp+'0';
			n = i;
			a[3]='\0';
		  for(i=0;i<n;i++)
			a[i] = '0';
			switch(TextSize)
			{
			case 0:
				OLED_P6x8Str(x,y,a);
				break;
			case 1:
        OLED_P8x16Str(x,y,a);
				break;
			default:
				break;
		}
}

void LCD_ShowxNum( char x,char y,float  num)
{ int i=0,temp,j=0;
	float c=0;
	
	unsigned char ch1[20],ch2[20];
	c=num;
	if(c>=0)
	{temp=c;
	while(temp>=10)
	{
		ch1[i++]=temp-10*(temp/10);
		temp=temp/10;
	}
	ch1[i]=temp;
	while(i>=0)
	{
		ch2[j++]=ch1[i--];
	}
  ch2[j]='x';
	j=0;
	while(ch2[j]!='x')
{	OLED_Set_Pos(x,y);
	for(i=0;i<8;i++)
	OLED_WrDat(num_show[ch2[j]*16+i]);
	OLED_Set_Pos(x,y+1);
	for(i=0;i<8;i++)
	OLED_WrDat(num_show[ch2[j]*16+i+8]);
  j=j+1;
  x+=8;}
}
	if(c<0)
	{
		temp=0-c;
	while(temp>=10)
	{
		ch1[i++]=temp-10*(temp/10);
		temp=temp/10;
	}
	ch1[i]=temp;
	while(i>=0)
	{
		ch2[j++]=ch1[i--];
	}
  ch2[j]='x';
	j=0;
	while(ch2[j]!='x')
{	OLED_Set_Pos(x,y);
	for(i=0;i<8;i++)
	OLED_WrDat(num_show[ch2[j]*16+i]);
	OLED_Set_Pos(x,y+1);
	for(i=0;i<8;i++)
	OLED_WrDat(num_show[ch2[j]*16+i+8]);
  j=j+1;
  x+=8;}
	}
}
void OLED_ShowNum(unsigned char x, unsigned char y, u16 num, unsigned char TextSize)
 {
	int i=0;
	int  n;
	int a[4];   //最长为五位数
  if (num==0)
  {
	 OLED_ShowOneNum(x,y,num,TextSize);
    for(i=1;i<5;i++)
    OLED_ShowOneNum(x+6*i,y,-16,TextSize);
  }
	else
	{			
    while(num>=10) 
		{a[i]=num%10; num/=10;i++;}
		 a[i]=num;
		 n=i;
		for(i=0;i<5;i++)  
		{
			if(n-i<0)  OLED_ShowOneNum(x+6*i,y,-16,TextSize);
			else  OLED_ShowOneNum(x+6*i,y,a[n-i],TextSize);
		}
	}
}
 
void OLED_ShowOneNum(unsigned char x, unsigned char y, int num, unsigned char TextSize)
	{
	unsigned char c = 0,i = 0;
	switch(TextSize)
	{
		case 1:
		{
			  c=num+16;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
					OLED_WrDat(F6x8[c][i]);
		}break;
		case 2:
		{			
				c = 16+num;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_Set_Pos(x,y);
				for(i=0;i<8;i++)
					OLED_WrDat(F8X16[c*16+i]);
				OLED_Set_Pos(x,y+1);
				for(i=0;i<8;i++)
					OLED_WrDat(F8X16[c*16+i+8]);
				x += 8;
		}break;
	}
 }
	void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char Nn)
{
    unsigned char wm=0;
    unsigned int  adder=32*Nn;
    OLED_Set_Pos(x , y);
    for(wm = 0;wm < 16;wm++)
    {
        OLED_WrDat(Hzk[adder]);
        adder += 1;
    }
    OLED_Set_Pos(x,y + 1);
    for(wm = 0;wm < 16;wm++)
    {
        OLED_WrDat(Hzk[adder]);
        adder += 1;
    }
}
	
void Oled_Printf_Float(unsigned char x,unsigned char y,float num,unsigned char TextSize)
{
    unsigned char i;
    unsigned char result[7];
    unsigned char  tempH;
    unsigned char tempL;
    result[6]='\0';
    result[3] = '.';
    if(num < 0)
    {
    num = -num;
        result[0] = '-';
    }
    else
        result[0] = '+';
    tempH = (unsigned int)num;
    tempL = (unsigned char)((num - tempH)*100);
  for(i=0;i<4;i++)
    {
        switch(i)
        {
            case 0:result[2] = tempH%10 + '0';tempH/=10;break;
            case 1:result[1] = tempH + '0';break;
            case 2:result[5] = tempL%10 + '0';tempL/=10;break;
            case 3:result[4] = tempL + '0';break;
         default:break;
        }
    }
     for(i=1;i<2;i++)
    {
        if(result[i]=='0')
            result[i]=' ';
        else
            break;
    }
    switch(TextSize)
    {
    case 0:
        OLED_P6x8Str(x,y,result);break;
    case 1:
        OLED_P8x16Str(x,y,result);break;
    default:break;
    }
}

void Oled_Printf_U32(unsigned char x,unsigned char y,unsigned long int num,unsigned char TextSize)
{
    unsigned char temp[10];
    temp[9]= '\0';
    temp[8] = (unsigned char)(num%10+'0');num/=10;
    temp[7] = (unsigned char)(num%10+'0');num/=10;
    temp[6] = (unsigned char)(num%10+'0');num/=10;
    temp[5] = (unsigned char)(num%10+'0');num/=10;
    temp[4] = (unsigned char)(num%10+'0');num/=10;
    temp[3] = (unsigned char)(num%10+'0');num/=10;
    temp[2] = (unsigned char)(num%10+'0');num/=10;
    temp[1] = (unsigned char)(num%10+'0');num/=10;
    temp[0] = (unsigned char)(num%10+'0');

    switch(TextSize)
    {
        case 0:
            OLED_P6x8Str(x,y,temp);
            break;
        case 1:
            OLED_P8x16Str(x,y,temp);
            break;
        default:
            break;
    }
}

 void UI_R(void)
{
  u8 m,n;
  for(m=2;m<8;m++)
  {
  	OLED_WrCmd(0xb0+m);		//page0-page1
		OLED_WrCmd(0x00);		//low column start address
		OLED_WrCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_WrDat(0x00);
			}
   }
 }

void OLED_PaperNumTitle(void)//纸张计数，显示标题等
{	
						OLED_ShowCN(0,0,20);
						OLED_ShowCN(16,0,20);
						OLED_ShowCN(96,0,20);
						
						//第一行显示
						if(bFunctionIndex==1)//测量模式
						{
							OLED_ShowCN(32,0,28);						
							OLED_ShowCN(48,0,29);
							OLED_ShowCN(64,0,44);	
							OLED_ShowCN(80,0,45);
						}
						else//校准模式
						{
							OLED_ShowCN(32,0,10);						
							OLED_ShowCN(48,0,11);
							OLED_ShowCN(64,0,44);	
							OLED_ShowCN(80,0,45);	
							
						}

//						//第一行显示 纸张计数	
//						OLED_ShowCN(32,0,0);						
//						OLED_ShowCN(48,0,1);
//						OLED_ShowCN(64,0,2);	
//						OLED_ShowCN(80,0,3);	
//						OLED_ShowCN(64,0,4);	
//						OLED_ShowCN(80,0,5);	
						
						//第二行显示 校准		
						OLED_ShowCN(0,2,10);	
						OLED_ShowCN(16,2,11);
						OLED_ShowStr(32,2,": ",2);
						OLED_ShowStr(72,2," ",2);						
						OLED_ShowCN(80,2,1);	
						OLED_ShowStr(96,2," ",2);	
						OLED_ShowStr(104,2," ",2);	
						
					//第三行显示 测试		
						OLED_ShowCN(0,4,28);	
						OLED_ShowCN(16,4,29);		
						OLED_ShowStr(32,4,": ",2);
						OLED_ShowStr(72,4," ",2);						
						OLED_ShowCN(80,4,1);	
						OLED_ShowStr(96,4," ",2);	
						OLED_ShowStr(104,4," ",2);	

					//第四行显示通道采样值	
						OLED_ShowStr(2,6,"Val:",2);	
						//OLED_ShowCN(48,4,20);
						//OLED_ShowCN(64,4,20);
}


void Data2ASCII3Figure( u32 Data_32 )//转化为ASCII码输出
{
		AsciiBuff3[0] =( u8 )(Data_32/100)%10 + 0x30;  
		AsciiBuff3[1] =( u8 )(Data_32/10)%10 + 0x30; 
		AsciiBuff3[2]= (Data_32)%10 + 0x30; 	
}

void Data2ASCII( u32 Data_32 )//转化为ASCII码输出
{
    AsciiBuff[0] = 48;
		AsciiBuff[1] =( u8 )(Data_32/1000000000)%10 + 0x30;  
		AsciiBuff[2] =( u8 )(Data_32/100000000)%10 + 0x30;  
		AsciiBuff[3] =( u8 )(Data_32/10000000)%10 + 0x30;  
		AsciiBuff[4] =( u8 )(Data_32/1000000)%10 + 0x30;  
		AsciiBuff[5] =( u8 )(Data_32/100000)%10 + 0x30;  
		AsciiBuff[6] =( u8 )(Data_32/10000)%10 + 0x30;  
		AsciiBuff[7] =( u8 )(Data_32/1000)%10 + 0x30;  
		AsciiBuff[8] =( u8 )(Data_32/100)%10 + 0x30;  
		AsciiBuff[9] =( u8 )(Data_32/10)%10 + 0x30; 
		AsciiBuff[10]=( u8 )(Data_32)%10 + 0x30; 	
}
