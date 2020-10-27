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
	ultrasonic_Init();         //��������عܽų�ʼ����PB8=trig;PB9=echo
	TIM2_Init();              //��ʼ����ʱ��TIM2�����ھ�׼��ʱ
	TIM3_Init();              //��ʼ����ʱ��TIM3�����ڲɼ���������������ʱ��
	TIM4_Init();
	SysTick_Init();            //ϵͳʱ�����ڶ�ʱ���ͳ�����
	usart1_init(9600);
	usart2_init(115200);
	usart3_init(9600);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	u16 len,t;
 	while(1)
	{
		len=USART2_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
		if(len != 0)
		{
			for(t = 0;t<len;t++)
			{
				USART_SendData(USART2, USART2_RX_BUF[t]);//�򴮿�2��������
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			USART2_RX_STA=0;
		}
		actuator_control(data5,data6);
		propeller_control();
	}
}
