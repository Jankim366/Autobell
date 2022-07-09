#ifndef __USART2_H
#define __USART2_H			 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//USART2驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
	  	
extern u8 USART2_RX_BUF[]; 		//接收缓冲,最大USART2_RX_BUF_SIZE个字节
extern u16 USART2_RX_CNT;   			//接收到的数据长度

//模式控制
#define USART2_TX_EN		PGout(9)	//485模式控制.0,接收;1,发送.
//如果想串口中断接收，请不要注释以下宏定义
#define EN_USART2_RX 	1			//0,不接收;1,接收.
#define USART2_RX_BUF_SIZE 1024		//WIFI模块启动时接收到数据稍微超过这个数量，报出USART3_RX_BUF FULL!
														 
void usart2_init(u32 pclk2,u32 bound);
void USART2_Send_Data(u8 *buf,u16 len);
void USART2_Receive_Data(u8 *buf,u16 *len);		 
#endif	   
















