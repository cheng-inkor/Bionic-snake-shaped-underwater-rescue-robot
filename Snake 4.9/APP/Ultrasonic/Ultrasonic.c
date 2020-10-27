#include "Ultrasonic.h"
/*
���ڳ��������
*/

u32 time;         //���ڲɼ�����������ʱ��ļ���
u32 delay_u;
volatile u32 Distance0;
volatile u32 Distance1;
volatile u32 count;
volatile u8 Data[3];
volatile u8 BUFFER;
//////////////////////////////////////////////////
//
//     ������   ultrasonic_Init()
//     ���룺   ��
//     �����   ��
//     ���ܣ�   �������ܽų�ʼ��
//     ˵����   ��
//
////////////////////////////////////////////////
void ultrasonic_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8; //trio
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;  //echo
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
}

void usart3_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART3��GPIOAʱ��

	//USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3_RX	  GPIOB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART3, ENABLE);                   

}

///////////////////////////////////////////////////
//
//     ������   TIME3_Init()
//     ���룺   ��
//     �����   ��
//     ���ܣ�   ��ʼ����ʱ��3���ö�ʱ�����ڲɼ�������
//              ���ص�ʱ��
//     ˵����   �ú��������˶�ʱ��TIM3�ĸ����жϣ�
//              ÿ100us�ж�һ�Σ������ԣ��ü��ʱ��
//              �²�õľ����׼ȷ
//
////////////////////////////////////////////////////
void TIM3_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseInitStructure.TIM_Period=99;                       //��������
	TIM_TimeBaseInitStructure.TIM_Prescaler=17;                    //Ԥ��Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=0;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                       //�򿪶�ʱ��3�ĸ����ж�
}

//////////////////////////////////////////////////
//
//     ������   TIME2_Init()
//     ���룺   ��
//     �����   ��
//     ���ܣ�   ��ʼ����ʱ��2�����ھ�׼��ʱ
//     ˵����   �ú��������˶�ʱ���ĸ����жϣ�
//              ÿ10us�ж�һ��
//
/////////////////////////////////////////////////
void TIM2_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=0;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period=39;                        //��������Ϊ40
	TIM_TimeBaseInitStructure.TIM_Prescaler=17;                     //Ԥ��Ƶϵ��Ϊ18
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                        //�򿪶�ʱ�������ж�
}

//////////////////////////////////////////////////
//
//     ������   delay_usx()
//     ���룺   time1����ʱ���ٸ�10us��
//     �����   ��
//     ���ܣ�   ������ʱ X*10 ΢��
//     ˵����   ��TIM2��TIM2ÿ10us�ж�һ�Σ�ÿ��
//              �жϣ�delay_u����һ���ﵽ��ʱĿ��
//
////////////////////////////////////////////////////
void delay_usx(u32 time1)
{
	delay_u=time1;
	TIM_Cmd(TIM2,ENABLE);
	while(delay_u>0);
	TIM_Cmd(TIM2,DISABLE);
}

void delay_ms(u32 time1)
{
	delay_u=time1*100;
	delay_usx(delay_u);
}
///////////////////////////////////////////////////
//
//     ������   GetDistance()
//     ���룺   ��
//     �����   ��
//     ���ܣ�   ����ɼ����ľ��벢������ʾ
//     ˵����   1������ԭ��trig����10us�󣬴�TIM3
//                 ���ɼ����������ص�ʱ�䣬Ȼ��ͨ��
//                 ���㹫ʽ��þ���
//              2����õľ��������ξ����ƽ��ֵ
//
/////////////////////////////////////////////////////
void GetDistance(void)
{
	int i;
	
//  for(i=5;i>0;i--)                                    //ѭ�����βɼ�����
//	{
//		GPIO_ResetBits(GPIOB,GPIO_Pin_8);
//		GPIO_SetBits(GPIOB,GPIO_Pin_8);                     //����trig
//		delay_usx(1);                                       //����10us
//		GPIO_ResetBits(GPIOB,GPIO_Pin_8);                   //����trig
//		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==0);  //�ȴ�echo�ܽ�����ߵ�ƽ���TIM3��ʱ
//		TIM_Cmd(TIM3,ENABLE);                               //��TIM3����ʼ��ȡ���������ص�ʱ��
//		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==1);  //�ȴ�����ʱ��Ľ���
//		TIM_Cmd(TIM3,DISABLE);                              //�ر�TIM3������ʱ��Ĳɼ�
//		count=TIM_GetCounter(TIM3);                         //��ȡTIM3�ĸôμ���ֵ������ȡTIM3�Ĵ���CNT
//		Distance0=calculer(count);	                        //���ú����������
//		Distance1=Distance1+Distance0;                      //���������ۼӣ����ں����������ƽ��ֵ
//		delay_usx(5000);                                    
//	}
//	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//	USART_SendData(USART3,0X55);
	USART_SendData(USART3,0X50);
	delay_ms(10);
	Distance1 = Data[0]*256 + Data[1];	

//	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//	USART_SendData(USART3,0X50);
//	Distance1=Distance1/6;                                //����ƽ��ֵ
//	printf("Dis:%d.",Distance1/100);                 //�������
//	printf("%dcm",Distance1%100);
//	printf("\r\n");
//	printf("Dis:%d.",Distance1/10);                 //�������
//	printf("%dcm",Distance1%10);
//	printf("\r\n");
	printf("Tem:%d C ",Data[0]-45);                 		//����¶�
	printf("\r\n");
	delay_usx(60000);
}

///////////////////////////////////////////////////////
//
//     ������   calculer()
//     ���룺   count��TIM3�ôεļ���ֵ��
//     �����   Distance������õ��ľ��룬��
//     ���ܣ�   �������ĺ��Ĺ�ʽ
//     ˵����   1������õ��ľ��뵥λ��cm�����ҷŴ���100��
//              2�����㹫ʽ�ǣ�TIM3�ļ�������*18/72000000*340/2*100*100
//                 ����18ΪTIM3��Ԥ��Ƶϵ����72000000Ϊϵͳ����Ƶ��
//                 340Ϊ���٣�����2����Ϊ���أ�100�����ٵ��׻��������
//                 ��һ��100�ǷŴ�100��
//
////////////////////////////////////////////////////////
u32 calculer(u32 count)
{
	u32 Distance;
	Distance=((float)(count+100*time)*0.425);   //������㹫ʽ��������ʽ
	time=0;                                     //���TIM3���жϴ���
	
	return Distance;
	
}

/////////////////////////////////////////////////////
//
//     ������   TIM3_IRQHandler()
//     ���룺   ��
//     �����   ��
//     ���ܣ�   TIM3�жϺ���
//     ˵����   ÿ100us����ú�������ȡ����������ʱ������
//              TIM3�жϵĴ��������ڼ���ʱ������ľ���
//
/////////////////////////////////////////////////////
void TIM3_IRQHandler(void)
{
  if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update)!=RESET)
	{
		time++;                                   	 //�жϱ�־�Լ�1
		TIM_ClearFlag(TIM3,TIM_FLAG_Update);	     //����жϱ�־
	}
}

///////////////////////////////////////////////////
//
//     ������   TIM2_IRQHandler()
//     ���룺   ��
//     �����   ��
//     ���ܣ�   TIM2�жϺ���
//     ˵����   ÿ10us����ú�����ÿ����ú�����
//              delay_u�Լ�1��������0ʱ����ʱ����
//              
/////////////////////////////////////////////////////
void TIM2_IRQHandler(void)
{
  if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update)!=RESET)
	{
		delay_u--;                               //��ʱ��־�Լ�1
		TIM_ClearFlag(TIM2,TIM_FLAG_Update);     //����жϱ�־
	}
}

void USART3_IRQHandler(void)                
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) 
	{
//		USART_ClearITPendingBit(USART3,USART_IT_RXNE); 
		if(BUFFER < 1)
		{
			Data[BUFFER] = USART_ReceiveData(USART3);
			BUFFER++;
		}
		else
			BUFFER = 0;
     } 
} 