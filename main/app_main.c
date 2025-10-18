/* Play music from Bluetooth device and HTTP stream

*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_avrc_api.h"
#include "esp_peripherals.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "http_stream.h"
#include "mp3_decoder.h"
#include "periph_wifi.h"

#include "board.h"

#include "input_key_service.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"

#include "boombox.h"
#include "gui.h"
#include "commons.h"

static const char *TAG = "BOOMBOX";


static TaskHandle_t bt_task_handle = NULL;
static TaskHandle_t http_task_handle = NULL;
// Глобальные переменные для управления переключением источников

//*********************************************************************************************************************


void app_main(void)
{
    ESP_LOGI(TAG, "===  BOOMBOX STARTING ===");
    
    // Инициализация NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    gui_boombox_queue_init();
    boombox_gui_queue_init();

    // Создаем задачи для плеера
    xTaskCreate(boombox_task, "boombox_task", 8192, NULL, 5, NULL);
    //xTaskCreate(task_gui, "task_gui", 8192, NULL, 5, NULL);
    xTaskCreate(task_gui_calibrate, "task_gui_calibrate", 8192, NULL, 5, NULL);
}
