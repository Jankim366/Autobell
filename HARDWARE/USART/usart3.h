#ifndef __USART3_H
#define __USART3_H			 
#include "../sys/sys.h"	 								  
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
	  	
extern u8 USART3_RX_BUF[]; 		//接收缓冲,最大USART3_RX_BUF_SIZE个字节
extern u16 USART3_RX_CNT;   			//接收到的数据长度

//模式控制
#define USART3_TX_EN		PGout(9)	//485模式控制.0,接收;1,发送.
//如果想串口中断接收，请不要注释以下宏定义
#define EN_USART3_RX 	1			//0,不接收;1,接收.
#define USART3_RX_BUF_SIZE 1024		
#define MAX_WIFI_BUF_SIZE 1024

void usart3_init(u32 pclk2,u32 bound);
void USART3_Send_Data(u8 *buf,u16 len);
void USART3_Receive_Data(u8 *buf, u16 rear, u16 total_len, u16 *len);
void USART3_Receive_Data_No_Queue(u8 *buf,u16 *len);
#endif	   
















