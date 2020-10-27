#include "sys.h"
#include "usart1.h"
#include "usart2.h"
#include "SysTick.h"
#include "stdio.h"
#include "Ultrasonic.h"
#include "Actuator.h"
#include "Propeller.h"
u16 data5 = 1500,data6 = 1500;
int main(void)
{
	ultrasonic_Init();         //超声波相关管脚初始化：PB8=trig;PB9=echo
	TIM2_Init();              //初始化定时器TIM2，用于精准延时
	TIM3_Init();              //初始化定时器TIM3，用于采集超声波来回所用时间
	TIM4_Init();
	SysTick_Init();            //系统时钟用于定时发送超声波
	usart1_init(9600);
	usart2_init(115200);
	usart3_init(9600);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	u16 len,t;
 	while(1)
	{
		len=USART2_RX_STA&0x3fff;//得到此次接收到的数据长度
		if(len != 0)
		{
			for(t = 0;t<len;t++)
			{
				USART_SendData(USART2, USART2_RX_BUF[t]);//向串口2发送数据
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
			}
			USART2_RX_STA=0;
		}
		actuator_control(data5,data6);
		propeller_control();
	}
}
