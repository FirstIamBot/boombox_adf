#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "boombox.h"
#include "bt_task.h"
#include "http_task.h"
#include "air_task.h"

#include "nvs_flash.h"
#include "nvs.h"


#define NVS_NAMESPACE "boombox_cfg"
#define CONFIG_KEY "config"

static const char *TAG = "BOOMBOX_TASK";

typedef enum {
    PLAYER_INACTIVE = 0,
    PLAYER_ACTIVE
} player_state_t;

// Глобальные переменные состояния для каждого плеера
player_state_t air_player_state = PLAYER_INACTIVE;
player_state_t http_player_state = PLAYER_INACTIVE;
player_state_t bt_player_state = PLAYER_INACTIVE;

Data_GUI_Boombox_t xResiveGUItoBoombox;// Переменная для хранения полученных данных от GUI к Boombox
Data_Boombox_GUI_t xTransmitBoomboxToGUI; // Переменная для передачи данных от Boombox к GUI

BoomBox_config_t xBoomBox_config; // Глобальная структура конфигурации Boombox  

extern QueueHandle_t xGuiToBoomboxQueue;
extern QueueHandle_t xBoomboxToGuiQueue;

// Глобальная переменная для выбора источника
audio_source_t g_current_source; // Изначально запускаем AIR радио;

esp_err_t boombox_config_load_from_nvs(BoomBox_config_t *config)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    size_t required_size = sizeof(BoomBox_config_t);

    // Открываем NVS хранилище
    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return err;
    }

    // Читаем конфигурацию
    err = nvs_get_blob(nvs_handle, CONFIG_KEY, config, &required_size);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGW(TAG, "Configuration not found in NVS, using defaults");
            // Инициализируем конфигурацию по умолчанию
            boombox_config_init_default(config);
        } else {
            ESP_LOGE(TAG, "Error reading config from NVS: %s", esp_err_to_name(err));
        }
    } else {
        ESP_LOGI(TAG, "Configuration loaded from NVS successfully");
    }

    // Закрываем NVS хранилище
    nvs_close(nvs_handle);
    return err;
}

esp_err_t boombox_config_save_to_nvs(const BoomBox_config_t *config)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Открываем NVS хранилище для записи
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle for write: %s", esp_err_to_name(err));
        return err;
    }

    // Сохраняем конфигурацию
    err = nvs_set_blob(nvs_handle, CONFIG_KEY, config, sizeof(BoomBox_config_t));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving config to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    // Коммитим изменения
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error committing to NVS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Configuration saved to NVS successfully");
    }

    // Закрываем NVS хранилище
    nvs_close(nvs_handle);
    return err;
}

void boombox_config_init_default(BoomBox_config_t *config)
{
    config->eModeBoombox = eAir; // Режим по умолчанию - AIR
    config->air_radio_config.currentBandType = 3; // FM_BAND_TYPE
    config->air_radio_config.currentMod = 1;     // 0 - AM, 1 - LSB, 2 - USB
    config->air_radio_config.currentStepFM = 1; // 1 - 10 КГц, 5 - 50КГц, 10 - 100 КГц
    config->air_radio_config.currentStepAM = 1;
    config->air_radio_config.currentFrequency = 10030;    
    config->air_radio_config.currentVOL = 60;   // 0 = Минимум, 64 = Максимум
    config->air_radio_config.BandWidthFM = 0;     // AUT-0, 110-1
    config->air_radio_config.BandWidthAM = 0;     // 6kHz - 0
    config->air_radio_config.BandWidthSSB = 0;    // 1.2KHz - 0
    config->air_radio_config.currentAGCgain = 36; // 0 = мин. аттенюация (макс. усиление), 36 - макс. аттенюация (мин. усиление)
    config->air_radio_config.onoffAGCgain = 1;    // 0 = AGC включен, 1 = выключен
    config->air_radio_config.rssi_thresh_seek = 15; // Минимальный уровень RSSI для принятия станции
    config->air_radio_config.snr_thresh_seek = 8;   // Минимальный уровень SNR для принятия станции

    config->air_radio_config.air_FM_station.currentStationIndex = 0;
    config->air_radio_config.air_LW_station.currentStationIndex = 0;
    config->air_radio_config.air_MW_station.currentStationIndex = 0;
    config->air_radio_config.air_SW_station.currentStationIndex = 0;

    for(int i = 0; i < 50; i++) {
        config->air_radio_config.air_FM_station.stations[i] = 0; // Инициализация массива частот станций
    }
    for(int i = 0; i < 50; i++) {
        config->air_radio_config.air_MW_station.stations[i] = 0; // Инициализация массива частот станций
    } 
    for(int i = 0; i < 50; i++) {
        config->air_radio_config.air_SW_station.stations[i] = 0; // Инициализация массива частот станций
    }    
    ESP_LOGI(TAG, "Default configuration initialized");
}

void boombox_task(void *pvParameters)
{

    // Инициализация NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Загрузка конфигурации из NVS
    if (boombox_config_load_from_nvs(&xBoomBox_config) != ESP_OK) {
        ESP_LOGW(TAG, "Failed to load config, using defaults");
        boombox_config_init_default(&xBoomBox_config);
    }
    // Устанавливаем источник по умолчанию из конфигурации
    g_current_source = xBoomBox_config.eModeBoombox;


    while (1) {
        /*****************************************************************************************
        *   Receive data from GUI
        ******************************************************************************************/
        if(pdTRUE == xQueueReceive(xGuiToBoomboxQueue, &xResiveGUItoBoombox, pdPASS))
        {
            g_current_source = (audio_source_t)xResiveGUItoBoombox.eModeBoombox;// Обработка полученных данных
        }
        /******************************************************************************************
         * Обработка данных от GUI 
         ******************************************************************************************/
        if (g_current_source == SOURCE_BLUETOOTH) {
            ESP_LOGI(TAG, "Switching to Bluetooth player");
            if(http_player_state == PLAYER_ACTIVE) {
                // Остановить HTTP плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!
                http_player_state = PLAYER_INACTIVE;
                // deinit_http_player(); // Функция деинициализации HTTP плеера
                ESP_LOGI(TAG, "http_player_state = PLAYER_INACTIVE;");
            }
            else if(air_player_state == PLAYER_ACTIVE) {
                boombox_config_save_to_nvs(&xBoomBox_config); // Сохраняем конфигурацию перед отключением AIR плеера
                air_player_state = PLAYER_INACTIVE;// Остановить AIR плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!
                ESP_LOGI(TAG, "air_player_state = PLAYER_INACTIVE;");
                deinit_air_player(); // Функция деинициализации AIR плеера
            }
            else if(bt_player_state == PLAYER_INACTIVE) {
                // Инициализация Bluetooth плеера, если он был неактивен
                //init_bt_player();
                ESP_LOGI(TAG, "Initializing Bluetooth player");
                bt_player_state = PLAYER_ACTIVE;
            }
            //bt_player(&xResiveGUItoBoombox); // Запуск Bluetooth плеера
        } else if (g_current_source == SOURCE_HTTP) {
            ESP_LOGI(TAG, "Switching to HTTP player");
            if(air_player_state == PLAYER_ACTIVE) {
                boombox_config_save_to_nvs(&xBoomBox_config); // Сохраняем конфигурацию перед отключением AIR плеера
                air_player_state = PLAYER_INACTIVE;// Остановить AIR плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!!
                ESP_LOGI(TAG, "air_player_state = PLAYER_INACTIVE;");
                deinit_air_player(); // Функция деинициализации AIR плеера
            }
            else if(bt_player_state == PLAYER_ACTIVE) {
                // Инициализация Bluetooth плеера, если он был неактивен
                bt_player_state = PLAYER_INACTIVE;
                ESP_LOGI(TAG, "Initializing Bluetooth player");
                // deinit_bt_player(); // Функция деинициализации Bluetooth плеера
            }
            else if(http_player_state == PLAYER_INACTIVE) {
                // Остановить HTTP плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!
                http_player_state = PLAYER_ACTIVE;
                ESP_LOGI(TAG, "http_player_state = PLAYER_ACTIVE;");
                // init_http_player(); // Функция деинициализации HTTP плеера
            }
            //http_player(); // Запуск HTTP плеера
        } else if (g_current_source == SOURCE_AIR) {
            //ESP_LOGI(TAG, "Switching to AIR radio player");
            if(bt_player_state == PLAYER_ACTIVE) {
                // Остановить Bluetooth плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!
                bt_player_state = PLAYER_INACTIVE;
                ESP_LOGI(TAG, "bt_player_state = PLAYER_INACTIVE;");
                // deinit_bt_player(); // Функция деинициализации Bluetooth плеера
            }
            else if(http_player_state == PLAYER_ACTIVE) {
                // Остановить AIR плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!
                http_player_state = PLAYER_INACTIVE;
                ESP_LOGI(TAG, "http_player_state = PLAYER_INACTIVE;");
                // deinit_http_player(&xResiveGUItoBoombox); // Функция деинициализации AIR плеера
            }
            else if(air_player_state == PLAYER_INACTIVE) {
                air_player_state = PLAYER_ACTIVE;
                ESP_LOGI(TAG, "Initializing Air player");                
                init_air_player(&xBoomBox_config); // Функция инициализации AIR плеера
            }
            else {
                air_player(&xResiveGUItoBoombox, &xTransmitBoomboxToGUI);// Запуск AIR плеера
            }
        }
        /******************************************************************************************
         *  Отправка данных от Boombox к  GUI очередь xBoomboxToGuiQueue
        ******************************************************************************************/
       if(xTransmitBoomboxToGUI.State == true ){
            if(pdTRUE != xQueueSend(xBoomboxToGuiQueue, &xTransmitBoomboxToGUI, 25 / portTICK_PERIOD_MS))
            {
                ESP_LOGE(TAG, "Error to xBoomboxToGuiQueue"); //??????????????????????
            }
            xTransmitBoomboxToGUI.State = false;
       }
       // После завершения работы плеера можно добавить логику ожидания или переключения
       vTaskDelay(pdMS_TO_TICKS(1000));
    }
}