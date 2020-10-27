#include "Ultrasonic.h"
/*
用于超声波测距
*/

u32 time;         //用于采集超声波来回时间的计数
u32 delay_u;
volatile u32 Distance0;
volatile u32 Distance1;
volatile u32 count;
volatile u8 Data[3];
volatile u8 BUFFER;
//////////////////////////////////////////////////
//
//     函数：   ultrasonic_Init()
//     输入：   无
//     输出：   无
//     功能：   超声波管脚初始化
//     说明：   无
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
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART3，GPIOA时钟

	//USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3_RX	  GPIOB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART3, ENABLE);                   

}

///////////////////////////////////////////////////
//
//     函数：   TIME3_Init()
//     输入：   无
//     输出：   无
//     功能：   初始化定时器3，该定时器用于采集超声波
//              来回的时间
//     说明：   该函数设置了定时器TIM3的更新中断，
//              每100us中断一次，经测试，该间隔时间
//              下测得的距离更准确
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
	
	TIM_TimeBaseInitStructure.TIM_Period=99;                       //计数次数
	TIM_TimeBaseInitStructure.TIM_Prescaler=17;                    //预分频数
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=0;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                       //打开定时器3的更新中断
}

//////////////////////////////////////////////////
//
//     函数：   TIME2_Init()
//     输入：   无
//     输出：   无
//     功能：   初始化定时器2，用于精准延时
//     说明：   该函数设置了定时器的更新中断，
//              每10us中断一次
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
	TIM_TimeBaseInitStructure.TIM_Period=39;                        //计数次数为40
	TIM_TimeBaseInitStructure.TIM_Prescaler=17;                     //预分频系数为18
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                        //打开定时器更新中断
}

//////////////////////////////////////////////////
//
//     函数：   delay_usx()
//     输入：   time1（延时多少个10us）
//     输出：   无
//     功能：   用于延时 X*10 微秒
//     说明：   打开TIM2后，TIM2每10us中断一次，每次
//              中断，delay_u都减一，达到延时目的
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
//     函数：   GetDistance()
//     输入：   无
//     输出：   无
//     功能：   计算采集到的距离并进行显示
//     说明：   1、工作原理：trig拉高10us后，打开TIM3
//                 来采集超声波来回的时间，然后通过
//                 计算公式求得距离
//              2、求得的距离是六次距离的平均值
//
/////////////////////////////////////////////////////
void GetDistance(void)
{
	int i;
	
//  for(i=5;i>0;i--)                                    //循环六次采集距离
//	{
//		GPIO_ResetBits(GPIOB,GPIO_Pin_8);
//		GPIO_SetBits(GPIOB,GPIO_Pin_8);                     //拉高trig
//		delay_usx(1);                                       //保持10us
//		GPIO_ResetBits(GPIOB,GPIO_Pin_8);                   //拉低trig
//		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==0);  //等待echo管脚输出高电平后打开TIM3计时
//		TIM_Cmd(TIM3,ENABLE);                               //打开TIM3，开始获取超声波来回的时间
//		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==1);  //等待来回时间的结束
//		TIM_Cmd(TIM3,DISABLE);                              //关闭TIM3，结束时间的采集
//		count=TIM_GetCounter(TIM3);                         //获取TIM3的该次计数值，即读取TIM3寄存器CNT
//		Distance0=calculer(count);	                        //调用函数计算距离
//		Distance1=Distance1+Distance0;                      //距离距离的累加，用于后续计算距离平均值
//		delay_usx(5000);                                    
//	}
//	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//	USART_SendData(USART3,0X55);
	USART_SendData(USART3,0X50);
	delay_ms(10);
	Distance1 = Data[0]*256 + Data[1];	

//	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//	USART_SendData(USART3,0X50);
//	Distance1=Distance1/6;                                //计算平均值
//	printf("Dis:%d.",Distance1/100);                 //输出距离
//	printf("%dcm",Distance1%100);
//	printf("\r\n");
//	printf("Dis:%d.",Distance1/10);                 //输出距离
//	printf("%dcm",Distance1%10);
//	printf("\r\n");
	printf("Tem:%d C ",Data[0]-45);                 		//输出温度
	printf("\r\n");
	delay_usx(60000);
}

///////////////////////////////////////////////////////
//
//     函数：   calculer()
//     输入：   count（TIM3该次的计数值）
//     输出：   Distance（计算得到的距离，）
//     功能：   计算距离的核心公式
//     说明：   1、计算得到的距离单位是cm，并且放大了100倍
//              2、计算公式是：TIM3的计数次数*18/72000000*340/2*100*100
//                 其中18为TIM3的预分频系数，72000000为系统的主频率
//                 340为声速，除以2是因为来回，100是声速的米换算成厘米
//                 另一个100是放大100倍
//
////////////////////////////////////////////////////////
u32 calculer(u32 count)
{
	u32 Distance;
	Distance=((float)(count+100*time)*0.425);   //距离计算公式化简后的形式
	time=0;                                     //清空TIM3的中断次数
	
	return Distance;
	
}

/////////////////////////////////////////////////////
//
//     函数：   TIM3_IRQHandler()
//     输入：   无
//     输出：   无
//     功能：   TIM3中断函数
//     说明：   每100us进入该函数，获取超声波来回时间间隔内
//              TIM3中断的次数，用于计算时间和最后的距离
//
/////////////////////////////////////////////////////
void TIM3_IRQHandler(void)
{
  if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update)!=RESET)
	{
		time++;                                   	 //中断标志自加1
		TIM_ClearFlag(TIM3,TIM_FLAG_Update);	     //清除中断标志
	}
}

///////////////////////////////////////////////////
//
//     函数：   TIM2_IRQHandler()
//     输入：   无
//     输出：   无
//     功能：   TIM2中断函数
//     说明：   每10us进入该函数，每进入该函数，
//              delay_u自减1，当减到0时，延时结束
//              
/////////////////////////////////////////////////////
void TIM2_IRQHandler(void)
{
  if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update)!=RESET)
	{
		delay_u--;                               //延时标志自减1
		TIM_ClearFlag(TIM2,TIM_FLAG_Update);     //清空中断标志
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