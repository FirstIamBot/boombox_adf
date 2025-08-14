

#include "board.h"
#include "bt_task.h"
#include "audio_equalizer.h"

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

static const char *TAG = "BLUETOOTH_TASK";
static const char *nameBT = "BOOMBOX";

esp_periph_handle_t bt_periph = NULL;

extern audio_source_t g_current_source;

void bt_player()
{
    ESP_LOGI(TAG, "[ * ] Bluetooth player started");
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t bt_stream_reader, i2s_stream_writer;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "[ 1 ] Init Bluetooth");
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    esp_bt_gap_set_device_name(nameBT);

    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

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
        .user_callback = {0},

    };
    bt_stream_reader = a2dp_stream_init(&a2dp_config);

    ESP_LOGI(TAG, "[4.2] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, bt_stream_reader, "bt");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    ESP_LOGI(TAG, "[4.3] Link it together [Bluetooth]-->bt_stream_reader-->i2s_stream_writer-->[codec_chip]");

    const char *link_tag[2] = {"bt", "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 2);


    ESP_LOGI(TAG, "[ 5 ] Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    //audio_board_key_init(set);

    ESP_LOGI(TAG, "[ 5.1 ] Create and start input key service");
    //input_key_service_info_t input_key_info[] = INPUT_KEY_DEFAULT_INFO();
    //input_key_service_cfg_t input_cfg = INPUT_KEY_SERVICE_DEFAULT_CONFIG();
    //input_cfg.handle = set;
    //periph_service_handle_t input_ser = input_key_service_create(&input_cfg);
    //input_key_service_add_key(input_ser, input_key_info, INPUT_KEY_NUM);
    //periph_service_set_callback(input_ser, input_key_service_cb, NULL);

    ESP_LOGI(TAG, "[5.2] Create Bluetooth peripheral");
    bt_periph = bt_create_periph();

    ESP_LOGI(TAG, "[5.3] Start all peripherals");
    esp_periph_start(set, bt_periph);

    ESP_LOGI(TAG, "[ 6 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[6.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[ 7 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);

    ESP_LOGI(TAG, "[ 7.1 ] Starting Audio Equalizer for Bluetooth");
    TaskHandle_t equalizer_handle = NULL;
    audio_equalizer_config_t eq_config = AUDIO_EQUALIZER_DEFAULT_CONFIG();
    eq_config.i2s_element = i2s_stream_writer;
    eq_config.num_bands = 20;
    eq_config.display_height = 18;
    eq_config.display_width = 2;
    eq_config.update_interval_ms = 80;
    eq_config.enable_peak_hold = true;
    eq_config.show_frequency_labels = true;
    
    esp_err_t eq_err = audio_equalizer_start(&eq_config, &equalizer_handle);
    if (eq_err == ESP_OK) {
        ESP_LOGI(TAG, "Audio equalizer started successfully");
    } else {
        ESP_LOGE(TAG, "Failed to start audio equalizer: %d", eq_err);
        equalizer_handle = NULL;
    }

    ESP_LOGI(TAG, "[ 8 ] Listen for all pipeline events");
    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) bt_stream_reader && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {0};
            audio_element_getinfo(bt_stream_reader, &music_info);

            ESP_LOGI(TAG, "[ * ] Receive music info from Bluetooth, sample_rates=%d, bits=%d, ch=%d",
                     music_info.sample_rates, music_info.bits, music_info.channels);

            audio_element_set_music_info(i2s_stream_writer, music_info.sample_rates, music_info.channels, music_info.bits);

            i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);

            continue;
        }

        /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(TAG, "[ * ] Stop event received");
            break;
        }
    }

    ESP_LOGI(TAG, "[ 9 ] Stop audio_pipeline");
    
    // Останавливаем эквалайзер
    if (equalizer_handle) {
        ESP_LOGI(TAG, "[ 9.1 ] Stopping audio equalizer");
        audio_equalizer_stop(equalizer_handle);
    }
    
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    /* Terminate the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);

    /* Stop all periph before removing the listener */
    esp_periph_set_stop_all(set);
    audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    audio_pipeline_unregister(pipeline, bt_stream_reader);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);

    audio_pipeline_deinit(pipeline);
    audio_element_deinit(bt_stream_reader);
    audio_element_deinit(i2s_stream_writer);
    //periph_service_destroy(input_ser);
    esp_periph_set_destroy(set);
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
}

void bt_player_task(void *pvParameters)
{
    while (1) {
        if (g_current_source == SOURCE_BLUETOOTH) {
            bt_player();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
