#include <stdio.h>
#include "../sys/sys.h"		    
#include "usart3.h"	 
#include "../delay/delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//USART3驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////


#ifdef EN_USART3_RX   	//如果使能了接收
//接收缓存区
u8 USART3_RX_BUF[USART3_RX_BUF_SIZE];  	//接收缓冲,最大USART3_RX_BUF_SIZE个字节.
//接收到的数据长度
u16 USART3_RX_CNT=0;   
void USART3_IRQHandler(void)
{
	u8 res;	    
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntEnter();    
#endif
	if(USART3->SR&(1<<5))//接收到数据
	{	 
		res=USART3->DR; 			 
		if(USART3_RX_CNT<USART3_RX_BUF_SIZE)
		{
			USART3_RX_BUF[USART3_RX_CNT]=res;		//记录接收到的值
			USART3_RX_CNT++;						//接收数据增加1 
		} else {
			printf("\r\nUSART3_RX_BUF FULL!\r\n");
		} 
	}  											 
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif										 
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void usart3_init(u32 pclk1,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);//得到USARTDIV
	mantissa=(u16)temp;				 //得到整数部分
	fraction=(u16)(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	// RCC->APB2ENR|=1<<8;   	//使能PORTG口时钟  
 	// GPIOG->CRH&=0XFFFFFF0F;	//IO状态设置
	// GPIOG->CRH|=0X00000030;	//IO状态设置	  
	
	
	// RCC->APB2ENR|=1<<5;   	//使能PORTD口时钟  
	// GPIOD->CRH&=0XFFFFFF00;	//IO状态设置	PD8、PD9
	// GPIOD->CRH|=0X0000008B;	//IO状态设置	
	
	// RCC->APB2ENR|=1<<0;     //开启辅助时钟	
	// AFIO->MAPR&=0XFFFFFF3F; //清除MAPR的[5:4]
	// AFIO->MAPR|=3<<4;       //重映射,USART3->PD8-PD12
 
	RCC->APB2ENR|=1<<3;   	//使能PORTB口时钟  
	GPIOB->CRH&=0XFFFF00FF;	//IO状态设置	PB10、PB11
	GPIOB->CRH|=0X00008B00;	//IO状态设置	  

	RCC->APB1ENR|=1<<18;  	//使能串口时钟	USART3
		  
	RCC->APB1RSTR|=1<<18;   //复位串口3
	RCC->APB1RSTR&=~(1<<18);//停止复位	   	   
	//波特率设置
 	USART3->BRR=mantissa; 	// 波特率设置	 
	USART3->CR1|=0X200C;  	//1位停止,无校验位.
#ifdef EN_USART3_RX		  	//如果使能了接收
	//使能接收中断
	USART3->CR1|=1<<8;    	//PE中断使能
	USART3->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART3_IRQn,2);//组2，最低优先级 
#endif
	// USART3_TX_EN=0;			//默认为接收模式	
}

//USART3发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void USART3_Send_Data(u8 *buf,u16 len)
{
	u16 t;
	// USART3_TX_EN=1;			//设置为发送模式
  	for(t=0;t<len;t++)		//循环发送数据
	{
		while ((USART3->SR&0X40)==0);//等待发送结束		  
		USART3->DR=buf[t];
	}	 
	while ((USART3->SR&0X40)==0);//等待发送结束	
	// USART3_RX_CNT=0;	  
	// USART3_TX_EN=0;				//设置为接收模式	
}
//USART3查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void USART3_Receive_Data(u8 *buf, u16 rear, u16 total_len, u16 *len)
{
	u16 rxlen=USART3_RX_CNT;
	u16 copy_len, i=0;
	*len=0;				//默认为0
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if((rxlen==USART3_RX_CNT)&&rxlen)//接收到了数据,且接收完成了
	{
		if (total_len < MAX_WIFI_BUF_SIZE) {
			if ((MAX_WIFI_BUF_SIZE - total_len) >= rxlen) {
				copy_len = rxlen;
			} else {
				copy_len = MAX_WIFI_BUF_SIZE - total_len;	//部分接收时，导致数据不完整，有待改进
				printf("\r\ncopy_len = MAX_WIFI_BUF_SIZE - total_len\r\n");
			}
			for (i = 0;i < copy_len; i++) {
				buf[(i+rear)%MAX_WIFI_BUF_SIZE]=USART3_RX_BUF[i];	
			}
			*len=copy_len;	//记录本次数据长度
			USART3_RX_CNT=0;//清零
		} else {
			printf("\r\ntotal_len >= MAX_WIFI_BUF_SIZE\r\n");
		}
	}
}

//USART3查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void USART3_Receive_Data_No_Queue(u8 *buf,u16 *len)
{
	u16 rxlen=USART3_RX_CNT;
	u16 i=0;
	*len=0;				//默认为0
	//delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==USART3_RX_CNT&&rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART3_RX_BUF[i];	
		}		
		*len=USART3_RX_CNT;	//记录本次数据长度
		USART3_RX_CNT=0;		//清零
	}
}




