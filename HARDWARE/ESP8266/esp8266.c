#include "sys.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "includes.h"
#include "esp8266.h"

extern uint8_t err;
extern OS_EVENT *USART3_Mutex;

char *replaceRN(char *str, unsigned int len);

int Check_WIFI_ESP8266(void)
{
	char recv_buf[MAX_WIFI_RECV_BUF_SIZE];
	char command[50];

	memset(recv_buf, 0, MAX_WIFI_RECV_BUF_SIZE);

	printf("\r\n WIFI: Detecting ESP8266 ...\r\n");
	
	strcpy(command, "+++");                     //退出透传模式
	WIFI_Send_Recv(command, recv_buf);
	delay_ms(100);
	
	strcpy(command, "ATE0\r\n");                //关闭回显
	WIFI_Send_Recv(command, recv_buf);
	
	if (strstr(recv_buf, "OK")) {
		return 1;
	} else {
		return 0;
	}
}


void wifi_connect(void)
{
	char recv_buf[MAX_WIFI_RECV_BUF_SIZE];
	char command[50];

	while (Check_WIFI_ESP8266() == 0) {
		delay_ms(1000);
	}
	printf("\r\n In wifi_connect(): ESP8266 is Detected.\r\n");

	strcpy(command, "AT+CWMODE_DEF=1\r\n");     //设置Wi-Fi模式(STA/AP/STA+AP)，保存到Flash
	WIFI_Send_Recv(command, recv_buf);

	strcpy(command, "AT+CWAUTOCONN=0\r\n");     //设置上电时是否自动连接AP
	WIFI_Send_Recv(command, recv_buf);

	strcpy(command, "AT+CWJAP_CUR=\"cpdd\",\"11111111\"\r\n");  //连接AP，不保存到Flash
	WIFI_Send_Recv(command, recv_buf);

	strcpy(command, "AT+CIFSR\r\n");            //查询本地IP地址
	WIFI_Send_Recv(command, recv_buf);

	strcpy(command, "AT+CIPSTART=\"TCP\",\"192.168.43.107\",8080\r\n");  //建立TCP连接
	WIFI_Send_Recv(command, recv_buf);

	strcpy(command, "AT+CIPMODE=1\r\n");        //设置透传模式
	WIFI_Send_Recv(command, recv_buf);

	strcpy(command, "AT+CIPSEND\r\n");          //发送数据
	WIFI_Send_Recv(command, recv_buf);
	
	printf("\r\n WIFI: Connect has finished.\r\n");
}


void WIFI_Send_Recv(char *command, char *recv_buf)
{
	u16 len;
	int ret;


	OSMutexPend(USART3_Mutex,0,&err);
	if (err != 0) {
		printf("\r\n10:OSMutexPend ERROR! USART3_Mutex: err: %d\r\n", err);
	}
	USART3_Receive_Data_No_Queue((u8 *)recv_buf, &len); //读一次，目的是为清空已存在数据
	// printf("\r\n In WIFI_Send_Recv(): recv_buf: %s, len: %d\r\n", recv_buf, len);
	ret = OSMutexPost(USART3_Mutex);
	if (ret != 0) {    
		printf("\r\n In WIFI_Send_Recv(): OSMutexPost ERROR! USART3_Mutex: ret: %d\r\n", ret);
	}

	delay_ms(15);
	
	OSMutexPend(USART3_Mutex,0,&err);
	if (err != 0) {
		printf("\r\n10:OSMutexPend ERROR! USART3_Mutex: err: %d\r\n", err);
	}
	USART3_Send_Data((u8 *)command, strlen(command));
	ret = OSMutexPost(USART3_Mutex);
	if (ret != 0) {    
		printf("\r\n In WIFI_Send_Recv(): OSMutexPost ERROR! USART3_Mutex: ret: %d\r\n", ret);
	}

	memset(recv_buf, 0, MAX_WIFI_RECV_BUF_SIZE);
	if (strstr(command, "AT+CWJAP")) {
		delay_ms(5000);
	} else {
		delay_ms(2000);
	}

	OSMutexPend(USART3_Mutex,0,&err);
	if (err != 0) {
		printf("\r\n10:OSMutexPend ERROR! USART3_Mutex: err: %d\r\n", err);
	}
	USART3_Receive_Data_No_Queue((u8 *)recv_buf, &len);
	ret = OSMutexPost(USART3_Mutex);
	if (ret != 0) {    
		printf("\r\n In WIFI_Send_Recv(): OSMutexPost ERROR! USART3_Mutex: ret: %d\r\n", ret);
	}

	printf("\r\n In WIFI_Send_Recv(): command: %s, recv_buf: %s, len: %d\r\n", replaceRN(command, strlen(command)), replaceRN(recv_buf, strlen(recv_buf)), len);
}

char *replaceRN(char *str, unsigned int len)    //替换\r和\n为空格
{
	int i;

	if (str != NULL) {
		for (i = 0; i < len; i++) {
			if (str[i] == '\r' || str[i] == '\n') {
			str[i] = ' ';
			}
		}
	}
	return str;
}
