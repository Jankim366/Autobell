#include <stdio.h>
#include "../sys/sys.h"		    
#include "usart3.h"	 
#include "../delay/delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//USART3���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////


#ifdef EN_USART3_RX   	//���ʹ���˽���
//���ջ�����
u8 USART3_RX_BUF[USART3_RX_BUF_SIZE];  	//���ջ���,���USART3_RX_BUF_SIZE���ֽ�.
//���յ������ݳ���
u16 USART3_RX_CNT=0;   
void USART3_IRQHandler(void)
{
	u8 res;	    
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART3->SR&(1<<5))//���յ�����
	{	 
		res=USART3->DR; 			 
		if(USART3_RX_CNT<USART3_RX_BUF_SIZE)
		{
			USART3_RX_BUF[USART3_RX_CNT]=res;		//��¼���յ���ֵ
			USART3_RX_CNT++;						//������������1 
		} else {
			printf("\r\nUSART3_RX_BUF FULL!\r\n");
		} 
	}  											 
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
#endif										 
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void usart3_init(u32 pclk1,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=(u16)temp;				 //�õ���������
	fraction=(u16)(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	// RCC->APB2ENR|=1<<8;   	//ʹ��PORTG��ʱ��  
 	// GPIOG->CRH&=0XFFFFFF0F;	//IO״̬����
	// GPIOG->CRH|=0X00000030;	//IO״̬����	  
	
	
	// RCC->APB2ENR|=1<<5;   	//ʹ��PORTD��ʱ��  
	// GPIOD->CRH&=0XFFFFFF00;	//IO״̬����	PD8��PD9
	// GPIOD->CRH|=0X0000008B;	//IO״̬����	
	
	// RCC->APB2ENR|=1<<0;     //��������ʱ��	
	// AFIO->MAPR&=0XFFFFFF3F; //���MAPR��[5:4]
	// AFIO->MAPR|=3<<4;       //��ӳ��,USART3->PD8-PD12
 
	RCC->APB2ENR|=1<<3;   	//ʹ��PORTB��ʱ��  
	GPIOB->CRH&=0XFFFF00FF;	//IO״̬����	PB10��PB11
	GPIOB->CRH|=0X00008B00;	//IO״̬����	  

	RCC->APB1ENR|=1<<18;  	//ʹ�ܴ���ʱ��	USART3
		  
	RCC->APB1RSTR|=1<<18;   //��λ����3
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ	   	   
	//����������
 	USART3->BRR=mantissa; 	// ����������	 
	USART3->CR1|=0X200C;  	//1λֹͣ,��У��λ.
#ifdef EN_USART3_RX		  	//���ʹ���˽���
	//ʹ�ܽ����ж�
	USART3->CR1|=1<<8;    	//PE�ж�ʹ��
	USART3->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART3_IRQn,2);//��2��������ȼ� 
#endif
	// USART3_TX_EN=0;			//Ĭ��Ϊ����ģʽ	
}

//USART3����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void USART3_Send_Data(u8 *buf,u16 len)
{
	u16 t;
	// USART3_TX_EN=1;			//����Ϊ����ģʽ
  	for(t=0;t<len;t++)		//ѭ����������
	{
		while ((USART3->SR&0X40)==0);//�ȴ����ͽ���		  
		USART3->DR=buf[t];
	}	 
	while ((USART3->SR&0X40)==0);//�ȴ����ͽ���	
	// USART3_RX_CNT=0;	  
	// USART3_TX_EN=0;				//����Ϊ����ģʽ	
}
//USART3��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void USART3_Receive_Data(u8 *buf, u16 rear, u16 total_len, u16 *len)
{
	u16 rxlen=USART3_RX_CNT;
	u16 copy_len, i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if((rxlen==USART3_RX_CNT)&&rxlen)//���յ�������,�ҽ��������
	{
		if (total_len < MAX_WIFI_BUF_SIZE) {
			if ((MAX_WIFI_BUF_SIZE - total_len) >= rxlen) {
				copy_len = rxlen;
			} else {
				copy_len = MAX_WIFI_BUF_SIZE - total_len;	//���ֽ���ʱ���������ݲ��������д��Ľ�
				printf("\r\ncopy_len = MAX_WIFI_BUF_SIZE - total_len\r\n");
			}
			for (i = 0;i < copy_len; i++) {
				buf[(i+rear)%MAX_WIFI_BUF_SIZE]=USART3_RX_BUF[i];	
			}
			*len=copy_len;	//��¼�������ݳ���
			USART3_RX_CNT=0;//����
		} else {
			printf("\r\ntotal_len >= MAX_WIFI_BUF_SIZE\r\n");
		}
	}
}

//USART3��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void USART3_Receive_Data_No_Queue(u8 *buf,u16 *len)
{
	u16 rxlen=USART3_RX_CNT;
	u16 i=0;
	*len=0;				//Ĭ��Ϊ0
	//delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==USART3_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART3_RX_BUF[i];	
		}		
		*len=USART3_RX_CNT;	//��¼�������ݳ���
		USART3_RX_CNT=0;		//����
	}
}




