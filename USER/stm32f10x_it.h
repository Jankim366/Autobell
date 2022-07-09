/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.h
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
//void PendSV_Handler(void);

/**
@brief  This function handles SysTick Handler.
@param  None
@retval None
*/
void SysTick_Handler(void);
/**
@brief		记录定时器1的溢出计数
@param		None
@retval 	None
*/
void TIM1_UP_IRQHandler(void);

/**
@brief		记录定时器8的溢出计数
@param		None
@retval 	None
*/
void TIM8_UP_IRQHandler(void);

/**
@brief		定时器2每隔100ms会触发此中断进行PID控制，因此PID的采样周期为100ms
@param		None
@retval 	None
*/
void TIM1_CC_IRQHandler(void);

/**
@brief		用于调试模糊决策算法的距离参数，包括安全阈值，障碍物距离远，障碍物距离近，模糊逻辑的作用时间，模糊逻辑算法的时间变化量，速度快的型心，速度慢的型心（参考论文模糊逻辑算法）
@param		None
@retval 	None
@note		通过调整上述的参数可以优化小车的避障效果；距离单位为m，时间单位为ms，速度单位为dm/s
*/
void USART2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
