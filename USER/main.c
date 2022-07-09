#include "sys.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "led.h"
#include "includes.h"
#include "public.h"
#include "adc.h"
#include "communication.h"
#include "beep.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "leda.h"
#include "keya.h"

/////////////////////////UCOSII任务设置///////////////////////////////////

//任务堆栈
__align(8) OS_STK START_TASK_STK[START_STK_SIZE];
__align(8) OS_STK LED1_TASK_STK[LED1_STK_SIZE];
__align(8) OS_STK LED2_TASK_STK[LED2_STK_SIZE];
__align(8) OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
__align(8) OS_STK COMMUNICATION_TASK_STK[COMMUNICATION_TASK_STK_SIZE];
__align(8) OS_STK POWER_TASK_STK[POWER_STK_SIZE];

uint8_t err;
struct system_param_info system_param;

OS_EVENT *WIFI_Sem;
OS_EVENT *USART2_Mutex;
OS_EVENT *USART3_Mutex;
OS_EVENT *MboxSem_beep;

void system_param_init(void);
void system_param_set(void);
void modular_use_function(void);
void led0_task(void *pdata);
void led1_task(void *pdata);

extern __IO uint16_t ADC1ConvertedValue, ADC2ConvertedValue, ADC3ConvertedValue;  //对应PC 4/3/2

int main(void)
{
	Stm32_Clock_Init(9);	//系统时钟设置
	delay_init(72);			//延时初始化
	LED_Init();				//初始化与LED连接的硬件接口
	usart1_init(72,115200);	//初始化USART1
	usart2_init(36,115200);	//初始化USART2
	usart3_init(36,115200);	//初始化USART3
	power_config();			//配置ADC

	
	strcpy(system_param.SID, "RB001");
	
	printf("\r\n Start OS!\r\n");

	OSInit();				//初始化uCOSII

	WIFI_Sem = OSSemCreate(0);
	if (WIFI_Sem == 0) {
		printf("\r\n OSSemCreate ERROR! NO event control blocks were available.\r\n");
		return -1;
	}
	USART2_Mutex = OSMutexCreate(2,&err);   //各OSMutexCreate的prio参数不能相同，不与任务优先级相同
	if (err != 0) {    
		printf("\r\n OSMutexCreate ERROR! USART2_Mutex: err: %d\r\n", err);
		return -1;
	}
	USART3_Mutex = OSMutexCreate(3,&err);   //各OSMutexCreate的prio参数不能相同，不与任务优先级相同
	if (err != 0) {    
		printf("\r\n OSMutexCreate ERROR! USART3_Mutex: err: %d\r\n", err);
		return -1;
	}

	MboxSem_beep = OSMboxCreate((void *)0);
	if (MboxSem_beep == 0) {    
		printf("\r\nOSMboxCreate ERROR! No event control blocks were available!\r\n");
		return -1;
	}


	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();				//启动系统
}

//开始任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;

	pdata = pdata;

	printf("\r\n Start Tasks!\r\n");

	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)
	OSTaskCreate(LED1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
	OSTaskCreate(LED2_task,(void *)0,(OS_STK*)&LED2_TASK_STK[LED2_STK_SIZE-1],LED2_TASK_PRIO);
	OSTaskCreate(beep_task,(void *)0,(OS_STK*)&BEEP_TASK_STK[BEEP_STK_SIZE-1],BEEP_TASK_PRIO);
	OSTaskCreate(communication_task,(void *)0, (OS_STK*)&COMMUNICATION_TASK_STK[COMMUNICATION_TASK_STK_SIZE-1], COMMUNICATION_TASK_PRIO);
	OSTaskCreate(power_task,(void *)0,(OS_STK*)&POWER_TASK_STK[POWER_STK_SIZE-1],POWER_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

//LED1任务
void LED1_task(void *pdata)
{
	while(1) {
		LED1=0;
		delay_ms(500);
		LED1=1;
		delay_ms(500);
	}
}

//LED2任务
void LED2_task(void *pdata)
{
	while(1) {
//		LED2=0;
//		delay_ms(100);
//		LED2=1;
//		delay_ms(900);
	led_init();	//???LED1
	key_init();	//?????S1
	
	while(1){
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)!=0){	//????
			//????
			led_green();		//??LED1
			
		 delay_ms(900);
		}else{	
			led_red();	//??LED1
			
		delay_ms(900);
		}
	}
		
		
	}
}

//电池电压监测任务
void power_task(void *pdata)
{
	u32 serial = 0;
	float voltage = 0;
	
	OSSemPend(WIFI_Sem, 0, &err);	//等待建立网络
	if (err != 0) {
		printf("\r\nIn power_task(): OSSemPend(): err: %d\r\n", err);
	}

	while(1) {
		printf("\r\n --In power_task()\r\n");
		voltage = 3.3f/4095*ADC1ConvertedValue;
		printf("\r\n ADC: %f\r\n", voltage); //对应PC4
		wifi_send_cmd(serial++, "POWER", (char*)&voltage, sizeof(float));
		if (serial == 10000) {
			serial = 0;
		}
		
		delay_ms(1000);
	}
}

