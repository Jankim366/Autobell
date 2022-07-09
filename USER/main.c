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

/////////////////////////UCOSII��������///////////////////////////////////

//�����ջ
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

extern __IO uint16_t ADC1ConvertedValue, ADC2ConvertedValue, ADC3ConvertedValue;  //��ӦPC 4/3/2

int main(void)
{
	Stm32_Clock_Init(9);	//ϵͳʱ������
	delay_init(72);			//��ʱ��ʼ��
	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
	usart1_init(72,115200);	//��ʼ��USART1
	usart2_init(36,115200);	//��ʼ��USART2
	usart3_init(36,115200);	//��ʼ��USART3
	power_config();			//����ADC

	
	strcpy(system_param.SID, "RB001");
	
	printf("\r\n Start OS!\r\n");

	OSInit();				//��ʼ��uCOSII

	WIFI_Sem = OSSemCreate(0);
	if (WIFI_Sem == 0) {
		printf("\r\n OSSemCreate ERROR! NO event control blocks were available.\r\n");
		return -1;
	}
	USART2_Mutex = OSMutexCreate(2,&err);   //��OSMutexCreate��prio����������ͬ�������������ȼ���ͬ
	if (err != 0) {    
		printf("\r\n OSMutexCreate ERROR! USART2_Mutex: err: %d\r\n", err);
		return -1;
	}
	USART3_Mutex = OSMutexCreate(3,&err);   //��OSMutexCreate��prio����������ͬ�������������ȼ���ͬ
	if (err != 0) {    
		printf("\r\n OSMutexCreate ERROR! USART3_Mutex: err: %d\r\n", err);
		return -1;
	}

	MboxSem_beep = OSMboxCreate((void *)0);
	if (MboxSem_beep == 0) {    
		printf("\r\nOSMboxCreate ERROR! No event control blocks were available!\r\n");
		return -1;
	}


	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();				//����ϵͳ
}

//��ʼ����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;

	pdata = pdata;

	printf("\r\n Start Tasks!\r\n");

	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)
	OSTaskCreate(LED1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
	OSTaskCreate(LED2_task,(void *)0,(OS_STK*)&LED2_TASK_STK[LED2_STK_SIZE-1],LED2_TASK_PRIO);
	OSTaskCreate(beep_task,(void *)0,(OS_STK*)&BEEP_TASK_STK[BEEP_STK_SIZE-1],BEEP_TASK_PRIO);
	OSTaskCreate(communication_task,(void *)0, (OS_STK*)&COMMUNICATION_TASK_STK[COMMUNICATION_TASK_STK_SIZE-1], COMMUNICATION_TASK_PRIO);
	OSTaskCreate(power_task,(void *)0,(OS_STK*)&POWER_TASK_STK[POWER_STK_SIZE-1],POWER_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

//LED1����
void LED1_task(void *pdata)
{
	while(1) {
		LED1=0;
		delay_ms(500);
		LED1=1;
		delay_ms(500);
	}
}

//LED2����
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

//��ص�ѹ�������
void power_task(void *pdata)
{
	u32 serial = 0;
	float voltage = 0;
	
	OSSemPend(WIFI_Sem, 0, &err);	//�ȴ���������
	if (err != 0) {
		printf("\r\nIn power_task(): OSSemPend(): err: %d\r\n", err);
	}

	while(1) {
		printf("\r\n --In power_task()\r\n");
		voltage = 3.3f/4095*ADC1ConvertedValue;
		printf("\r\n ADC: %f\r\n", voltage); //��ӦPC4
		wifi_send_cmd(serial++, "POWER", (char*)&voltage, sizeof(float));
		if (serial == 10000) {
			serial = 0;
		}
		
		delay_ms(1000);
	}
}

