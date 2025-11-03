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
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	KEYTaskHandle = osThreadNew(KEYTask, NULL, &KEYTask_attributes);
	LVGLTaskHandle = osThreadNew(LVGLTask, NULL, &LVGLTask_attributes);
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
	/* Infinite loop */
	for (;;)
	{
		// lcd_show_num(10, 130, 240, 24, 24);
		// lcd_show_string(10, 130, 240, 24, 24, "https://github.com/qianxiaohan", RED);
		// uart_transmit_str(huart3, (uint8_t*)"hello lyx\r\n");
		// osDelay(pdMS_TO_TICKS(1000));

		RTC_TimeTypeDef stimestructureget;
		RTC_DateTypeDef sdatestructureget;

		/* Get the RTC current Time */
		HAL_StatusTypeDef status1 = HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
		HAL_StatusTypeDef status2 = HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
		char aShowTime[50] = {0};

		sprintf(aShowTime, "%2d:%2d:%2d\r\n", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
		
		uart_transmit_str(huart3, (uint8_t*)aShowTime);
		osDelay(pdMS_TO_TICKS(1000));

    /* Get the RTC current Date */
    // HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
	}
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void KEYTask(void *argument)
{
	/* Infinite loop */
	uint8_t pressed = 0;	//1表示按键按下，0表示按键松开
	uint8_t erase_value[ADJDATA_SIZE];
	memset(erase_value, 0xFF, sizeof(erase_value));
	for (;;)
	{
		if(HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET)
		{
			osDelay(pdMS_TO_TICKS(20));
			if((HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET) && (pressed == 0))
			{
				/* 清除FLASH中的13个字节的校准数据 */
				norflash_write(erase_value, TP_SAVE_ADDR_BASE, ADJDATA_SIZE);
			}
		}else{
			pressed = 0;	/* 按键松开 */
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
/* USER CODE END Application */

