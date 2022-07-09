/**
@file		power.c
@brief		实现有关的函数
@author		power
*/

#include "adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

static void power_rcc_config(void);
static void power_gpio_config(void);
static void power_dma_config(void);
static void power_adc_config(void);
static void power_nvic_config(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define ADC3_DR_Address    ((uint32_t)0x40013C4C)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
__IO uint16_t ADC1ConvertedValue = 0, ADC3ConvertedValue = 0;
__IO uint16_t ADC2ConvertedValue;


/**
@brief		配置时钟，GPIO口，ADC，中断
@param		None
@retval 	None
*/
void power_config(void)
{
	power_rcc_config();
	power_gpio_config();
	power_dma_config();
	power_adc_config();
	power_nvic_config();
}

/**
@brief		配置时钟
@param		None
@retval 	None
*/
static void power_rcc_config(void)
{
  /* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
    
  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 and DMA2 clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);

  /* Enable ADC1, ADC2, ADC3 and GPIOC clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 |
                         RCC_APB2Periph_ADC3 | RCC_APB2Periph_GPIOC, ENABLE);
}

/**
@brief		配置GPIO口
@param		None
@retval 	None
@note
*/
static void power_gpio_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure PC.02, PC.03 and PC.04 (ADC Channel12, ADC Channel13 and 
     ADC Channel14) as analog inputs */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

}
/**
@brief		配置DMA
@param		None
@retval 	None
@note
*/
static void power_dma_config(void)
{
  /* DMA1 channel 1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC1ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  /* DMA2 channel 5 configuration ----------------------------------------------*/
  DMA_DeInit(DMA2_Channel5);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC3_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC3ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA2_Channel5, &DMA_InitStructure);  
  /* Enable DMA2 channel5 */
  DMA_Cmd(DMA2_Channel5, ENABLE);

}

/**
@brief		配置ADC
@param		None
@retval 	None
@note
*/
static void power_adc_config(void)
{
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channels configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_28Cycles5);    
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* ADC2 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC2, &ADC_InitStructure);
  /* ADC2 regular channels configuration */ 
  ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 1, ADC_SampleTime_28Cycles5);
  /* Enable ADC2 EOC interrupt */
  ADC_ITConfig(ADC2, ADC_IT_EOC, ENABLE);

  /* ADC3 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC3, &ADC_InitStructure);
  /* ADC3 regular channel14 configuration */ 
  ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_28Cycles5);
  /* Enable ADC3 DMA */
  ADC_DMACmd(ADC3, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  /* Enable ADC2 */
  ADC_Cmd(ADC2, ENABLE);

  /* Enable ADC2 reset calibration register */   
  ADC_ResetCalibration(ADC2);
  /* Check the end of ADC2 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC2));

  /* Start ADC2 calibration */
  ADC_StartCalibration(ADC2);
  /* Check the end of ADC2 calibration */
  while(ADC_GetCalibrationStatus(ADC2));

  /* Enable ADC3 */
  ADC_Cmd(ADC3, ENABLE);

  /* Enable ADC3 reset calibration register */   
  ADC_ResetCalibration(ADC3);
  /* Check the end of ADC3 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC3));

  /* Start ADC3 calibration */
  ADC_StartCalibration(ADC3);
  /* Check the end of ADC3 calibration */
  while(ADC_GetCalibrationStatus(ADC3));

  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  /* Start ADC2 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC2, ENABLE);
  /* Start ADC3 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC3, ENABLE);
}
/**
@brief		配置中断
@param		None
@retval 	None
@note
*/
static void power_nvic_config(void){
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure and enable ADC interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
