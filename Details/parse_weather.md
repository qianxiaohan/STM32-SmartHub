## :signal_strength:解析来自服务器的天气和日期

### 天气解析

考虑到ESP8266返回的JSON数据过大，如果使用串口会丢失数据，因此使用**DMA2来接收ESP8266发来的数据**，并通过**DMA1将解析后的数据发送**至串口助手中查看结果。

通过心知天气API获取到未来天气的JSON数据：

```json
{
  "results": [
    {
      "location": {
        "id": "WM6N2PM3WY2K",
        "name": "Chengdu",
        "country": "CN",
        "path": "Chengdu,Chengdu,Sichuan,China",
        "timezone": "Asia/Shanghai",
        "timezone_offset": "+08:00"
      },
      "daily": [
        {
          "date": "2025-10-21",
          "text_day": "Overcast",
          "code_day": "9",
          "text_night": "Overcast",
          "code_night": "9",
          "high": "17",
          "low": "12",
          "rainfall": "0.00",
          "precip": "0.00",
          "wind_direction": "NE",
          "wind_direction_degree": "45",
          "wind_speed": "8.4",
          "wind_scale": "2",
          "humidity": "88"
        }
      ],
      "last_update": "2025-10-21T08:00:00+08:00"
    }
  ]
}
```

可以cJSON来解析json数据，但是需要修改STM32启动文件的堆栈大小，本例程中使用字符串匹配的方式来进行了简单的解析，分别对应：地区，日期，白天天气，晚上天气，最高温度，最低温度

```shell
Chengdu, 2025-10-21, Overcast, Overcast, 17, 12
```

同样地，获取实况天气的json数据：

```json
{
  "results": [
    {
      "location": {
        "id": "WM6N2PM3WY2K",
        "name": "Chengdu",
        "country": "CN",
        "path": "Chengdu,Chengdu,Sichuan,China",
        "timezone": "Asia/Shanghai",
        "timezone_offset": "+08:00"
      },
      "now": {
        "text": "Light rain",
        "code": "13",
        "temperature": "15"
      },
      "last_update": "2025-11-03T12:47:42+08:00"
    }
  ]
}
```

解析后如下，分别对应：地区，日期，当前温度，天气，天气代码

```shell
Chengdu, 2025-11-03, 15, Light rain, 13
```

### 日期解析

使用AT指令`AT+CIPSNTPTIME?`获取SNTP时间，返回：

```
+CIPSNTPTIME:Tue Oct 19 17:47:56 2021
OK
```

ESP8266通过串口发送给STM32实际上接收到的是, `"\r\n"`是回车+换行符

```
+CIPSNTPTIME:Tue Oct 19 17:47:56 2021\r\n
OK\r\n
```

HAL库生成的Keil工程，使用的是-o2优化等级，经过测试使用`sscanf`、`atoi`库函数，在普通的C程序中可以解析出上面的时间。但是在STM32中使用有些程序不会执行，可能是优化等级的问题？使用以下代码解析时间，strToNum函数是将字符串的数转为无符号数："1234" --> 1234。

```c
unsigned char parse_date(const char *date_str, date_t *dt)
{

    char *pos = strstr(date_str, "TIME:") + strlen("TIME:");
    char *end = strstr(date_str, "OK");

    volatile unsigned short len = end - pos - strlen("\r\n");
    char buffer[64];
    strncpy(buffer, pos, len);
    buffer[len] = '\0';

    char* fields[7];  // 存储6个字段的指针
    unsigned char fieldCount = 0;
    char* token = strtok(buffer, " ");  // 用空格分割

    while (token != NULL && fieldCount < 7) {
        fields[fieldCount++] = token;
        token = strtok(NULL, " :");
    }
    if (fieldCount == 7) 
    {
        // 提取星期（第0个字段："Wed"）
        strncpy(dt->week, fields[0], 3);
        dt->week[3] = '\0';

        // 提取月份（第1个字段："Oct"）
        strncpy(dt->month, fields[1], 3);
        dt->month[3] = '\0';
        
        //在-o2优化等级下使用atoi函数会出问题，疑似被优化?，重写了一个函数来实现str --> uint8_t
        //提取日期和时间
        dt->day = strToNum(fields[2]);
        dt->hour = strToNum(fields[3]);
        dt->minute = strToNum(fields[4]);
        dt->second = strToNum(fields[5]);
        dt->year = strToNum(fields[6]);

        return 1;
    }
    return 0;
}
```



## :package:定时更新天气

代码见[freertos.c](./SmartHub/Core/Src/freertos.c)。获取天气流程：

```shell
//连接服务器指令
AT+CIPSTART="TCP","api.seniverse.com",80

//非透传模式下发送数据
AT+CIPSEND=120

//等到接收到'>'，准备发送GET请求，注意换行+回车
GET /v3/weather/now.json?key=你自己的秘钥&location=地区&language=en&unit=c HTTP/1.1
Host: api.seniverse.com

```

在C语言中GET请求是这样的：

```c
uint8_t get[] = "GET /v3/weather/now.json?key=%s&location=%s&language=en&unit=c HTTP/1.1\r\nHost: api.seniverse.com\r\n\r\n"
```

因为向服务器发送获取天气的GET请求，返回的数据有几百个字节，如下所示。

```
+IPD,893:HTTP/1.1 200 OK
Date: Tue, 10 Mar 2026 09:10:45 GMT
Content-Type: application/json; charset=utf-8
Content-Length: 479
Connection: keep-alive
access-control-allow-origin: *
etag: W/"1df-nRiu8ZVjFyi1ab4hNgnbWDMSyI8"
x-powered-by: Express
x-ratelimit-limit: 20;w=60
x-ratelimit-remaining: 17
x-ratelimit-reset: 8.173
x-request-id: GJtv1psGDH9tWJb5eFdB
x-tenant-id: aa49e14d-a651-40d7-ba45-a4f5f7570831

{"results":[{"location":{"id":"WM6N2PM3WY2K","name":"Chengdu","country":"CN","path":"Chengdu,Chengdu,Sichuan,China","timezone":"Asia/Shanghai","timezone_offset":"+08:00"},"daily":[{"date":"2026-03-10","text_day":"Cloudy","code_day":"4","text_night":"Clear","code_night":"1","high":"23","low":"10","rainfall":"0.00","precip":"0.09","wind_direction":"CLM","wind_direction_degree":"","wind_speed":"8.4","wind_scale":"2","humidity":"96"}],"last_update":"2026-03-10T08:00:00+08:00"}]}
```

为了不影响整个系统的实时性，使用==空闲中断+DMA==来接收ESP8266发来的数据，但是呢，对于ESP8266来说，发送这么大的数据，会分多次发送，会触发多次空闲中断，如何才能接收到完整的数据？

- 在裸机中，可以延时一段时间来等待数据接收完成
- 在freertos中，可以采用同样的思路，延时一段时间(使用freertos软件定时器实现)，当在发送任务中，向ESP8266发送数据时，就可以启动这个定时器(我将时间设置为2s)，等到定时器定时时间到了，在定时器回调函数里面，释放接收完成的信号量；而在接收任务中，获取该信号量后，就对接收到的数据进行处理，来决定是否发送下一条指令。

创建两个定时器和两个定时器回调函数：

```c
/**
 * @brief 串口超时定时器回调函数
 * 
 * @param arg 
 */
void Timeout_timer_cb(void *arg)
{
	// 超时时间到了，可以任务数据接收完成了
	// 释放信号量，通知指令处理任务
	osSemaphoreRelease(dma_rx_sem);
}


/**
 * @brief 软件定时器回调函数
 * 
 * @param arg 
 */
void Timer1_Callback(void *arg)
{
	// 定时时间到，释放信号量
	osSemaphoreRelease(update_weather_sem);
}

/**
 * @brief 创建并启动获取天气的定时器
 * 
 */
void WeatherTimer(void)
{
	// 使用UL后缀确保为无符号长整型，防止计算溢出，定时时间30分钟
    const TickType_t xTicks = pdMS_TO_TICKS( 30 * 60 * 1000UL );

	//返回定时器状态
	osStatus_t status; 

	//创建freertos软件定时器，参数：回调函数、周期性定时器、回调函数参数、定时器属性
	timer_id1 = osTimerNew(Timer1_Callback, osTimerPeriodic, NULL, NULL);

	if(timer_id1 != NULL)
	{
		//定时器创建成功

		//启动定时器
		status = osTimerStart(timer_id1, xTicks);

		if(status != osOK)
		{
			//启动定时器失败
			uint8_t temp[] = "timer error";
			HAL_UART_Transmit_DMA(&huart3, temp, sizeof(temp)-1);
		}
	}
}

void TimeoutTimer(void)
{

	//创建freertos软件定时器，参数：回调函数、周期性定时器、回调函数参数、定时器属性
	timeout_timer = osTimerNew(Timeout_timer_cb, osTimerOnce, NULL, NULL);
}
```

在发送任务中：

```c
void Send_Data_To_Server_Task(void *argument)
{
    ...
    for (;;)
    {
        //软件定时时间到了，准备更新天气
        if (osSemaphoreAcquire(update_weather_sem, osWaitForever) == osOK)
        {
            // 向ESP8266发送数据
            HAL_UART_Transmit_DMA(&huart1, cmd1, sizeof(cmd1)-1);

            // 启动超时定时器，超时时间2s
            osTimerStart(timeout_timer, 2000);
            queue_status = osMessageQueueGet(ack_queue, &ack_code, NULL, 5000);

            // 准备发送下一条指令
            if(queue_status == osOK && ack_code==0x01)
            {
                ....
            }
        }
    }
    ...
}

```

在接收任务中：

```c
void Uart_dma_rx_task(void *argument)
{
	osStatus_t queue_status;
	uint8_t ack_code = 0x00;
	for(;;)
	{
		// 等到超时定时器回调函数，释放信号量，认为数据接收完成了
		if(osSemaphoreAcquire(dma_rx_sem, osWaitForever) == osOK)
		{
			//清空上次的状态码
			ack_code = 0x00;

			// 判断响应
			if(strstr((char*)debugBuffer, "CONNECT") != NULL)
			{
				ack_code = 0x01;
			}
			else if(strstr((char*)debugBuffer, ">") != NULL)
			{
				ack_code = 0x02;
			}
			else if(strstr((char *)debugBuffer, "now") != NULL)
			{
				ack_code = 0x03;
			}

			// HAL_UART_Transmit_DMA(&huart3, debugBuffer, debug_data_len);
			// 清空缓冲区
			if(ack_code != 0x03)
				ESP8266_Clear();

			queue_status = osMessageQueuePut(ack_queue, &ack_code, 0, 0);
			if(queue_status != osOK)
			{
				//队列满，需要处理
			}
		}
	}
}
```

### 测试结果

为了方便测试将定时时间设置为了30s，因为不需要精确时间，使用软件定时器即可，在串口助手中打印出：

```
[2026-03-10 17:09:51.796]
RX：Ready update weather...

[2026-03-10 17:09:53.795]
RX：Connect Server...

[2026-03-10 17:09:55.797]
RX：Ready send data to server...

[2026-03-10 17:09:57.783]
RX：Chengdu, 2026-03-10, 19, Sunny, 0

[2026-03-10 17:10:21.700]
RX：Ready update weather...

[2026-03-10 17:10:23.709]
RX：Connect Server...

[2026-03-10 17:10:25.691]
RX：Ready send data to server...

[2026-03-10 17:10:27.684]
RX：Chengdu, 2026-03-10, 19, Sunny, 0
```



在定时30分钟情况下，串口助手接收到的数据：

```
[2026-03-10 17:10:55.703]
RX：ESP8266 Init OK!
LCD ID:7789

[2026-03-10 17:40:49.457]
RX：Ready update weather...

[2026-03-10 17:40:51.456]
RX：Connect Server...

[2026-03-10 17:40:53.455]
RX：Ready send data to server...

[2026-03-10 17:40:55.451]
RX：Chengdu, 2026-03-10, 18, Sunny, 0
```

