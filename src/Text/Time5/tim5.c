/******************************************
定时器中断
定时器5  产生5ms中断
*******************************************/
#include "time5.h"

//定时器5 的相关设定
void TIM2_init(u16 arr,u16 psc)
{
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

TIM_TimeBaseStructure.TIM_Period = arr;
TIM_TimeBaseStructure.TIM_Prescaler =psc; 
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 

NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
NVIC_Init(&NVIC_InitStructure); 
TIM_Cmd(TIM2, ENABLE); 
}

void TIM3_init(u16 arr,u16 psc)
{
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

TIM_TimeBaseStructure.TIM_Period = arr;
TIM_TimeBaseStructure.TIM_Prescaler =psc; 
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 
//优先级设定
NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
NVIC_Init(&NVIC_InitStructure); 
TIM_Cmd(TIM3, ENABLE); 
}

