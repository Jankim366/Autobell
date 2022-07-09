/***************************************
File name: communication.h
Author: vcanv
Version: 2.0
Date: 2020
Description: communication
****************************************/
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_
#include "public.h"

#define WIFI_MESSAGE_HEAD_SIZE sizeof(struct wifi_message_head)		//报文头部长度
#define WIFI_MESSAGE_PARAM_SIZE	2									//报文参数字段的长度
#define WIFI_MESSAGE_END_SIZE		2									//报文结束符长度
#define MAX_WIFI_BUF_SIZE			1024									//最大接收缓冲区大小
#define MAX_WIFI_MESSAGE_SIZE		85									//限制报文长度最多85字节
#define MAX_WIFI_MESSAGE_PARAM_SIZE MAX_WIFI_MESSAGE_SIZE-WIFI_MESSAGE_HEAD_SIZE-WIFI_MESSAGE_END_SIZE
#define WIFI_FLAG_CHAR_POS		6									//标志字符的位置，如R字符在6号位

struct wifi_message_head {
    char serial[4];
    char param_len[2];
    char source[5];
    char destination[5];
    char cmd[8];
};

void communication_task(void *pdata);
void wifi_send(char *send_buf, uint16_t len);
void wifi_send_cmd(int serial, char *cmd, char *result, int param_len);


#endif
