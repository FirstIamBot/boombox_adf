
#include "board.h"
#include "bt_task.h"
#include "commons.h"
#include <inttypes.h>  // Для PRIx32
#include <string.h> // Для strlen

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_peripherals.h"
#include "esp_log.h"

#include "i2s_stream.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_idf_version.h"
#include "filter_resample.h"
#include "audio_mem.h"
#include "audio_common.h"

#include "esp_avrc_api.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "a2dp_stream.h"
#include "board_pins_config.h"

#include "bluetooth_service.h"

static const char *TAG = "BLUETOOTH_TASK";

extern audio_pipeline_handle_t pipeline;
extern audio_element_handle_t bt_stream_reader, i2s_stream_writer, selected_decoder;
extern audio_event_iface_msg_t msg;
extern audio_event_iface_handle_t evt;
extern esp_periph_set_handle_t set; 
extern audio_source_t g_current_source;
extern BoomBox_config_t xBoomBox_config; // Глобальная структура конфигурации Boombox  
extern QueueHandle_t xBoomboxToGuiQueue;

esp_periph_handle_t bt_periph = NULL;
static bool g_a2dp_connect_state = false;

// Структура для хранения AVRC метаданных
typedef struct {
    char title[128];
    char artist[128];
    char album[128];
    bool metadata_updated;
} bt_avrc_metadata_t;

static bt_avrc_metadata_t g_avrc_metadata = {0};

// Callback для AVRC событий
static void bt_avrc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
    switch (event) {
        case ESP_AVRC_CT_METADATA_RSP_EVT: {
            uint8_t *attr_text = (uint8_t *)param->meta_rsp.attr_text;
            ESP_LOGD(TAG, "AVRC metadata event, attr_id: 0x%x, attr_text: %s", 
                     param->meta_rsp.attr_id, attr_text);
            
            switch (param->meta_rsp.attr_id) {
                case ESP_AVRC_MD_ATTR_TITLE:
                    snprintf(g_avrc_metadata.title, sizeof(g_avrc_metadata.title), "%s", attr_text);
                    ESP_LOGI(TAG, "AVRC metadata title: %s", g_avrc_metadata.title); 
                    g_avrc_metadata.metadata_updated = true;
                    break;
                case ESP_AVRC_MD_ATTR_ARTIST:
                    snprintf(g_avrc_metadata.artist, sizeof(g_avrc_metadata.artist), "%s", attr_text);
                    ESP_LOGI(TAG, "AVRC metadata artist: %s", g_avrc_metadata.artist);
                    g_avrc_metadata.metadata_updated = true;
                    break;
                case ESP_AVRC_MD_ATTR_ALBUM:
                    snprintf(g_avrc_metadata.album, sizeof(g_avrc_metadata.album), "%s", attr_text);
                    ESP_LOGI(TAG, "AVRC metadata album: %s", g_avrc_metadata.album);
                    g_avrc_metadata.metadata_updated = true;
                    break;
                case ESP_AVRC_MD_ATTR_TRACK_NUM:
                case ESP_AVRC_MD_ATTR_NUM_TRACKS:
                case ESP_AVRC_MD_ATTR_GENRE:
                case ESP_AVRC_MD_ATTR_PLAYING_TIME:
                    ESP_LOGI(TAG, "AVRC attr_id: 0x%x, attr_text: %s", 
                             param->meta_rsp.attr_id, attr_text);
                    break;
                default:
                    break;
            }
            break;
        }
        case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
            uint8_t connected = param->conn_stat.connected;
            ESP_LOGI(TAG, "AVRC connection state: %s", 
                     connected ? "connected" : "disconnected");
            if (connected) {
                // Регистрируем уведомления о событиях
                ESP_LOGI(TAG, "Registering notifications...");
                vTaskDelay(pdMS_TO_TICKS(200));
                esp_avrc_ct_send_register_notification_cmd(1, ESP_AVRC_RN_TRACK_CHANGE, 0);
                vTaskDelay(pdMS_TO_TICKS(100));
                esp_avrc_ct_send_register_notification_cmd(2, ESP_AVRC_RN_PLAY_STATUS_CHANGE, 0);
                
                // Запрашиваем метаданные при подключении
                vTaskDelay(pdMS_TO_TICKS(200));
                ESP_LOGI(TAG, "Requesting metadata...");
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_TITLE);
                vTaskDelay(pdMS_TO_TICKS(200));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_ARTIST);
                vTaskDelay(pdMS_TO_TICKS(200));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_ALBUM);
            } else {
                // Очищаем метаданные при отключении
                memset(&g_avrc_metadata, 0, sizeof(g_avrc_metadata));
            }
            break;
        }
        case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
            ESP_LOGI(TAG, "AVRC passthrough rsp: key_code 0x%x, key_state %d", 
                     param->psth_rsp.key_code, param->psth_rsp.key_state);
            break;
        }
        case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
            ESP_LOGI(TAG, "AVRC event notification: %d", param->change_ntf.event_id);
            // При изменении трека запрашиваем новые метаданные
            if (param->change_ntf.event_id == ESP_AVRC_RN_TRACK_CHANGE) {
                ESP_LOGI(TAG, "Track changed, requesting new metadata...");
                // Перерегистрируем уведомление для следующей смены трека
                esp_avrc_ct_send_register_notification_cmd(1, ESP_AVRC_RN_TRACK_CHANGE, 0);
                vTaskDelay(pdMS_TO_TICKS(200));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_TITLE);
                vTaskDelay(pdMS_TO_TICKS(200));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_ARTIST);
                vTaskDelay(pdMS_TO_TICKS(200));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_ALBUM);
            } else if (param->change_ntf.event_id == ESP_AVRC_RN_PLAY_STATUS_CHANGE) {
                ESP_LOGI(TAG, "Play status changed");
                // Перерегистрируем уведомление
                esp_avrc_ct_send_register_notification_cmd(2, ESP_AVRC_RN_PLAY_STATUS_CHANGE, 0);
            }
            break;
        }
        case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
            ESP_LOGI(TAG, "AVRC remote features: 0x%" PRIx32, param->rmt_feats.feat_mask);
            break;
        }
        case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
            ESP_LOGI(TAG, "AVRC get capabilities response");
            break;
        }
        default:
            ESP_LOGI(TAG, "AVRC event: %d", event);
            break;
    }
}

// Функции для получения метаданных
const char* bt_get_current_title(void) {
    return g_avrc_metadata.title;
}

const char* bt_get_current_artist(void) {
    return g_avrc_metadata.artist;
}

const char* bt_get_current_album(void) {
    return g_avrc_metadata.album;
}

bool bt_metadata_updated(void) {
    if (g_avrc_metadata.metadata_updated) {
        g_avrc_metadata.metadata_updated = false;
        return true;
    }
    return false;
}

static void user_a2dp_sink_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    ESP_LOGI(TAG, "A2DP sink user cb");
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                ESP_LOGI(TAG, "A2DP disconnected");
                g_a2dp_connect_state = false;
            } else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
                ESP_LOGI(TAG, "A2DP connected");
                g_a2dp_connect_state = true;
                // Запрашиваем метаданные после небольшой задержки
                vTaskDelay(pdMS_TO_TICKS(500));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_TITLE);
                vTaskDelay(pdMS_TO_TICKS(100));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_ARTIST);
                vTaskDelay(pdMS_TO_TICKS(100));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_ALBUM);
            }
            break;
                                       
        case ESP_A2D_AUDIO_STATE_EVT:
            if (param->audio_stat.state == ESP_A2D_AUDIO_STATE_STARTED) {
                ESP_LOGI(TAG, "A2DP audio started");
                param->conn_stat.disc_rsn = 0;
                // Запрашиваем метаданные при начале воспроизведения
                vTaskDelay(pdMS_TO_TICKS(300));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_TITLE);
                vTaskDelay(pdMS_TO_TICKS(100));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_ARTIST);
                vTaskDelay(pdMS_TO_TICKS(100));
                esp_avrc_ct_send_metadata_cmd(0, ESP_AVRC_MD_ATTR_ALBUM);
            } else if (param->audio_stat.state == ESP_A2D_AUDIO_STATE_STOPPED) {
                ESP_LOGI(TAG, "A2DP audio stopped");
            }
            else if(param->audio_stat.state == ESP_A2D_AUDIO_STATE_SUSPEND) {
                ESP_LOGI(TAG, "A2DP audio suspend");
            }
            break;
        case ESP_A2D_AUDIO_CFG_EVT: 
            ESP_LOGI(TAG, "A2DP audio cfg event");
            break;
        case ESP_A2D_MEDIA_CTRL_ACK_EVT:
            ESP_LOGI(TAG, "A2DP media ctrl ack event");
            break;
        case ESP_A2D_PROF_STATE_EVT:
            ESP_LOGI(TAG, "A2DP profile state event");
            break;
        case ESP_A2D_SNK_PSC_CFG_EVT:
            ESP_LOGI(TAG, "A2DP sink psc cfg event");
            break;
        case ESP_A2D_SNK_SET_DELAY_VALUE_EVT:
            ESP_LOGI(TAG, "A2DP sink set delay value event");
            break;
        case ESP_A2D_SNK_GET_DELAY_VALUE_EVT:
            ESP_LOGI(TAG, "A2DP sink get delay value event");
            break;
        case ESP_A2D_REPORT_SNK_DELAY_VALUE_EVT:
            ESP_LOGI(TAG, "A2DP report sink delay value event");
            break;
        default: 
            ESP_LOGI(TAG, "User cb A2DP event: %d", event);
            break;
    }
}
// Функция запуска BT проигрывателя 
void init_bt_player( ) {
    // Защита от повторной инициализации - проверяем, не инициализирован ли уже
    if (pipeline != NULL || bt_stream_reader != NULL) {
        ESP_LOGW(TAG, "Bluetooth player already initialized or not fully deinitialized - cleaning up first");
        deinit_bt_player();
        vTaskDelay(pdMS_TO_TICKS(500)); // Даем время на полную очистку
    }
    
    ESP_LOGI(TAG, "[ * ] Bluetooth player started");

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "[ 2 ] Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "[ 3 ] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);

    ESP_LOGI(TAG, "[4] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[4.1] Get Bluetooth stream");
    a2dp_stream_config_t a2dp_config = {
        .type = AUDIO_STREAM_READER,
        //.user_callback = {0},
        .user_callback.user_a2d_cb = user_a2dp_sink_cb,

    };
    bt_stream_reader = a2dp_stream_init(&a2dp_config);

    ESP_LOGI(TAG, "[4.2] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, bt_stream_reader, "bt");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    ESP_LOGI(TAG, "[4.3] Link it together [Bluetooth]-->bt_stream_reader-->i2s_stream_writer-->[codec_chip]");

    const char *link_tag[2] = {"bt", "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 2);

    ESP_LOGI(TAG, "[ 5 ] Initialize peripherals");
    set = NULL; // или сделать для задачи http and bt свою переменную set   
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    set = esp_periph_set_init(&periph_cfg);

    ESP_LOGI(TAG, "[5.2] Create Bluetooth peripheral");
    bt_periph = bt_create_periph();

    ESP_LOGI(TAG, "[5.2.1] Register AVRC callback");
    esp_avrc_ct_init();
    esp_avrc_ct_register_callback(bt_avrc_ct_cb);

    ESP_LOGI(TAG, "[5.3] Start all peripherals");
    esp_periph_start(set, bt_periph);

    ESP_LOGI(TAG, "[ 6 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[6.1] Listening event from all elements of pipeline");   
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[ 7 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);

}
// Функция роботы BT проигрывателя
void bt_player_run(Data_GUI_Boombox_t *xDataGUI,  Data_Boombox_GUI_t *xDataBoomBox) {

    if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) bt_stream_reader && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
        audio_element_info_t music_info = {0};
        audio_element_getinfo(bt_stream_reader, &music_info);

        ESP_LOGI(TAG, "[ * ] Receive music info from Bluetooth, sample_rates=%d, bits=%d, ch=%d",
                music_info.sample_rates, music_info.bits, music_info.channels);

        audio_element_set_music_info(i2s_stream_writer, music_info.sample_rates, music_info.channels, music_info.bits);
        i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
    }

    /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
    if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
        && msg.cmd == AEL_MSG_CMD_REPORT_STATUS && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
        ESP_LOGW(TAG, "[ * ] Stop event received");
    }

    // Проверяем и обновляем метаданные
    if (bt_metadata_updated()) {
        xDataBoomBox->eModeBoombox = eBT;
        xDataBoomBox->eBtDescription.vcTitle = bt_get_current_title();
        xDataBoomBox->eBtDescription.vcArtist = bt_get_current_artist();
        xDataBoomBox->eBtDescription.vcAlbum = bt_get_current_album();
        xDataBoomBox->State = true;
    } else {
        g_avrc_metadata.artist[0] = '\0';
        g_avrc_metadata.title[0] = '\0';
        g_avrc_metadata.album[0] = '\0';
    }
}
// Функция остановки BT проигрывателя 
void deinit_bt_player(){
    
    // Защита от повторного вызова - проверяем, инициализирован ли pipeline
    if (pipeline == NULL || bt_stream_reader == NULL) {
        ESP_LOGW(TAG, "Bluetooth player already deinitialized, skipping");
        return;
    }
    
    ESP_LOGI(TAG, "[ 9 ] DeInit Bluetooth");
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    // Terminate the pipeline before removing the listener 
    audio_pipeline_remove_listener(pipeline);

    // Stop all periph before removing the listener
    if (set != NULL) {
        esp_periph_set_stop_all(set);
        if (evt != NULL) {
            audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt);
        }
    }

    // Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface
    if (evt != NULL) {
        audio_event_iface_destroy(evt);
        evt = NULL;
    }
    
    // Release all resources 
    audio_pipeline_unregister(pipeline, bt_stream_reader);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);

    audio_pipeline_deinit(pipeline);
    pipeline = NULL;
    
    audio_element_deinit(bt_stream_reader);
    bt_stream_reader = NULL;
    
    audio_element_deinit(i2s_stream_writer);
    i2s_stream_writer = NULL;

    if (set != NULL) {
        esp_periph_set_destroy(set);
        set = NULL;
    }
    
    ESP_LOGI(TAG, "Bluetooth completely disabled");    

}
