/**
 * @file esp8266.c
 * @author lyx
 * @brief ESP8266驱动文件
 * @version 0.1
 * @date 2025-10-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "stm32f4xx_hal.h"
#include "rtc.h"
// 网络设备驱动
#include "esp8266.h"

// 硬件驱动
#include "usart.h"

// C库
#include <string.h>
#include <stdio.h>

//解析天气和日期
#include "weather.h" 

//freertos需要的头文件
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

#define SNTPTIME_LEN	43	//返回的SNTP时间的数据长度是固定的

/* ESP8266指令 */
const char* const test_at = "AT\r\n";															 // 测试AT指令
const char* const set_sta_mode = "AT+CWMODE=1\r\n";											 // 设置STA模式
const char* const set_dhcp = "AT+CWDHCP=1,1\r\n";												 // 设置DHCP
const char* const connect_wifi = "AT+CWJAP=\"Redmi K70\",\"qq1804068646\"\r\n";				 // 连接wifi指令
const char* const connect_server = "AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n"; // 连接TCP服务器指令
const char* const set_cipmode = "AT+CIPMODE=0\r\n";									// 非透传模式，发送数据时需要指定数据的大小
const char* const send_data_cmd = "AT+CIPSEND=%d\r\n";											 // 透传模式下发送数据
const char* const exit_send_cmd = "+++";														 // 退出透传模式
const char* const close_tcp_cmd = "AT+CIPCLOSE\r\n";											 // 关闭TCP连接
const char* const config_sntp = "AT+CIPSNTPCFG=1,8,\"ntp.aliyun.com\",\"ntp.sjtu.edu.cn\"\r\n"; // 配置SNTP服务器
const char* const get_sntp_time = "AT+CIPSNTPTIME?\r\n";										 // 获取STNP时间
const char* const wifi_state = "AT+CWSTATE?\r\n";	//获取WIFI状态

/* API可配置的参数 */
const char* const key = "SigzO8oaCNRAS96L8"; // API私钥
const char* const location = "chengdu";	   // 地区
const char* const days = "1";				   // 查询1~3天的天气，注：只实现了解析1天的未来天气

/* 获取未来天气的请求头 */
const char* const get_daily_weather = "GET /v3/weather/daily.json?key=%s&location=%s&language=en&unit=c&start=0&days=%s HTTP/1.1\r\nHost: api.seniverse.com\r\n\r\n";

/* 获取实况天气的请求头 */
const char* const get_now_weather = "GET /v3/weather/now.json?key=%s&location=%s&language=en&unit=c HTTP/1.1\r\nHost: api.seniverse.com\r\n\r\n";

/* 用于解析日期的缓冲区 */
uint8_t date_buffer[64];
uint8_t date_buffer_cnt;

/* 标志位，判断是否获取了正确的sntp, 1表示获取到了正确的sntp时间 */
uint8_t sntp_ok = 0;

static void ESP8266_RTC_CalendarConfig(void);	//配置RTC

/**
 * @brief 清空缓冲区
 *
 */
void ESP8266_Clear(void)
{
	memset(debugBuffer, 0, sizeof(debugBuffer));
	debug_data_len = 0;
}

/**
 * @brief ESP8266发送指令
 *
 * @param cmd 要发送的指令
 * @param res 期望收到的指令
 * @return unsigned char
 */
unsigned char ESP8266_SendCmd(const char *cmd, char *res)
{
	uint16_t timeOut = 200; // 超时时间2S

	/* 向ESP8266发送指令 */
	if (uart_transmit_str(huart1, (uint8_t *)cmd) != HAL_OK)
	{
		/* 发送失败 */
	}

	/* 等待ESP8266发来的数据 */
	while (timeOut--)
	{
		if (rx_data_ready == 1)
		{
			rx_data_ready = 0;
			if (strstr((const char *)debugBuffer, res) != NULL) // 如果检索到关键词
			{
				/* 如果获取时间指令，将返回的结果拷贝到新的缓冲区用于日期解析 */
				if(strstr(cmd, "SNTPTIME") != NULL)
				{
					memcpy(date_buffer, debugBuffer, debug_data_len);
					date_buffer_cnt = debug_data_len;
					/* 获取到正确的SNTP时间，而非1970 */
					if(strstr((char*)date_buffer, "1970") == NULL)
					{
						sntp_ok = 1;
					}
				}
				else if(strstr(cmd, "CWSTATE") != NULL)
				{
					memcpy(date_buffer, debugBuffer, debug_data_len);
					date_buffer_cnt = debug_data_len;
				}
				ESP8266_Clear(); // 清空缓存
				return 0;
			}
		}
		HAL_Delay(10);
	}

	/* 如果匹配失败也要清空缓存 */
	ESP8266_Clear();

	return 1;
}

/**
 * @brief ESP8266发送指定长度的数据
 *
 * @param data 要发送的数据
 * @param len 数据长度
 */
void ESP8266_SendLenData(unsigned char *data, unsigned short len)
{
}

/**
 * @brief 向服务器发送指定长度的数据
 *
 *
 * @param data 要发送的数据
 */
void ESP8266_SendData(unsigned char *data)
{

	char buffer2[128];
	char *ret;
	uint16_t timeOut = 200; // 等待5S
	future_weather_t wt;
	now_weather_t nwt;
	// ESP8266_Clear();						  // 清空接收缓存

	unsigned char now_flag = 0;		//0:是实况天气，1：是未来天气

	char cmd[256];

	sprintf(cmd, send_data_cmd, strlen((char*)data));

	/* 判断发送实况/未来天气请求 */
	if(strstr((char*)data, "now"))
	{
		now_flag = 0;
	}
	else if(strstr((char*)data, "daily"))
	{
		now_flag = 1;
	}
	else
	{
		now_flag = 2;
	}

	// 串口助手发送指令
	uart_transmit_str(huart3, (uint8_t*)cmd);
	

	unsigned char times = 5;

	while(ESP8266_SendCmd(connect_server, "OK") && (times > 0))
	{
		times--;
		HAL_Delay(500);
	}
	
	if(times != 0)
	{
		uart_transmit_str(huart3, (uint8_t *)"Connect Server Successed!\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"Connect Server Failed!\r\n");
		return;
	}

	HAL_Delay(500);

	if (!ESP8266_SendCmd(cmd, ">")) // 收到‘>’时可以发送数据
	{
		/* 向ESP8266发送数据 */
		uart_transmit_str(huart1, data);
		// uart_transmit_str(huart3, data);

		HAL_Delay(2000);	//延迟2s检查缓冲区


		/* 等待ESP8266发来的数据 */
		while (timeOut--)
		{
			if (rx_data_ready == 1) // 接收完成
			{
				rx_data_ready = 0; // 标志位置0，准备下次DMA接收

				// uart_transmit_str(huart3, debugBuffer);
				HAL_UART_Transmit_DMA(&huart3, debugBuffer, debug_data_len);

				if(now_flag == 0)
				{
					/* GET请求返回来的响应包括请求头，这里是为了找到json格式的数据以便解析 */
					ret = strchr((char *)debugBuffer, '{');

					/* 解析实况天气 */
					nwt = parse_now_weather_data(ret);

					/* 解析完后复制到全局变量，用于LVGL显示 */
					strcpy(mdt.weather, nwt.weather);
					strcpy(mdt.temp, nwt.temp);

					/* 实况天气写入缓冲区用于调试 */
					sprintf(buffer2, "%s, %s, %s, %s, %s\r\n", nwt.city_name, nwt.date, nwt.temp, nwt.weather, nwt.weather_code);
				}
				else if(now_flag == 1)
				{
					/* GET请求返回来的响应包括请求头，这里是为了找到json格式的数据以便解析 */
					ret = strchr((char *)debugBuffer, '{');

					/* 解析未来天气 */
					wt = parse_future_weather_data(ret);
					strcpy(mdt.high_temp, wt.high_temp);
					strcpy(mdt.low_temp, wt.low_temp);

					/* 未来天气写入缓冲区用于调试 */
					sprintf(buffer2, "%s, %s, %s, %s, %s, %s\r\n", wt.city_name, wt.date, wt.weather_day, wt.weather_night, wt.high_temp, wt.low_temp);
				}
				else
				{
					/* do nothing... */
				}
				/* 使用DMA发送至串口助手 */
				// HAL_UART_Transmit_DMA(&huart3, (uint8_t *)buffer2, strlen(buffer2));
				// ESP8266_Clear();
				break;
			}

			HAL_Delay(10);
		}

		HAL_Delay(500);

		ESP8266_Clear();


		/* 关闭TCP连接 */
		if (!ESP8266_SendCmd(close_tcp_cmd, "OK"))
		{
			uart_transmit_str(huart3, (uint8_t *)"Connect Closed Succeed!\r\n");
		}
		else
		{
			uart_transmit_str(huart3, (uint8_t *)"Connect Closed Failed\r\n");
		}
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"Send data to server error!\r\n");
	}
}

/**
 * @brief 从DMA缓冲区缓冲区读数据
 *
 * @param buffer
 * @param max_len
 * @return uint16_t
 */
unsigned short ESP8266_ReadData(unsigned char *buffer, unsigned short max_len)
{
	uint16_t bytes_to_read = 0;
	uint16_t write_snapshot = 0;

	write_snapshot = Write_index; // 获取当前写入位置快照
	// 计算有多少数据可以读
	if (write_snapshot >= PreWrite_index)
	{
		bytes_to_read = write_snapshot - PreWrite_index;
	}
	else
	{
		// 环形缓冲区绕回的情况
		bytes_to_read = RX_BUFFER_SIZE - PreWrite_index + write_snapshot;
	}

	// 限制读取长度，避免溢出
	if (bytes_to_read > max_len)
	{
		bytes_to_read = max_len;
	}

	// 如果有数据可读
	if (bytes_to_read > 0)
	{
		// 处理环形缓冲区绕回的情况
		if (PreWrite_index + bytes_to_read > RX_BUFFER_SIZE)
		{
			// 数据分成两段
			uint16_t len1 = RX_BUFFER_SIZE - PreWrite_index;
			memcpy(buffer, &rxBuffer[PreWrite_index], len1);
			memcpy(buffer + len1, rxBuffer, bytes_to_read - len1);
			PreWrite_index = bytes_to_read - len1;
		}
		else
		{
			// 数据在一段连续内存中
			memcpy(buffer, &rxBuffer[PreWrite_index], bytes_to_read);
			PreWrite_index += bytes_to_read;
		}
	}
	return bytes_to_read; // 返回实际读取的字节数
}


/**
 * @brief 向ESP8266发送GET请求
 *
 */
void ESP8266_GetResponse(const char **data, unsigned char data_num)
{
	// 开启常规发送模式
	if (!ESP8266_SendCmd(set_cipmode, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t *)"AT CIPMODE OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"AT CIPMODE ERROR\r\n");
		return;
	}

	// HAL_Delay(500);

	

	// 向服务器发送多条数据
	for (uint8_t i = 0; i < data_num; i++)
	{
		ESP8266_SendData((uint8_t*)data[i]);
		HAL_Delay(1000);
	}	
}

/**
 * @brief ESP8266通过SNTP获取时间用于更新本地RTC
 *
 *
 */
static void ESP8266_RTC_CalendarConfig(void)
{
	uint8_t times = 5;	//的次数

	/* 配置SNTP服务器 */
	if (!ESP8266_SendCmd(config_sntp, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t *)"Config SNTP OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"Config SNTP ERROR\r\n");
		return;
	}

	/* 等待sntp同步完成，防止获取到错误的时间 */
	HAL_Delay(1500);

	while((sntp_ok == 0) && (times > 0))
	{
		/* 尝试获取sntp时间 */
		ESP8266_SendCmd(get_sntp_time, "OK");
		times--;
		HAL_Delay(500);
	}
	if (times != 0)
	{
		/* 准备解析出数据，从ESP8266删除数据前会拷贝一份数据到debugbuffer */
		
		/**
		 *     AT+CIPSNTPTIME?\r\n
		 * 	   +CIPSNTPTIME:Wed Oct 22 13:38:10 2025\r\n
		 *     OK\r\n
		 *     \r\n
		 * 
		 */
		date_t dt;
		/* 解析日期成功 */
		char temp[64];
		sprintf(temp, "date_buffer: %d\r\n", date_buffer_cnt);
		uart_transmit_str(huart3, (uint8_t*)temp);
		uart_transmit_str(huart3, date_buffer);
		if(parse_date((char*)date_buffer, &dt) == 1)
		{
			/* 配置RTC的日期 */
			RTC_DateTypeDef sdatestructure;
			RTC_TimeTypeDef stimestructure;

			unsigned char m = getMonthNumber(dt.month);
			unsigned char w = getWeekNumber(dt.week);

			// char bff[12];
			// sprintf(bff, "%x, %x\r\n", m, w);
			// uart_transmit_str(huart3, (uint8_t*)bff);

			/*##-1- Configure the Date #################################################*/
			/* 设置日期 */
			sdatestructure.Year = intToBCD((dt.year % 100));	//年份只取后两位
			sdatestructure.Month = m;
			sdatestructure.Date = intToBCD(dt.day);
			sdatestructure.WeekDay = w;

			if (HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BCD) != HAL_OK)
			{
				/* Initialization Error */
				Error_Handler();
			}

			/*##-2- Configure the Time #################################################*/
			/* 设置时间 */
			stimestructure.Hours = intToBCD(dt.hour);
			stimestructure.Minutes = intToBCD(dt.minute);
			stimestructure.Seconds = intToBCD(dt.second);
			stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
			stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

			if (HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
			{
				/* Initialization Error */
				Error_Handler();
			}

			/*##-3- Writes a data in a RTC Backup data Register1 #######################*/
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x1234);

			char dates[50];
			sprintf(dates, "%02d:%02d:%02d  %s, %d %s, %d\r\n", dt.hour, dt.minute, dt.second, dt.week, dt.day, dt.month, dt.year);

			// uart_transmit_str(huart3, (uint8_t *)"Config RTC OK\r\n");
			uart_transmit_str(huart3, (uint8_t *)dates);
		}
		else
		{
			uart_transmit_str(huart3, (uint8_t *)"Parse date error!\r\n");
		}
		
		memset(date_buffer, 0, 64);	//清空解析日期的缓存区
		date_buffer_cnt = 0;
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"Config RTC ERROR\r\n");
	}
}

/**
 * @brief 初始化ESP8266
 *
 * @return uint8_t 1：成功，0：失败
 */
uint8_t ESP8266_Init(void)
{
	ESP8266_Clear();

	uint8_t initSuccess = 1; // 初始化成功标志
	uint8_t times = 5;		 // wifi重连次数

	/* 测试AT指令 */
	if (!ESP8266_SendCmd(test_at, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t *)"AT TEST OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"AT TEST ERROR\r\n");
		initSuccess = 0;
		goto INIT_END;
	}

	HAL_Delay(500);

	/* 设置STA模式 */
	if (!ESP8266_SendCmd(set_sta_mode, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t *)"AT CWMODE OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"AT CWMODE ERROR\r\n");
		initSuccess = 0;
		goto INIT_END;
	}

	HAL_Delay(500);

	/* 设置DHCP */
	if (!ESP8266_SendCmd(set_dhcp, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t *)"AT CWDHCP OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"AT CWDHCP ERROR\r\n");
		initSuccess = 0;
		goto INIT_END;
	}

	HAL_Delay(500);

	/* 连接WIFI，最多重试5次 */
	while (ESP8266_SendCmd(connect_wifi, "GOT IP") && (times > 0))
	{
		HAL_Delay(1000);
		times--;
	}

	if (times != 0)
	{
		uart_transmit_str(huart3, (uint8_t *)"WIFI Connect Success!\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"WIFI Connect Failed!\r\n");
		initSuccess = 0;
		goto INIT_END;
	}

	HAL_Delay(500);

	/* WIFI连接成功进行RTC校准 */
	ESP8266_RTC_CalendarConfig();

	/* WIFI连接成功获取未来天气 */
	char *data[2];

	char temp[512];
	sprintf(temp, get_daily_weather, key, location, days);
	data[0] = temp;

	/* WIFI连接成功获取实况天气 */
	char temp2[512];
	sprintf(temp2, get_now_weather, key, location);
	data[1] = temp2;

	/* 发送多条数据 */
	ESP8266_GetResponse((const char**)data, 2);

	uart_transmit_str(huart3, (uint8_t *)"ESP8266 Init OK!\r\n");

INIT_END:
	if (!initSuccess)
	{
		uart_transmit_str(huart3, (uint8_t *)"ESP8266 Init Failed!\r\n");
		// 这里可以添加错误处理代码，比如重启模块或者进入错误状态
	}
	return initSuccess;
}
