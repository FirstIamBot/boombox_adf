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
  
	lv_obj_t *pageAirradio;
	bool pageAirradio_del;
	lv_obj_t *pageAirradio_RDS;
	lv_obj_t *pageAirradio_up_step;
	lv_obj_t *pageAirradio_up_step_label;
	lv_obj_t *pageAirradio_down_step;
	lv_obj_t *pageAirradio_down_step_label;
	lv_obj_t *pageAirradio_label_mono;
	lv_obj_t *pageAirradio_label_FreqRange;
	lv_obj_t *pageAirradio_label_Freq;
	lv_obj_t *pageAirradio_label_step_val;
	lv_obj_t *pageAirradio_label_step_range;
	lv_obj_t *pageAirradio_label_step_name;
	lv_obj_t *pageAirradio_label_wb_name;
	lv_obj_t *pageAirradio_btnm_Main;
	lv_obj_t *pageAirradio_label_wb_range;
	lv_obj_t *pageAirradio_label_wb_val;
	lv_obj_t *pageAirradio_cont_StepAM;
	lv_obj_t *pageAirradio_btnm_StepAM;
	lv_obj_t *pageAirradio_cont_StepFM;
	lv_obj_t *pageAirradio_btnm_StepFM;
	lv_obj_t *pageAirradio_cont_BandWFM;
	lv_obj_t *pageAirradio_btnm_BandWFM;
	lv_obj_t *pageAirradio_cont_BandWSSB;
	lv_obj_t *pageAirradio_btnm_BandWSSB;
	lv_obj_t *pageAirradio_cont_BandWAM;
	lv_obj_t *pageAirradio_btnm_BandWAM;
	lv_obj_t *pageAirradio_cont_Mod;
	lv_obj_t *pageAirradio_btnm_Mod;
	lv_obj_t *pageAirradio_label_rssi_val;
	lv_obj_t *pageAirradio_label_rrsi_name;
	lv_obj_t *pageAirradio_label_rssi_range;
	lv_obj_t *pageAirradio_label_snr_val;
	lv_obj_t *pageAirradio_label_snr_name;
	lv_obj_t *pageAirradio_label_snr_range;
	lv_obj_t *pageAirradio_cont_AGC;
	lv_obj_t *pageAirradio_textprogress_AGC;
	lv_obj_t *pageAirradio_slider_AGC;
	lv_obj_t *pageAirradio_cb_AGC;
	lv_obj_t *pageAirradio_label_set;
	lv_obj_t *pageAirradio_label_vol;
	lv_obj_t *pageAirradio_label_bt;
	lv_obj_t *pageAirradio_cont_vol;
	lv_obj_t *pageAirradio_textprogress_vol;
	lv_obj_t *pageAirradio_slider_vol;
	lv_obj_t *pageAirradio_imgbtn_webradio;
	lv_obj_t *pageAirradio_imgbtn_webradio_label;
	lv_obj_t *pageAirradio_cont_set_freq;
	lv_obj_t *pageAirradio_btnm_set_freq;
	lv_obj_t *pageAirradio_label_set_freq;
	lv_obj_t *pageAirradio_label_band;
	lv_obj_t *pageAirradio_btnm_band;
	lv_obj_t *Bluetooth;
	bool Bluetooth_del;
	lv_obj_t *Bluetooth_imgbtn_airradio;
	lv_obj_t *Bluetooth_imgbtn_airradio_label;
	lv_obj_t *Bluetooth_imgbtn_webradio;
	lv_obj_t *Bluetooth_imgbtn_webradio_label;
	lv_obj_t *Bluetooth_label_BT;
	lv_obj_t *Bluetooth_label_bt_song;
	lv_obj_t *pageWebradio;
	bool pageWebradio_del;
	lv_obj_t *pageWebradio_label_BT;
	lv_obj_t *pageWebradio_label_menu_web;
	lv_obj_t *pageWebradio_label_vol_web;
	lv_obj_t *pageWebradio_label_web_song;
	lv_obj_t *pageWebradio_btnm_Web_main;
	lv_obj_t *pageWebradio_cont_vol;
	lv_obj_t *pageWebradio_textprogress_vol_web;
	lv_obj_t *pageWebradio_slider_vol_web;
	lv_obj_t *pageWebradio_imgbtn_Airradio;
	lv_obj_t *pageWebradio_imgbtn_Airradio_label;
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


void setup_scr_pageAirradio(lv_ui *ui);
void setup_scr_Bluetooth(lv_ui *ui);
void setup_scr_pageWebradio(lv_ui *ui);
LV_IMG_DECLARE(_website_alpha_22x22);
LV_IMG_DECLARE(_radio_alpha_22x22);
LV_IMG_DECLARE(_website_alpha_22x22);
LV_IMG_DECLARE(_radio_alpha_22x22);

LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_Antonio_Regular_16)
LV_FONT_DECLARE(lv_font_montserratMedium_10)
LV_FONT_DECLARE(lv_font_montserratMedium_26)
LV_FONT_DECLARE(lv_font_montserratMedium_14)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_montserratMedium_18)
LV_FONT_DECLARE(lv_font_montserratMedium_56)


#ifdef __cplusplus
}
#endif
#endif
