/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

#define UART_TIMEOUT 1000
#define RX_BUFFER_SIZE 1024
extern uint8_t rxBuffer[RX_BUFFER_SIZE];  //DMA所使用的缓冲区
extern uint8_t debugBuffer[RX_BUFFER_SIZE * 2];  //总缓冲区，所有DMA缓冲区的数据
extern uint8_t Read_Buffer[RX_BUFFER_SIZE]; //读指令需要的缓冲区
extern uint16_t buffer_cnt;     
extern uint16_t buffer_cntPre;  
extern uint8_t rxData;
extern uint16_t rx_data_len;  //接收到的实际长度
extern uint16_t debug_data_len;  //记录总缓冲区的长度
extern uint8_t rx_data_ready; //接收完成标志 

extern uint16_t Write_index; //准备写入环形缓冲区的索引位置
extern uint16_t PreWrite_index;  //上一次写入环形缓冲区的索引位置

extern uint8_t g_rx_token;  //等于1时，允许释放信号量


/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
HAL_StatusTypeDef uart_receive(UART_HandleTypeDef huart, uint8_t *data, uint16_t length);
HAL_StatusTypeDef uart_transmit_str(UART_HandleTypeDef huart, uint8_t *data);
HAL_StatusTypeDef uart_transmit_ch(UART_HandleTypeDef huart, uint8_t data);
void MX_USART1_UART_Init_DMA(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

