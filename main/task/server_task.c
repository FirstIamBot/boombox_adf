#include "server_task.h"
#include "commons.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include <string.h>
#include "esp_avrc_api.h"
#include "esp_netif.h"  // Добавьте этот заголовок

static const char *TAG = "WEB_SERVER";

extern QueueHandle_t xBoomboxToGuiQueue;
extern audio_source_t g_current_source;
extern BoomBox_config_t xBoomBox_config;

static httpd_handle_t server = NULL;


// Объявление встроенных файлов из flash
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

// Обработчик главной страницы
static esp_err_t root_handler(httpd_req_t *req)
{
    const size_t index_html_size = (index_html_end - index_html_start);
    
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_size);
    
    ESP_LOGI(TAG, "Served index.html (%d bytes)", index_html_size);
    return ESP_OK;
}

// Обработчик статуса
static esp_err_t status_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();
    
    const char *source_str = "unknown";
    switch(g_current_source) {
        case SOURCE_BLUETOOTH: source_str = "Bluetooth"; break;
        case SOURCE_HTTP: source_str = "HTTP Radio"; break;
        case SOURCE_AIR: source_str = "Air Radio"; break;
        default: source_str = "None"; break;
    }
    
    cJSON_AddStringToObject(root, "source", source_str);
    cJSON_AddNumberToObject(root, "volume", xBoomBox_config.volume);
    
    const char *json_str = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));
    
    free((void *)json_str);
    cJSON_Delete(root);
    return ESP_OK;
}

// Обработчик смены источника
static esp_err_t source_handler(httpd_req_t *req)
{
    char query[100];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        char src[10];
        if (httpd_query_key_value(query, "src", src, sizeof(src)) == ESP_OK) {
            Data_GUI_Boombox_t msg = {0};
            
            if (strcmp(src, "bt") == 0) {
                msg.eModeBoombox = eBT;
                ESP_LOGI(TAG, "Switching to Bluetooth");
            } else if (strcmp(src, "http") == 0) {
                msg.eModeBoombox = eWeb;
                ESP_LOGI(TAG, "Switching to HTTP Radio");
            } else if (strcmp(src, "air") == 0) {
                msg.eModeBoombox = eAir;
                ESP_LOGI(TAG, "Switching to Air Radio");
            } else {
                httpd_resp_send(req, "Invalid source", HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            }
            
            if (xQueueSend(xBoomboxToGuiQueue, &msg, pdMS_TO_TICKS(1000)) != pdPASS) {
                httpd_resp_send(req, "Failed to send command", HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            }
            
            httpd_resp_send(req, "Source changed", HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
    }
    
    httpd_resp_send(req, "Missing parameters", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/** Обработчик управления воспроизведением */
static esp_err_t control_handler(httpd_req_t *req)
{
    char query[100];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        char cmd[10];
        if (httpd_query_key_value(query, "cmd", cmd, sizeof(cmd)) == ESP_OK) {
            Data_GUI_Boombox_t msg = {0};
            msg.eModeBoombox = g_current_source;
            
            // Используем правильное имя поля из структуры
            if (strcmp(cmd, "play") == 0) {
                msg.eDataDescription = ePLAY;  // Изменено с eControlPlayer на eControl
                ESP_LOGI(TAG, "Play command");
            } else if (strcmp(cmd, "pause") == 0) {
                msg.eDataDescription = ePAUSE;  // Изменено с eControlPlayer на eControl
                ESP_LOGI(TAG, "Pause command");
            } else if (strcmp(cmd, "stop") == 0) {
                msg.eDataDescription = eSTOP;  // Изменено с eControlPlayer на eControl
                ESP_LOGI(TAG, "Stop command");
            } else {
                httpd_resp_send(req, "Invalid command", HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            }
            
            if (xQueueSend(xBoomboxToGuiQueue, &msg, pdMS_TO_TICKS(1000)) != pdPASS) {
                httpd_resp_send(req, "Failed to send command", HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            }
            
            httpd_resp_send(req, "Command sent", HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
    }
    
    httpd_resp_send(req, "Missing parameters", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Обработчик управления Bluetooth
static esp_err_t bt_handler(httpd_req_t *req)
{
    char query[100];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        char cmd[10];
        if (httpd_query_key_value(query, "cmd", cmd, sizeof(cmd)) == ESP_OK) {
            if (strcmp(cmd, "next") == 0) {
                esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_FORWARD, ESP_AVRC_PT_CMD_STATE_PRESSED);
                vTaskDelay(pdMS_TO_TICKS(100));
                esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_FORWARD, ESP_AVRC_PT_CMD_STATE_RELEASED);
                httpd_resp_send(req, "Next track", HTTPD_RESP_USE_STRLEN);
            } else if (strcmp(cmd, "prev") == 0) {
                esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_BACKWARD, ESP_AVRC_PT_CMD_STATE_PRESSED);
                vTaskDelay(pdMS_TO_TICKS(100));
                esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_BACKWARD, ESP_AVRC_PT_CMD_STATE_RELEASED);
                httpd_resp_send(req, "Previous track", HTTPD_RESP_USE_STRLEN);
            } else {
                httpd_resp_send(req, "Invalid command", HTTPD_RESP_USE_STRLEN);
            }
            return ESP_OK;
        }
    }
    
    httpd_resp_send(req, "Missing parameters", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Обработчик HTTP URL
static esp_err_t http_url_handler(httpd_req_t *req)
{
    char query[256];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        char url[200];
        if (httpd_query_key_value(query, "url", url, sizeof(url)) == ESP_OK) {
            // Декодируем URL
            // Здесь можно добавить сохранение URL в конфигурацию
            ESP_LOGI(TAG, "HTTP URL set to: %s", url);
            httpd_resp_send(req, "URL set", HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
    }
    
    httpd_resp_send(req, "Missing URL", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Обработчик частоты Air Radio
static esp_err_t air_freq_handler(httpd_req_t *req)
{
    char query[100];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        char freq_str[10];
        if (httpd_query_key_value(query, "freq", freq_str, sizeof(freq_str)) == ESP_OK) {
            float freq = atof(freq_str);
            ESP_LOGI(TAG, "Air Radio frequency set to: %.1f MHz", freq);
            // Здесь можно добавить сохранение частоты в конфигурацию
            httpd_resp_send(req, "Frequency set", HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
    }
    
    httpd_resp_send(req, "Missing frequency", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Инициализация веб-сервера
void init_web_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10;
    config.stack_size = 8192;
    
    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);

    if (httpd_start(&server, &config) == ESP_OK) {
        // Регистрация обработчиков
        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &root);
        
        httpd_uri_t status = {
            .uri = "/api/status",
            .method = HTTP_GET,
            .handler = status_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &status);
        
        httpd_uri_t source = {
            .uri = "/api/source",
            .method = HTTP_GET,
            .handler = source_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &source);
        
        httpd_uri_t control = {
            .uri = "/api/control",
            .method = HTTP_GET,
            .handler = control_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &control);
        
        httpd_uri_t bt = {
            .uri = "/api/bt",
            .method = HTTP_GET,
            .handler = bt_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &bt);
        
        httpd_uri_t http_url = {
            .uri = "/api/http",
            .method = HTTP_GET,
            .handler = http_url_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &http_url);
        
        httpd_uri_t air_freq = {
            .uri = "/api/air",
            .method = HTTP_GET,
            .handler = air_freq_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &air_freq);
        
        ESP_LOGI(TAG, "Web server started successfully");
    } else {
        ESP_LOGE(TAG, "Failed to start web server");
    }
}

// Остановка веб-сервера
void deinit_web_server(void)
{
    if (server) {
        httpd_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "Web server stopped");
    }
}

// Задача веб-сервера
void web_server_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Web server task started");
    
    // Ждем, пока Wi-Fi подключится и получит IP
    ESP_LOGI(TAG, "Waiting for network connection...");
    
    esp_netif_t *netif = NULL;
    esp_netif_ip_info_t ip_info;
    int retry_count = 0;
    const int max_retries = 30; // 30 секунд ожидания
    bool network_ready = false;
    
    while (retry_count < max_retries) {
        netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif == NULL) {
            netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
        }
        
        if (netif != NULL && esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
            if (ip_info.ip.addr != 0) {
                ESP_LOGI(TAG, "========================================");
                ESP_LOGI(TAG, "Network ready!");
                ESP_LOGI(TAG, "IP Address : " IPSTR, IP2STR(&ip_info.ip));
                ESP_LOGI(TAG, "Netmask    : " IPSTR, IP2STR(&ip_info.netmask));
                ESP_LOGI(TAG, "Gateway    : " IPSTR, IP2STR(&ip_info.gw));
                ESP_LOGI(TAG, "========================================");
                network_ready = true;
                break;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        retry_count++;
    }
    
    if (!network_ready) {
        ESP_LOGW(TAG, "Network not ready after %d seconds", max_retries);
    }
    
    init_web_server();
    
    // Выводим IP адрес для доступа к веб-серверу
    if (network_ready) {
        ESP_LOGI(TAG, "========================================");
        ESP_LOGI(TAG, "Web Server is running!");
        ESP_LOGI(TAG, "Open in browser: http://" IPSTR, IP2STR(&ip_info.ip));
        ESP_LOGI(TAG, "========================================");
    }
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        // Здесь можно добавить периодические проверки или обновления
    }
    
    deinit_web_server();
    vTaskDelete(NULL);
}