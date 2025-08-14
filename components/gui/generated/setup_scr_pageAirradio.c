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



void setup_scr_pageAirradio(lv_ui *ui)
{
    //Write codes pageAirradio
    ui->pageAirradio = lv_obj_create(NULL);
    lv_obj_set_size(ui->pageAirradio, 320, 240);
    lv_obj_set_scrollbar_mode(ui->pageAirradio, LV_SCROLLBAR_MODE_OFF);

    //Write style for pageAirradio, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageAirradio, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio, lv_color_hex(0x37d5f7), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio, LV_GRAD_DIR_VER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->pageAirradio, lv_color_hex(0x00eeff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->pageAirradio, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->pageAirradio, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_RDS
    ui->pageAirradio_RDS = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_RDS, "");
    lv_label_set_long_mode(ui->pageAirradio_RDS, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_pos(ui->pageAirradio_RDS, 74, 141);
    lv_obj_set_size(ui->pageAirradio_RDS, 176, 14);

    //Write style for pageAirradio_RDS, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_RDS, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_RDS, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_RDS, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_RDS, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_RDS, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_RDS, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_RDS, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_RDS, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_RDS, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_RDS, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_RDS, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_RDS, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_RDS, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_RDS, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_up_step
    ui->pageAirradio_up_step = lv_btn_create(ui->pageAirradio);
    ui->pageAirradio_up_step_label = lv_label_create(ui->pageAirradio_up_step);
    lv_label_set_text(ui->pageAirradio_up_step_label, "" LV_SYMBOL_NEXT " ");
    lv_label_set_long_mode(ui->pageAirradio_up_step_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->pageAirradio_up_step_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->pageAirradio_up_step, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->pageAirradio_up_step_label, LV_PCT(100));
    lv_obj_set_pos(ui->pageAirradio_up_step, 261, 89);
    lv_obj_set_size(ui->pageAirradio_up_step, 40, 38);

    //Write style for pageAirradio_up_step, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageAirradio_up_step, 71, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_up_step, lv_color_hex(0x666666), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_up_step, LV_GRAD_DIR_HOR, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->pageAirradio_up_step, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->pageAirradio_up_step, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->pageAirradio_up_step, 187, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->pageAirradio_up_step, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_up_step, 44, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_up_step, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_up_step, lv_color_hex(0x1b614f), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_up_step, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_up_step, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_up_step, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_down_step
    ui->pageAirradio_down_step = lv_btn_create(ui->pageAirradio);
    ui->pageAirradio_down_step_label = lv_label_create(ui->pageAirradio_down_step);
    lv_label_set_text(ui->pageAirradio_down_step_label, "" LV_SYMBOL_PREV " ");
    lv_label_set_long_mode(ui->pageAirradio_down_step_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->pageAirradio_down_step_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->pageAirradio_down_step, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->pageAirradio_down_step_label, LV_PCT(100));
    lv_obj_set_pos(ui->pageAirradio_down_step, 19, 89);
    lv_obj_set_size(ui->pageAirradio_down_step, 40, 38);

    //Write style for pageAirradio_down_step, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageAirradio_down_step, 40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_down_step, lv_color_hex(0x666666), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_down_step, LV_GRAD_DIR_HOR, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->pageAirradio_down_step, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->pageAirradio_down_step, 59, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->pageAirradio_down_step, 189, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->pageAirradio_down_step, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_down_step, 44, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_down_step, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_down_step, lv_color_hex(0x1b614f), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_down_step, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_down_step, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_down_step, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_mono
    ui->pageAirradio_label_mono = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_mono, "");
    lv_label_set_long_mode(ui->pageAirradio_label_mono, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_mono, 122, 62);
    lv_obj_set_size(ui->pageAirradio_label_mono, 66, 15);
    lv_obj_add_flag(ui->pageAirradio_label_mono, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_label_mono, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_mono, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_mono, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_mono, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_mono, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_mono, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_mono, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_mono, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_mono, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_mono, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_mono, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_mono, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_mono, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_mono, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_mono, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_FreqRange
    ui->pageAirradio_label_FreqRange = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_FreqRange, "");
    lv_label_set_long_mode(ui->pageAirradio_label_FreqRange, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_FreqRange, 190, 104);
    lv_obj_set_size(ui->pageAirradio_label_FreqRange, 59, 15);

    //Write style for pageAirradio_label_FreqRange, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_FreqRange, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_FreqRange, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_FreqRange, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_FreqRange, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_FreqRange, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_FreqRange, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_FreqRange, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_FreqRange, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_FreqRange, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_FreqRange, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_FreqRange, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_FreqRange, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_FreqRange, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_FreqRange, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_Freq
    ui->pageAirradio_label_Freq = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_Freq, "");
    lv_label_set_long_mode(ui->pageAirradio_label_Freq, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_Freq, 74, 94);
    lv_obj_set_size(ui->pageAirradio_label_Freq, 124, 27);
    lv_obj_add_flag(ui->pageAirradio_label_Freq, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_label_Freq, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_Freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_Freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_Freq, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_Freq, &lv_font_montserratMedium_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_Freq, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_Freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_Freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_Freq, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_Freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_Freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_Freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_Freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_Freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_Freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_step_val
    ui->pageAirradio_label_step_val = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_step_val, "100");
    lv_label_set_long_mode(ui->pageAirradio_label_step_val, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_step_val, 52, 22);
    lv_obj_set_size(ui->pageAirradio_label_step_val, 40, 17);
    lv_obj_add_flag(ui->pageAirradio_label_step_val, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_label_step_val, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_step_val, lv_color_hex(0xd80000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_step_val, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_step_val, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_step_val, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_step_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_step_range
    ui->pageAirradio_label_step_range = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_step_range, "KHz");
    lv_label_set_long_mode(ui->pageAirradio_label_step_range, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_step_range, 55, 39);
    lv_obj_set_size(ui->pageAirradio_label_step_range, 35, 13);

    //Write style for pageAirradio_label_step_range, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_step_range, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_step_range, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_step_range, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_step_range, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_step_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_step_name
    ui->pageAirradio_label_step_name = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_step_name, "Step");
    lv_label_set_long_mode(ui->pageAirradio_label_step_name, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_step_name, 58, 8);
    lv_obj_set_size(ui->pageAirradio_label_step_name, 29, 13);

    //Write style for pageAirradio_label_step_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_step_name, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_step_name, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_step_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_step_name, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_step_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_wb_name
    ui->pageAirradio_label_wb_name = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_wb_name, "Wb");
    lv_label_set_long_mode(ui->pageAirradio_label_wb_name, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_wb_name, 14, 8);
    lv_obj_set_size(ui->pageAirradio_label_wb_name, 26, 14);

    //Write style for pageAirradio_label_wb_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_wb_name, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_wb_name, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_wb_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_wb_name, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_wb_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_btnm_Main
    ui->pageAirradio_btnm_Main = lv_btnmatrix_create(ui->pageAirradio);
    static const char *pageAirradio_btnm_Main_text_map[] = {"LW", "MW", "SW", "FM", "\n", "Mod", "AGC", "Scan", "BT", "",};
    lv_btnmatrix_set_map(ui->pageAirradio_btnm_Main, pageAirradio_btnm_Main_text_map);
    lv_obj_set_pos(ui->pageAirradio_btnm_Main, 7, 141);
    lv_obj_set_size(ui->pageAirradio_btnm_Main, 306, 94);
    lv_obj_add_flag(ui->pageAirradio_btnm_Main, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_btnm_Main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_Main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_Main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_Main, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_Main, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_btnm_Main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_btnm_Main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_btnm_Main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_btnm_Main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageAirradio_btnm_Main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageAirradio_btnm_Main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_Main, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_Main, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_Main, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_Main, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_btnm_Main, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_Main, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_Main, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_Main, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_Main, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_btnm_Main, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_btnm_Main, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_btnm_Main, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_Main, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_Main, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_Main, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_Main, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_btnm_Main, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_wb_range
    ui->pageAirradio_label_wb_range = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_wb_range, "KHz");
    lv_label_set_long_mode(ui->pageAirradio_label_wb_range, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_wb_range, 10, 39);
    lv_obj_set_size(ui->pageAirradio_label_wb_range, 35, 13);

    //Write style for pageAirradio_label_wb_range, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_wb_range, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_wb_range, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_wb_range, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_wb_range, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_wb_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_wb_val
    ui->pageAirradio_label_wb_val = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_wb_val, "AUTO");
    lv_label_set_long_mode(ui->pageAirradio_label_wb_val, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_wb_val, 4, 22);
    lv_obj_set_size(ui->pageAirradio_label_wb_val, 50, 16);
    lv_obj_add_flag(ui->pageAirradio_label_wb_val, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_label_wb_val, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_wb_val, lv_color_hex(0xd80000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_wb_val, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_wb_val, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_wb_val, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_wb_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_cont_StepAM
    ui->pageAirradio_cont_StepAM = lv_obj_create(ui->pageAirradio);
    lv_obj_set_pos(ui->pageAirradio_cont_StepAM, 5, 16);
    lv_obj_set_size(ui->pageAirradio_cont_StepAM, 73, 182);
    lv_obj_set_scrollbar_mode(ui->pageAirradio_cont_StepAM, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageAirradio_cont_StepAM, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_cont_StepAM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_cont_StepAM, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cont_StepAM, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cont_StepAM, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cont_StepAM, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cont_StepAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cont_StepAM, 197, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cont_StepAM, lv_color_hex(0x314760), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cont_StepAM, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_cont_StepAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cont_StepAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cont_StepAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cont_StepAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cont_StepAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_btnm_StepAM
    ui->pageAirradio_btnm_StepAM = lv_btnmatrix_create(ui->pageAirradio_cont_StepAM);
    static const char *pageAirradio_btnm_StepAM_text_map[] = {"1", "\n", "5", "\n", "8", "\n", "10", "",};
    lv_btnmatrix_set_map(ui->pageAirradio_btnm_StepAM, pageAirradio_btnm_StepAM_text_map);
    lv_obj_set_pos(ui->pageAirradio_btnm_StepAM, 5, 10);
    lv_obj_set_size(ui->pageAirradio_btnm_StepAM, 59, 159);
    lv_obj_add_flag(ui->pageAirradio_btnm_StepAM, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_btnm_StepAM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_StepAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_StepAM, 31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_StepAM, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_StepAM, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_btnm_StepAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_btnm_StepAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_btnm_StepAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_btnm_StepAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageAirradio_btnm_StepAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageAirradio_btnm_StepAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_StepAM, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_StepAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_btnm_StepAM, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_StepAM, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_StepAM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_StepAM, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_StepAM, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_btnm_StepAM, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_btnm_StepAM, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_btnm_StepAM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_StepAM, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_StepAM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_StepAM, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_StepAM, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_btnm_StepAM, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes pageAirradio_cont_StepFM
    ui->pageAirradio_cont_StepFM = lv_obj_create(ui->pageAirradio);
    lv_obj_set_pos(ui->pageAirradio_cont_StepFM, 4, 6);
    lv_obj_set_size(ui->pageAirradio_cont_StepFM, 77, 182);
    lv_obj_set_scrollbar_mode(ui->pageAirradio_cont_StepFM, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageAirradio_cont_StepFM, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_cont_StepFM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_cont_StepFM, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cont_StepFM, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cont_StepFM, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cont_StepFM, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cont_StepFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cont_StepFM, 197, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cont_StepFM, lv_color_hex(0x314760), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cont_StepFM, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_cont_StepFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cont_StepFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cont_StepFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cont_StepFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cont_StepFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_btnm_StepFM
    ui->pageAirradio_btnm_StepFM = lv_btnmatrix_create(ui->pageAirradio_cont_StepFM);
    static const char *pageAirradio_btnm_StepFM_text_map[] = {"10", "\n", "50", "\n", "100", "\n", "200", "",};
    lv_btnmatrix_set_map(ui->pageAirradio_btnm_StepFM, pageAirradio_btnm_StepFM_text_map);
    lv_obj_set_pos(ui->pageAirradio_btnm_StepFM, 8, 10);
    lv_obj_set_size(ui->pageAirradio_btnm_StepFM, 59, 159);
    lv_obj_add_flag(ui->pageAirradio_btnm_StepFM, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_btnm_StepFM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_StepFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_StepFM, 31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_StepFM, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_StepFM, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_btnm_StepFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_btnm_StepFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_btnm_StepFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_btnm_StepFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageAirradio_btnm_StepFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageAirradio_btnm_StepFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_StepFM, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_StepFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_btnm_StepFM, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_StepFM, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_StepFM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_StepFM, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_StepFM, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_btnm_StepFM, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_btnm_StepFM, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_btnm_StepFM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_StepFM, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_StepFM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_StepFM, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_StepFM, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_btnm_StepFM, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes pageAirradio_cont_BandWFM
    ui->pageAirradio_cont_BandWFM = lv_obj_create(ui->pageAirradio);
    lv_obj_set_pos(ui->pageAirradio_cont_BandWFM, 3, 7);
    lv_obj_set_size(ui->pageAirradio_cont_BandWFM, 66, 188);
    lv_obj_set_scrollbar_mode(ui->pageAirradio_cont_BandWFM, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageAirradio_cont_BandWFM, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_cont_BandWFM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_cont_BandWFM, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cont_BandWFM, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cont_BandWFM, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cont_BandWFM, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cont_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cont_BandWFM, 152, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cont_BandWFM, lv_color_hex(0x2a4052), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cont_BandWFM, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_cont_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cont_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cont_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cont_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cont_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_btnm_BandWFM
    ui->pageAirradio_btnm_BandWFM = lv_btnmatrix_create(ui->pageAirradio_cont_BandWFM);
    static const char *pageAirradio_btnm_BandWFM_text_map[] = {"AUT", "\n", "110", "\n", "84", "\n", "60", "\n", "40", "",};
    lv_btnmatrix_set_map(ui->pageAirradio_btnm_BandWFM, pageAirradio_btnm_BandWFM_text_map);
    lv_obj_set_pos(ui->pageAirradio_btnm_BandWFM, 7, 9);
    lv_obj_set_size(ui->pageAirradio_btnm_BandWFM, 48, 167);

    //Write style for pageAirradio_btnm_BandWFM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_BandWFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_BandWFM, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_BandWFM, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_btnm_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_btnm_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_btnm_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_btnm_BandWFM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageAirradio_btnm_BandWFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageAirradio_btnm_BandWFM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_BandWFM, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_BandWFM, 151, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_BandWFM, lv_color_hex(0x0b4f72), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_BandWFM, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_btnm_BandWFM, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_BandWFM, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_BandWFM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_BandWFM, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_BandWFM, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_btnm_BandWFM, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_btnm_BandWFM, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_btnm_BandWFM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_BandWFM, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_BandWFM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_BandWFM, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_BandWFM, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_btnm_BandWFM, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes pageAirradio_cont_BandWSSB
    ui->pageAirradio_cont_BandWSSB = lv_obj_create(ui->pageAirradio);
    lv_obj_set_pos(ui->pageAirradio_cont_BandWSSB, 3, 5);
    lv_obj_set_size(ui->pageAirradio_cont_BandWSSB, 66, 188);
    lv_obj_set_scrollbar_mode(ui->pageAirradio_cont_BandWSSB, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageAirradio_cont_BandWSSB, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_cont_BandWSSB, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_cont_BandWSSB, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cont_BandWSSB, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cont_BandWSSB, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cont_BandWSSB, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cont_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cont_BandWSSB, 152, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cont_BandWSSB, lv_color_hex(0x2a4052), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cont_BandWSSB, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_cont_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cont_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cont_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cont_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cont_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_btnm_BandWSSB
    ui->pageAirradio_btnm_BandWSSB = lv_btnmatrix_create(ui->pageAirradio_cont_BandWSSB);
    static const char *pageAirradio_btnm_BandWSSB_text_map[] = {"4.0", "\n", "3.0", "\n", "2.2", "\n", "1.2", "\n", "1.0", "\n", "0.5", "",};
    lv_btnmatrix_set_map(ui->pageAirradio_btnm_BandWSSB, pageAirradio_btnm_BandWSSB_text_map);
    lv_obj_set_pos(ui->pageAirradio_btnm_BandWSSB, 7, 9);
    lv_obj_set_size(ui->pageAirradio_btnm_BandWSSB, 48, 167);

    //Write style for pageAirradio_btnm_BandWSSB, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_BandWSSB, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_BandWSSB, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_BandWSSB, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_btnm_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_btnm_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_btnm_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_btnm_BandWSSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageAirradio_btnm_BandWSSB, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageAirradio_btnm_BandWSSB, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_BandWSSB, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_BandWSSB, 151, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_BandWSSB, lv_color_hex(0x0b4f72), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_BandWSSB, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_btnm_BandWSSB, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_BandWSSB, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_BandWSSB, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_BandWSSB, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_BandWSSB, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_btnm_BandWSSB, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_btnm_BandWSSB, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_btnm_BandWSSB, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_BandWSSB, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_BandWSSB, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_BandWSSB, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_BandWSSB, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_btnm_BandWSSB, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes pageAirradio_cont_BandWAM
    ui->pageAirradio_cont_BandWAM = lv_obj_create(ui->pageAirradio);
    lv_obj_set_pos(ui->pageAirradio_cont_BandWAM, 3, 3);
    lv_obj_set_size(ui->pageAirradio_cont_BandWAM, 66, 188);
    lv_obj_set_scrollbar_mode(ui->pageAirradio_cont_BandWAM, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageAirradio_cont_BandWAM, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_cont_BandWAM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_cont_BandWAM, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cont_BandWAM, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cont_BandWAM, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cont_BandWAM, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cont_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cont_BandWAM, 152, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cont_BandWAM, lv_color_hex(0x2a4052), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cont_BandWAM, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_cont_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cont_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cont_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cont_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cont_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_btnm_BandWAM
    ui->pageAirradio_btnm_BandWAM = lv_btnmatrix_create(ui->pageAirradio_cont_BandWAM);
    static const char *pageAirradio_btnm_BandWAM_text_map[] = {"6.0", "\n", "4.0", "\n", "3.0", "\n", "2.5", "\n", "2.0", "\n", "1.8", "\n", "1.0", "",};
    lv_btnmatrix_set_map(ui->pageAirradio_btnm_BandWAM, pageAirradio_btnm_BandWAM_text_map);
    lv_obj_set_pos(ui->pageAirradio_btnm_BandWAM, 7, 8);
    lv_obj_set_size(ui->pageAirradio_btnm_BandWAM, 48, 167);

    //Write style for pageAirradio_btnm_BandWAM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_BandWAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_BandWAM, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_BandWAM, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_btnm_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_btnm_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_btnm_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_btnm_BandWAM, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageAirradio_btnm_BandWAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageAirradio_btnm_BandWAM, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_BandWAM, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_BandWAM, 151, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_BandWAM, lv_color_hex(0x0b4f72), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_BandWAM, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_btnm_BandWAM, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_BandWAM, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_BandWAM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_BandWAM, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_BandWAM, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_btnm_BandWAM, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_btnm_BandWAM, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_btnm_BandWAM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_BandWAM, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_BandWAM, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_BandWAM, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_BandWAM, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_btnm_BandWAM, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes pageAirradio_cont_Mod
    ui->pageAirradio_cont_Mod = lv_obj_create(ui->pageAirradio);
    lv_obj_set_pos(ui->pageAirradio_cont_Mod, 11, 27);
    lv_obj_set_size(ui->pageAirradio_cont_Mod, 66, 188);
    lv_obj_set_scrollbar_mode(ui->pageAirradio_cont_Mod, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageAirradio_cont_Mod, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_cont_Mod, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_cont_Mod, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cont_Mod, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cont_Mod, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cont_Mod, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cont_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cont_Mod, 152, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cont_Mod, lv_color_hex(0x2a4052), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cont_Mod, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_cont_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cont_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cont_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cont_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cont_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_btnm_Mod
    ui->pageAirradio_btnm_Mod = lv_btnmatrix_create(ui->pageAirradio_cont_Mod);
    static const char *pageAirradio_btnm_Mod_text_map[] = {"AM", "\n", "USB", "\n", "LSB", "",};
    lv_btnmatrix_set_map(ui->pageAirradio_btnm_Mod, pageAirradio_btnm_Mod_text_map);
    lv_obj_set_pos(ui->pageAirradio_btnm_Mod, 7, 8);
    lv_obj_set_size(ui->pageAirradio_btnm_Mod, 48, 167);

    //Write style for pageAirradio_btnm_Mod, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_Mod, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_Mod, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_Mod, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_btnm_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_btnm_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_btnm_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_btnm_Mod, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageAirradio_btnm_Mod, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageAirradio_btnm_Mod, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_Mod, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_Mod, 151, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_Mod, lv_color_hex(0x0b4f72), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_Mod, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_btnm_Mod, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_Mod, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_Mod, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_Mod, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_Mod, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_btnm_Mod, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_btnm_Mod, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_btnm_Mod, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_Mod, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_Mod, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_Mod, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_Mod, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_btnm_Mod, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_rssi_val
    ui->pageAirradio_label_rssi_val = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_rssi_val, "40");
    lv_label_set_long_mode(ui->pageAirradio_label_rssi_val, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_rssi_val, 134, 9);
    lv_obj_set_size(ui->pageAirradio_label_rssi_val, 29, 14);

    //Write style for pageAirradio_label_rssi_val, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_rssi_val, lv_color_hex(0xd80000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_rssi_val, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_rssi_val, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_rssi_val, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_rssi_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_rrsi_name
    ui->pageAirradio_label_rrsi_name = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_rrsi_name, "RSSI");
    lv_label_set_long_mode(ui->pageAirradio_label_rrsi_name, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_rrsi_name, 105, 10);
    lv_obj_set_size(ui->pageAirradio_label_rrsi_name, 29, 13);

    //Write style for pageAirradio_label_rrsi_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_rrsi_name, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_rrsi_name, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_rrsi_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_rrsi_name, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_rrsi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_rssi_range
    ui->pageAirradio_label_rssi_range = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_rssi_range, "dBW");
    lv_label_set_long_mode(ui->pageAirradio_label_rssi_range, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_rssi_range, 163, 12);
    lv_obj_set_size(ui->pageAirradio_label_rssi_range, 32, 11);

    //Write style for pageAirradio_label_rssi_range, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_rssi_range, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_rssi_range, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_rssi_range, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_rssi_range, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_rssi_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_snr_val
    ui->pageAirradio_label_snr_val = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_snr_val, "40");
    lv_label_set_long_mode(ui->pageAirradio_label_snr_val, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_snr_val, 241, 9);
    lv_obj_set_size(ui->pageAirradio_label_snr_val, 27, 14);

    //Write style for pageAirradio_label_snr_val, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_snr_val, lv_color_hex(0xd80000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_snr_val, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_snr_val, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_snr_val, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_snr_val, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_snr_name
    ui->pageAirradio_label_snr_name = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_snr_name, "SNR");
    lv_label_set_long_mode(ui->pageAirradio_label_snr_name, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_snr_name, 213, 11);
    lv_obj_set_size(ui->pageAirradio_label_snr_name, 27, 12);

    //Write style for pageAirradio_label_snr_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_snr_name, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_snr_name, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_snr_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_snr_name, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_snr_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_snr_range
    ui->pageAirradio_label_snr_range = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_snr_range, "dB");
    lv_label_set_long_mode(ui->pageAirradio_label_snr_range, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_snr_range, 270, 11);
    lv_obj_set_size(ui->pageAirradio_label_snr_range, 22, 12);

    //Write style for pageAirradio_label_snr_range, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_snr_range, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_snr_range, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_snr_range, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_snr_range, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_snr_range, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_cont_AGC
    ui->pageAirradio_cont_AGC = lv_obj_create(ui->pageAirradio);
    lv_obj_set_pos(ui->pageAirradio_cont_AGC, 6, 107);
    lv_obj_set_size(ui->pageAirradio_cont_AGC, 309, 47);
    lv_obj_set_scrollbar_mode(ui->pageAirradio_cont_AGC, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageAirradio_cont_AGC, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_cont_AGC, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_cont_AGC, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cont_AGC, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cont_AGC, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cont_AGC, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cont_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cont_AGC, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cont_AGC, lv_color_hex(0x245793), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cont_AGC, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_cont_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cont_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cont_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cont_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cont_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_textprogress_AGC
    ui->pageAirradio_textprogress_AGC = lv_textprogress_create(ui->pageAirradio_cont_AGC);
    lv_textprogress_set_range_value(ui->pageAirradio_textprogress_AGC, 0, 100, 50, 0);
    lv_textprogress_set_decimal(ui->pageAirradio_textprogress_AGC, 0);
    lv_textprogress_set_value(ui->pageAirradio_textprogress_AGC, 50);
    lv_obj_set_pos(ui->pageAirradio_textprogress_AGC, 168, -1);
    lv_obj_set_size(ui->pageAirradio_textprogress_AGC, 47, 15);

    //Write style for pageAirradio_textprogress_AGC, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->pageAirradio_textprogress_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_textprogress_AGC, lv_color_hex(0xff0027), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_textprogress_AGC, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_textprogress_AGC, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_textprogress_AGC, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_textprogress_AGC, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_textprogress_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_textprogress_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_textprogress_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_textprogress_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_textprogress_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_slider_AGC
    ui->pageAirradio_slider_AGC = lv_slider_create(ui->pageAirradio_cont_AGC);
    lv_slider_set_range(ui->pageAirradio_slider_AGC, 0, 100);
    lv_slider_set_mode(ui->pageAirradio_slider_AGC, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->pageAirradio_slider_AGC, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->pageAirradio_slider_AGC, 87, 21);
    lv_obj_set_size(ui->pageAirradio_slider_AGC, 210, 10);

    //Write style for pageAirradio_slider_AGC, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageAirradio_slider_AGC, 170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_slider_AGC, lv_color_hex(0x050706), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_slider_AGC, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_slider_AGC, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->pageAirradio_slider_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_slider_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_slider_AGC, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageAirradio_slider_AGC, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_slider_AGC, lv_color_hex(0x34ff98), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_slider_AGC, LV_GRAD_DIR_HOR, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->pageAirradio_slider_AGC, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->pageAirradio_slider_AGC, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->pageAirradio_slider_AGC, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_slider_AGC, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for pageAirradio_slider_AGC, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageAirradio_slider_AGC, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_slider_AGC, lv_color_hex(0x00ed5c), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_slider_AGC, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_slider_AGC, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes pageAirradio_cb_AGC
    ui->pageAirradio_cb_AGC = lv_checkbox_create(ui->pageAirradio_cont_AGC);
    lv_checkbox_set_text(ui->pageAirradio_cb_AGC, "AGC");
    lv_obj_set_pos(ui->pageAirradio_cb_AGC, 5, 10);

    //Write style for pageAirradio_cb_AGC, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_pad_top(ui->pageAirradio_cb_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cb_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cb_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cb_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_cb_AGC, lv_color_hex(0x0D3055), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_cb_AGC, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_cb_AGC, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_cb_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->pageAirradio_cb_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cb_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cb_AGC, 24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cb_AGC, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cb_AGC, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cb_AGC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_cb_AGC, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_pad_all(ui->pageAirradio_cb_AGC, 3, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->pageAirradio_cb_AGC, 2, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cb_AGC, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cb_AGC, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cb_AGC, LV_BORDER_SIDE_FULL, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cb_AGC, 6, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cb_AGC, 144, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cb_AGC, lv_color_hex(0xffffff), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cb_AGC, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_set
    ui->pageAirradio_label_set = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_set, "" LV_SYMBOL_SETTINGS " ");
    lv_label_set_long_mode(ui->pageAirradio_label_set, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_set, 257, 37);
    lv_obj_set_size(ui->pageAirradio_label_set, 20, 20);
    lv_obj_add_flag(ui->pageAirradio_label_set, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_label_set, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_set, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_set, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_set, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_set, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_set, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_set, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_vol
    ui->pageAirradio_label_vol = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_vol, "" LV_SYMBOL_VOLUME_MAX " ");
    lv_label_set_long_mode(ui->pageAirradio_label_vol, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_vol, 285, 37);
    lv_obj_set_size(ui->pageAirradio_label_vol, 20, 20);
    lv_obj_add_flag(ui->pageAirradio_label_vol, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_label_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_vol, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_vol, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_vol, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_vol, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_vol, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_vol, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_vol, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_bt
    ui->pageAirradio_label_bt = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_bt, "" LV_SYMBOL_BLUETOOTH " ");
    lv_label_set_long_mode(ui->pageAirradio_label_bt, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_bt, 232, 36);
    lv_obj_set_size(ui->pageAirradio_label_bt, 21, 21);
    lv_obj_add_flag(ui->pageAirradio_label_bt, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_label_bt, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_bt, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_bt, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_bt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_bt, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_bt, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_bt, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_bt, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_cont_vol
    ui->pageAirradio_cont_vol = lv_obj_create(ui->pageAirradio);
    lv_obj_set_pos(ui->pageAirradio_cont_vol, 241, 5);
    lv_obj_set_size(ui->pageAirradio_cont_vol, 68, 233);
    lv_obj_set_scrollbar_mode(ui->pageAirradio_cont_vol, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageAirradio_cont_vol, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_cont_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_cont_vol, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cont_vol, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cont_vol, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cont_vol, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cont_vol, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cont_vol, lv_color_hex(0x245793), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cont_vol, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_textprogress_vol
    ui->pageAirradio_textprogress_vol = lv_textprogress_create(ui->pageAirradio_cont_vol);
    lv_textprogress_set_range_value(ui->pageAirradio_textprogress_vol, 0, 100, 50, 0);
    lv_textprogress_set_decimal(ui->pageAirradio_textprogress_vol, 0);
    lv_textprogress_set_value(ui->pageAirradio_textprogress_vol, 50);
    lv_obj_set_pos(ui->pageAirradio_textprogress_vol, 9, 3);
    lv_obj_set_size(ui->pageAirradio_textprogress_vol, 45, 27);

    //Write style for pageAirradio_textprogress_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->pageAirradio_textprogress_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_textprogress_vol, lv_color_hex(0xdcf30c), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_textprogress_vol, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_textprogress_vol, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_textprogress_vol, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_textprogress_vol, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_textprogress_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_textprogress_vol, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_textprogress_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_textprogress_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_textprogress_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_slider_vol
    ui->pageAirradio_slider_vol = lv_slider_create(ui->pageAirradio_cont_vol);
    lv_slider_set_range(ui->pageAirradio_slider_vol, 0, 100);
    lv_slider_set_mode(ui->pageAirradio_slider_vol, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->pageAirradio_slider_vol, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->pageAirradio_slider_vol, 26, 41);
    lv_obj_set_size(ui->pageAirradio_slider_vol, 11, 177);

    //Write style for pageAirradio_slider_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageAirradio_slider_vol, 170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_slider_vol, lv_color_hex(0x050706), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_slider_vol, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_slider_vol, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->pageAirradio_slider_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_slider_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_slider_vol, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageAirradio_slider_vol, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_slider_vol, lv_color_hex(0x34ff98), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_slider_vol, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_slider_vol, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for pageAirradio_slider_vol, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageAirradio_slider_vol, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_slider_vol, lv_color_hex(0x00ed5c), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_slider_vol, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_slider_vol, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes pageAirradio_imgbtn_webradio
    ui->pageAirradio_imgbtn_webradio = lv_imgbtn_create(ui->pageAirradio);
    lv_obj_add_flag(ui->pageAirradio_imgbtn_webradio, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->pageAirradio_imgbtn_webradio, LV_IMGBTN_STATE_RELEASED, NULL, &_website_alpha_22x22, NULL);
    ui->pageAirradio_imgbtn_webradio_label = lv_label_create(ui->pageAirradio_imgbtn_webradio);
    lv_label_set_text(ui->pageAirradio_imgbtn_webradio_label, "");
    lv_label_set_long_mode(ui->pageAirradio_imgbtn_webradio_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->pageAirradio_imgbtn_webradio_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->pageAirradio_imgbtn_webradio, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->pageAirradio_imgbtn_webradio, 204, 36);
    lv_obj_set_size(ui->pageAirradio_imgbtn_webradio, 22, 22);

    //Write style for pageAirradio_imgbtn_webradio, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->pageAirradio_imgbtn_webradio, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_imgbtn_webradio, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_imgbtn_webradio, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_imgbtn_webradio, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_imgbtn_webradio, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->pageAirradio_imgbtn_webradio, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_imgbtn_webradio, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_imgbtn_webradio, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->pageAirradio_imgbtn_webradio, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->pageAirradio_imgbtn_webradio, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->pageAirradio_imgbtn_webradio, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->pageAirradio_imgbtn_webradio, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->pageAirradio_imgbtn_webradio, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->pageAirradio_imgbtn_webradio, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for pageAirradio_imgbtn_webradio, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->pageAirradio_imgbtn_webradio, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->pageAirradio_imgbtn_webradio, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->pageAirradio_imgbtn_webradio, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->pageAirradio_imgbtn_webradio, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->pageAirradio_imgbtn_webradio, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->pageAirradio_imgbtn_webradio, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for pageAirradio_imgbtn_webradio, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->pageAirradio_imgbtn_webradio, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->pageAirradio_imgbtn_webradio, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes pageAirradio_cont_set_freq
    ui->pageAirradio_cont_set_freq = lv_obj_create(ui->pageAirradio);
    lv_obj_set_pos(ui->pageAirradio_cont_set_freq, 7, 5);
    lv_obj_set_size(ui->pageAirradio_cont_set_freq, 305, 232);
    lv_obj_set_scrollbar_mode(ui->pageAirradio_cont_set_freq, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageAirradio_cont_set_freq, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageAirradio_cont_set_freq, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_cont_set_freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_cont_set_freq, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_cont_set_freq, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_cont_set_freq, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_cont_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_cont_set_freq, 126, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_cont_set_freq, lv_color_hex(0x35424d), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_cont_set_freq, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_cont_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_cont_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_cont_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_cont_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_cont_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_btnm_set_freq
    ui->pageAirradio_btnm_set_freq = lv_btnmatrix_create(ui->pageAirradio_cont_set_freq);
    static const char *pageAirradio_btnm_set_freq_text_map[] = {"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n", "Del", "0", "Ent", "",};
    lv_btnmatrix_set_map(ui->pageAirradio_btnm_set_freq, pageAirradio_btnm_set_freq_text_map);
    lv_obj_set_pos(ui->pageAirradio_btnm_set_freq, 5, 49);
    lv_obj_set_size(ui->pageAirradio_btnm_set_freq, 293, 177);
    lv_obj_add_flag(ui->pageAirradio_btnm_set_freq, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_btnm_set_freq, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_set_freq, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_set_freq, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_set_freq, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_set_freq, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_btnm_set_freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_btnm_set_freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_btnm_set_freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_btnm_set_freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageAirradio_btnm_set_freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageAirradio_btnm_set_freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_set_freq, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_set_freq, 28, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_set_freq, lv_color_hex(0x24004b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_set_freq, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_btnm_set_freq, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_set_freq, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_set_freq, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_set_freq, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_set_freq, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_btnm_set_freq, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_btnm_set_freq, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_btnm_set_freq, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_set_freq, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_set_freq, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_set_freq, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_set_freq, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_btnm_set_freq, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_set_freq
    ui->pageAirradio_label_set_freq = lv_label_create(ui->pageAirradio_cont_set_freq);
    lv_label_set_text(ui->pageAirradio_label_set_freq, "");
    lv_label_set_long_mode(ui->pageAirradio_label_set_freq, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_set_freq, 29, 10);
    lv_obj_set_size(ui->pageAirradio_label_set_freq, 246, 24);

    //Write style for pageAirradio_label_set_freq, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_set_freq, lv_color_hex(0xf3fbff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_set_freq, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_set_freq, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_set_freq, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_set_freq, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_set_freq, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_set_freq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_label_band
    ui->pageAirradio_label_band = lv_label_create(ui->pageAirradio);
    lv_label_set_text(ui->pageAirradio_label_band, "");
    lv_label_set_long_mode(ui->pageAirradio_label_band, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageAirradio_label_band, 114, 38);
    lv_obj_set_size(ui->pageAirradio_label_band, 75, 20);
    lv_obj_add_flag(ui->pageAirradio_label_band, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_label_band, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_label_band, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_label_band, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_label_band, lv_color_hex(0x02041a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_label_band, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_label_band, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageAirradio_label_band, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageAirradio_label_band, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageAirradio_label_band, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_label_band, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_label_band, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_label_band, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_label_band, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_label_band, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_label_band, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageAirradio_btnm_band
    ui->pageAirradio_btnm_band = lv_btnmatrix_create(ui->pageAirradio);
    static const char *pageAirradio_btnm_band_text_map[] = {"2220M", "630M", "160M", "120M", "90M", "\n", "80M", "75M", "60M", "49M", "40M", "\n", "41M", "31M", "30M", "25M", "22M", "\n", "20M", "19M", "17M", "16M", "15M", "\n", "15M", "13M", "12M", "11M", "10M", "",};
    lv_btnmatrix_set_map(ui->pageAirradio_btnm_band, pageAirradio_btnm_band_text_map);
    lv_obj_set_pos(ui->pageAirradio_btnm_band, 7, 8);
    lv_obj_set_size(ui->pageAirradio_btnm_band, 305, 228);
    lv_obj_add_flag(ui->pageAirradio_btnm_band, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->pageAirradio_btnm_band, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageAirradio_btnm_band, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_band, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_band, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_band, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_band, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageAirradio_btnm_band, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageAirradio_btnm_band, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageAirradio_btnm_band, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageAirradio_btnm_band, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageAirradio_btnm_band, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageAirradio_btnm_band, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_band, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_band, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_band, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_band, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageAirradio_btnm_band, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageAirradio_btnm_band, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageAirradio_btnm_band, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageAirradio_btnm_band, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageAirradio_btnm_band, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageAirradio_btnm_band, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageAirradio_btnm_band, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageAirradio_btnm_band, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageAirradio_btnm_band, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageAirradio_btnm_band, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageAirradio_btnm_band, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageAirradio_btnm_band, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageAirradio_btnm_band, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //The custom code of pageAirradio.


    //Update current screen layout.
    lv_obj_update_layout(ui->pageAirradio);

    //Init events for screen.
    events_init_pageAirradio(ui);
}
