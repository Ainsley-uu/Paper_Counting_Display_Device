#ifndef __NIMING_H
#define __NIMING_H
#include "sys.h"
void usart1_send_char(u8 c);
void usart1_niming_report(u8 fun,u8*data,u8 len);
void mpu6050_send_data(float res0,float res1,float res2,float res3);
#endif


