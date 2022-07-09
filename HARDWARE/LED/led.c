#include "led.h"

//��ʼ��PB0��PFΪ�����.��ʹ���������ڵ�ʱ��
//LED IO��ʼ��
void LED_Init(void)
{
	RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��
	RCC->APB2ENR|=1<<7;    //ʹ��PORTFʱ��

	GPIOB->CRL&=0XFFFFFFF0;
	GPIOB->CRL|=0X00000003;//PB.0 �������
	GPIOB->ODR|=1<<0;      //PB.0 �����

	GPIOF->CRL&=0X0FFFFFFF;
	GPIOF->CRL|=0X30000000;//PF.7�������
	GPIOF->ODR|=1<<7;      //PF.7�����

	GPIOF->CRH&=0XFFFFFFF0;
	GPIOF->CRH|=0X00000003;//PF.8�������
	GPIOF->ODR|=1<<8;      //PF.8�����
}






