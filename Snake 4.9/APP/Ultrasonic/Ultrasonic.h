#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H


#include "stm32f10x.h"
#include "usart2.h"



extern  volatile u32 count,Distance0,Distance1;
void usart3_init(u32 bound);
void USART3_IRQHandler(void);
void ultrasonic_Init(void);
void TIM3_Init(void);
u32 calculer(u32 count);
void TIM2_Init(void);
void Display(u32 Distance);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void GetDistance(void);
void GetTem(void);
void delay_usx(u32 time1);
void delay_ms(u32 time1);
#endif


