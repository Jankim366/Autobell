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

#define WIFI_MESSAGE_HEAD_SIZE sizeof(struct wifi_message_head)		//����ͷ������
#define WIFI_MESSAGE_PARAM_SIZE	2									//���Ĳ����ֶεĳ���
#define WIFI_MESSAGE_END_SIZE		2									//���Ľ���������
#define MAX_WIFI_BUF_SIZE			1024									//�����ջ�������С
#define MAX_WIFI_MESSAGE_SIZE		85									//���Ʊ��ĳ������85�ֽ�
#define MAX_WIFI_MESSAGE_PARAM_SIZE MAX_WIFI_MESSAGE_SIZE-WIFI_MESSAGE_HEAD_SIZE-WIFI_MESSAGE_END_SIZE
#define WIFI_FLAG_CHAR_POS		6									//��־�ַ���λ�ã���R�ַ���6��λ

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
