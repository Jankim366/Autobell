#include "includes.h"

/////////////////////////UCOSII任务设置///////////////////////////////////

//设置任务优先级
#define START_TASK_PRIO					9
#define LED1_TASK_PRIO					11
#define LED2_TASK_PRIO					12
#define BEEP_TASK_PRIO					13
#define COMMUNICATION_TASK_PRIO			14
#define POWER_TASK_PRIO					15


//设置任务堆栈大小
#define START_STK_SIZE					512
#define LED1_STK_SIZE					128
#define LED2_STK_SIZE				512
#define BEEP_STK_SIZE					256
#define COMMUNICATION_TASK_STK_SIZE		1024
#define POWER_STK_SIZE					512


//任务函数声明
void start_task(void *pdata);
void LED1_task(void *pdata);
void LED2_task(void *pdata);
void beep_task(void *pdata);
void communication_task(void *pdata);
void power_task(void *pdata);

