#ifndef _ESP8266_H_
#define _ESP8266_H_

#define OK  1
#define ERROR 0

extern const char* const key; // API私钥
extern const char* const location;	   // 地区
extern const char* const days;	    //要查询的天数
extern const char* const get_daily_weather; //获取未来天气的请求头
extern const char* const get_now_weather; //获取实况天气的请求头
extern const char* const connect_server;    //连接TCP服务器指令
extern const char* const send_data_cmd;     //透传模式下准备发送指令，期望收到'>'
extern const char* const close_tcp_cmd;

unsigned char ESP8266_Init(void);   //ESP8266初始化

void ESP8266_Clear(void);   //清空ESP8266缓存

unsigned char ESP8266_SendCmd(const char *cmd, char *res);    //发送AT指令

void ESP8266_SendLenData(unsigned char *data, unsigned short len);  //发送长度为len的数据

void ESP8266_SendData(unsigned char *data); //发送数据

void ESP8266_GetResponse(const char **data, unsigned char data_num); //ESP8266获取响应

unsigned short ESP8266_ReadData(unsigned char *buffer, unsigned short max_len); //读取缓冲区数据


#endif
