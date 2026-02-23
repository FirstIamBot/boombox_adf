/**
 * @file esp_wifi_manager_boombox.c
 * @brief Boombox Data API for WiFi Manager
 * 
 * Provides REST API endpoints for accessing boombox status and control data
 */

#include "esp_wifi_manager_priv.h"
#include "esp_log.h"
#include "cJSON.h"
#include "commons.h"
#include "boombox_playlist.h"
#include <string.h>

static const char *TAG = "wifi_mgr_boombox";

// Внешние переменные из boombox.c
extern Data_GUI_Boombox_t xResiveGUItoBoombox;
extern Data_Boombox_GUI_t xTransmitBoomboxToGUI;
extern BoomBox_config_t xBoomBox_config;
extern QueueHandle_t xGuiToBoomboxQueue;

// =============================================================================
// Helper Functions
// =============================================================================

/**
 * @brief Конвертирует ModeBoombox_t в строку
 */
static const char* mode_to_string(ModeBoombox_t mode)
{
    switch (mode) {
        case eAir: return "Air";
        case eBT:  return "Bluetooth";
        case eWeb: return "Web";
        default:   return "Unknown";
    }
}

/**
 * @brief Конвертирует DataDescription_t в строку
 */
static const char* data_description_to_string(DataDescription_t desc)
{
    switch (desc) {
        case ebandIDx:          return "BandIndex";
        case eModIdx:           return "ModulationIndex";
        case eStepFM:           return "StepFM";
        case eStepAM:           return "StepAM";
        case eBandWFM:          return "BandwidthFM";
        case eBandWAM:          return "BandwidthAM";
        case eBandWSSB:         return "BandwidthSSB";
        case eStepUP:           return "StepUp";
        case eStepDown:         return "StepDown";
        case eSeekUP:           return "SeekUp";
        case eStationStepUP:    return "StationStepUp";
        case eStationStepDOWN:  return "StationStepDown";
        case eAGCgain:          return "AGCGain";
        case eSlider_agc:       return "SliderAGC";
        case eslider_vol:       return "Volume";
        case eSetFreq:          return "SetFrequency";
        case ePlayCNTR:         return "PlayControl";
        default:                return "Unknown";
    }
}

/**
 * @brief Добавляет CORS заголовки
 */
static void add_cors_headers(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
}

/**
 * @brief Отправляет JSON ответ
 */
static esp_err_t send_json_response(httpd_req_t *req, cJSON *json)
{
    char *str = cJSON_PrintUnformatted(json);
    if (!str) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    add_cors_headers(req);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, str, strlen(str));
    free(str);
    return ESP_OK;
}

/**
 * @brief Отправляет сообщение об ошибке
 */
static esp_err_t send_error(httpd_req_t *req, int code, const char *msg)
{
    const char *status;
    switch (code) {
        case 400: status = "400 Bad Request"; break;
        case 404: status = "404 Not Found"; break;
        case 500: status = "500 Internal Server Error"; break;
        default:  status = "400 Bad Request"; break;
    }
    add_cors_headers(req);
    httpd_resp_set_status(req, status);
    httpd_resp_set_type(req, "application/json");

    char buf[128];
    snprintf(buf, sizeof(buf), "{\"error\":\"%s\"}", msg);
    httpd_resp_sendstr(req, buf);
    return ESP_FAIL;
}

/**
 * @brief Читает JSON из тела запроса
 */
static cJSON *read_json_body(httpd_req_t *req)
{
    int content_len = req->content_len;
    if (content_len <= 0 || content_len > 2048) {
        return NULL;
    }
    
    char *buf = malloc(content_len + 1);
    if (!buf) return NULL;
    
    int ret = httpd_req_recv(req, buf, content_len);
    if (ret <= 0) {
        free(buf);
        return NULL;
    }
    buf[ret] = '\0';
    
    cJSON *json = cJSON_Parse(buf);
    free(buf);
    return json;
}

// =============================================================================
// HTTP Handlers
// =============================================================================

/**
 * @brief GET /api/boombox/status - Получить текущее состояние boombox
 */
static esp_err_t handler_get_boombox_status(httpd_req_t *req)
{
    ESP_LOGI(TAG, "GET /api/boombox/status");

    cJSON *json = cJSON_CreateObject();
    
    // Данные от GUI к Boombox (последние полученные команды)
    cJSON *gui_data = cJSON_CreateObject();
    cJSON_AddBoolToObject(gui_data, "hasChanges", xResiveGUItoBoombox.State);
    cJSON_AddStringToObject(gui_data, "mode", mode_to_string(xResiveGUItoBoombox.eModeBoombox));
    cJSON_AddStringToObject(gui_data, "control", data_description_to_string(xResiveGUItoBoombox.eDataDescription));
    cJSON_AddNumberToObject(gui_data, "value", xResiveGUItoBoombox.ucValue);
    cJSON_AddItemToObject(json, "lastCommand", gui_data);

    // Данные от Boombox к GUI (текущее состояние плеера)
    cJSON *status_data = cJSON_CreateObject();
    cJSON_AddBoolToObject(status_data, "hasUpdate", xTransmitBoomboxToGUI.State);
    cJSON_AddStringToObject(status_data, "mode", mode_to_string(xTransmitBoomboxToGUI.eModeBoombox));
    
    // AIR Radio данные
    if (xTransmitBoomboxToGUI.eModeBoombox == eAir) {
        cJSON *air = cJSON_CreateObject();
        if (xTransmitBoomboxToGUI.eAirDescription.vcBand) {
            cJSON_AddStringToObject(air, "band", xTransmitBoomboxToGUI.eAirDescription.vcBand);
        }
        cJSON_AddNumberToObject(air, "stationIndex", xTransmitBoomboxToGUI.eAirDescription.ucStationIDx);
        cJSON_AddNumberToObject(air, "frequency", xTransmitBoomboxToGUI.eAirDescription.ucFreq);
        cJSON_AddNumberToObject(air, "snr", xTransmitBoomboxToGUI.eAirDescription.ucSNR);
        cJSON_AddNumberToObject(air, "rssi", xTransmitBoomboxToGUI.eAirDescription.ucRSSI);
        cJSON_AddNumberToObject(air, "volume", xTransmitBoomboxToGUI.eAirDescription.ucslider_vol);
        if (xTransmitBoomboxToGUI.eAirDescription.vcFreqRange) {
            cJSON_AddStringToObject(air, "freqRange", xTransmitBoomboxToGUI.eAirDescription.vcFreqRange);
        }
        if (xTransmitBoomboxToGUI.eAirDescription.vcStereoMono) {
            cJSON_AddStringToObject(air, "stereoMono", xTransmitBoomboxToGUI.eAirDescription.vcStereoMono);
        }
        if (xTransmitBoomboxToGUI.eAirDescription.vcBW) {
            cJSON_AddStringToObject(air, "bandwidth", xTransmitBoomboxToGUI.eAirDescription.vcBW);
        }
        if (xTransmitBoomboxToGUI.eAirDescription.vcStep) {
            cJSON_AddStringToObject(air, "step", xTransmitBoomboxToGUI.eAirDescription.vcStep);
        }
        if (xTransmitBoomboxToGUI.eAirDescription.vcRDSdata) {
            cJSON_AddStringToObject(air, "rds", xTransmitBoomboxToGUI.eAirDescription.vcRDSdata);
        }
        cJSON_AddItemToObject(status_data, "air", air);
    }
    
    // Web Radio данные
    if (xTransmitBoomboxToGUI.eModeBoombox == eWeb) {
        cJSON *web = cJSON_CreateObject();
        if (xTransmitBoomboxToGUI.eWebDescription.vcURIStation) {
            cJSON_AddStringToObject(web, "uri", xTransmitBoomboxToGUI.eWebDescription.vcURIStation);
        }
        if (xTransmitBoomboxToGUI.eWebDescription.vcStation) {
            cJSON_AddStringToObject(web, "station", xTransmitBoomboxToGUI.eWebDescription.vcStation);
        }
        cJSON_AddNumberToObject(web, "stationIndex", xTransmitBoomboxToGUI.eWebDescription.ucStationIDx);
        if (xTransmitBoomboxToGUI.eWebDescription.vcTitle) {
            cJSON_AddStringToObject(web, "title", xTransmitBoomboxToGUI.eWebDescription.vcTitle);
        }
        if (xTransmitBoomboxToGUI.eWebDescription.vcArtist) {
            cJSON_AddStringToObject(web, "artist", xTransmitBoomboxToGUI.eWebDescription.vcArtist);
        }
        if (xTransmitBoomboxToGUI.eWebDescription.vcAlbum) {
            cJSON_AddStringToObject(web, "album", xTransmitBoomboxToGUI.eWebDescription.vcAlbum);
        }
        cJSON_AddItemToObject(status_data, "web", web);
    }
    
    // Bluetooth данные
    if (xTransmitBoomboxToGUI.eModeBoombox == eBT) {
        cJSON *bt = cJSON_CreateObject();
        if (xTransmitBoomboxToGUI.eBtDescription.vcTitle) {
            cJSON_AddStringToObject(bt, "title", xTransmitBoomboxToGUI.eBtDescription.vcTitle);
        }
        if (xTransmitBoomboxToGUI.eBtDescription.vcArtist) {
            cJSON_AddStringToObject(bt, "artist", xTransmitBoomboxToGUI.eBtDescription.vcArtist);
        }
        if (xTransmitBoomboxToGUI.eBtDescription.vcAlbum) {
            cJSON_AddStringToObject(bt, "album", xTransmitBoomboxToGUI.eBtDescription.vcAlbum);
        }
        cJSON_AddItemToObject(status_data, "bluetooth", bt);
    }
    
    cJSON_AddItemToObject(json, "currentStatus", status_data);

    esp_err_t ret = send_json_response(req, json);
    cJSON_Delete(json);
    return ret;
}

/**
 * @brief GET /api/boombox/config - Получить конфигурацию boombox
 */
static esp_err_t handler_get_boombox_config(httpd_req_t *req)
{
    ESP_LOGI(TAG, "GET /api/boombox/config");

    cJSON *json = cJSON_CreateObject();
    
    cJSON_AddStringToObject(json, "mode", mode_to_string(xBoomBox_config.eModeBoombox));
    cJSON_AddNumberToObject(json, "currentSource", xBoomBox_config.current_source);
    cJSON_AddNumberToObject(json, "volume", xBoomBox_config.Volume);

    // AIR Radio конфигурация
    cJSON *air_config = cJSON_CreateObject();
    cJSON_AddNumberToObject(air_config, "bandType", xBoomBox_config.air_radio_config.currentBandType);
    cJSON_AddNumberToObject(air_config, "modulation", xBoomBox_config.air_radio_config.currentMod);
    cJSON_AddNumberToObject(air_config, "stepFM", xBoomBox_config.air_radio_config.currentStepFM);
    cJSON_AddNumberToObject(air_config, "stepAM", xBoomBox_config.air_radio_config.currentStepAM);
    cJSON_AddNumberToObject(air_config, "frequency", xBoomBox_config.air_radio_config.currentFrequency);
    cJSON_AddNumberToObject(air_config, "volume", xBoomBox_config.air_radio_config.currentVOL);
    cJSON_AddNumberToObject(air_config, "bandwidthFM", xBoomBox_config.air_radio_config.BandWidthFM);
    cJSON_AddNumberToObject(air_config, "bandwidthAM", xBoomBox_config.air_radio_config.BandWidthAM);
    cJSON_AddNumberToObject(air_config, "bandwidthSSB", xBoomBox_config.air_radio_config.BandWidthSSB);
    cJSON_AddNumberToObject(air_config, "agcGain", xBoomBox_config.air_radio_config.currentAGCgain);
    cJSON_AddNumberToObject(air_config, "agcEnabled", xBoomBox_config.air_radio_config.onoffAGCgain == 0 ? 1 : 0);
    cJSON_AddNumberToObject(air_config, "rssiThreshold", xBoomBox_config.air_radio_config.rssi_thresh_seek);
    cJSON_AddNumberToObject(air_config, "snrThreshold", xBoomBox_config.air_radio_config.snr_thresh_seek);
    
    // FM станции
    cJSON *fm_stations = cJSON_CreateArray();
    for (int i = 0; i < MAX_FOUND_STATIONS; i++) {
        if (xBoomBox_config.air_radio_config.air_FM_station.stations[i] > 0) {
            cJSON_AddItemToArray(fm_stations, cJSON_CreateNumber(xBoomBox_config.air_radio_config.air_FM_station.stations[i]));
        }
    }
    cJSON_AddItemToObject(air_config, "fmStations", fm_stations);
    cJSON_AddNumberToObject(air_config, "currentFMStation", xBoomBox_config.air_radio_config.air_FM_station.currentStationIndex);
    
    cJSON_AddItemToObject(json, "airConfig", air_config);

    esp_err_t ret = send_json_response(req, json);
    cJSON_Delete(json);
    return ret;
}

/**
 * @brief POST /api/boombox/control - Отправить команду управления
 */
static esp_err_t handler_post_boombox_control(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST /api/boombox/control");

    cJSON *json = read_json_body(req);
    if (!json) {
        return send_error(req, 400, "Invalid JSON");
    }

    Data_GUI_Boombox_t command = {0};
    command.State = true;

    // Парсинг команды
    cJSON *mode = cJSON_GetObjectItem(json, "mode");
    if (mode && cJSON_IsString(mode)) {
        const char *mode_str = mode->valuestring;
        if (strcmp(mode_str, "Air") == 0) {
            command.eModeBoombox = eAir;
        } else if (strcmp(mode_str, "Bluetooth") == 0 || strcmp(mode_str, "BT") == 0) {
            command.eModeBoombox = eBT;
        } else if (strcmp(mode_str, "Web") == 0) {
            command.eModeBoombox = eWeb;
        }
    }

    cJSON *control = cJSON_GetObjectItem(json, "control");
    if (control && cJSON_IsNumber(control)) {
        command.eDataDescription = (DataDescription_t)control->valueint;
    }

    cJSON *value = cJSON_GetObjectItem(json, "value");
    if (value && cJSON_IsNumber(value)) {
        command.ucValue = value->valueint;
    }

    cJSON_Delete(json);

    // Отправка команды в очередь
    if (xGuiToBoomboxQueue != NULL) {
        if (xQueueSend(xGuiToBoomboxQueue, &command, pdMS_TO_TICKS(100)) == pdTRUE) {
            ESP_LOGI(TAG, "Command sent to boombox queue: mode=%d, control=%d, value=%d",
                     command.eModeBoombox, command.eDataDescription, command.ucValue);
            
            cJSON *response = cJSON_CreateObject();
            cJSON_AddStringToObject(response, "status", "ok");
            cJSON_AddStringToObject(response, "message", "Command sent successfully");
            esp_err_t ret = send_json_response(req, response);
            cJSON_Delete(response);
            return ret;
        } else {
            ESP_LOGE(TAG, "Failed to send command to queue");
            return send_error(req, 500, "Queue send failed");
        }
    } else {
        ESP_LOGE(TAG, "Queue not initialized");
        return send_error(req, 500, "Queue not initialized");
    }
}

/**
 * @brief OPTIONS handler для CORS preflight
 */
static esp_err_t handler_options(httpd_req_t *req)
{
    add_cors_headers(req);
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

/**
 * @brief GET /api/boombox/playlist - Получить список радиостанций
 */
static esp_err_t handler_get_playlist(httpd_req_t *req)
{
    ESP_LOGI(TAG, "GET /api/boombox/playlist");

    int count = playlist_get_count();
    
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "count", count);
    
    cJSON *stations = cJSON_CreateArray();
    for (int i = 0; i < count; i++) {
        cJSON *station = cJSON_CreateObject();
        cJSON_AddNumberToObject(station, "index", i);
        
        const char *title = playlist_get_title(i);
        if (title) {
            cJSON_AddStringToObject(station, "title", title);
        }
        
        const char *url = playlist_get_url(i);
        if (url) {
            cJSON_AddStringToObject(station, "url", url);
        }
        
        cJSON_AddItemToArray(stations, station);
    }
    
    cJSON_AddItemToObject(json, "stations", stations);

    esp_err_t ret = send_json_response(req, json);
    cJSON_Delete(json);
    return ret;
}

/**
 * @brief POST /api/boombox/playlist/select - Выбрать станцию для воспроизведения
 */
static esp_err_t handler_select_station(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST /api/boombox/playlist/select");

    cJSON *json = read_json_body(req);
    if (!json) {
        return send_error(req, 400, "Invalid JSON");
    }

    cJSON *index_item = cJSON_GetObjectItem(json, "index");
    if (!index_item || !cJSON_IsNumber(index_item)) {
        cJSON_Delete(json);
        return send_error(req, 400, "Missing or invalid 'index' field");
    }

    int index = index_item->valueint;
    cJSON_Delete(json);

    // Отправка команды переключения станции
    // Используем ePlayCNTR с index + 100 для прямого выбора станции
    Data_GUI_Boombox_t command = {0};
    command.State = true;
    command.eModeBoombox = eWeb;
    command.eDataDescription = ePlayCNTR;
    command.ucValue = index + 100;  // +100 to distinguish from regular commands (0-4)

    if (xGuiToBoomboxQueue != NULL) {
        if (xQueueSend(xGuiToBoomboxQueue, &command, pdMS_TO_TICKS(100)) == pdTRUE) {
            cJSON *response = cJSON_CreateObject();
            cJSON_AddStringToObject(response, "status", "ok");
            cJSON_AddStringToObject(response, "message", "Station selected");
            cJSON_AddNumberToObject(response, "index", index);
            esp_err_t ret = send_json_response(req, response);
            cJSON_Delete(response);
            return ret;
        } else {
            return send_error(req, 500, "Queue send failed");
        }
    }
    
    return send_error(req, 500, "Queue not initialized");
}

/**
 * @brief POST /api/boombox/playlist/add - Добавить станцию в плейлист
 * Body: { "title": "Station title", "url": "http://..." }
 */
static esp_err_t handler_add_station(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST /api/boombox/playlist/add");
    cJSON *json = read_json_body(req);
    if (!json) return send_error(req, 400, "Invalid JSON");

    cJSON *title = cJSON_GetObjectItem(json, "title");
    cJSON *url = cJSON_GetObjectItem(json, "url");
    if (!title || !cJSON_IsString(title) || !url || !cJSON_IsString(url)) {
        cJSON_Delete(json);
        return send_error(req, 400, "Missing 'title' or 'url'");
    }

    int res = playlist_add_entry(title->valuestring, url->valuestring);
    cJSON_Delete(json);
    if (res != 0) {
        return send_error(req, 500, "Failed to add station");
    }

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddStringToObject(resp, "status", "ok");
    cJSON_AddStringToObject(resp, "message", "Station added");
    esp_err_t ret = send_json_response(req, resp);
    cJSON_Delete(resp);
    return ret;
}


/**
 * @brief POST /api/boombox/playlist/delete - Удалить станцию по индексу
 * Body: { "index": 2 }
 */
static esp_err_t handler_delete_station(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST /api/boombox/playlist/delete");
    cJSON *json = read_json_body(req);
    if (!json) return send_error(req, 400, "Invalid JSON");

    cJSON *index_item = cJSON_GetObjectItem(json, "index");
    if (!index_item || !cJSON_IsNumber(index_item)) {
        cJSON_Delete(json);
        return send_error(req, 400, "Missing or invalid 'index'");
    }

    int index = index_item->valueint;
    cJSON_Delete(json);

    int res = playlist_delete_entry(index);
    if (res != 0) {
        return send_error(req, 500, "Failed to delete station");
    }

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddStringToObject(resp, "status", "ok");
    cJSON_AddStringToObject(resp, "message", "Station deleted");
    cJSON_AddNumberToObject(resp, "index", index);
    esp_err_t ret = send_json_response(req, resp);
    cJSON_Delete(resp);
    return ret;
}

/**
 * @brief POST /api/boombox/playlist/update - Обновить запись
 * Body: { "index": 0, "title": "New title", "url": "http://..." }
 */
static esp_err_t handler_update_station(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST /api/boombox/playlist/update");
    cJSON *json = read_json_body(req);
    if (!json) return send_error(req, 400, "Invalid JSON");

    cJSON *index_item = cJSON_GetObjectItem(json, "index");
    if (!index_item || !cJSON_IsNumber(index_item)) {
        cJSON_Delete(json);
        return send_error(req, 400, "Missing or invalid 'index'");
    }
    int index = index_item->valueint;

    cJSON *title = cJSON_GetObjectItem(json, "title");
    cJSON *url = cJSON_GetObjectItem(json, "url");

    const char *title_str = (title && cJSON_IsString(title)) ? title->valuestring : NULL;
    const char *url_str = (url && cJSON_IsString(url)) ? url->valuestring : NULL;

    cJSON_Delete(json);

    int res = playlist_update_entry(index, title_str, url_str);
    if (res != 0) return send_error(req, 500, "Failed to update station");

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddStringToObject(resp, "status", "ok");
    cJSON_AddStringToObject(resp, "message", "Station updated");
    cJSON_AddNumberToObject(resp, "index", index);
    esp_err_t ret = send_json_response(req, resp);
    cJSON_Delete(resp);
    return ret;
}


/**
 * @brief POST /api/boombox/playlist/move - Переместить запись
 * Body: { "from": 2, "to": 0 }
 */
static esp_err_t handler_move_station(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST /api/boombox/playlist/move");
    cJSON *json = read_json_body(req);
    if (!json) return send_error(req, 400, "Invalid JSON");

    cJSON *from_item = cJSON_GetObjectItem(json, "from");
    cJSON *to_item = cJSON_GetObjectItem(json, "to");
    if (!from_item || !cJSON_IsNumber(from_item) || !to_item || !cJSON_IsNumber(to_item)) {
        cJSON_Delete(json);
        return send_error(req, 400, "Missing or invalid 'from'/'to'");
    }
    int from = from_item->valueint;
    int to = to_item->valueint;
    cJSON_Delete(json);

    int res = playlist_move_entry(from, to);
    if (res != 0) return send_error(req, 500, "Failed to move station");

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddStringToObject(resp, "status", "ok");
    cJSON_AddStringToObject(resp, "message", "Station moved");
    cJSON_AddNumberToObject(resp, "from", from);
    cJSON_AddNumberToObject(resp, "to", to);
    esp_err_t ret = send_json_response(req, resp);
    cJSON_Delete(resp);
    return ret;
}

// =============================================================================
// Public API
// =============================================================================

/**
 * @brief Регистрирует HTTP handlers для boombox API
 */
esp_err_t wifi_manager_register_boombox_handlers(httpd_handle_t server)
{
    if (!server) {
        ESP_LOGE(TAG, "Invalid server handle");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Registering boombox HTTP handlers");

    esp_err_t ret;

    // GET /api/boombox/status
    httpd_uri_t uri_get_status = {
        .uri       = "/api/boombox/status",
        .method    = HTTP_GET,
        .handler   = handler_get_boombox_status,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_get_status);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register GET /api/boombox/status: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "Registered: GET /api/boombox/status");

    // GET /api/boombox/config
    httpd_uri_t uri_get_config = {
        .uri       = "/api/boombox/config",
        .method    = HTTP_GET,
        .handler   = handler_get_boombox_config,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_get_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register GET /api/boombox/config: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "Registered: GET /api/boombox/config");

    // POST /api/boombox/control
    httpd_uri_t uri_post_control = {
        .uri       = "/api/boombox/control",
        .method    = HTTP_POST,
        .handler   = handler_post_boombox_control,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_post_control);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register POST /api/boombox/control: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "Registered: POST /api/boombox/control");

    // OPTIONS handlers для CORS
    httpd_uri_t uri_options_status = {
        .uri       = "/api/boombox/status",
        .method    = HTTP_OPTIONS,
        .handler   = handler_options,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_options_status);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to register OPTIONS /api/boombox/status: %s", esp_err_to_name(ret));
    }

    httpd_uri_t uri_options_config = {
        .uri       = "/api/boombox/config",
        .method    = HTTP_OPTIONS,
        .handler   = handler_options,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_options_config);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to register OPTIONS /api/boombox/config: %s", esp_err_to_name(ret));
    }

    httpd_uri_t uri_options_control = {
        .uri       = "/api/boombox/control",
        .method    = HTTP_OPTIONS,
        .handler   = handler_options,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_options_control);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to register OPTIONS /api/boombox/control: %s", esp_err_to_name(ret));
    }

    // GET /api/boombox/playlist
    httpd_uri_t uri_get_playlist = {
        .uri       = "/api/boombox/playlist",
        .method    = HTTP_GET,
        .handler   = handler_get_playlist,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_get_playlist);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register GET /api/boombox/playlist: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Registered: GET /api/boombox/playlist");
    }

    // POST /api/boombox/playlist/select
    httpd_uri_t uri_select_station = {
        .uri       = "/api/boombox/playlist/select",
        .method    = HTTP_POST,
        .handler   = handler_select_station,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_select_station);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register POST /api/boombox/playlist/select: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Registered: POST /api/boombox/playlist/select");
    }

    // POST /api/boombox/playlist/add
    httpd_uri_t uri_add_station = {
        .uri       = "/api/boombox/playlist/add",
        .method    = HTTP_POST,
        .handler   = handler_add_station,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_add_station);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register POST /api/boombox/playlist/add: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Registered: POST /api/boombox/playlist/add");
    }

    // POST /api/boombox/playlist/delete
    httpd_uri_t uri_delete_station = {
        .uri       = "/api/boombox/playlist/delete",
        .method    = HTTP_POST,
        .handler   = handler_delete_station,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_delete_station);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register POST /api/boombox/playlist/delete: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Registered: POST /api/boombox/playlist/delete");
    }

    // POST /api/boombox/playlist/update
    httpd_uri_t uri_update_station = {
        .uri       = "/api/boombox/playlist/update",
        .method    = HTTP_POST,
        .handler   = handler_update_station,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_update_station);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register POST /api/boombox/playlist/update: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Registered: POST /api/boombox/playlist/update");
    }

    // POST /api/boombox/playlist/move
    httpd_uri_t uri_move_station = {
        .uri       = "/api/boombox/playlist/move",
        .method    = HTTP_POST,
        .handler   = handler_move_station,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &uri_move_station);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register POST /api/boombox/playlist/move: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Registered: POST /api/boombox/playlist/move");
    }

    ESP_LOGI(TAG, "Boombox HTTP handlers registered successfully");
    return ESP_OK;
}
