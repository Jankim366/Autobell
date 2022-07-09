#ifndef __USART3_H
#define __USART3_H			 
#include "../sys/sys.h"	 								  
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
	  	
extern u8 USART3_RX_BUF[]; 		//���ջ���,���USART3_RX_BUF_SIZE���ֽ�
extern u16 USART3_RX_CNT;   			//���յ������ݳ���

//ģʽ����
#define USART3_TX_EN		PGout(9)	//485ģʽ����.0,����;1,����.
//����봮���жϽ��գ��벻Ҫע�����º궨��
#define EN_USART3_RX 	1			//0,������;1,����.
#define USART3_RX_BUF_SIZE 1024		
#define MAX_WIFI_BUF_SIZE 1024

void usart3_init(u32 pclk2,u32 bound);
void USART3_Send_Data(u8 *buf,u16 len);
void USART3_Receive_Data(u8 *buf, u16 rear, u16 total_len, u16 *len);
void USART3_Receive_Data_No_Queue(u8 *buf,u16 *len);
#endif	   
















