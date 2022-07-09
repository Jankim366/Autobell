#include "usart2.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//USART2���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////


#ifdef EN_USART2_RX   	//���ʹ���˽���
//���ջ�����
u8 USART2_RX_BUF[USART2_RX_BUF_SIZE];  	//���ջ���,���USART2_RX_BUF_SIZE���ֽ�.
//���յ������ݳ���
u16 USART2_RX_CNT=0;   
void USART2_IRQHandler(void)
{
	u8 res;	    
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		res=USART2->DR; 			 
		if(USART2_RX_CNT<USART2_RX_BUF_SIZE)
		{
			USART2_RX_BUF[USART2_RX_CNT]=res;		//��¼���յ���ֵ
			USART2_RX_CNT++;						//������������1 
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
void usart2_init(u32 pclk1,u32 bound)
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
 
	RCC->APB2ENR|=1<<2;   	//ʹ��PORTA��ʱ��  
	GPIOA->CRL&=0XFFFF00FF;	//IO״̬����	PA2��PA3
	GPIOA->CRL|=0X00008B00;	//IO״̬����	  

	RCC->APB1ENR|=1<<17;  	//ʹ�ܴ���ʱ��	USART2
		  
	RCC->APB1RSTR|=1<<17;   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=mantissa; 	// ����������	 
	USART2->CR1|=0X200C;  	//1λֹͣ,��У��λ.
#ifdef EN_USART2_RX		  	//���ʹ���˽���
	//ʹ�ܽ����ж�
	USART2->CR1|=1<<8;    	//PE�ж�ʹ��
	USART2->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//��2��������ȼ� 
#endif
	USART2_TX_EN=0;			//Ĭ��Ϊ����ģʽ	
}

//USART2����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void USART2_Send_Data(u8 *buf,u16 len)
{
	u16 t;
	USART2_TX_EN=1;			//����Ϊ����ģʽ
  	for(t=0;t<len;t++)		//ѭ����������
	{
		while ((USART2->SR&0X40)==0);//�ȴ����ͽ���		  
		USART2->DR=buf[t];
	}	 
	while ((USART2->SR&0X40)==0);//�ȴ����ͽ���	
	USART2_RX_CNT=0;	  
	USART2_TX_EN=0;				//����Ϊ����ģʽ	
}
//USART2��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void USART2_Receive_Data(u8 *buf,u16 *len)
{
	u16 rxlen=USART2_RX_CNT;
	u16 i=0;
	*len=0;				//Ĭ��Ϊ0
	//delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==USART2_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART2_RX_BUF[i];	
		}		
		*len=USART2_RX_CNT;	//��¼�������ݳ���
		USART2_RX_CNT=0;		//����
	}
}





