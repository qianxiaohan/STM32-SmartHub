/**
 * 使用定时器实现微秒级延时
 ********************************************************
 */

#include "delay.h"
#include "stm32f4xx_hal.h"

TIM_HandleTypeDef btim4_handle;

void delay_init()
{
	/* 开启TIM4时钟 */
	__HAL_RCC_TIM4_CLK_ENABLE();

	btim4_handle.Instance = TIM4;

	/* 设置定时器计数周期为1MHz(即1us) */
	btim4_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	btim4_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	btim4_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	btim4_handle.Init.Period = 0xffff - 1;
	btim4_handle.Init.Prescaler = 84 - 1;
	btim4_handle.Init.RepetitionCounter = 0;

	HAL_TIM_Base_Init(&btim4_handle);
}

void delay_us(uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&btim4_handle, 0); // 清零计数器
	HAL_TIM_Base_Start(&btim4_handle);
	while (__HAL_TIM_GET_COUNTER(&btim4_handle) < us); // 等待计数值达到目标
	HAL_TIM_Base_Stop(&btim4_handle);
}

void delay_ms(uint32_t ms)
{
	while(ms--)
		delay_us(1000);
}

