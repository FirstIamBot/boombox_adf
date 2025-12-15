/*
* Copyright 2023 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
lv_group_t *group;
extern lv_indev_t * indev_touchpad;
extern lv_indev_t * indev_encoder;
extern int32_t encoder_diff;
/**
 * Create a demo application
 */
static void event_cb_ecoder_button(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
        case LV_EVENT_CLICKED:
            if(button_pres == 1 )
            {
                lv_obj_add_flag(guider_ui.pageAirradio_cont_vol, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.pageAirradio_slider_vol, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.pageAirradio_textprogress_vol, LV_OBJ_FLAG_HIDDEN);
                button_pres = 0;
            }
            else{
                lv_obj_clear_flag(guider_ui.pageAirradio_cont_vol, LV_OBJ_FLAG_HIDDEN);
		        lv_obj_clear_flag(guider_ui.pageAirradio_slider_vol, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.pageAirradio_textprogress_vol, LV_OBJ_FLAG_HIDDEN);
                button_pres = 1;
            }
        break;
	default:
		break;
	}
}
/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
       /* Add your codes here */
    group = lv_group_create();
    lv_group_set_default(group);
    
    lv_indev_set_group(indev_touchpad, group);
    lv_indev_set_group(indev_encoder, group);
    
    
    lv_group_add_obj(group, ui->pageAirradio_cont_vol);
    lv_group_add_obj(group, ui->pageAirradio_slider_vol);  
    lv_group_add_obj(group, ui->pageAirradio_textprogress_vol);
 
}


