/**
 * @file touch_driver.h
 */

#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

#if defined (CONFIG_LV_TOUCH_CONTROLLER_XPT2046)
#include "xpt2046.h"
#endif

/*********************
*      DEFINES
*********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void touch_driver_init(void);

void touch_driver_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TOUCH_DRIVER_H */

