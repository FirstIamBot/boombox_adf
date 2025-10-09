#ifndef BOOMBOX_H
#define BOOMBOX_H


#include "commons.h"

void boombox_task(void *pvParameters);

void boombox_config_init_default(BoomBox_config_t *config);
esp_err_t boombox_config_load_from_nvs(BoomBox_config_t *config);
esp_err_t boombox_config_save_to_nvs(const BoomBox_config_t *config);

#endif // BOOMBOX_H