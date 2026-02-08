#ifndef BOOMBOX_H
#define BOOMBOX_H


#include "commons.h"
#include "esp_wifi_manager.h"

void boombox_task(void *pvParameters);

void boombox_config_init_default(BoomBox_config_t *config);
esp_err_t boombox_config_load_from_nvs(BoomBox_config_t *config);
esp_err_t boombox_config_save_to_nvs(const BoomBox_config_t *config);

// Функции инициализации и деинициализации Bluetooth и WiFi coexistence
esp_err_t init_bt_wifi_coex(void);
void deinit_bt_wifi_coex(void);

#endif // BOOMBOX_H