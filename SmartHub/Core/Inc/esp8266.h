#ifndef _ESP8266_H_
#define _ESP8266_H_

unsigned char ESP8266_Init(void);   //ESP8266初始化

void ESP8266_Clear(void);   //清空ESP8266缓存

unsigned char ESP8266_SendCmd(const char *cmd, char *res);    //发送AT指令

void ESP8266_SendLenData(unsigned char *data, unsigned short len);  //发送长度为len的数据

void ESP8266_SendData(unsigned char **data, unsigned char data_num); //透传模式下发送数据

void ESP8266_GetResponse(const char **data, unsigned char data_num); //ESP8266获取响应

unsigned short ESP8266_ReadData(unsigned char *buffer, unsigned short max_len); //读取缓冲区数据

#endif
