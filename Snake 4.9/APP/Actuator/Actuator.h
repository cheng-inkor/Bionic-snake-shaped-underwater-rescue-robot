#ifndef __ACTUATOR_H
#define __ACTUATOR_H


#include "stm32f10x.h"
#include "usart2.h"

void actuator_Init();
void USART1_SendOrder(u8 CH, u16 Data);
void actuator_control(u16 data5,u16 data6);

#endif


