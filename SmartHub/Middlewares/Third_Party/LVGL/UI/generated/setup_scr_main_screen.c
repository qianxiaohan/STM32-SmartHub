/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"
#include "rtc.h"
#include "weather.h"


/* To update date label */
const char *weekdays_full[] = {
    "Monday",      
    "Tuesday",   
    "Wednesday", 
    "Thursday",  
    "Friday",    
    "Saturday"   
    "Sunday",   
};


const char *months_full[] = {
    "January",    
    "February",   
    "March",      
    "April",      
    "May",        
    "June",       
    "July",       
    "August",     
    "September",  
    "October",    
    "November",   
    "December"   
};

void setup_scr_main_screen(lv_ui *ui)
{
    //Write codes main_screen
    ui->main_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->main_screen, 320, 240);
    lv_obj_set_scrollbar_mode(ui->main_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for main_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->main_screen, 43, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_screen, lv_color_hex(0x370008), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_cont_main
    ui->main_screen_cont_main = lv_obj_create(ui->main_screen);
    lv_obj_set_pos(ui->main_screen_cont_main, 0, 0);
    lv_obj_set_size(ui->main_screen_cont_main, 320, 240);
    lv_obj_set_scrollbar_mode(ui->main_screen_cont_main, LV_SCROLLBAR_MODE_OFF);

    //Write style for main_screen_cont_main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_screen_cont_main, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->main_screen_cont_main, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->main_screen_cont_main, lv_color_hex(0xcecece), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->main_screen_cont_main, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_screen_cont_main, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_screen_cont_main, lv_color_hex(0xedd0d0), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_screen_cont_main, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_screen_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_screen_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_screen_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_screen_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_label_outdoor
    ui->main_screen_label_outdoor = lv_label_create(ui->main_screen_cont_main);
    lv_label_set_text(ui->main_screen_label_outdoor, "Outdoor");
    lv_label_set_long_mode(ui->main_screen_label_outdoor, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->main_screen_label_outdoor, 232, 95);
    lv_obj_set_size(ui->main_screen_label_outdoor, 82, 16);

    //Write style for main_screen_label_outdoor, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_screen_label_outdoor, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_screen_label_outdoor, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_screen_label_outdoor, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_screen_label_outdoor, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_label_outdoor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_label_time
    ui->main_screen_label_time = lv_label_create(ui->main_screen_cont_main);
    // lv_label_set_text(ui->main_screen_label_time, "16:07");

    /* -------------use rtc to set time label-----------------  */

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
    lv_label_set_text(ui->main_screen_label_time, aShowTime); //获取本地RTC

    /*----------------------------------------------------------*/

    lv_label_set_long_mode(ui->main_screen_label_time, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->main_screen_label_time, 3, 47);
    lv_obj_set_size(ui->main_screen_label_time, 122, 43);

    //Write style for main_screen_label_time, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_screen_label_time, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_screen_label_time, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_screen_label_time, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_screen_label_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_label_date
    ui->main_screen_label_date = lv_label_create(ui->main_screen_cont_main);
    // lv_label_set_text(ui->main_screen_label_date, "Wednesday, 1 August");

    /*----------Set Date Label-------------*/

    char aShowDate[128] = {0};
    unsigned char weekday_index = sdatestructureget.WeekDay - 1;
    unsigned char months_index = sdatestructureget.Month - 1;
    sprintf(aShowDate, "%s, %d %s, %d", weekdays_full[weekday_index], sdatestructureget.Date, months_full[months_index], 2000 + sdatestructureget.Year);

    lv_label_set_text(ui->main_screen_label_date, aShowDate);
    /*---------------------------------*/

    lv_label_set_long_mode(ui->main_screen_label_date, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->main_screen_label_date, 17, 32);
    lv_obj_set_size(ui->main_screen_label_date, 230, 14);

    //Write style for main_screen_label_date, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_screen_label_date, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_screen_label_date, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_screen_label_date, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_screen_label_date, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_label_temp
    ui->main_screen_label_temp = lv_label_create(ui->main_screen_cont_main);
    // lv_label_set_text(ui->main_screen_label_temp, "23°");

    /*---------Get temp to update temp label----------*/
    char buff[32];
    sprintf(buff, "%s°", mdt.temp);
    lv_label_set_text(ui->main_screen_label_temp, buff);
    /*-----------------------------------------------*/

    lv_label_set_long_mode(ui->main_screen_label_temp, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->main_screen_label_temp, 240, 50);
    lv_obj_set_size(ui->main_screen_label_temp, 74, 46);

    //Write style for main_screen_label_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_screen_label_temp, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_screen_label_temp, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_screen_label_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_screen_label_temp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_label_weather
    ui->main_screen_label_weather = lv_label_create(ui->main_screen_cont_main);
    // lv_label_set_text(ui->main_screen_label_weather, "Cloudy");

    /*---------Get weather to update label----------*/
    // char buff[32];
    sprintf(buff, "%s", mdt.weather);
    lv_label_set_text(ui->main_screen_label_weather, buff);
    /*-----------------------------------------------*/

    lv_label_set_long_mode(ui->main_screen_label_weather, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->main_screen_label_weather, 148, 93);
    lv_obj_set_size(ui->main_screen_label_weather, 71, 16);

    //Write style for main_screen_label_weather, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_screen_label_weather, lv_color_hex(0xb80707), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_screen_label_weather, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_screen_label_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_screen_label_weather, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_label_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_label_hltemp
    ui->main_screen_label_hltemp = lv_label_create(ui->main_screen_cont_main);
    // lv_label_set_text(ui->main_screen_label_hltemp, "H: 19°  L: 10°");

    /*---------Get low temp and high temp to update label----------------------------*/
    sprintf(buff, "H: %s°  L: %s°", mdt.high_temp, mdt.low_temp);
    lv_label_set_text(ui->main_screen_label_hltemp, buff);
    /*----------------------------------------------------------------------------*/

    lv_label_set_long_mode(ui->main_screen_label_hltemp, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->main_screen_label_hltemp, 158, 111);
    lv_obj_set_size(ui->main_screen_label_hltemp, 80, 18);

    //Write style for main_screen_label_hltemp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_screen_label_hltemp, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_screen_label_hltemp, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_screen_label_hltemp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_screen_label_hltemp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_label_hltemp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_img_weather
    ui->main_screen_img_weather = lv_img_create(ui->main_screen_cont_main);
    lv_obj_add_flag(ui->main_screen_img_weather, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->main_screen_img_weather, &_weather_alpha_32x32);
    lv_img_set_pivot(ui->main_screen_img_weather, 50,50);
    lv_img_set_angle(ui->main_screen_img_weather, 0);
    lv_obj_set_pos(ui->main_screen_img_weather, 162, 50);
    lv_obj_set_size(ui->main_screen_img_weather, 32, 32);

    //Write style for main_screen_img_weather, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->main_screen_img_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->main_screen_img_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_img_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->main_screen_img_weather, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_img_wifi
    ui->main_screen_img_wifi = lv_img_create(ui->main_screen_cont_main);
    lv_obj_add_flag(ui->main_screen_img_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->main_screen_img_wifi, &_wifi_alpha_20x20);
    lv_img_set_pivot(ui->main_screen_img_wifi, 50,50);
    lv_img_set_angle(ui->main_screen_img_wifi, 0);
    lv_obj_set_pos(ui->main_screen_img_wifi, 256, 4);
    lv_obj_set_size(ui->main_screen_img_wifi, 20, 20);

    //Write style for main_screen_img_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->main_screen_img_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->main_screen_img_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_img_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->main_screen_img_wifi, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_img_setting
    ui->main_screen_img_setting = lv_img_create(ui->main_screen_cont_main);
    lv_obj_add_flag(ui->main_screen_img_setting, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->main_screen_img_setting, &_settings_alpha_20x20);
    lv_img_set_pivot(ui->main_screen_img_setting, 50,50);
    lv_img_set_angle(ui->main_screen_img_setting, 0);
    lv_obj_set_pos(ui->main_screen_img_setting, 283, 4);
    lv_obj_set_size(ui->main_screen_img_setting, 20, 20);

    //Write style for main_screen_img_setting, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->main_screen_img_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->main_screen_img_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_img_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->main_screen_img_setting, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_label_theme
    ui->main_screen_label_theme = lv_label_create(ui->main_screen_cont_main);
    lv_label_set_text(ui->main_screen_label_theme, "Theme:");
    lv_label_set_long_mode(ui->main_screen_label_theme, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->main_screen_label_theme, 173, 215);
    lv_obj_set_size(ui->main_screen_label_theme, 63, 15);

    //Write style for main_screen_label_theme, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_screen_label_theme, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_screen_label_theme, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_screen_label_theme, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_screen_label_theme, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_label_theme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_label_ip
    ui->main_screen_label_ip = lv_label_create(ui->main_screen_cont_main);
    lv_label_set_text(ui->main_screen_label_ip, "IP: 192.168.1.1\nSSID: 1234567");
    lv_label_set_long_mode(ui->main_screen_label_ip, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->main_screen_label_ip, 138, 22);
    lv_obj_set_size(ui->main_screen_label_ip, 132, 38);
    lv_obj_add_flag(ui->main_screen_label_ip, LV_OBJ_FLAG_HIDDEN);

    //Write style for main_screen_label_ip, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_screen_label_ip, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_screen_label_ip, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_screen_label_ip, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_screen_label_ip, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_screen_label_ip, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_screen_label_ip, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_screen_label_ip, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_screen_label_ip, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for main_screen_label_ip, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_border_width(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->main_screen_label_ip, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->main_screen_label_ip, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->main_screen_label_ip, 255, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_letter_space(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_line_space(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_align(ui->main_screen_label_ip, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_opa(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_pad_top(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_pad_right(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_pad_bottom(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_pad_left(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->main_screen_label_ip, 0, LV_PART_MAIN|LV_STATE_DISABLED);

    //Write codes main_screen_btn_home
    ui->main_screen_btn_home = lv_btn_create(ui->main_screen_cont_main);
    ui->main_screen_btn_home_label = lv_label_create(ui->main_screen_btn_home);
    lv_label_set_text(ui->main_screen_btn_home_label, "" LV_SYMBOL_HOME " ");
    lv_label_set_long_mode(ui->main_screen_btn_home_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->main_screen_btn_home_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->main_screen_btn_home, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->main_screen_btn_home_label, LV_PCT(100));
    lv_obj_set_pos(ui->main_screen_btn_home, 17, 147);
    lv_obj_set_size(ui->main_screen_btn_home, 68, 52);

    //Write style for main_screen_btn_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->main_screen_btn_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_screen_btn_home, lv_color_hex(0xbb949a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_screen_btn_home, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->main_screen_btn_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_screen_btn_home, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_screen_btn_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_screen_btn_home, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_screen_btn_home, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_screen_btn_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_screen_btn_home, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_screen_line_1
    ui->main_screen_line_1 = lv_line_create(ui->main_screen_cont_main);
    static lv_point_t main_screen_line_1[] = {{0, 0},{320, 0},};
    lv_line_set_points(ui->main_screen_line_1, main_screen_line_1, 2);
    lv_obj_set_pos(ui->main_screen_line_1, 0, 210);
    lv_obj_set_size(ui->main_screen_line_1, 318, 1);

    //Write style for main_screen_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->main_screen_line_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->main_screen_line_1, lv_color_hex(0xb4b0b0), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->main_screen_line_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->main_screen_line_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of main_screen.
    

    //Update current screen layout.
    lv_obj_update_layout(ui->main_screen);

    //Init events for screen.
    events_init_main_screen(ui);
}
