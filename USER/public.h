/***************************************
File name: public.h
Author: vcanv
Version: 1.0
Date: 2015
Description: ����ͷ�ļ����궨�塢�ṹ�嶨��
****************************************/
#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#include "includes.h"
#include "task.h"


struct system_param_info {
	int exist_flag;
	char SID[20];
	char SensorInfo[48];
	char Vendor[16];
	char Model[8];
	char VER[8];
};

#endif

