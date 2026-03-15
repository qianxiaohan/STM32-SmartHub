## :desktop_computer:LCD显示

使用LVGL当做LCD的界面，LVGL配合GUI Guider设计界面。需要注意的点，LVGL需要在定时器中中断调用`lv_tick_inc(1);`，以此来告诉LVGL时间经过了1个tick(一般1tick=1ms)。在[freertos.c](../SmartHub/Core/Src/freertos.c)下，需要在钩子函数`vApplicationTickHook()`中调用该函数：

```c
void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
	lv_tick_inc(1);
}
```

定义LVGL任务，大约每5ms处理LVGL任务

```c
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
```

### 刷新界面

代码见[events_init.c](../SmartHub/Middlewares/Third_Party/LVGL/UI/generated/events_init.c)。时间需要每秒获取并刷新界面，可以使用定时器。LVGL内部是有定时器的，那么刷新LVGL界面就应该交给内部定时器，而不是由freertos来刷新。创建1秒的LVGL定时器，并在定时器中调用定时器回调函数：

```c
void events_init_main_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->main_screen_img_wifi, main_screen_img_wifi_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_screen_btn_home, main_screen_btn_home_event_handler, LV_EVENT_ALL, ui);

    // /* 每秒读取RTC的值 */
    timer = lv_timer_create(update_time, 1000, ui->main_screen_label_time);
}

static void update_time(lv_timer_t *timer){

    lv_obj_t* time_label = (lv_obj_t*)timer->user_data;

    if(time_label == NULL) return;

    RTC_TimeTypeDef stimestructureget;
    RTC_DateTypeDef sdatestructureget;

    /* Get the RTC current Time */
    HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);

    /* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);

    char aShowTime[50] = {0};

    /* Get the RTC current Date */
    // HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
    /* Display time Format : hh:mm:ss */
    sprintf(aShowTime, "%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes);
    lv_label_set_text(time_label, aShowTime); //刷新界面
}
```

同理，要刷新温度和天气标签，再创建一个定时器，并将温度标签和天气标签(使用结构体将两个组合在一起)作为定时器参数。