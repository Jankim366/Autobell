#ifndef __USART2_H
#define __USART2_H			 
#include "sys.h"
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
	  	
extern u8 USART2_RX_BUF[]; 		//���ջ���,���USART2_RX_BUF_SIZE���ֽ�
extern u16 USART2_RX_CNT;   			//���յ������ݳ���

//ģʽ����
#define USART2_TX_EN		PGout(9)	//485ģʽ����.0,����;1,����.
//����봮���жϽ��գ��벻Ҫע�����º궨��
#define EN_USART2_RX 	1			//0,������;1,����.
#define USART2_RX_BUF_SIZE 1024		//WIFIģ������ʱ���յ�������΢�����������������USART3_RX_BUF FULL!
														 
void usart2_init(u32 pclk2,u32 bound);
void USART2_Send_Data(u8 *buf,u16 len);
void USART2_Receive_Data(u8 *buf,u16 *len);		 
#endif	   
















