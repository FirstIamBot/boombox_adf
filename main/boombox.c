#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_netif.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"

#include "boombox.h"
#include "bt_task.h"
#include "http_task.h"
#include "air_task.h"
#include "esp_wifi_manager.h"

#define NVS_NAMESPACE "boombox_cfg"
#define CONFIG_KEY "config"

static const char *TAG = "BOOMBOX_TASK";
static const char *nameBT = "BOOMBOX";

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
    config->air_radio_config.currentVOL = 32;   // 0 = Минимум, 64 = Максимум
    config->air_radio_config.BandWidthFM = 0;     // AUT-0, 110-1
    config->air_radio_config.BandWidthAM = 0;     // 6kHz - 0
    config->air_radio_config.BandWidthSSB = 0;    // 1.2KHz - 0
    config->air_radio_config.currentAGCgain = 10; // 0 = мин. аттенюация (макс. усиление), 36 - макс. аттенюация (мин. усиление)
    config->air_radio_config.onoffAGCgain = 0;    // 0 = AGC включен, 1 = выключен
    config->air_radio_config.rssi_thresh_seek = 15; // Минимальный уровень RSSI для принятия станции
    config->air_radio_config.snr_thresh_seek = 10;   // Минимальный уровень SNR для принятия станции

    config->air_radio_config.air_FM_station.currentStationIndex = 0;
    config->air_radio_config.air_LW_station.currentStationIndex = 0;
    config->air_radio_config.air_MW_station.currentStationIndex = 0;
    config->air_radio_config.air_SW_station.currentStationIndex = 0;

    for(int i = 0; i < MAX_FOUND_STATIONS; i++) {
        config->air_radio_config.air_FM_station.stations[i] = 0; // Инициализация массива FM частот станций
        config->air_radio_config.air_LW_station.stations[i] = 0; // Инициализация массива LW частот станций
        config->air_radio_config.air_MW_station.stations[i] = 0; // Инициализация массива MW частот станций
        config->air_radio_config.air_SW_station.stations[i] = 0; // Инициализация массива SW частот станций
    }
    ESP_LOGI(TAG, "Default configuration initialized");
}

// Функция инициализации Bluetooth и WiFi coexistence
esp_err_t init_bt_wifi_coex(void)
{
    esp_err_t ret = ESP_OK;
    
    ESP_LOGI(TAG, "Initializing Bluetooth and WiFi coexistence");
    
    // Инициализация NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Инициализация сетевого интерфейса
    ESP_ERROR_CHECK(esp_netif_init());
    
    // Инициализация Bluetooth контроллера (если еще не инициализирован)
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE) {
        ESP_LOGI(TAG, "Initializing Bluetooth controller");
        ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
        esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
        ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
        ESP_ERROR_CHECK(esp_bluedroid_init());
        ESP_ERROR_CHECK(esp_bluedroid_enable());
    } else {
        ESP_LOGW(TAG, "Bluetooth controller already initialized");
    }
    // Настройка Bluetooth устройства (предполагается, что контроллер уже инициализирован)
    esp_bt_gap_set_device_name(nameBT);
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    ESP_LOGI(TAG, "Bluetooth and WiFi coexistence initialized successfully");
    return ret;
}

// Функция деинициализации Bluetooth и WiFi coexistence
void deinit_bt_wifi_coex(void)
{
    ESP_LOGI(TAG, "Deinitializing Bluetooth and WiFi coexistence");
    
    // Деинициализация Bluetooth (закомментировано для стабильности)
    // if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_IDLE) {
    //     esp_bluedroid_disable();
    //     esp_bluedroid_deinit();
    //     esp_bt_controller_disable();
    //     esp_bt_controller_deinit();
    //     esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    // }
    
    ESP_LOGI(TAG, "Bluetooth and WiFi coexistence deinitialized");
}

void boombox_task(void *pvParameters)
{
    // Инициализация Bluetooth и WiFi coexistence
    ESP_ERROR_CHECK(init_bt_wifi_coex());

    // Инициализация WiFi Manager
    ESP_LOGI(TAG, "Initializing WiFi Manager...");
    wifi_manager_config_t wifi_config = {
        .auto_reconnect = true,
        .max_retry_per_network = 3,
        .retry_interval_ms = 5000,
        .enable_captive_portal = true,
        .stop_ap_on_connect = true,
        .default_ap = {
            .ssid = "BOOMBOX-Setup",
            .password = "12345678",
            .channel = 1,
            .max_connections = 4,
        },
        .http = {
            .enable = true,
        },
    };
    
    ESP_ERROR_CHECK(wifi_manager_init(&wifi_config));
    ESP_LOGI(TAG, "WiFi Manager initialized");

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
        if(pdTRUE == xQueueReceive(xGuiToBoomboxQueue, &xResiveGUItoBoombox, pdMS_TO_TICKS(100)))
        {
            g_current_source = (audio_source_t)xResiveGUItoBoombox.eModeBoombox;// Обработка полученных данных
        }
        /******************************************************************************************
         * Обработка данных от GUI 
         ******************************************************************************************/
        if (g_current_source == SOURCE_BLUETOOTH) {
            ESP_LOGD(TAG, "Switching to Bluetooth player");
            if(http_player_state == PLAYER_ACTIVE) {
                // Остановить HTTP плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!
                http_player_state = PLAYER_INACTIVE;
                deinit_http_player(); // Функция деинициализации HTTP плеера
            }
            else if(air_player_state == PLAYER_ACTIVE) {
                boombox_config_save_to_nvs(&xBoomBox_config); // Сохраняем конфигурацию перед отключением AIR плеера
                air_player_state = PLAYER_INACTIVE;// Остановить AIR плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!
                ESP_LOGI(TAG, "air_player_state = PLAYER_INACTIVE;");
                deinit_air_player(); // Функция деинициализации AIR плеера
            }
            else if(bt_player_state == PLAYER_INACTIVE) {
                // Инициализация Bluetooth плеера, если он был неактивен
                ESP_LOGI(TAG, "Initializing Bluetooth player");
                bt_player_state = PLAYER_ACTIVE;
                init_bt_player(); // Функция инициализации Bluetooth плеера
            }
            else{
                bt_player_run(&xResiveGUItoBoombox, &xTransmitBoomboxToGUI); // Запуск Bluetooth плеера
            }
        } else if (g_current_source == SOURCE_HTTP) {
            ESP_LOGI(TAG, "Switching to HTTP player");
            if(air_player_state == PLAYER_ACTIVE) {
                boombox_config_save_to_nvs(&xBoomBox_config); // Сохраняем конфигурацию перед отключением AIR плеера
                air_player_state = PLAYER_INACTIVE;// Остановить AIR плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!!
                ESP_LOGI(TAG, "air_player_state = PLAYER_INACTIVE;");
                deinit_air_player(); // Функция деинициализации AIR плеера
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            else if(bt_player_state == PLAYER_ACTIVE) {
                // Инициализация Bluetooth плеера, если он был неактивен
                bt_player_state = PLAYER_INACTIVE;
                ESP_LOGI(TAG, "DeInitializing Bluetooth player");
                deinit_bt_player(); // Функция деинициализации Bluetooth плеера0
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            else if(http_player_state == PLAYER_INACTIVE) {
                // Проверяем доступную память перед инициализацией
                size_t free_heap = esp_get_free_heap_size();
                ESP_LOGI(TAG, "Free heap before HTTP init: %d bytes", free_heap);
                http_player_state = PLAYER_ACTIVE;
                ESP_LOGI(TAG, "http_player_state = PLAYER_ACTIVE;");
                init_http_player( ); // Функция инициализации HTTP плеера
            }
            else{
                ESP_LOGI(TAG, " http_player_run ");
                http_player_run(&xResiveGUItoBoombox, &xTransmitBoomboxToGUI);
            }
        } else if (g_current_source == SOURCE_AIR) {
            //ESP_LOGI(TAG, "Switching to AIR radio player");
            if(bt_player_state == PLAYER_ACTIVE) {
                // Остановить Bluetooth плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!
                bt_player_state = PLAYER_INACTIVE;
                ESP_LOGI(TAG, "bt_player_state = PLAYER_INACTIVE;");
                deinit_bt_player(); // Функция деинициализации Bluetooth плеера
            }
            else if(http_player_state == PLAYER_ACTIVE) {
                // Остановить AIR плеер, если он был активен !!!!!!!!!!!!!!!!!!!!!!!!!!!
                http_player_state = PLAYER_INACTIVE;
                ESP_LOGI(TAG, "http_player_state = PLAYER_INACTIVE;");
                deinit_http_player(); // Функция деинициализации AIR плеера
            }
            else if(air_player_state == PLAYER_INACTIVE) {
                air_player_state = PLAYER_ACTIVE;
                ESP_LOGI(TAG, "Initializing Air player");                
                boombox_config_load_from_nvs(&xBoomBox_config); // Загружаем конфигурацию перед инициализацией AIR плеера
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
       vTaskDelay(pdMS_TO_TICKS(200));
    }
}