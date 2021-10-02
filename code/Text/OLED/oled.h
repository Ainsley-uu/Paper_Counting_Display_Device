#ifndef  __OLED_H
#define  __OLED_H

#include "sys.h"


#define SCL PAout(8)
#define SDA PAout(9) 

#define high 1
#define low 0
 
#define X_WIDTH 	128
#define Y_WIDTH 	64

#define Brightness 200

void IIC_Start(void);// -- 开启I2C总线
void IIC_Stop(void);// -- 关闭I2C总线
void Write_IIC_Byte(unsigned char IIC_Byte);// -- 通过I2C总线写一个byte的数据
void OLED_WrDat(unsigned char dat);// -- 向OLED屏写数据
void OLED_WrCmd(unsigned char cmd);// -- 向OLED屏写命令
void OLED_Set_Pos(unsigned char x, unsigned char y);// -- 设置显示坐标
void OLED_Fill(unsigned char bmp_dat);// -- 全屏显示(显示BMP图片时才会用到此功能)
void OLED_CLS(void);// -- 复位/清屏
void OLED_ON(void);
void OLED_OFF(void);
void Oled_Init(void);// -- OLED屏初始化程序，此函数应在操作屏幕之前最先调用
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);// -- 6x8点整，用于显示ASCII码的最小阵列，不太清晰
void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[]);// -- 8x16点整，用于显示ASCII码，非常清晰
void OLED_Show8x16(unsigned char x,unsigned char y,unsigned N);
void OLED_Show6x8(unsigned char x,unsigned char y,unsigned N);
void OLED_keyboard(unsigned char x,unsigned char y,unsigned N);

void OLED_Show16x16(unsigned char x,unsigned char y,unsigned int N);// -- 16x16点整，用于显示汉字的最小阵列，可设置各种字体、加粗、倾斜、下划线等
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);// -- 将128x64像素的BMP位图在取字软件中算出字表，然后复制到codetab中，此函数调用即可
void Oled_Show_X(unsigned char x,unsigned y,unsigned int temp,unsigned char TextSize);
void Oled_Show_3(unsigned char x,unsigned y,unsigned int temp,unsigned char TextSize);
void LCD_ShowxNum( char x,char y,float  num);
void OLED_ShowNum(unsigned char x, unsigned char y, u16 num, unsigned char TextSize);
void OLED_ShowOneNum(unsigned char x, unsigned char y, int num, unsigned char TextSize);

void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_ShowOneNum(unsigned char x, unsigned char y, int num, unsigned char TextSize);
void OLED_ShowNum(unsigned char x, unsigned char y, u16 num, unsigned char TextSize);
void Oled_Printf_Float(unsigned char x,unsigned char y,float num,unsigned char TextSize);
void Oled_Printf_U32(unsigned char x,unsigned char y,unsigned long int num,unsigned char TextSize);
void Show(u8 mark,u8 mark1);
void OLED_PaperNumTitle(void);//纸张计数，显示标题等
void Data2ASCII( u32 Data_32 );//转化为ASCII码输出
void Data2ASCII3Figure(u32 Data_32);
#endif
