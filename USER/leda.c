/*
@file	led.c
@brief	实现控制LED相关的接口
*/
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "delay.h"

static void rcc_init(void);
static void gpio_init(void);

/*
@brief	初始化与LED控制相关的时钟和GPIO
@params	None
@retval	None
*/
void led_init(void){
	rcc_init();
	gpio_init();
}



/*
@brief	通过置低电平点亮LED灯
@params	None
@retval	None
*/

void led_red(void){
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
}

void led_green(void){
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
}
/*
@brief	通过置高电平熄灭LED灯
@params	None
@retval	None
*/


/*
@brief	初始化LED控制相关的时钟
@params	None
@retval	None
@note	这是一个私有函数
*/
static void rcc_init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
}

/*
@brief	初始化LED控制相关的GPIO
@params	None
@retval	None
@note	这是一个私有函数
*/
static void gpio_init(void){
	GPIO_InitTypeDef gpio_init;

	//将PA1设置为推挽输出
	gpio_init.GPIO_Mode=GPIO_Mode_Out_PP;
	gpio_init.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_8|GPIO_Pin_6;
	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio_init);
} 
