/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "usart.h"
#include "lvgl.h"
#include "norflash.h"
#include "touch.h"
#include "string.h"
#include "rtc.h"
#include "stdio.h"
#include "esp8266.h"
#include "weather.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* 采用cmsis标准来设置句柄 */

osThreadId_t LVGLTaskHandle;
const osThreadAttr_t LVGLTask_attributes = {
	.name = "LVGLTask",
	.stack_size = 1024 * 10,
	.priority = (osPriority_t)osPriorityHigh,
};

osThreadId_t KEYTaskHandle;
const osThreadAttr_t KEYTask_attributes = {
	.name = "KEYTask",
	.stack_size = 128,
	.priority = (osPriority_t)osPriorityNormal,
};

osThreadId_t WifiTaskHandle;
const osThreadAttr_t WifiTask_attributes = {
	.name = "WifiTask",
	.stack_size = 128,
	.priority = (osPriority_t)osPriorityBelowNormal,
};

osThreadId_t Send_Data_To_Server_TaskHandle;
const osThreadAttr_t Send_Data_To_Server_Task_attributes = {
	.name = "Send_Data_To_Server_Task",
	.stack_size = 1024 * 2,
	.priority = (osPriority_t)osPriorityBelowNormal1,
};

osThreadId_t Uart_dma_rx_TaskHandle;
const osThreadAttr_t Uart_dma_rx_Task_attributes = {
	.name = "Uart_dma_rx_Task",
	.stack_size = 1024 * 2,
	.priority = (osPriority_t)osPriorityBelowNormal2,
};



// 天气定时器
osTimerId_t timer_id1;

//超时定时器，用于判断数据是否接收完整
osTimerId_t timeout_timer;


/* 当flag 0号位被置位时，触发更新天气任务 */
uint32_t update_weather_flag = (0x01 << 0);

/* DMA接收任务信号量 */
osSemaphoreId_t dma_rx_sem;

/* 更新天气的信号 */
osSemaphoreId_t update_weather_sem;

/* 串口发送完成信号量 */
osSemaphoreId_t uart_tc_sem;

/* 传递接收数据的队列（存储数据长度） */
osMessageQueueId_t g_rx_data_queue;

/* ESP8266指令队列 */
osMessageQueueId_t cmd_queue;

// 用与判断接收到数据的响应
osMessageQueueId_t ack_queue;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void KEYTask(void *argument);
void LVGLTask(void *argument);
void WifiTask(void *argument);
void Cmd_Send_and_Response_Task(void *argument);
void Uart_dma_rx_task(void *argument);
void Send_Data_To_Server_Task(void *argument);

void WeatherTimer(void); 	//定时更新天气
void TimeoutTimer(void);	//超时定时器

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationTickHook(void);

/* USER CODE BEGIN 3 */
void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
	lv_tick_inc(1);
}
/* USER CODE END 3 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* 配置：最大令牌1（二进制），初始令牌0（创建后无令牌，任务需等待） */
	dma_rx_sem = osSemaphoreNew(1, 0, NULL);

	/* 配置：最大令牌1（二进制），初始令牌1（创建后即可使用UART发送） */
	// uart_tc_sem = osSemaphoreNew(1, 0, NULL);

	update_weather_sem = osSemaphoreNew(1, 0, NULL);

	/* 接收数据队列：存储uint16_t（数据长度），队列长度8，无属性 */
	// g_rx_data_queue = osMessageQueueNew(8, sizeof(uint16_t), NULL);
	// cmd_queue = osMessageQueueNew(5, sizeof(char*), NULL);
	ack_queue = osMessageQueueNew(5, sizeof(uint8_t), NULL);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
//   defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	// KEYTaskHandle = osThreadNew(KEYTask, NULL, &KEYTask_attributes);
	LVGLTaskHandle = osThreadNew(LVGLTask, NULL, &LVGLTask_attributes);
	// Cmd_Send_and_Response_TaskHandle = osThreadNew(Cmd_Send_and_Response_Task, NULL, &Cmd_Send_and_Response_Task_attributes);
	Uart_dma_rx_TaskHandle = osThreadNew(Uart_dma_rx_task, NULL, &Uart_dma_rx_Task_attributes);
	Send_Data_To_Server_TaskHandle = osThreadNew(Send_Data_To_Server_Task, NULL, &Send_Data_To_Server_Task_attributes);
	// Uart_tx_TaskHandle = osThreadNew(Uart_tx_task, NULL, &Uart_tx_Task_attributes);
	// WifiTaskHandle = osThreadNew(WifiTask, NULL, &LVGLTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	RTC_TimeTypeDef lastPrintTime = {0};
	uint8_t isFirstRun = 1;
	/* Infinite loop */
	for (;;)
	{
		// lcd_show_num(10, 130, 240, 24, 24);
		// lcd_show_string(10, 130, 240, 24, 24, "https://github.com/qianxiaohan", RED);
		// uart_transmit_str(huart3, (uint8_t*)"hello lyx\r\n");
		// osDelay(pdMS_TO_TICKS(1000));
		char aShowTime[50] = {0};
		RTC_TimeTypeDef currentTime;
		RTC_DateTypeDef currentDate;

		/* Get the RTC current Time */
		HAL_StatusTypeDef status1 = HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
		HAL_StatusTypeDef status2 = HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);

		/* 第一次运行 */
		if(isFirstRun)
		{
			lastPrintTime = currentTime;
			isFirstRun = 0;
			sprintf(aShowTime, "%2d:%2d:%2d\r\n", currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
				
			if (osSemaphoreAcquire(uart_tc_sem, 1000) == osOK)
			{
				uart_transmit_str(huart3, (uint8_t *)aShowTime);
				osSemaphoreRelease(uart_tc_sem);
			}
		}
		else
		{
			/* 上次打印到现在经过的分钟数 */
			uint16_t totalMinutesLast = lastPrintTime.Hours * 60 + lastPrintTime.Minutes;
			uint16_t totalMinutesCurrent = currentTime.Hours * 60 + currentTime.Minutes;
			uint16_t minuteDifference = totalMinutesCurrent - totalMinutesLast;

			if(minuteDifference >= 1)
			{
				lastPrintTime = currentTime;
				sprintf(aShowTime, "%2d:%2d:%2d\r\n", currentTime.Hours, currentTime.Minutes, currentTime.Seconds);

				if (osSemaphoreAcquire(uart_tc_sem, 1000) == osOK)
				{
					uart_transmit_str(huart3, (uint8_t *)aShowTime);
					osSemaphoreRelease(uart_tc_sem);
				}
				osSemaphoreRelease(update_weather_sem);
			}
		}

		osDelay(pdMS_TO_TICKS(5000));

	}
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void KEYTask(void *argument)
{
	/* Infinite loop */
	uint8_t pressed = 0; // 1表示按键按下，0表示按键松开
	uint8_t erase_value[ADJDATA_SIZE];
	memset(erase_value, 0xFF, sizeof(erase_value));
	for (;;)
	{
		if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET)
		{
			osDelay(pdMS_TO_TICKS(20));
			if ((HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET) && (pressed == 0))
			{
				/* 清除FLASH中的13个字节的校准数据 */
				norflash_write(erase_value, TP_SAVE_ADDR_BASE, ADJDATA_SIZE);
			}
		}
		else
		{
			pressed = 0; /* 按键松开 */
		}
		/* delay 100ms */
		osDelay(pdMS_TO_TICKS(100));
	}
}

void LVGLTask(void *argument)
{
	/* Infinite loop */
	for (;;)
	{
		lv_timer_handler();
		/* delay 5ms */
		osDelay(pdMS_TO_TICKS(5));
	}
}

void WifiTask(void *argument)
{
	/* 周期性地检查WIFI是否连接成功 */
	for (;;)
	{
	}
}


/**
 * @brief 发送与解析任务，通过串口向esp8266发送消息，并且解析来自esp8266的数据
 *
 * @param argument
 */
void Cmd_Send_and_Response_Task(void *argument)
{
	
}

void Send_Data_To_Server_Task(void *argument)
{
	char buffer2[128]; //用于调试程序
	char *raw_json;
	now_weather_t nwt;

	uint8_t ack_code;
	osStatus_t queue_status;
	uint8_t cnt;

	//尝试更新天气
	//1.开启透传模式
	uint8_t cmd1[] = "AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n"; // 连接TCP服务器指令

	uint8_t timer_out[] = "Ready update weather...\r\n";				 
	uint8_t connect_ok[] = "Connect Server...\r\n";

	uint8_t ready_send_data[] = "Ready send data to server...\r\n";
	
	uint8_t cmd3[] = "GET /v3/weather/now.json?key=SigzO8oaCNRAS96L8&location=chengdu&language=en&unit=c HTTP/1.1\r\nHost: api.seniverse.com\r\n\r\n";
	cnt = strlen((char*)cmd3);
	char cmd2[256];
	sprintf(cmd2, "AT+CIPSEND=%d\r\n", cnt);

	//创建天气定时器并启动，周期性更新天气
	WeatherTimer();

	//创建指令响应定时器，用于判断接收是否完成
	TimeoutTimer();

	for (;;)
	{
		//软件定时时间到了，准备更新天气
		if (osSemaphoreAcquire(update_weather_sem, osWaitForever) == osOK)
		{
			// 向串口助手发送数据
			HAL_UART_Transmit_DMA(&huart3, timer_out, sizeof(timer_out)-1);

			// 向ESP8266发送数据
			HAL_UART_Transmit_DMA(&huart1, cmd1, sizeof(cmd1)-1);

			// 启动超时定时器，超时时间2s
			osTimerStart(timeout_timer, 2000);

			// 如果没有获取到响应，最多等待5s
			queue_status = osMessageQueueGet(ack_queue, &ack_code, NULL, 5000);

			if(queue_status == osOK && ack_code==0x01)
			{
				//使用DMA发送至串口助手
				HAL_UART_Transmit_DMA(&huart3, connect_ok, strlen((char*)connect_ok));

				// 非透传模式下发送数据
				HAL_UART_Transmit_DMA(&huart1, (uint8_t*)cmd2, strlen(cmd2));
				// HAL_UART_Transmit_DMA(&huart1, wifis, sizeof(wifis)-1);
				
				// 启动超时定时器，超时时间2s
				osTimerStart(timeout_timer, 2000);

				// // 如果没有获取到响应，最多等待5s
				queue_status = osMessageQueueGet(ack_queue, &ack_code, NULL, 5000);

				if(queue_status == osOK && ack_code==0x02)
				{
					//使用DMA发送至串口助手，准备发送数据
					HAL_UART_Transmit_DMA(&huart3, ready_send_data, strlen((char*)ready_send_data));

					//使用DMA发送第三条指令
					HAL_UART_Transmit_DMA(&huart1, cmd3, sizeof(cmd3)-1);

					// 启动超时定时器，超时时间2s
					osTimerStart(timeout_timer, 2000);

					// 等待连接TCP服务器指令响应
					queue_status = osMessageQueueGet(ack_queue, &ack_code, NULL, 5000);
					if(queue_status == osOK && ack_code==0x03)
					{
						// HAL_UART_Transmit_DMA(&huart3, debugBuffer, strlen((char*)debugBuffer));

						//找到要解析的json字符串开头
						raw_json = strchr((char*)debugBuffer, '{');

						//解析实况天气
						nwt = parse_now_weather_data(raw_json);

						//解析完后复制到全局变量，用于LVGL显示
						strcpy(mdt.weather, nwt.weather);
						strcpy(mdt.temp, nwt.temp);

						sprintf(buffer2, "%s, %s, %s, %s, %s\r\n", nwt.city_name, nwt.date, nwt.temp, nwt.weather, nwt.weather_code);

						/* 解析后的数据使用DMA发送至串口助手 */
						HAL_UART_Transmit_DMA(&huart3, (uint8_t *)buffer2, strlen(buffer2));

						//清空缓冲区
						ESP8266_Clear();
					}
					else
					{
						uint8_t temp[] = "Get response error!";
						HAL_UART_Transmit_DMA(&huart3, temp, sizeof(temp)-1);
					}
				}
				else
				{
					uint8_t temp[] = "Send data to server error!";
					HAL_UART_Transmit_DMA(&huart3, temp, sizeof(temp)-1);
				}
			}
			else
			{
				uint8_t temp[] = "Connect server error!";
				HAL_UART_Transmit_DMA(&huart3, temp, sizeof(temp)-1);
			}
		}
	}
	
}


/**
 * @brief 处理以DMA方式接收的数据
 * 
 * @param argument 
 */
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
			// ESP8266_Clear();
		}
	}
}

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
    const TickType_t xTicks = pdMS_TO_TICKS( 10 * 60 * 1000UL );

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

/* USER CODE END Application */

