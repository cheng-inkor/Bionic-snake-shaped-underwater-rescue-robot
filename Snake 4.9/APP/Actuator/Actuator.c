#include "Actuator.h"
#include "usart1.h"

/*
用于舵机控制
*/

void actuator_Init()
{
	u8 t;
	for(t=0; t<8 ;t++)
	{
		USART1_SendOrder(t,1500);
	}
}

void actuator_control(u16 data5,u16 data6)
{
	u8 t;
	switch(BLT)
	{//蓝牙传输数据完成遥控
		case 'l':
				USART1_SendOrder(0,2000);
				USART1_SendOrder(1,2000);
				USART1_SendOrder(2,1000);
				USART1_SendOrder(3,2000);
				USART1_SendOrder(4,1000);
			break;
		case 'r':
				USART1_SendOrder(0,1000);
				USART1_SendOrder(1,1000);
				USART1_SendOrder(2,2000);
				USART1_SendOrder(3,1000);
				USART1_SendOrder(4,2000);
			break;
		case 'b':
			for(t=0; t<8 ;t++)
			{
				USART1_SendOrder(t,1500);
			}
			break;
		case 'a':
			USART1_SendOrder(7,834);
			break;
		case 'c':
			USART1_SendOrder(7,2166);
			break;
		case '+':
			//螺旋桨向上转动
			data5++;
			data6++;
			USART1_SendOrder(5,data5);
			USART1_SendOrder(6,data6);
			break;
		case '-':
			//螺旋桨向下转动
			data5--;
			data6--;
			USART1_SendOrder(5,data5);
			USART1_SendOrder(6,data6);
			break;
		
		case '0':
			TIM_SetCompare1(TIM3,190);
			break;
		case '1':
			TIM_SetCompare1(TIM3,189);
			break;
	}	
	
}

void USART1_SendOrder(u8 CH, u16 Data)
{
	u8 i =0;
	u8 DataL =(u8) Data;
	u8 DataH =(u8) (Data>>8);
	u8 data[5] = {0xFF,0x02,CH,DataL,DataH};
	for(i = 0;i<5;i++)
	{
		USART1->DR = data[i];
		while((USART1->SR&0x40)==0);
	}
}