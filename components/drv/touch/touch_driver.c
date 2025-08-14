/**
 * @file touch_driver.c
 */

#include "touch_driver.h"
#include "xpt2046.h"

void touch_driver_init(void)
{
#if defined (CONFIG_LV_TOUCH_CONTROLLER_XPT2046)
    xpt2046_init();
#endif
}

#if LVGL_VERSION_MAJOR >= 8
void touch_driver_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
#else
bool touch_driver_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
#endif
{
    bool res = false;

#if defined (CONFIG_LV_TOUCH_CONTROLLER_XPT2046)
    res = xpt2046_read(drv, data);
#endif

#if LVGL_VERSION_MAJOR >= 8
    data->continue_reading = res;
#else
    return res;
#endif
}

