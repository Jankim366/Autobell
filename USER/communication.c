/***************************************
File name: communication.c
Author: vcanv
Version: 2.0
Date: 2020
Description: communication
****************************************/
#include <string.h>
#include "communication.h"
#include "usart3.h"	
#include "delay.h"
#include "beep.h"
#include "esp8266.h"


extern uint8_t err;
extern struct system_param_info system_param;
extern OS_EVENT *WIFI_Sem;
extern OS_EVENT *USART3_Mutex;
extern OS_EVENT *MboxSem_beep;


void communication_task(void *pdata)
{
	char send_buf[MAX_WIFI_MESSAGE_SIZE];
	char recv_buf[MAX_WIFI_BUF_SIZE];
	char wifi_message[MAX_WIFI_MESSAGE_SIZE + 1];
	char error_msg[20];
	struct wifi_message_head wifi_msg_recv, wifi_msg_send;
	u16 len, total_len, message_len, rear, front;
	int i, j, k, param_len, ret, match_cmd_flag, match_0xff_flag, match_again_flag;
	char param_len_str[WIFI_MESSAGE_PARAM_SIZE];
	enum BEEP_STATUS beep_action;
	
	
	wifi_connect();

	ret = OSSemPost(WIFI_Sem);      //通知power任务可工作
	if (ret != 0) {
		printf("\r\n In communication_task(): OSSemPost: ret: %d \r\n", ret);
	}

	total_len = 0;
	rear = 0;
	front = 0;
	memset(recv_buf, 0, MAX_WIFI_BUF_SIZE);
	while (1) {
		len = 0;
		param_len = 0;
		match_cmd_flag = 0;
		match_0xff_flag = 0;
		match_again_flag = 0;
		memset(send_buf, 0, MAX_WIFI_MESSAGE_SIZE);
		memset(param_len_str, 0, WIFI_MESSAGE_PARAM_SIZE);
		memset(error_msg, 0, 20);
		memset(&wifi_msg_recv, 0, sizeof(struct wifi_message_head));
		memset(&wifi_msg_send, 0, sizeof(struct wifi_message_head));

		OSMutexPend(USART3_Mutex,0,&err);
		if (err != 0) {    
			printf("\r\n 10:OSMutexPend ERROR! USART3_Mutex: err: %d\r\n", err);
		}
		USART3_Receive_Data((u8 *)recv_buf, rear, total_len, &len);
		ret = OSMutexPost(USART3_Mutex);
		if (ret != 0) {    
			printf("\r\n In communication_task(): OSMutexPost ERROR! USART3_Mutex: ret: %d\r\n", ret);
		}
		//printf("\r\n Receive len: %d\r\n", len);
		
		rear = (rear + len)%MAX_WIFI_BUF_SIZE;
		total_len += len;
		// for (i = 0; i < total_len; i++) {
			// printf("%02X ", recv_buf[(front + i)%MAX_WIFI_BUF_SIZE]);
		// }
		// printf("\r\n");
		if (total_len >= sizeof(struct wifi_message_head) + WIFI_MESSAGE_END_SIZE) {
			printf("\r\n Before total_len: %d\r\n", total_len);
			for (i = WIFI_FLAG_CHAR_POS; i <= total_len - (sizeof(struct wifi_message_head) + WIFI_MESSAGE_END_SIZE) + WIFI_FLAG_CHAR_POS; i++) {
				if (recv_buf[(front + i)%MAX_WIFI_BUF_SIZE] == 'R' && recv_buf[(front + i + 5)%MAX_WIFI_BUF_SIZE] == 'R') {
					// printf("\r\n recv_buf buf: \r\n");
					// for (k = 0; k < sizeof(struct wifi_message_head); k++ ) {
							// printf("%02X ", recv_buf[(front + i - WIFI_FLAG_CHAR_POS + k)%MAX_WIFI_BUF_SIZE]);
					// }
					printf("\r\n");
					param_len_str[0] = recv_buf[(front + i - WIFI_MESSAGE_PARAM_SIZE)%MAX_WIFI_BUF_SIZE];
					param_len_str[1] = recv_buf[(front + i - WIFI_MESSAGE_PARAM_SIZE + 1)%MAX_WIFI_BUF_SIZE];
					param_len = (unsigned int)atoi((const char *)param_len_str);
					// printf("\r\n param_len_str[0]: %c, param_len_str[1]: %c, param_len_str[2]: %c, param_len: %d \r\n", param_len_str[0], param_len_str[1], param_len_str[2], param_len);
					// printf("\r\n recv_buf[front]: %c, recv_buf[front + 1]: %c, recv_buf[front + 2]: %c \r\n", recv_buf[(front + i - WIFI_FLAG_CHAR_POS)%MAX_WIFI_BUF_SIZE], recv_buf[(front + i - WIFI_FLAG_CHAR_POS + 1)%MAX_WIFI_BUF_SIZE], recv_buf[(front + i - WIFI_FLAG_CHAR_POS + 2)%MAX_WIFI_BUF_SIZE]);
					if (param_len < 0 || param_len > (MAX_WIFI_MESSAGE_SIZE - sizeof(struct wifi_message_head)) - WIFI_MESSAGE_END_SIZE) {
						printf("\r\nCOMMAND Param_len ERROR!\r\n");
						continue;
					}
					message_len = sizeof(struct wifi_message_head) + param_len + WIFI_MESSAGE_END_SIZE;	//头部+参数+校验码+结束符
					// printf("\r\n message_len: %d i: %d front: %d\r\n", message_len, i, front);
					// printf("recv_buf buf: \r\n");
					// for (l = 0; l < message_len; l++ ) {
							// printf("%c", recv_buf[(front + i - WIFI_FLAG_CHAR_POS + l)%MAX_WIFI_BUF_SIZE]);
					// }
					if (total_len >= (i + message_len - WIFI_FLAG_CHAR_POS)) {
						for (k = message_len; k > WIFI_MESSAGE_HEAD_SIZE; k--) {
							if (recv_buf[(front + i - WIFI_FLAG_CHAR_POS + k - 1)%MAX_WIFI_BUF_SIZE] == '\n') {
								match_0xff_flag = 1;
								break;
							}
						}
						
						if (!match_0xff_flag) {		//往回找不到'\n'
							printf("\r\n No '\n' ERROR!\r\n");
							total_len = total_len - (i + sizeof(struct wifi_message_head) - WIFI_FLAG_CHAR_POS);
							front = (front + i + sizeof(struct wifi_message_head) - WIFI_FLAG_CHAR_POS)%MAX_WIFI_BUF_SIZE;
							match_again_flag = 1;
							break;
						} else if (k < message_len) {	//找到'\n'不在报文末尾，下次从下一字节开始匹配
							printf("\r\n '\n' shift > 2 ERROR!\r\n");
							total_len = total_len - (i + k - WIFI_FLAG_CHAR_POS);
							front = (front + i + k - WIFI_FLAG_CHAR_POS)%MAX_WIFI_BUF_SIZE;
							match_again_flag = 1;
							break;
						} else {						//找到'\n'在末尾
							for (j = 0; j < message_len; j++) {
								wifi_message[j] = recv_buf[(front + i - WIFI_FLAG_CHAR_POS + j)%MAX_WIFI_BUF_SIZE];
							}
							total_len = total_len - (i + message_len - WIFI_FLAG_CHAR_POS);
							front = (front + i + message_len - WIFI_FLAG_CHAR_POS)%MAX_WIFI_BUF_SIZE;
							match_cmd_flag = 1;
							break;
						}
					} else {
						printf("\r\n ERROR: message_len!\r\n");
					}
				} else {
					printf("\r\n ERROR: Wifi command not match!\r\n");
				}
			}
			if (!match_cmd_flag && !match_again_flag) {
				front = (front + i)%MAX_WIFI_BUF_SIZE;
				total_len -= i;
			}
			printf("\r\n After total_len: %d\r\n", total_len);
		}
		if (match_cmd_flag) {
			strncpy(wifi_msg_recv.serial, &wifi_message[0], 4);
			strncpy(wifi_msg_recv.param_len, &wifi_message[4], 2);
			strncpy(wifi_msg_recv.source, &wifi_message[6], 5);
			strncpy(wifi_msg_recv.destination, &wifi_message[11], 5);
			strncpy(wifi_msg_recv.cmd, &wifi_message[16], 8);
			
			if (!strncmp(wifi_msg_recv.destination, system_param.SID, 5) || !strncmp(wifi_msg_recv.destination, "RB999", 5)) {
				printf("\r\n wifi_msg:\r\n");
				printf("wifi_msg_recv.serial: %.4s\r\n", wifi_msg_recv.serial);
				printf("wifi_msg_recv.param_len: %.2s\r\n", wifi_msg_recv.param_len);
				printf("wifi_msg_recv.source: %.5s\r\n", wifi_msg_recv.source);
				printf("wifi_msg_recv.destination: %.5s\r\n", wifi_msg_recv.destination);
				printf("wifi_msg_recv.cmd: %.8s\r\n", wifi_msg_recv.cmd);

				strcpy(wifi_msg_send.serial, wifi_msg_recv.serial);
				strcpy(wifi_msg_send.destination, wifi_msg_recv.source);
				strcpy(wifi_msg_send.source, wifi_msg_recv.destination);
				strcpy(wifi_msg_send.cmd, wifi_msg_recv.cmd);

				if (strncmp(wifi_msg_recv.cmd, "ONESOUND", 8) == 0) {
					printf("\r\n wifi_msg_recv.cmd: %.8s\r\n", wifi_msg_recv.cmd);
					beep_action = ONE_SOUND;
					ret = OSMboxPostOpt(MboxSem_beep, (void *)&beep_action, OS_POST_OPT_BROADCAST);
					if (ret != OS_ERR_NONE) {
						printf("\r\n In communication_task: Send mail to MboxSem_beep was not successful\r\n");
					}
				} else if (strncmp(wifi_msg_recv.cmd, "TWOSOUND", 8) == 0) {
					printf("\r\n wifi_msg_recv.cmd: %.8s\r\n", wifi_msg_recv.cmd);
					beep_action = TWO_SOUND;
					ret = OSMboxPostOpt(MboxSem_beep, (void *)&beep_action, OS_POST_OPT_BROADCAST);
					if (ret != OS_ERR_NONE) {
						printf("\r\n In communication_task: Send mail to MboxSem_beep was not successful\r\n");
					}
				} else {
					printf("\r\n ERROR wifi_msg_recv.cmd: %.8s\r\n", wifi_msg_recv.cmd);
				}
			} else {
				printf("\r\n Not Match! wifi_msg_recv.destination: %.5s, system_param.SID: %s\r\n", wifi_msg_recv.destination, system_param.SID);
			}
		} else {
			delay_ms(100);
		}
	}
}

void wifi_send(char *send_buf, u16 len)
{
	int ret;
	
	printf("\r\n Send data use wifi.\r\n");
	OSMutexPend(USART3_Mutex,0,&err);
	if (err != 0) {    
		printf("\r\n10:OSMutexPend ERROR! USART3_Mutex: err: %d\r\n", err);
	}
	delay_ms(15);
	USART3_Send_Data((u8 *)send_buf, len);
	ret = OSMutexPost(USART3_Mutex);
	if (ret != 0) {    
		printf("\r\n In communication_task(): OSMutexPost ERROR! USART3_Mutex: ret: %d\r\n", ret);
	}
}

void wifi_send_cmd(int serial, char *cmd, char *result, int param_len)
{
	struct wifi_message_head wifi_msg_send;
	char send_buf[MAX_WIFI_MESSAGE_SIZE];
	unsigned int len;
	
	memset(&wifi_msg_send, 0, sizeof(struct wifi_message_head));
	memset(send_buf, 0, MAX_WIFI_MESSAGE_SIZE);

	if (serial < 0 || serial > 999) {
		serial = 0;
	}
	sprintf(wifi_msg_send.serial, "%d", ++serial);
	sprintf(wifi_msg_send.param_len, "%d", param_len + 3);
	strcpy(wifi_msg_send.source, system_param.SID);
	strcpy(wifi_msg_send.destination, "RB000");
	if (cmd != NULL) {
		strcpy(wifi_msg_send.cmd, cmd);
	} else {
		strcpy(wifi_msg_send.cmd, "DataSink");
	}
	
	len = sizeof(struct wifi_message_head);
	memcpy(send_buf, &wifi_msg_send, len);

	memcpy(send_buf + len, result, param_len);
	len += param_len;

	send_buf[len] = '\r';
	len += 1;
	send_buf[len] = '\n';
	len += 1;

	wifi_send(send_buf, len);
}
