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

#include "weather.h" //解析天气和日期

#define SNTPTIME_LEN	43	//返回的SNTP时间的数据长度是固定的

/* ESP8266指令 */
const char *test_at = "AT\r\n";															 // 测试AT指令
const char *set_sta_mode = "AT+CWMODE=1\r\n";											 // 设置STA模式
const char *set_dhcp = "AT+CWDHCP=1,1\r\n";												 // 设置DHCP
const char *connect_wifi = "AT+CWJAP=\"wifi名称\",\"wifi密码\"\r\n";				 // 连接wifi指令
const char *connect_server = "AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n";		 // 连接TCP服务器指令
const char *set_cipmode = "AT+CIPMODE=1\r\n";											 // 透传模式，发送数据时不用指定数据的大小
const char *send_data_cmd = "AT+CIPSEND\r\n";											 // 透传模式下发送数据
const char *exit_send_cmd = "+++";														 // 退出透传模式
const char *close_tcp_cmd = "AT+CIPCLOSE\r\n";											 // 关闭TCP连接
const char *config_sntp = "AT+CIPSNTPCFG=1,8,\"ntp.aliyun.com\",\"ntp.sjtu.edu.cn\"\r\n"; // 配置SNTP服务器
const char *get_sntp_time = "AT+CIPSNTPTIME?\r\n";										 // 获取STNP时间

/* API可配置的参数 */
const char *key = "*******"; // API私钥，需要自行申请
const char *location = "chengdu";	   // 地区
const char *days = "1";				   // 查询1~3天的天气

/* 用于解析日期的缓冲区 */
uint8_t date_buffer[64];
uint8_t date_buffer_cnt;

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
	uint16_t timeOut = 500; // 超时时间2S

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
 * @brief 透传模式下发送数据
 *
 *
 * @param data 要发送的数据
 */
void ESP8266_SendData(unsigned char **data, unsigned char data_num)
{

	char buffer2[128];
	char *ret;
	uint16_t timeOut = 500; // 等待5S
	future_weather_t wt;
	now_weather_t nwt;
	ESP8266_Clear();						  // 清空接收缓存

	unsigned char now_flag = 0;		//0:是实况天气，1：是未来天气

	if (!ESP8266_SendCmd(send_data_cmd, ">")) // 收到‘>’时可以发送数据
	{
		for (uint8_t i = 0; i < data_num; i++)
		{
			/* 向ESP8266发送数据 */
			uart_transmit_str(huart1, data[i]);

			/* 判断发送实况/未来天气请求 */
			if(strstr((char*)data[i], "now"))
			{
				now_flag = 0;
			}
			else if(strstr((char*)data[i], "daily"))
			{
				now_flag = 1;
			}
			else
			{
				now_flag = 2;
			}

			/* 等待ESP8266发来的数据 */
			while (timeOut--)
			{
				if (rx_data_ready == 1) // 接收完成
				{
					rx_data_ready = 0; // 标志位置0，准备下次DMA接收

					if(now_flag == 0)
					{
						/* GET请求返回来的响应包括请求头，这里是为了找到json格式的数据以便解析 */
						ret = strchr((char *)debugBuffer, '{');

						/* 解析实况天气 */
						nwt = parse_now_weather_data(ret);

						/* 解析完后复制到全局变量 */
						strcpy(mdt.weather, nwt.weather);
						strcpy(mdt.temp, nwt.temp);
						// strcpy(mdt.);

						/* DMA的速度过快，直接将缓冲区的数据发送可能会出问题？将解析后的数据写入缓冲区 */
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

						/* DMA的速度过快，直接将缓冲区的数据发送可能会出问题？将解析后的数据写入缓冲区 */
						sprintf(buffer2, "%s, %s, %s, %s, %s, %s\r\n", wt.city_name, wt.date, wt.weather_day, wt.weather_night, wt.high_temp, wt.low_temp);
					}
					else
					{
						/* do nothing... */
					}
					/* 使用DMA发送至串口助手 */
					HAL_UART_Transmit_DMA(&huart3, (uint8_t *)buffer2, strlen(buffer2));
					break;
				}

				HAL_Delay(10);
			}

			/* 等待500ms发送下一条数据 */
			HAL_Delay(500);
		}
		
		/* 关闭透传 */
		uart_transmit_str(huart1, (uint8_t *)exit_send_cmd);

		/* 发送透传指令后需等待至少1s */
		HAL_Delay(2000);

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
	// 开启透传模式
	if (!ESP8266_SendCmd(set_cipmode, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t *)"AT CIPMODE OK\r\n");
	}
	else
	{
		uart_transmit_str(huart3, (uint8_t *)"AT CIPMODE ERROR\r\n");
		return;
	}

	HAL_Delay(500);

	unsigned char times = 5;

	while(ESP8266_SendCmd(connect_server, "OK") && (times > 0))
	{
		times--;
		HAL_Delay(1000);
	}
	if(times != 0)
	{
		uart_transmit_str(huart3, (uint8_t *)"Connect Server Successed!\r\n");
	}
	if (!ESP8266_SendCmd(connect_server, "OK"))
	{
		uart_transmit_str(huart3, (uint8_t *)"Connect Server Failed!\r\n");
		return;
	}

	HAL_Delay(500);

	// 向服务器发送多条数据
	ESP8266_SendData((uint8_t**)data, data_num);
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

	HAL_Delay(500);

	/* 获取SNTP时间 */
	while(ESP8266_SendCmd(get_sntp_time, "OK") && (times > 0))
	{
		times--;
		HAL_Delay(1000);
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

	char get_future_header[512];
	sprintf(get_future_header, "GET /v3/weather/daily.json?key=%s&location=%s&language=en&unit=c&start=0&days=%s HTTP/1.1\r\nHost: api.seniverse.com\r\n\r\n\r\n", key, location, days);
	data[0] = get_future_header;

	// ESP8266_GetResponse(get_future_header);
	/* WIFI连接成功获取实况天气 */
	char get_now_header[512];
	sprintf(get_now_header, "GET /v3/weather/now.json?key=%s&location=%s&language=en&unit=c HTTP/1.1\r\nHost: api.seniverse.com\r\n\r\n\r\n", key, location);
	data[1] = get_now_header;

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
