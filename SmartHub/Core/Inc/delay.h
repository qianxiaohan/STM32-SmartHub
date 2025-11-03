#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef btim4_handle;

void delay_init(void);           /* 初始化 */
void delay_ms(uint32_t ms);                /* 延时nms */
void delay_us(uint32_t us);                /* 延时nus */

#endif

