#include "niming.h"
#include "sys.h"

//串口1发送1个字符 
//c:要发送的字符
void usart1_send_char(u8 c)
{
	while((USART1->SR&0X40)==0);  
	USART1->DR=c;   	
} 

void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	 
	send_buf[len+3]=0;
	send_buf[0]=0X88;
	send_buf[1]=fun;
	send_buf[2]=len;	
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	 
}

void mpu6050_send_data(float res0,float res1,float res2,float res3)
{
    u8 tbuf[16]; 
    unsigned char *p;
    p=(unsigned char *)&res0;
    tbuf[0]=(unsigned char)(*(p+3));
    tbuf[1]=(unsigned char)(*(p+2));
    tbuf[2]=(unsigned char)(*(p+1));
    tbuf[3]=(unsigned char)(*(p+0));
     
    p=(unsigned char *)&res1;
    tbuf[4]=(unsigned char)(*(p+3));
    tbuf[5]=(unsigned char)(*(p+2));
    tbuf[6]=(unsigned char)(*(p+1));
    tbuf[7]=(unsigned char)(*(p+0));
     
    p=(unsigned char *)&res2;
    tbuf[8]=(unsigned char)(*(p+3));
    tbuf[9]=(unsigned char)(*(p+2));
    tbuf[10]=(unsigned char)(*(p+1));
    tbuf[11]=(unsigned char)(*(p+0));
	
	    p=(unsigned char *)&res3;
    tbuf[12]=(unsigned char)(*(p+3));
    tbuf[13]=(unsigned char)(*(p+2));
    tbuf[14]=(unsigned char)(*(p+1));
    tbuf[15]=(unsigned char)(*(p+0));
     
    usart1_niming_report(0XA2,tbuf,16);
}   





