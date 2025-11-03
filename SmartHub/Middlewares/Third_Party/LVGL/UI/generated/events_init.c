/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
#include "gpio.h"
#include "rtc.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif




uint8_t aShowDate[50] = {0};

static void main_screen_img_wifi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);

        if(lv_obj_has_flag(guider_ui.main_screen_label_ip, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_clear_flag(guider_ui.main_screen_label_ip, LV_OBJ_FLAG_HIDDEN); // clear hidden flag
        } else {
            lv_obj_add_flag(guider_ui.main_screen_label_ip, LV_OBJ_FLAG_HIDDEN); //add hidden flag
        }
        break;
    }
    default:
        break;
    }
}

static void main_screen_btn_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.main_screen_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

// static void main_screen_label_time_event_handler(lv_event_t *e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     // static int count = 0;
    

//     switch (code) {
//     case LV_EVENT_REFRESH:
//     {
//         // RTC_DateTypeDef sdatestructureget;
//         RTC_TimeTypeDef stimestructureget;

//         /* Get the RTC current Time */
//         HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
//         /* Get the RTC current Date */
//         // HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
//         /* Display time Format : hh:mm:ss */
//         sprintf((char *)aShowTime, "%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes);
//         lv_label_set_text(guider_ui.main_screen_label_time, (char *)aShowTime); //获取本地RTC
//         // sprintf((char *)aShowDate, "%2d-%2d-%2d\r\n", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);

//         // if(count > 60)
//         // count = 0;
//         // sprintf((char *)aShowTime, "16:%2d", count++);
//         // lv_label_set_text(guider_ui.main_screen_label_time, (char *)aShowTime);
//         break;
//     }
//     default:
//         break;
//     }
// }

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
    lv_label_set_text(time_label, aShowTime); //获取本地RTC


    // if(count > 60)
    //     count = 0;
    // sprintf(aShowTime, "16:%2d", count++);
    // lv_label_set_text(time_label, aShowTime); //获取本地RTC
}

void events_init_main_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->main_screen_img_wifi, main_screen_img_wifi_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_screen_btn_home, main_screen_btn_home_event_handler, LV_EVENT_ALL, ui);

    // /* 为时间标签创建事件回调 */
    // lv_obj_add_event_cb(ui->main_screen_label_time, main_screen_label_time_event_handler, LV_EVENT_ALL, ui);

    // /* 每秒读取RTC的值 */
    timer = lv_timer_create(update_time, 1000, ui->main_screen_label_time);
}

static void screen_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCROLL_END:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main_screen, guider_ui.main_screen_del, &guider_ui.screen_home_del, setup_scr_main_screen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_home_sw_LED2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        if(status == true)
        {
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_home_sw_LED1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        if(status == true)
        {
            HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_home_img_return_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main_screen, guider_ui.main_screen_del, &guider_ui.screen_home_del, setup_scr_main_screen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_home (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_home, screen_home_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_sw_LED2, screen_home_sw_LED2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_sw_LED1, screen_home_sw_LED1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_img_return, screen_home_img_return_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
