#ifndef __ESP8266_H
#define __ESP8266_H

#define MAX_WIFI_SEND_BUF_SIZE 1024
#define MAX_WIFI_RECV_BUF_SIZE 1024

int Check_WIFI_ESP8266(void);
void wifi_connect(void);
void WIFI_Send_Recv(char *command, char *recv_buf);


#endif /* _ESP8266_H */







