/*
* Copyright 2026 NXP
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



void setup_scr_pageWebradio(lv_ui *ui)
{
    //Write codes pageWebradio
    ui->pageWebradio = lv_obj_create(NULL);
    lv_obj_set_size(ui->pageWebradio, 320, 240);
    lv_obj_set_scrollbar_mode(ui->pageWebradio, LV_SCROLLBAR_MODE_OFF);

    //Write style for pageWebradio, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageWebradio, 185, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageWebradio, lv_color_hex(0x00d2ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageWebradio, LV_GRAD_DIR_VER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->pageWebradio, lv_color_hex(0x2FCADA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->pageWebradio, 104, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->pageWebradio, 161, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageWebradio_label_web_id
    ui->pageWebradio_label_web_id = lv_label_create(ui->pageWebradio);
    lv_label_set_text(ui->pageWebradio_label_web_id, "");
    lv_label_set_long_mode(ui->pageWebradio_label_web_id, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageWebradio_label_web_id, 10, 210);
    lv_obj_set_size(ui->pageWebradio_label_web_id, 230, 15);

    //Write style for pageWebradio_label_web_id, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageWebradio_label_web_id, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageWebradio_label_web_id, &lv_font_DejaVuSans_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageWebradio_label_web_id, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageWebradio_label_web_id, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_label_web_id, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageWebradio_label_BT
    ui->pageWebradio_label_BT = lv_label_create(ui->pageWebradio);
    lv_label_set_text(ui->pageWebradio_label_BT, "" LV_SYMBOL_BLUETOOTH " ");
    lv_label_set_long_mode(ui->pageWebradio_label_BT, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageWebradio_label_BT, 223, 13);
    lv_obj_set_size(ui->pageWebradio_label_BT, 22, 22);
    lv_obj_add_flag(ui->pageWebradio_label_BT, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageWebradio_label_BT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageWebradio_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageWebradio_label_BT, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageWebradio_label_BT, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageWebradio_label_BT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageWebradio_label_BT, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageWebradio_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageWebradio_label_BT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageWebradio_label_BT, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageWebradio_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageWebradio_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageWebradio_label_BT, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageWebradio_label_menu_web
    ui->pageWebradio_label_menu_web = lv_label_create(ui->pageWebradio);
    lv_label_set_text(ui->pageWebradio_label_menu_web, "" LV_SYMBOL_SETTINGS " ");
    lv_label_set_long_mode(ui->pageWebradio_label_menu_web, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageWebradio_label_menu_web, 253, 16);
    lv_obj_set_size(ui->pageWebradio_label_menu_web, 20, 20);
    lv_obj_add_flag(ui->pageWebradio_label_menu_web, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageWebradio_label_menu_web, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageWebradio_label_menu_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_label_menu_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageWebradio_label_menu_web, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageWebradio_label_menu_web, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageWebradio_label_menu_web, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageWebradio_label_menu_web, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageWebradio_label_menu_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageWebradio_label_menu_web, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_label_menu_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageWebradio_label_menu_web, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageWebradio_label_menu_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageWebradio_label_menu_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageWebradio_label_menu_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_label_menu_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageWebradio_label_vol_web
    ui->pageWebradio_label_vol_web = lv_label_create(ui->pageWebradio);
    lv_label_set_text(ui->pageWebradio_label_vol_web, "" LV_SYMBOL_VOLUME_MAX " ");
    lv_label_set_long_mode(ui->pageWebradio_label_vol_web, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageWebradio_label_vol_web, 283, 15);
    lv_obj_set_size(ui->pageWebradio_label_vol_web, 20, 20);
    lv_obj_add_flag(ui->pageWebradio_label_vol_web, LV_OBJ_FLAG_CLICKABLE);

    //Write style for pageWebradio_label_vol_web, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageWebradio_label_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_label_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageWebradio_label_vol_web, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageWebradio_label_vol_web, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageWebradio_label_vol_web, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageWebradio_label_vol_web, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageWebradio_label_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageWebradio_label_vol_web, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_label_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageWebradio_label_vol_web, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageWebradio_label_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageWebradio_label_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageWebradio_label_vol_web, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_label_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageWebradio_idWeb
    ui->pageWebradio_idWeb = lv_label_create(ui->pageWebradio);
    lv_label_set_text(ui->pageWebradio_idWeb, "");
    lv_label_set_long_mode(ui->pageWebradio_idWeb, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageWebradio_idWeb, 30, 88);
    lv_obj_set_size(ui->pageWebradio_idWeb, 40, 21);

    //Write style for pageWebradio_idWeb, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageWebradio_idWeb, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageWebradio_idWeb, &lv_font_DejaVuSans_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageWebradio_idWeb, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageWebradio_idWeb, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_idWeb, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageWebradio_label_web_title
    ui->pageWebradio_label_web_title = lv_label_create(ui->pageWebradio);
    lv_label_set_text(ui->pageWebradio_label_web_title, "");
    lv_label_set_long_mode(ui->pageWebradio_label_web_title, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->pageWebradio_label_web_title, 77, 88);
    lv_obj_set_size(ui->pageWebradio_label_web_title, 232, 19);

    //Write style for pageWebradio_label_web_title, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageWebradio_label_web_title, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageWebradio_label_web_title, &lv_font_DejaVuSans_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageWebradio_label_web_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageWebradio_label_web_title, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_label_web_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageWebradio_cont_vol
    ui->pageWebradio_cont_vol = lv_obj_create(ui->pageWebradio);
    lv_obj_set_pos(ui->pageWebradio_cont_vol, 190, 4);
    lv_obj_set_size(ui->pageWebradio_cont_vol, 68, 233);
    lv_obj_set_scrollbar_mode(ui->pageWebradio_cont_vol, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->pageWebradio_cont_vol, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageWebradio_cont_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageWebradio_cont_vol, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageWebradio_cont_vol, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageWebradio_cont_vol, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageWebradio_cont_vol, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_cont_vol, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageWebradio_cont_vol, lv_color_hex(0x245793), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageWebradio_cont_vol, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageWebradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageWebradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageWebradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageWebradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_cont_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageWebradio_textprogress_vol_web
    ui->pageWebradio_textprogress_vol_web = lv_textprogress_create(ui->pageWebradio_cont_vol);
    lv_textprogress_set_range_value(ui->pageWebradio_textprogress_vol_web, 0, 100, 50, 0);
    lv_textprogress_set_decimal(ui->pageWebradio_textprogress_vol_web, 0);
    lv_textprogress_set_value(ui->pageWebradio_textprogress_vol_web, 50);
    lv_obj_set_pos(ui->pageWebradio_textprogress_vol_web, 9, 3);
    lv_obj_set_size(ui->pageWebradio_textprogress_vol_web, 45, 27);

    //Write style for pageWebradio_textprogress_vol_web, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->pageWebradio_textprogress_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageWebradio_textprogress_vol_web, lv_color_hex(0xdcf30c), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageWebradio_textprogress_vol_web, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageWebradio_textprogress_vol_web, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->pageWebradio_textprogress_vol_web, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageWebradio_textprogress_vol_web, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_textprogress_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageWebradio_textprogress_vol_web, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageWebradio_textprogress_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageWebradio_textprogress_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_textprogress_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes pageWebradio_slider_vol_web
    ui->pageWebradio_slider_vol_web = lv_slider_create(ui->pageWebradio_cont_vol);
    lv_slider_set_range(ui->pageWebradio_slider_vol_web, 0, 100);
    lv_slider_set_mode(ui->pageWebradio_slider_vol_web, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->pageWebradio_slider_vol_web, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->pageWebradio_slider_vol_web, 26, 41);
    lv_obj_set_size(ui->pageWebradio_slider_vol_web, 11, 177);

    //Write style for pageWebradio_slider_vol_web, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageWebradio_slider_vol_web, 170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageWebradio_slider_vol_web, lv_color_hex(0x050706), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageWebradio_slider_vol_web, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_slider_vol_web, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->pageWebradio_slider_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_slider_vol_web, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageWebradio_slider_vol_web, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageWebradio_slider_vol_web, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageWebradio_slider_vol_web, lv_color_hex(0x34ff98), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageWebradio_slider_vol_web, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_slider_vol_web, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for pageWebradio_slider_vol_web, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->pageWebradio_slider_vol_web, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageWebradio_slider_vol_web, lv_color_hex(0x00ed5c), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageWebradio_slider_vol_web, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_slider_vol_web, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes pageWebradio_btnm_Web_main
    ui->pageWebradio_btnm_Web_main = lv_btnmatrix_create(ui->pageWebradio);
    static const char *pageWebradio_btnm_Web_main_text_map[] = {"<<", "O", ">", ">>", "",};
    lv_btnmatrix_set_map(ui->pageWebradio_btnm_Web_main, pageWebradio_btnm_Web_main_text_map);
    lv_obj_set_pos(ui->pageWebradio_btnm_Web_main, 5, 160);
    lv_obj_set_size(ui->pageWebradio_btnm_Web_main, 310, 75);
    lv_obj_add_flag(ui->pageWebradio_btnm_Web_main, LV_OBJ_FLAG_HIDDEN);

    //Write style for pageWebradio_btnm_Web_main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageWebradio_btnm_Web_main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageWebradio_btnm_Web_main, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageWebradio_btnm_Web_main, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageWebradio_btnm_Web_main, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->pageWebradio_btnm_Web_main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->pageWebradio_btnm_Web_main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->pageWebradio_btnm_Web_main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->pageWebradio_btnm_Web_main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->pageWebradio_btnm_Web_main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->pageWebradio_btnm_Web_main, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_btnm_Web_main, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_btnm_Web_main, 31, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageWebradio_btnm_Web_main, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageWebradio_btnm_Web_main, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageWebradio_btnm_Web_main, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->pageWebradio_btnm_Web_main, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->pageWebradio_btnm_Web_main, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->pageWebradio_btnm_Web_main, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->pageWebradio_btnm_Web_main, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->pageWebradio_btnm_Web_main, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageWebradio_btnm_Web_main, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageWebradio_btnm_Web_main, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_btnm_Web_main, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->pageWebradio_btnm_Web_main, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->pageWebradio_btnm_Web_main, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->pageWebradio_btnm_Web_main, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_btnm_Web_main, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes pageWebradio_imgbtn_Airradio
    ui->pageWebradio_imgbtn_Airradio = lv_imgbtn_create(ui->pageWebradio);
    lv_obj_add_flag(ui->pageWebradio_imgbtn_Airradio, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->pageWebradio_imgbtn_Airradio, LV_IMGBTN_STATE_RELEASED, NULL, &_radio_alpha_22x22, NULL);
    ui->pageWebradio_imgbtn_Airradio_label = lv_label_create(ui->pageWebradio_imgbtn_Airradio);
    lv_label_set_text(ui->pageWebradio_imgbtn_Airradio_label, "");
    lv_label_set_long_mode(ui->pageWebradio_imgbtn_Airradio_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->pageWebradio_imgbtn_Airradio_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->pageWebradio_imgbtn_Airradio, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->pageWebradio_imgbtn_Airradio, 196, 11);
    lv_obj_set_size(ui->pageWebradio_imgbtn_Airradio, 22, 22);

    //Write style for pageWebradio_imgbtn_Airradio, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->pageWebradio_imgbtn_Airradio, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->pageWebradio_imgbtn_Airradio, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->pageWebradio_imgbtn_Airradio, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->pageWebradio_imgbtn_Airradio, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->pageWebradio_imgbtn_Airradio, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->pageWebradio_imgbtn_Airradio, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->pageWebradio_imgbtn_Airradio, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for pageWebradio_imgbtn_Airradio, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->pageWebradio_imgbtn_Airradio, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->pageWebradio_imgbtn_Airradio, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->pageWebradio_imgbtn_Airradio, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->pageWebradio_imgbtn_Airradio, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->pageWebradio_imgbtn_Airradio, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->pageWebradio_imgbtn_Airradio, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for pageWebradio_imgbtn_Airradio, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->pageWebradio_imgbtn_Airradio, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->pageWebradio_imgbtn_Airradio, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->pageWebradio_imgbtn_Airradio, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->pageWebradio_imgbtn_Airradio, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->pageWebradio_imgbtn_Airradio, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->pageWebradio_imgbtn_Airradio, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for pageWebradio_imgbtn_Airradio, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->pageWebradio_imgbtn_Airradio, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->pageWebradio_imgbtn_Airradio, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //The custom code of pageWebradio.


    //Update current screen layout.
    lv_obj_update_layout(ui->pageWebradio);

    //Init events for screen.
    events_init_pageWebradio(ui);
}
