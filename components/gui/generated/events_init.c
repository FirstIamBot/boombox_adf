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

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include "custom.h"
Data_GUI_Boombox_t xTransmitGUItoBoombox;
static uint8_t tempebandIDx = 1;
static uint8_t tempucucValue = 3;
static uint8_t tempeModIdx = 0;
uint8_t ucSlider_agc;
int slider_vol;
static char set_freq[8]; // Строка частоты для ручного ввода
static char temp_set_freq[8];
uint16_t sel_id;
uint8_t sel_num = 0;
int templen;
int slider_vol_web;

static void pageAirradio_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.pageAirradio_cont_BandWFM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_BandWFM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_StepFM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_StepFM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_vol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_slider_vol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_BandWSSB, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_textprogress_vol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_BandWAM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_BandWSSB, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_BandWAM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_StepAM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_StepAM, LV_OBJ_FLAG_HIDDEN);
        xTransmitGUItoBoombox.State = false;
        lv_obj_add_flag(guider_ui.pageAirradio_cont_AGC, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_cb_AGC, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_slider_AGC, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_textprogress_AGC, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_Main, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_up_step_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_LONG_PRESSED:
    {
        xTransmitGUItoBoombox.eDataDescription = eSeekUP;
        xTransmitGUItoBoombox.ucValue = 1;
        xTransmitGUItoBoombox.State = true;
        lv_obj_set_style_opa(guider_ui.pageAirradio_up_step, 255, 0);
        break;
    }
    case LV_EVENT_RELEASED:
    {
        lv_obj_set_style_opa(guider_ui.pageAirradio_up_step, 255, 0);
        break;
    }
    case LV_EVENT_PRESSED:
    {
        lv_obj_set_style_opa(guider_ui.pageAirradio_up_step, 132, 0);
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_down_step_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_LONG_PRESSED:
    {
        lv_obj_set_style_opa(guider_ui.pageAirradio_down_step, 255, 0);
        break;
    }
    case LV_EVENT_PRESSED:
    {
        xTransmitGUItoBoombox.eDataDescription = eStepDown;
        xTransmitGUItoBoombox.ucValue = 1;
        xTransmitGUItoBoombox.State = true;
        lv_obj_set_style_bg_color(guider_ui.pageAirradio_down_step, lv_color_hex(0x14d2c4), LV_PART_MAIN);
        break;
    }
    case LV_EVENT_RELEASED:
    {
        lv_obj_set_style_bg_color(guider_ui.pageAirradio_down_step, lv_color_hex(0x14d7c4), LV_PART_MAIN);
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_label_mono_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if (!(tempebandIDx == ebandIDx && tempucucValue == 3)) {
            lv_obj_clear_flag(guider_ui.pageAirradio_cont_Mod, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.pageAirradio_btnm_Mod, LV_OBJ_FLAG_HIDDEN);
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_label_Freq_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.pageAirradio_cont_set_freq, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_label_step_val_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(tempebandIDx == ebandIDx && tempucucValue == 3)
        {
            lv_obj_clear_flag(guider_ui.pageAirradio_cont_StepFM, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.pageAirradio_btnm_StepFM, LV_OBJ_FLAG_HIDDEN);
        }
        if(tempebandIDx == ebandIDx && tempucucValue != 3)
        {
            lv_obj_clear_flag(guider_ui.pageAirradio_cont_StepAM, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.pageAirradio_btnm_StepAM, LV_OBJ_FLAG_HIDDEN);
        }

        break;
    }
    default:
        break;
    }
}

static void pageAirradio_btnm_Main_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);

        switch (id) {
        case (0):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 0;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 0;

            break;
        }
        case (1):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 1;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 1;

            break;
        }
        case (2):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 2;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 2;

            break;
        }
        case (3):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 3;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 3;

            break;
        }
        case (4):
        {
            if(tempebandIDx == (ebandIDx && tempucucValue < 3 )||(tempebandIDx == ebandIDx && tempucucValue > 3))
            {
                lv_obj_clear_flag(guider_ui.pageAirradio_cont_Mod, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.pageAirradio_btnm_Mod, LV_OBJ_FLAG_HIDDEN);
            }

            break;
        }
        case (5):
        {
            lv_obj_clear_flag(guider_ui.pageAirradio_cont_AGC, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.pageAirradio_cb_AGC, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.pageAirradio_slider_AGC, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.pageAirradio_textprogress_AGC, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        case (6):
        {
            xTransmitGUItoBoombox.eDataDescription = eSeekUP;
            xTransmitGUItoBoombox.ucValue = 1;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (7):
        {
            ui_load_scr_animation(&guider_ui, &guider_ui.Bluetooth, guider_ui.Bluetooth_del, &guider_ui.pageAirradio_del, setup_scr_Bluetooth, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_label_wb_range_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        if(tempebandIDx == ebandIDx && tempucucValue == 3)
        {
            lv_obj_clear_flag(guider_ui.pageAirradio_cont_BandWFM, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.pageAirradio_btnm_BandWFM, LV_OBJ_FLAG_HIDDEN);
        }
        if(tempebandIDx == ebandIDx && tempucucValue != 3)
        {
            if(tempeModIdx == 0  ) {
                lv_obj_clear_flag(guider_ui.pageAirradio_cont_BandWAM, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.pageAirradio_btnm_BandWAM, LV_OBJ_FLAG_HIDDEN);
            }
            else
            {
                lv_obj_clear_flag(guider_ui.pageAirradio_cont_BandWSSB, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.pageAirradio_btnm_BandWSSB, LV_OBJ_FLAG_HIDDEN);
            }
        }

        break;
    }
    default:
        break;
    }
}

static void pageAirradio_label_wb_val_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(tempebandIDx == ebandIDx && tempucucValue == 3)
        {
            lv_obj_clear_flag(guider_ui.pageAirradio_cont_BandWFM, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.pageAirradio_btnm_BandWFM, LV_OBJ_FLAG_HIDDEN);
        }
        if(tempebandIDx == ebandIDx && tempucucValue != 3)
        {
            if(tempeModIdx == 0 ) {
                lv_obj_clear_flag(guider_ui.pageAirradio_cont_BandWAM, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.pageAirradio_btnm_BandWAM, LV_OBJ_FLAG_HIDDEN);
            }
            else if( tempeModIdx == 2 || tempeModIdx == 1)
            {
                lv_obj_clear_flag(guider_ui.pageAirradio_cont_BandWSSB, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.pageAirradio_btnm_BandWSSB, LV_OBJ_FLAG_HIDDEN);
            }
        }


        break;
    }
    default:
        break;
    }
}

static void pageAirradio_btnm_StepAM_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_StepAM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_StepAM, LV_OBJ_FLAG_HIDDEN);

        switch (id) {
        case (0):
        {
            xTransmitGUItoBoombox.eDataDescription = eStepAM;
            xTransmitGUItoBoombox.ucValue = 0;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (1):
        {
            xTransmitGUItoBoombox.eDataDescription = eStepAM;
            xTransmitGUItoBoombox.ucValue = 1;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (2):
        {
            xTransmitGUItoBoombox.eDataDescription = eStepAM;
            xTransmitGUItoBoombox.ucValue = 2;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (3):
        {
            xTransmitGUItoBoombox.eDataDescription = eStepAM;
            xTransmitGUItoBoombox.ucValue = 3;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_btnm_StepFM_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_StepFM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_StepFM, LV_OBJ_FLAG_HIDDEN);

        switch (id) {
        case (0):
        {
            xTransmitGUItoBoombox.eDataDescription = eStepFM;
            xTransmitGUItoBoombox.ucValue = 0;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (1):
        {
            xTransmitGUItoBoombox.eDataDescription = eStepFM;
            xTransmitGUItoBoombox.ucValue = 1;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (2):
        {
            xTransmitGUItoBoombox.eDataDescription = eStepFM;
            xTransmitGUItoBoombox.ucValue = 2;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (3):
        {
            xTransmitGUItoBoombox.eDataDescription = eStepFM;
            xTransmitGUItoBoombox.ucValue = 3;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_btnm_BandWFM_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_BandWFM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_BandWFM, LV_OBJ_FLAG_HIDDEN);

        switch (id) {
        case (0):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWFM;
            xTransmitGUItoBoombox.ucValue = 0;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (1):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWFM;
            xTransmitGUItoBoombox.ucValue = 1;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (2):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWFM;
            xTransmitGUItoBoombox.ucValue = 2;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (3):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWFM;
            xTransmitGUItoBoombox.ucValue = 3;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (4):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWFM;
            xTransmitGUItoBoombox.ucValue = 4;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_btnm_BandWSSB_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_BandWSSB, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_BandWSSB, LV_OBJ_FLAG_HIDDEN);

        switch (id) {
        case (0):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWSSB;
            xTransmitGUItoBoombox.ucValue = 3;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (1):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWSSB;
            xTransmitGUItoBoombox.ucValue = 2;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (2):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWSSB;
            xTransmitGUItoBoombox.ucValue = 1;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (3):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWSSB;
            xTransmitGUItoBoombox.ucValue = 0;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (4):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWSSB;
            xTransmitGUItoBoombox.ucValue = 5;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (5):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWSSB;
            xTransmitGUItoBoombox.ucValue = 4;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_btnm_BandWAM_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_BandWAM, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_BandWAM, LV_OBJ_FLAG_HIDDEN);

        switch (id) {
        case (0):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWAM;
            xTransmitGUItoBoombox.ucValue = 0;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (1):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWAM;
            xTransmitGUItoBoombox.ucValue = 1;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (2):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWAM;
            xTransmitGUItoBoombox.ucValue = 2;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (3):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWAM;
            xTransmitGUItoBoombox.ucValue = 6;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (4):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWAM;
            xTransmitGUItoBoombox.ucValue = 3;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (5):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWAM;
            xTransmitGUItoBoombox.ucValue = 5;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (6):
        {
            xTransmitGUItoBoombox.eDataDescription = eBandWAM;
            xTransmitGUItoBoombox.ucValue = 4;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_btnm_Mod_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        lv_obj_add_flag(guider_ui.pageAirradio_cont_Mod, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_Mod, LV_OBJ_FLAG_HIDDEN);

        switch (id) {
        case (0):
        {
            xTransmitGUItoBoombox.eDataDescription = eModIdx;
            xTransmitGUItoBoombox.ucValue = 0;
            tempeModIdx = xTransmitGUItoBoombox.ucValue;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (1):
        {
            xTransmitGUItoBoombox.eDataDescription = eModIdx;
            xTransmitGUItoBoombox.ucValue = 1;
            tempeModIdx = xTransmitGUItoBoombox.ucValue;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (2):
        {
            xTransmitGUItoBoombox.eDataDescription = eModIdx;
            xTransmitGUItoBoombox.ucValue = 2;
            tempeModIdx = xTransmitGUItoBoombox.ucValue;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_slider_AGC_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        if(tempebandIDx == ebandIDx && tempucucValue == 3)
        {
            ucSlider_agc = lv_slider_get_value(guider_ui.pageAirradio_slider_AGC);
            lv_textprogress_set_value(guider_ui.pageAirradio_textprogress_AGC, ucSlider_agc);
            xTransmitGUItoBoombox.eDataDescription = eSlider_agc;
            xTransmitGUItoBoombox.ucValue = ucSlider_agc*0.26;// нормализация FM
            xTransmitGUItoBoombox.State = true;
        }
        if(tempebandIDx == ebandIDx && tempucucValue != 3)
        {
            ucSlider_agc = lv_slider_get_value(guider_ui.pageAirradio_slider_AGC);
            lv_textprogress_set_value(guider_ui.pageAirradio_textprogress_AGC, ucSlider_agc);
            xTransmitGUItoBoombox.eDataDescription = eSlider_agc;
            xTransmitGUItoBoombox.ucValue = ucSlider_agc*0.36; // нормализация АМ
            xTransmitGUItoBoombox.State = true;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_cb_AGC_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);
        int status = lv_obj_get_state(status_obj) & LV_STATE_CHECKED ? true : false;

        switch (status) {
        case (true):
        {
            xTransmitGUItoBoombox.eDataDescription = eAGCgain;
            xTransmitGUItoBoombox.ucValue = 1;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        case (false):
        {
            xTransmitGUItoBoombox.eDataDescription = eAGCgain;
            xTransmitGUItoBoombox.ucValue = 0;
            xTransmitGUItoBoombox.State = true;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_label_set_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.pageAirradio_btnm_Main, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_label_vol_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.pageAirradio_cont_vol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.pageAirradio_slider_vol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.pageAirradio_textprogress_vol, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_label_bt_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.Bluetooth, guider_ui.Bluetooth_del, &guider_ui.pageAirradio_del, setup_scr_Bluetooth, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
        xTransmitGUItoBoombox.State = true;
        xTransmitGUItoBoombox.eModeBoombox = eBT;

        break;
    }
    default:
        break;
    }
}

static void pageAirradio_slider_vol_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        slider_vol = lv_slider_get_value(guider_ui.pageAirradio_slider_vol);
        lv_textprogress_set_value(guider_ui.pageAirradio_textprogress_vol, slider_vol);
        xTransmitGUItoBoombox.eDataDescription = eslider_vol;
        xTransmitGUItoBoombox.ucValue = slider_vol;
        xTransmitGUItoBoombox.State = true;
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_imgbtn_webradio_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.pageWebradio, guider_ui.pageWebradio_del, &guider_ui.pageAirradio_del, setup_scr_pageWebradio, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
        xTransmitGUItoBoombox.State = true;
        xTransmitGUItoBoombox.eModeBoombox = eWeb;
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_btnm_set_freq_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        int len = strlen(set_freq);
        if(tempebandIDx == ebandIDx && tempucucValue == 3)  // FM
        {
            templen = 4;
        }
        else if(tempebandIDx == ebandIDx && tempucucValue == 2)  // SW
        {
            templen = 4;
        }
        else if(tempebandIDx == ebandIDx && tempucucValue < 2)  // LW, MW
        {
            templen = 3;
        }
        else if(tempebandIDx == ebandIDx && tempucucValue > 3)  // LW, MW
        {
            templen = 4;
        }
        if (len <= templen)
        {
            sel_id = lv_btnmatrix_get_selected_btn(guider_ui.pageAirradio_btnm_set_freq);
            if(sel_id != 9 && sel_id != 11) {
                sel_num++;
                if((sel_num == 4) && (tempebandIDx == ebandIDx && tempucucValue == 3) ) {
                    strcat(temp_set_freq, ".");
                }
                else {
                    strcat(temp_set_freq, lv_btnmatrix_get_btn_text(guider_ui.pageAirradio_btnm_set_freq, sel_id));
                    strcat(set_freq, lv_btnmatrix_get_btn_text(guider_ui.pageAirradio_btnm_set_freq, sel_id));
                    lv_label_set_text(guider_ui.pageAirradio_label_set_freq, temp_set_freq);
                }
            }
        }


        switch (id) {
        case (9):
        {
            int p_len = strlen(set_freq);
            int temp_p_len = strlen(temp_set_freq);

            sel_num--;
            if (temp_p_len > 0)
            {
                set_freq[p_len-1] = '\0';
                temp_set_freq[temp_p_len-1] = '\0';
                lv_label_set_text(guider_ui.pageAirradio_label_set_freq, temp_set_freq);
            }
            break;
        }
        case (11):
        {
            xTransmitGUItoBoombox.eDataDescription = eSetFreq;
            xTransmitGUItoBoombox.ucValue = atoi(set_freq);
            xTransmitGUItoBoombox.State = true;
            for(uint8_t i = 0; i < 8; ++i) {
                set_freq[i] = '\0';
                temp_set_freq[i] = '\0';
            }
            lv_label_set_text(guider_ui.pageAirradio_label_set_freq, temp_set_freq);
            sel_num = 0;
            lv_obj_add_flag(guider_ui.pageAirradio_cont_set_freq, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_label_band_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.pageAirradio_btnm_band, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void pageAirradio_btnm_band_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        lv_obj_add_flag(guider_ui.pageAirradio_btnm_band, LV_OBJ_FLAG_HIDDEN);

        switch (id) {
        case (0):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 4;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 4;
            tempeModIdx = 1;
            break;
        }
        case (1):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 5;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 5;
            tempeModIdx = 1;
            break;
        }
        case (2):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 6;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 6;
            tempeModIdx = 1;
            break;
        }
        case (3):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 7;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 7;
            tempeModIdx = 0;
            break;
        }
        case (4):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 8;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 8;
            tempeModIdx = 0;
            break;
        }
        case (5):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 9;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 9;
            tempeModIdx = 0;
            break;
        }
        case (6):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 10;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 10;
            tempeModIdx = 0;
            break;
        }
        case (7):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 11;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 11;
            tempeModIdx = 2;
            break;
        }
        case (8):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 12;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 12;
            tempeModIdx = 0;
            break;
        }
        case (9):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 13;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 13;
            tempeModIdx = 1;
            break;
        }
        case (10):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 14;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 14;
            tempeModIdx = 0;
            break;
        }
        case (11):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 15;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 15;
            tempeModIdx = 0;
            break;
        }
        case (12):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 16;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 16;
            tempeModIdx = 2;
            break;
        }
        case (13):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 17;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 17;
            tempeModIdx = 0;
            break;
        }
        case (14):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 18;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 18;
            tempeModIdx = 0;
            break;
        }
        case (15):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 19;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 19;
            tempeModIdx = 2;
            break;
        }
        case (16):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 20;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 20;
            tempeModIdx = 0;
            break;
        }
        case (17):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 21;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 21;
            tempeModIdx = 2;
            break;
        }
        case (18):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 22;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 22;
            tempeModIdx = 0;
            break;
        }
        case (19):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 23;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 23;
            tempeModIdx = 0;
            break;
        }
        case (20):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 24;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 24;
            break;
        }
        case (21):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 25;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 25;
            tempeModIdx = 0;
            break;
        }
        case (22):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 26;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 26;
            tempeModIdx = 2;
            break;
        }
        case (23):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 27;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 27;
            tempeModIdx = 0;
            break;
        }
        case (24):
        {
            xTransmitGUItoBoombox.eDataDescription = ebandIDx;
            xTransmitGUItoBoombox.ucValue = 29;
            xTransmitGUItoBoombox.State = true;
            tempebandIDx = ebandIDx;
            tempucucValue = 29;
            tempeModIdx = 2;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_pageAirradio (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->pageAirradio, pageAirradio_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_up_step, pageAirradio_up_step_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_down_step, pageAirradio_down_step_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_label_mono, pageAirradio_label_mono_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_label_Freq, pageAirradio_label_Freq_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_label_step_val, pageAirradio_label_step_val_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_btnm_Main, pageAirradio_btnm_Main_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_label_wb_range, pageAirradio_label_wb_range_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_label_wb_val, pageAirradio_label_wb_val_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_btnm_StepAM, pageAirradio_btnm_StepAM_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_btnm_StepFM, pageAirradio_btnm_StepFM_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_btnm_BandWFM, pageAirradio_btnm_BandWFM_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_btnm_BandWSSB, pageAirradio_btnm_BandWSSB_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_btnm_BandWAM, pageAirradio_btnm_BandWAM_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_btnm_Mod, pageAirradio_btnm_Mod_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_slider_AGC, pageAirradio_slider_AGC_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_cb_AGC, pageAirradio_cb_AGC_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_label_set, pageAirradio_label_set_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_label_vol, pageAirradio_label_vol_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_label_bt, pageAirradio_label_bt_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_slider_vol, pageAirradio_slider_vol_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_imgbtn_webradio, pageAirradio_imgbtn_webradio_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_btnm_set_freq, pageAirradio_btnm_set_freq_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_label_band, pageAirradio_label_band_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageAirradio_btnm_band, pageAirradio_btnm_band_event_handler, LV_EVENT_ALL, ui);
}

static void Bluetooth_imgbtn_airradio_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.pageAirradio, guider_ui.pageAirradio_del, &guider_ui.Bluetooth_del, setup_scr_pageAirradio, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
        xTransmitGUItoBoombox.State = true;
        xTransmitGUItoBoombox.eModeBoombox = eAir;
        break;
    }
    default:
        break;
    }
}

static void Bluetooth_imgbtn_webradio_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.pageWebradio, guider_ui.pageWebradio_del, &guider_ui.Bluetooth_del, setup_scr_pageWebradio, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
        xTransmitGUItoBoombox.State = true;
        xTransmitGUItoBoombox.eModeBoombox = eWeb;
        break;
    }
    default:
        break;
    }
}

void events_init_Bluetooth (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->Bluetooth_imgbtn_airradio, Bluetooth_imgbtn_airradio_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->Bluetooth_imgbtn_webradio, Bluetooth_imgbtn_webradio_event_handler, LV_EVENT_ALL, ui);
}

static void pageWebradio_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.pageWebradio_cont_vol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageWebradio_slider_vol_web, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageWebradio_textprogress_vol_web, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.pageWebradio_btnm_Web_main, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void pageWebradio_label_BT_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.Bluetooth, guider_ui.Bluetooth_del, &guider_ui.pageWebradio_del, setup_scr_Bluetooth, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
        xTransmitGUItoBoombox.State = true;
        xTransmitGUItoBoombox.eModeBoombox = eBT;
        break;
    }
    default:
        break;
    }
}

static void pageWebradio_label_menu_web_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.pageWebradio_btnm_Web_main, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void pageWebradio_label_vol_web_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.pageWebradio_cont_vol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.pageWebradio_slider_vol_web, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.pageWebradio_textprogress_vol_web, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void pageWebradio_btnm_Web_main_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);

        switch (id) {
        case (3):
        {
            ui_load_scr_animation(&guider_ui, &guider_ui.Bluetooth, guider_ui.Bluetooth_del, &guider_ui.pageWebradio_del, setup_scr_Bluetooth, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void pageWebradio_slider_vol_web_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        slider_vol_web = lv_slider_get_value(guider_ui.pageWebradio_slider_vol_web);
        lv_textprogress_set_value(guider_ui.pageWebradio_textprogress_vol_web, slider_vol_web);
        xTransmitGUItoBoombox.eDataDescription = eslider_vol;
        xTransmitGUItoBoombox.ucValue = slider_vol_web;
        xTransmitGUItoBoombox.State = true;
        break;
    }
    default:
        break;
    }
}

static void pageWebradio_imgbtn_Airradio_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.pageAirradio, guider_ui.pageAirradio_del, &guider_ui.pageWebradio_del, setup_scr_pageAirradio, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
        xTransmitGUItoBoombox.State = true;
        xTransmitGUItoBoombox.eModeBoombox = eAir;
        break;
    }
    default:
        break;
    }
}

void events_init_pageWebradio (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->pageWebradio, pageWebradio_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageWebradio_label_BT, pageWebradio_label_BT_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageWebradio_label_menu_web, pageWebradio_label_menu_web_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageWebradio_label_vol_web, pageWebradio_label_vol_web_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageWebradio_btnm_Web_main, pageWebradio_btnm_Web_main_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageWebradio_slider_vol_web, pageWebradio_slider_vol_web_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->pageWebradio_imgbtn_Airradio, pageWebradio_imgbtn_Airradio_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
