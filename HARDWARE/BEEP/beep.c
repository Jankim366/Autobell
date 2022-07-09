#include "stm32f10x.h"
#include "includes.h"
#include "delay.h"
#include "beep.h"
#include "stm32f10x_gpio.h"

extern OS_EVENT *MboxSem_beep;


void beep_gpio_config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = BEEP_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStructure);

	GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_RESET);
}

//beep_task任务
void beep_task(void *pdata)
{
	u8 err;
	enum BEEP_STATUS *beep_status_action;

	beep_gpio_config();

	while(1) {
		printf("\r\n --In beep_task()\r\n");

		OSMboxAccept(MboxSem_beep);			//清空消息邮箱，防止读到旧数据
		beep_status_action = (enum BEEP_STATUS *)OSMboxPend(MboxSem_beep, 0, &err); //阻塞等待
		if (err == OS_ERR_NONE) {
			if (*beep_status_action == ALWAYS_SOUND) {
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_SET);
				delay_ms(200);
			} else if (*beep_status_action == ONE_SOUND) {
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_SET);
				delay_ms(200);
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_RESET);
				delay_ms(200);
			} else if (*beep_status_action == TWO_SOUND) {
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_SET);
				delay_ms(100);
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_RESET);
				delay_ms(100);
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_SET);
				delay_ms(100);
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_RESET);
				delay_ms(100);
			} else if (*beep_status_action == THREE_SOUND) {
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_SET);
				delay_ms(80);
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_RESET);
				delay_ms(80);
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_SET);
				delay_ms(80);
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_RESET);
				delay_ms(80);
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_SET);
				delay_ms(80);
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_RESET);
				delay_ms(80);
			} else if (*beep_status_action == KEEP_SILENT) {
				GPIO_WriteBit(BEEP_GPIO_Port, BEEP_Pin, Bit_RESET);
				delay_ms(100);
			} else {
				printf("\r\n beep_task: unknow beep_status_action\r\n");
			}
		} else {
			printf("\r\n beep_task: OSMboxPend ERROR: %d \r\n", err);
			delay_ms(200);
		}
	}
}
