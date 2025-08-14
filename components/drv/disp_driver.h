/**
 * @file disp_driver.h
 */

#ifndef DISP_DRIVER_H
#define DISP_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"

#include "ili9341.h"
#include "touch_driver.h"
/*********************
 *      DEFINES
 *********************/
/*********************
 *      DEFINES
 *********************/
/* DISP_BUF_SIZE value doesn't have an special meaning, but it's the size
 * of the buffer(s) passed to LVGL as display buffers. The default values used
 * were the values working for the contributor of the display controller.
 *
 * As LVGL supports partial display updates the DISP_BUF_SIZE doesn't
 * necessarily need to be equal to the display size.
 *
 * When using RGB displays the display buffer size will also depends on the
 * color format being used, for RGB565 each pixel needs 2 bytes.
 * When using the mono theme, the display pixels can be represented in one bit,
 * so the buffer size can be divided by 8, e.g. see SSD1306 display size. */


#define LV_HOR_RES_MAX 320
#define LV_VER_RES_MAX 240

#define DLV_HOR_RES_MAX  LV_HOR_RES_MAX
#define DLV_VER_RES_MAX  LV_VER_RES_MAX

#if defined (CONFIG_CUSTOM_DISPLAY_BUFFER_SIZE)
#define DISP_BUF_SIZE   CONFIG_CUSTOM_DISPLAY_BUFFER_BYTES
#else
#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9481
#define DISP_BUF_SIZE  (DLV_HOR_RES_MAX * DLV_VER_RES_MAX)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9486
#define DISP_BUF_SIZE  (DLV_HOR_RES_MAX * DLV_VER_RES_MAX)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9488
#define DISP_BUF_SIZE  (DLV_HOR_RES_MAX * DLV_VER_RES_MAX)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9341
#define DISP_BUF_SIZE  (DLV_HOR_RES_MAX * DLV_VER_RES_MAX)
#else
#error "No display controller selected"
#endif
#endif
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Initialize display */
void disp_driver_init(void);

/* Display flush callback */
void disp_driver_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map);

/* Initialize detected SPI and I2C bus and devices */
void lvgl_driver_init(void);

/* Initialize SPI master  */
bool lvgl_spi_driver_init(int host, int miso_pin, int mosi_pin, int sclk_pin,
    int max_transfer_sz, int dma_channel, int quadwp_pin, int quadhd_pin);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*DISP_DRIVER_H*/
