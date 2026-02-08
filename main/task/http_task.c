
#include "board.h"
#include "http_task.h"
#include "commons.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "i2s_stream.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_idf_version.h"
#include "filter_resample.h"
#include "audio_mem.h"
#include "audio_common.h"

#include "http_stream.h"
#include "mp3_decoder.h"
#include "periph_wifi.h"

#include "playlist_parser.h"

// WiFi Manager для проверки подключения
//#include "esp_wifi_manager.h"

static const char *TAG = "HTTP_PLAYER";

extern audio_pipeline_handle_t pipeline;
extern audio_element_handle_t http_stream_reader, i2s_stream_writer, selected_decoder;
extern audio_event_iface_msg_t msg;
extern audio_event_iface_handle_t evt;
extern esp_periph_set_handle_t set; 
extern audio_source_t g_current_source;
extern BoomBox_config_t xBoomBox_config; // Глобальная структура конфигурации Boombox  
//*********************************************************************************************************************
//#define SELECT_AAC_DECODER 1
#define SELECT_MP3_DECODER 1

#if defined SELECT_AAC_DECODER
#include "aac_decoder.h"
static const char *selected_decoder_name = "aac";
static const char *selected_file_to_play = "https://dl.espressif.com/dl/audio/ff-16b-2c-44100hz.aac";
#elif defined SELECT_AMR_DECODER
#include "amr_decoder.h"
static const char *selected_decoder_name = "amr";
static const char *selected_file_to_play = "https://dl.espressif.com/dl/audio/ff-16b-1c-8000hz.amr";
#elif defined SELECT_FLAC_DECODER
#include "flac_decoder.h"
static const char *selected_decoder_name = "flac";
static const char *selected_file_to_play = "https://dl.espressif.com/dl/audio/ff-16b-2c-44100hz.flac";
#elif defined SELECT_MP3_DECODER

static uint8_t countStation;
static uint8_t curStation = 0;
static const char *selected_decoder_name = "mp3";
static const char *selected_file_to_play = "http://online.radioroks.ua/RadioROKS";
#elif defined SELECT_OGG_DECODER
#include "ogg_decoder.h"
static const char *selected_decoder_name = "ogg";
static const char *selected_file_to_play = "https://dl.espressif.com/dl/audio/ff-16b-2c-44100hz.ogg";
#elif defined SELECT_OPUS_DECODER
#include "opus_decoder.h"
static const char *selected_decoder_name = "opus";
static const char *selected_file_to_play = "https://dl.espressif.com/dl/audio/ff-16b-2c-44100hz.opus";
#else
#include "wav_decoder.h"
static const char *selected_decoder_name = "wav";
static const char *selected_file_to_play = "https://dl.espressif.com/dl/audio/ff-16b-2c-44100hz.wav";
#endif
//*********************************************************************************************************************

// Функция для вывода данных из GUI в консоль
void log_gui_control_data(const Data_GUI_Boombox_t *xDataBoomBox) {
    if (xDataBoomBox == NULL) {
        ESP_LOGW(TAG, "[ GUI ] Received NULL pointer from GUI");
        return;
    }

    ESP_LOGI(TAG, "╔════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║     Data from  WEB GUI Boombox         ║");
    ESP_LOGI(TAG, "╠════════════════════════════════════════╣");
    
    // Вывод описания команды
    // Расшифровка типа данных (если есть enum)
    switch (xDataBoomBox->eDataDescription) {
        case ePlayCNTR:
            ESP_LOGI(TAG, "║   Type: Соntrol Player");
            break;
        default:
            ESP_LOGI(TAG, "║   Type: Unknown (%d)", xDataBoomBox->eDataDescription);
            break;
    }
    
    // Расшифровка типа данных (если есть enum)
    switch (xDataBoomBox->ucValue) {
        case eSTOP:
            ESP_LOGI(TAG, "║   Type: eSTOP          %d               ║",xDataBoomBox->ucValue);
            break;
        case ePLAY:
            ESP_LOGI(TAG, "║   Type: ePLAY          %d               ║",xDataBoomBox->ucValue);
            break;
        case ePAUSE:
            ESP_LOGI(TAG, "║   Type: ePAUSE         %d               ║",xDataBoomBox->ucValue);
            break;
        case eFOWARD:
            ESP_LOGI(TAG, "║   Type: eFOWARD        %d               ║",xDataBoomBox->ucValue);
            break;
        case eNEXT:
            ESP_LOGI(TAG, "║   Type: eNEXT          %d               ║",xDataBoomBox->ucValue);
            break;
        default:
            ESP_LOGI(TAG, "║   Type: Unknown (%d)", xDataBoomBox->ucValue);
            break;
    }

    
    ESP_LOGI(TAG, "╚════════════════════════════════════════╝");
}

// Управление HTTP плеером из GUI и Web сервера
void cntr_http_player(Data_GUI_Boombox_t *set_data) {
     /*      */ 
    // Функция установки параметров проигрывателя HTTP радио
    switch (set_data->eDataDescription) {
        case ePlayCNTR:
            switch (set_data->ucValue) {
                case eSTOP:
                    ESP_LOGI(TAG, "[ HTTP ] Stopping playback as per GUI command");
                    audio_pipeline_stop(pipeline);
                    audio_pipeline_wait_for_stop(pipeline);
                    audio_pipeline_reset_ringbuffer(pipeline);
                    audio_pipeline_reset_elements(pipeline);
                break;
                case ePLAY:
                    ESP_LOGI(TAG, "[ HTTP ] Starting playback as per GUI command");
                    audio_pipeline_run(pipeline);
                break;
                case eNEXT:
                    if(curStation >= countStation - 1) {
                        curStation = 0;
                    }
                    else {
                        curStation++;
                    }
                    selected_file_to_play = playlist_get_url(curStation);
                    audio_element_set_uri(http_stream_reader, selected_file_to_play);
                    ESP_LOGI(TAG, "[ HTTP ] Switching to next station: %d. %s - %s", curStation, playlist_get_title(curStation), playlist_get_url(curStation));
                    audio_pipeline_stop(pipeline);
                    audio_pipeline_wait_for_stop(pipeline);
                    audio_pipeline_reset_ringbuffer(pipeline);
                    audio_pipeline_reset_elements(pipeline);
                    audio_pipeline_run(pipeline);
                    break;
                case eFOWARD:
                    if(curStation == 0) {
                        curStation = countStation - 1;
                    }
                    else {
                        curStation--;
                    }
                    selected_file_to_play = playlist_get_url(curStation);
                    ESP_LOGI(TAG, "[ HTTP ] Switching to foward station: %d. %s - %s", curStation, playlist_get_title(curStation), playlist_get_url(curStation));
                    audio_element_set_uri(http_stream_reader, selected_file_to_play);
                    audio_pipeline_stop(pipeline);
                    audio_pipeline_wait_for_stop(pipeline);
                    audio_pipeline_reset_ringbuffer(pipeline);
                    audio_pipeline_reset_elements(pipeline);
                    audio_pipeline_run(pipeline);
                break;
                default:
                    ESP_LOGW(TAG, "[ HTTP ] Unknown control command from GUI: %d", set_data->ucValue);
                break;
                }
        break;

        default:
            ESP_LOGW(TAG, "[ HTTP ] Unknown data description from GUI: %d", set_data->eDataDescription);
        break;
    }
}
// Возврат данных проигрывателя HTTP радио на GUI  и Web сервер
void get_http_player(Data_Boombox_GUI_t *get_data) {
    /*      */ 
    // Функция получения параметров проигрывателя HTTP радио
    get_data->eModeBoombox = eWeb; // Устанавливаем режим Boombox как HTTP радио
    get_data->eWebDescription.ucStationIDx = curStation+1; // Текущий номер станции +1 для отображения пользователю
    get_data->eWebDescription.vcStation = playlist_get_title(curStation); // Текущее название станции
    get_data->eWebDescription.vcURIStation = playlist_get_url(curStation); // Текущий URI станции
    get_data->State = true; // Устанавливаем состояние данных для GUI
}
// Функция запуска проигрывателя HTTP радио
void init_http_player( ) {
  ESP_LOGI(TAG, "[ * ] HTTP player started");

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    // Проверка доступной памяти перед началом
    size_t free_heap = esp_get_free_heap_size();
    ESP_LOGW(TAG, "Free heap before HTTP init: %d bytes", free_heap);

    ESP_LOGI(TAG, "[ 1 ] Start audio codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "[2.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    ESP_LOGI(TAG, "[2.1] Create http stream to read data");
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.out_rb_size = 1024 * 1024;
    http_stream_reader = http_stream_init(&http_cfg);

    ESP_LOGI(TAG, "[2.2] Create %s decoder to decode %s file", selected_decoder_name, selected_decoder_name);

#if defined SELECT_AAC_DECODER
    aac_decoder_cfg_t aac_cfg = DEFAULT_AAC_DECODER_CONFIG();
    selected_decoder = aac_decoder_init(&aac_cfg);
#elif defined SELECT_AMR_DECODER
    amr_decoder_cfg_t amr_cfg = DEFAULT_AMR_DECODER_CONFIG();
    selected_decoder = amr_decoder_init(&amr_cfg);
#elif defined SELECT_FLAC_DECODER
    flac_decoder_cfg_t flac_cfg = DEFAULT_FLAC_DECODER_CONFIG();
    flac_cfg.out_rb_size = 500 * 1024;
    selected_decoder = flac_decoder_init(&flac_cfg);
#elif defined SELECT_MP3_DECODER
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    selected_decoder = mp3_decoder_init(&mp3_cfg);
#elif defined SELECT_OGG_DECODER
    ogg_decoder_cfg_t ogg_cfg = DEFAULT_OGG_DECODER_CONFIG();
    selected_decoder = ogg_decoder_init(&ogg_cfg);
#elif defined SELECT_OPUS_DECODER
    opus_decoder_cfg_t opus_cfg = DEFAULT_OPUS_DECODER_CONFIG();
    selected_decoder = decoder_opus_init(&opus_cfg);
#else
    wav_decoder_cfg_t wav_cfg = DEFAULT_WAV_DECODER_CONFIG();
    selected_decoder = wav_decoder_init(&wav_cfg);
#endif

    ESP_LOGI(TAG, "[2.3] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[2.4] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, http_stream_reader, "http");
    audio_pipeline_register(pipeline, selected_decoder,    selected_decoder_name);
    audio_pipeline_register(pipeline, i2s_stream_writer,  "i2s");

    ESP_LOGI(TAG, "[2.5] Link it together http_stream-->%s_decoder-->i2s_stream-->[codec_chip]", selected_decoder_name);
    const char *link_tag[3] = {"http", selected_decoder_name, "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 3);

    ESP_LOGI(TAG, "[2.6] Set up  uri (http as http_stream, %s as %s_decoder, and default output is i2s)",
             selected_decoder_name, selected_decoder_name);
      
    ESP_LOGI(TAG, "[2.7] Read /spiffs/playlist.pls file");
    countStation = parse_playlist("/spiffs/playlist.pls");

    if ( countStation > 0) {
        print_playlist();  
        selected_file_to_play = playlist_get_url(curStation);
        ESP_LOGI(TAG, "[2.8.1] countStation = %d selected station = %s", countStation, selected_file_to_play);
    } else {
        ESP_LOGE(TAG, "Failed to parse playlist");
    }    
    
    audio_element_set_uri(http_stream_reader, selected_file_to_play);

    ESP_LOGI(TAG, "Wi-Fi SSID: %s, PASSWORD: %s", CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);

    ESP_LOGI(TAG, "[ 3 ] Start and wait for Wi-Fi network");
    set = NULL; // или сделать для задачи http and bt свою переменную set
    // Проверяем, инициализирован ли уже esp_periph_set
    if (set == NULL) {
        esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
        set = esp_periph_set_init(&periph_cfg);
    }
    periph_wifi_cfg_t wifi_cfg = {
        .wifi_config.sta.ssid = CONFIG_WIFI_SSID,
        .wifi_config.sta.password = CONFIG_WIFI_PASSWORD,
    };
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    esp_periph_start(set, wifi_handle);
    periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY);

    ESP_LOGI(TAG, "[ 4 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[4.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[4.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

    ESP_LOGI(TAG, "[ 5 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);


}

// Функция проигрывателя HTTP радио
void http_player_run(Data_GUI_Boombox_t *set_data,  Data_Boombox_GUI_t *get_data){
    /**/
    ESP_LOGD(TAG, "[ 5-6 ] Listen for all pipeline events");

    esp_err_t ret = audio_event_iface_listen(evt, &msg, pdMS_TO_TICKS(1000)); // Уменьшаем таймаут
     ESP_LOGI(TAG, " ----- msg: source_type=%d, source=%p, cmd=%d, data=%p", msg.source_type, msg.source, msg.cmd, msg.data);
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_TIMEOUT) {
            // Таймаут - это нормально, просто возвращаемся
            ESP_LOGE(TAG, "[ * ] Event TIMEOUT : %s", esp_err_to_name(ret));
            //return;
        }
        ESP_LOGE(TAG, "[ * ] Event interface error : %s", esp_err_to_name(ret));
       //return;
    }

    if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
        && msg.source == (void *) selected_decoder
        && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
        audio_element_info_t music_info = {0};
        audio_element_getinfo(selected_decoder, &music_info);

        ESP_LOGI(TAG, "[ * ] Receive music info from %s decoder, sample_rates=%d, bits=%d, ch=%d",
                 selected_decoder_name, music_info.sample_rates, music_info.bits, music_info.channels);

        if (i2s_stream_writer && music_info.sample_rates > 0) {
            i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
        }
    }

    //Stop when the last pipeline element receives stop event
    if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
        && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
        && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED) 
            || ((int)msg.data == AEL_STATUS_ERROR_OPEN))) {
        ESP_LOGW(TAG, "[ * ] Stop/Error event received: %d", (int)msg.data);
        // Можно установить флаг для переинициализации или переключения источника
    }
//*****************************  Секция управления HTTP проигрователя получеными данными из GUI **********************
    // Установка данных для передачи в Boombox GUI
    if(set_data->State == true){    
       cntr_http_player(set_data);
    //log_gui_control_data(set_data); // Логируем полученные данные из GUI
    set_data->State = false; // Сбрасываем состояние после обработки  
    }
//******************************  Секция вывода данных на GUI ********************************************************
    get_http_player( get_data); 
//********************************************************************************************************************
}

// Функция остановки проигрывателя HTTP радио
void deinit_http_player(){
    ESP_LOGI(TAG, "[ 6 ] Stop audio_pipeline and release all resources");

    if (pipeline) {
        ESP_LOGI(TAG, "Stopping pipeline...");
        
        // Удаляем слушателя перед остановкой
        if (evt) {
            audio_pipeline_remove_listener(pipeline);
        }
        
        // Безусловно останавливаем пайплайн
        audio_pipeline_stop(pipeline);
        audio_pipeline_wait_for_stop(pipeline);
        audio_pipeline_terminate(pipeline);
        
        // Отменяем регистрацию элементов
        if (http_stream_reader) {
            audio_pipeline_unregister(pipeline, http_stream_reader);
        }
        if (selected_decoder) {
            audio_pipeline_unregister(pipeline, selected_decoder);
        }
        if (i2s_stream_writer) {
            audio_pipeline_unregister(pipeline, i2s_stream_writer);
        }
        
        // Освобождаем пайплайн
        audio_pipeline_deinit(pipeline);
        pipeline = NULL;
    }

    // Останавливаем периферию
    if (set) {
        ESP_LOGI(TAG, "Stopping peripherals...");
        esp_periph_set_stop_all(set);
        
        if (evt) {
            audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt);
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_periph_set_destroy(set);
        set = NULL;
    }

    // Уничтожаем интерфейс событий
    if (evt) {
        audio_event_iface_destroy(evt);
        evt = NULL;
    }

    // Освобождаем элементы
    if (http_stream_reader) {
        audio_element_deinit(http_stream_reader);
        http_stream_reader = NULL;
    }
    
    if (selected_decoder) {
        audio_element_deinit(selected_decoder);
        selected_decoder = NULL;
    }
    
    if (i2s_stream_writer) {
        audio_element_deinit(i2s_stream_writer);
        i2s_stream_writer = NULL;
    }
    
    vTaskDelay(pdMS_TO_TICKS(200));
    
    ESP_LOGI(TAG, "HTTP player deinitialized successfully");
    
    size_t free_heap = esp_get_free_heap_size();
    ESP_LOGI(TAG, "Free heap after HTTP deinit: %d bytes", free_heap);
}