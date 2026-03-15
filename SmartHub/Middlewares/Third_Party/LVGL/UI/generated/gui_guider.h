/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *main_screen;
	bool main_screen_del;
	lv_obj_t *main_screen_cont_main;
	lv_obj_t *main_screen_label_outdoor;
	lv_obj_t *main_screen_label_time;
	lv_obj_t *main_screen_label_date;
	lv_obj_t *main_screen_label_temp;
	lv_obj_t *main_screen_label_weather;
	lv_obj_t *main_screen_label_hltemp;
	lv_obj_t *main_screen_img_weather;
	lv_obj_t *main_screen_img_wifi;
	lv_obj_t *main_screen_img_setting;
	lv_obj_t *main_screen_label_theme;
	lv_obj_t *main_screen_label_ip;
	lv_obj_t *main_screen_btn_home;
	lv_obj_t *main_screen_btn_home_label;
	lv_obj_t *main_screen_line_1;
	lv_obj_t *screen_home;
	bool screen_home_del;
	lv_obj_t *screen_home_cont_home;
	lv_obj_t *screen_home_sw_LED2;
	lv_obj_t *screen_home_label_LED1;
	lv_obj_t *screen_home_sw_LED1;
	lv_obj_t *screen_home_label_LED2;
	lv_obj_t *screen_home_img_indoor_temp;
	lv_obj_t *screen_home_img_indoor_humi;
	lv_obj_t *screen_home_label_humi;
	lv_obj_t *screen_home_label_temp;
	lv_obj_t *screen_home_led_1;
	lv_obj_t *screen_home_label_status;
	lv_obj_t *screen_home_img_return;
	lv_obj_t *screen_home_label_indoor;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;

extern lv_timer_t *timer;
extern lv_timer_t *timer2;

//结构体，组合多个ui，当做定时器参数
typedef struct
{
	lv_obj_t * ui1;
	lv_obj_t * ui2;
}my_ui;

extern my_ui arg_ui;

void setup_scr_main_screen(lv_ui *ui);
void setup_scr_screen_home(lv_ui *ui);
LV_IMG_DECLARE(_weather_alpha_32x32);
LV_IMG_DECLARE(_wifi_alpha_20x20);
LV_IMG_DECLARE(_settings_alpha_20x20);
LV_IMG_DECLARE(_temp_alpha_50x50);
LV_IMG_DECLARE(_huminity_alpha_50x50);
LV_IMG_DECLARE(_return_alpha_32x32);

LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_32)
LV_FONT_DECLARE(lv_font_montserratMedium_40)


#ifdef __cplusplus
}
#endif
#endif
