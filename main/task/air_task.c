
#include "board.h"

#include "air_task.h"
#include "si4735.h"

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


extern QueueHandle_t xBoomboxToGuiQueue;
extern audio_pipeline_handle_t pipeline;
extern audio_element_handle_t i2s_stream_writer;
extern audio_event_iface_msg_t msg;
extern audio_event_iface_handle_t evt_1;
extern esp_periph_set_handle_t set; 
extern audio_source_t g_current_source;

#define RESET_PIN CONFIG_RESET_PIN

static const char *TAG = "AIR_TASK";

SI4735_t * rx_radio;
uint16_t WorkFrequency;//!<  current frequency
uint8_t currentVOL = 10;
uint16_t currentStep = 10; 


// Стуктура данных для сохранения временных(текущих) данных
air_config_t air_radio_config;
si4735_station_t air_stations[50]; // Массив для хранения найденных станций
/**
 * @brief Сканирует FM-диапазон и сохраняет найденные станции в массив структур si4735_station_t.
 * 
 * @param rx_radio      Указатель на структуру SI4735
 * @param stations      Массив структур si4735_station_t для сохранения станций
 * @param max_stations  Максимальное количество станций для поиска
 * @return int          Количество найденных станций
 */
int scan_fm_stations(SI4735_t *rx_radio, air_config_t * cnfg, si4735_station_t *stations, int max_stations)
{
    int found = 0;

    setFM(rx_radio, 8750, 10800, 8750, cnfg->currentStepFM); // Запуск FM

    uint16_t freq = 8750;
    uint16_t first_station = 0;
    bool first_found = false;

    while (found < max_stations) {
        if (!seekNextStation(rx_radio)) {
            break;
        }
        freq = getFrequency(rx_radio);

        int rssi = getCurrentRSSI(rx_radio);
        int snr  = getCurrentSNR(rx_radio);

        if (rssi >= cnfg->rssi_thresh_seek && snr >= cnfg->snr_thresh_seek) {
            if (!first_found) {
                first_station = freq;
                first_found = true;
            } else if (freq == first_station) {
                break;
            }
            stations[found].freq_khz = freq;
            stations[found].rssi = rssi;
            found++;
        }
    }
    return found;
}

void radio_init(air_config_t * cnfg){
    // Инициализация радио SI4735 с заданной конфигурацией
    // Use SI473X_DIGITAL_AUDIO1       - Digital audio output (SI4735 device pins: 3/DCLK, 24/LOUT/DFS, 23/ROUT/DIO )
    // Use SI473X_DIGITAL_AUDIO2       - Digital audio output (SI4735 device pins: 3/DCLK, 2/DFS, 1/DIO)
    // Use SI473X_ANALOG_DIGITAL_AUDIO - Analog and digital audio outputs (24/LOUT/ 23/ROUT and 3/DCLK, 2/DFS, 1/DIO)
    // XOSCEN_RCLK                     - Use external source clock (active crystal or signal generator)
    if (rx_radio == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for SI4735 control structure");
        return;
    }
    memset(rx_radio, 0, sizeof(SI4735_t));
    init_si4735(rx_radio, RESET_PIN, -1, FM_CURRENT_MODE, SI473X_DIGITAL_AUDIO2, XOSCEN_RCLK, 0);  // Analog and digital audio outputs (LOUT/ROUT and DCLK, DFS, DIO), external RCLK
    delay_ms(500); 
    // FM Setup
    ESP_LOGI(TAG, "air_config_t: currentVOL=%d \n, currentStepFM=%d\n, currentStepAM=%d\n, currentMod=%d\n, currentFrequency=%d\n, currentBandType=%d\n, BandWidthFM=%d\n, BandWidthAM=%d\n, BandWidthSSB=%d\n, currentAGCgain=%d\n, onoffAGCgain=%d\n, rssi_thresh_seek=%d\n, snr_thresh_seek=%d",
             cnfg->currentVOL,
             cnfg->currentStepFM,
             cnfg->currentStepAM,
             cnfg->currentMod,
             cnfg->currentFrequency,
             cnfg->currentBandType,
             cnfg->BandWidthFM,
             cnfg->BandWidthAM,
             cnfg->BandWidthSSB,
             cnfg->currentAGCgain,
             cnfg->onoffAGCgain,
             cnfg->rssi_thresh_seek,
             cnfg->snr_thresh_seek);

    setTuneFrequencyAntennaCapacitor(rx_radio, 0); // Автоматическая настройка конденсатора антенны
    setFmBandwidth(rx_radio, cnfg->BandWidthFM);    // Выбор фильтра канала FM
    setFMDeEmphasis(rx_radio, 1);                  // Деэмфазис FM (1 = Европа, 2 = США)
    setFmBlendRssiStereoThreshold(rx_radio, 10); // 49 force stereo, set this to 0. force mono, set this to 127
    setFmBLendRssiMonoThreshold(rx_radio, 30);
    setFmSoftMuteMaxAttenuation(rx_radio, 10); //Default: 0x0002 Range: 
    setFmSoftMuteSnrAttenuation(rx_radio, 6);
    setSeekFmLimits(rx_radio, 8750, 10800);        // Ограничения поиска FM
    setSeekFmSpacing(rx_radio, cnfg->currentStepFM); // Шаг поиска FM
    setSeekFmSrnThreshold(rx_radio, cnfg->snr_thresh_seek); // Порог SNR для поиска FM
    setSeekFmRssiThreshold(rx_radio, cnfg->rssi_thresh_seek);

    // AM Setup
    setAmBandwidth(rx_radio, 0, cnfg->currentStepAM); // Ширина полосы AM
    setAMDeEmphasis(rx_radio, 1);                  // Деэмфазис AM
    setAmSoftMuteMaxAttenuation(rx_radio, 10);
    setAMSoftMuteSnrThreshold(rx_radio, 9);
    setSeekAmLimits(rx_radio, 520, 1710);
    setSeekAmSrnThreshold(rx_radio, cnfg->snr_thresh_seek); // Порог SNR для поиска AM
    setSeekAmRssiThreshold(rx_radio, cnfg->rssi_thresh_seek);

    // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 10kHz) 
    setFM(rx_radio, 8750, 10800, 10400, cnfg->currentStepFM); // Запуск FM
    delay_ms(300);
    setVolume(rx_radio, cnfg->currentVOL);          // Установка громкости
    // Init Digital Output
    digitalOutputSampleRate(rx_radio, 48000);
    delay_ms(200);
    digitalOutputFormat(rx_radio, 0, 0, 0, 0);
    delay_ms(200);
    // Инициализация RDS
    RdsInit();
    setRdsConfig(rx_radio, 3, 3, 3, 3, 3);
    setFifoCount(rx_radio, 1);   
    // Настройка AGC
    setAutomaticGainControl(rx_radio, cnfg->onoffAGCgain, cnfg->currentAGCgain);
}

// Вывод статуса радио
void statusRadio(SI4735_t * rx)
{
    ESP_LOGI(TAG, "******************** si4735task Status ******************");
    ESP_LOGI(TAG, "Frequency = %d", getFrequency(rx));
    ESP_LOGI(TAG, "Volume = %d", getVolume(rx));
    ESP_LOGI(TAG, "******* si4735task Received Signal Quality **************");
    getCurrentReceivedSignalQuality(rx, 0 );
    ESP_LOGI(TAG, "Current SNR = %d", getCurrentSNR(rx));
    ESP_LOGI(TAG, "Current RSSI = %d", getCurrentRSSI(rx));
    ESP_LOGI(TAG, "********************************************************");
}

// Основная функция проигрывателя AIR радио
void air_player( ){

    uint8_t n = 0;
    uint8_t mode = 1;  // mode = 0 Fixed Frequency , mode = 1  Scan Frequency 
    uint8_t i=0;

    uint16_t station[50];
    uint16_t last_frequency = 0;

    // Инициализация конфигурации радио
    air_radio_config.currentVOL = 62;   // 0 = Минимум, 64 = Максимум
    air_radio_config.currentStepFM = 1; // 1 - 10 КГц, 5 - 50КГц, 10 - 100 КГц
    air_radio_config.currentStepAM = 1;
    air_radio_config.currentMod = 3;    // 3 - FM
    air_radio_config.currentFrequency = 10490;
    air_radio_config.currentBandType = 3; // FM_BAND_TYPE
    air_radio_config.BandWidthFM = 0;   // AUT-0, 110-1
    air_radio_config.BandWidthAM = 0;   // 6kHz - 0
    air_radio_config.BandWidthSSB = 0;  // 1.2KHz - 0
    air_radio_config.currentAGCgain = 20;   // 0 = мин. аттенюация (макс. усиление), 36 - макс. аттенюация (мин. усиление)
    air_radio_config.onoffAGCgain = 1;      // 0 = AGC включен, 1 = выключен
    air_radio_config.rssi_thresh_seek = 25; // Минимальный уровень RSSI для принятия станции
    air_radio_config.snr_thresh_seek = 6;   // Минимальный уровень SNR для принятия станции

    // add pipeline and elements
    ESP_LOGI(TAG, "[ * ] Air player started");
    //audio_pipeline_handle_t pipeline;
    //audio_element_handle_t  i2s_stream_writer;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
        ESP_ERROR_CHECK(i2c_master_init());

    ESP_LOGI(TAG, "[ 1 ] Init AIR radio");
    g_current_source = SOURCE_AIR;
    ESP_LOGI(TAG, "*** g_current_source=%d", g_current_source);
    // ************* Si4735 init *********************
    // Выделяем память для структуры SI4735
    rx_radio = (SI4735_t*)malloc(sizeof(SI4735_t));
    if (rx_radio == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for SI4735 control structure");
        return;
    }
    radio_init(&air_radio_config);

    ESP_LOGI(TAG, "[ 2 ] Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "[ 3 ] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);

    ESP_LOGI(TAG, "[4] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    
    i2s_cfg.type = AUDIO_STREAM_READER; // Чтение аудио потока AUDIO_STREAM_WRITER; // Запись аудио потока  

    i2s_stream_writer = i2s_stream_init(&i2s_cfg);
    // ***********************************************************************************************************
    // ***********************************************************************************************************
    ESP_LOGI(TAG, "[4.2] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");
    ESP_LOGI(TAG, "[4.3] Link -->i2s_stream_writer-->[codec_chip]");
    const char *link_tag[1] = { "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 1);

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[5.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[ 6 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);
 
    ESP_LOGI(TAG, "[ 7 ] Listen for all pipeline events");


    while(1){
                  
        setFM(rx_radio, 6400, 10800, 10210, 0);
        statusRadio(rx_radio);
        delay_ms(200);
        setVolume(rx_radio, 90);
        // Инициализация цифрового выхода
        digitalOutputSampleRate(rx_radio, 48000);
        delay_ms(200);
        digitalOutputFormat(rx_radio, 0, 0, 0, 0);
        vTaskDelay(1500); 
    }
    ESP_LOGI(TAG, "[ 8 ] Stop audio_pipeline and release all resources");
    
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);

    /* Terminate the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    audio_pipeline_deinit(pipeline);
    audio_element_deinit(i2s_stream_writer);

    ESP_LOGI(TAG, "[ 10 ] Cleaning up AIR player resources");
    // Здесь должна быть очистка ресурсов pipeline, radio и т.д.
}

// Задача для работы с радио SI4735
void si4735task(void *pvParameters)
{
    while (1) {
        if (g_current_source == SOURCE_AIR) {
            air_player();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Функция запуска проигрывателя AIR радио
void air_player_start(air_config_t * air_radio_cnfg ){

    // Инициализация конфигурации радио
    air_radio_cnfg->currentVOL = 62;   // 0 = Минимум, 64 = Максимум
    air_radio_cnfg->currentStepFM = 1; // 1 - 10 КГц, 5 - 50КГц, 10 - 100 КГц
    air_radio_cnfg->currentStepAM = 1;
    air_radio_cnfg->currentMod = 3;    // 3 - FM
    air_radio_cnfg->currentFrequency = 10490;
    air_radio_cnfg->currentBandType = 3; // FM_BAND_TYPE
    air_radio_cnfg->BandWidthFM = 0;   // AUT-0, 110-1
    air_radio_cnfg->BandWidthAM = 0;   // 6kHz - 0
    air_radio_cnfg->BandWidthSSB = 0;  // 1.2KHz - 0
    air_radio_cnfg->currentAGCgain = 20;    // 0 = мин. аттенюация (макс. усиление), 36 - макс. аттенюация (мин. усиление)
    air_radio_cnfg->onoffAGCgain = 1;  // 0 = AGC включен, 1 = выключен
    air_radio_cnfg->rssi_thresh_seek = 25;  // Минимальный уровень RSSI для принятия станции
    air_radio_cnfg->snr_thresh_seek = 6;    // Минимальный уровень SNR для принятия станции

    // add pipeline and elements
    ESP_LOGI(TAG, "[ * ] Air player started");
    //audio_pipeline_handle_t pipeline;
    //audio_element_handle_t  i2s_stream_writer;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "[ 1 ] Init AIR radio");
    // Si4735 init
    // Выделяем память для структуры SI4735
    rx_radio = (SI4735_t*)malloc(sizeof(SI4735_t));
    if (rx_radio == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for SI4735 control structure");
        return;
    }
    radio_init(air_radio_cnfg);

    ESP_LOGI(TAG, "[ 2 ] Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "[ 3 ] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);

    ESP_LOGI(TAG, "[4] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_READER; // Чтение аудио потока   AUDIO_STREAM_WRITER; // Запись аудио потока 
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[4.2] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");
    ESP_LOGI(TAG, "[4.3] Link -->i2s_stream_writer-->[codec_chip]");
    const char *link_tag[1] = { "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 1);

    audio_event_iface_cfg_t evt_cfg_1 = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt_1 = audio_event_iface_init(&evt_cfg_1);

    ESP_LOGI(TAG, "[5.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt_1);

    ESP_LOGI(TAG, "[ 6 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);

    ESP_LOGI(TAG, "[ 7 ] Listen for all pipeline events");

}

// Функция проигрывателя AIR радио
void air_player_run(air_config_t * air_radio_cnfg){
    
    uint8_t mode = 1;  // mode = 0 Fixed Frequency , mode = 1  Scan Frequency 
    uint8_t n = 0;    
    uint8_t i=0;
    uint16_t station[50];
    uint16_t last_frequency = 0;

    while(1){
        
        // Проверяем, не изменился ли источник аудио
        if (g_current_source != SOURCE_AIR) {
            ESP_LOGI(TAG, "Audio source changed, stopping AIR player");
            break;
        }
                  
        if(mode == 0) // play station
        {
            WorkFrequency = station[n];
            ESP_LOGI(TAG, "*****************  Station: № %d from %d *****************", n, i);
            if(n<i){
                ++n;
            }
            else{ 
                n=0;
            }
            setFM(rx_radio, 6400, 10800, WorkFrequency, air_radio_cnfg->currentStepFM);
            statusRadio(rx_radio);
            air_radio_cnfg->currentFrequency = WorkFrequency;
            delay_ms(200);
            // Инициализация цифрового выхода
            digitalOutputSampleRate(rx_radio, 48000);
            delay_ms(200);
            digitalOutputFormat(rx_radio, 0, 0, 0, 0);
            vTaskDelay(1500); 
        }
        else if (mode == 1) // scan station
        {
            ESP_LOGI(TAG, "seekNextStation = %d", seekNextStation(rx_radio));
            getCurrentReceivedSignalQuality(rx_radio, 0 );
            // Проверка качества сигнала    
            if(getCurrentRSSI(rx_radio) > 25 || getCurrentSNR(rx_radio) > 9){
                station[i] = getFrequency(rx_radio);
                if( last_frequency >  station[i] ){
                    mode = 0;
                    ESP_LOGI(TAG, " -------------  All station = %d  --------------", i);
                }
                else{
                    last_frequency = station[i];
                    i++;                   
                }
            }
        }
        else{
            ESP_LOGI(TAG, "mode error");
            mode = 1;
        }
        //---------------------------------------------------------------------------------------------------------------------
    }
    ESP_LOGI(TAG, "[ 8 ] Stop audio_pipeline and release all resources");

}

// Функция остановки проигрывателя AIR радио
void air_player_stop(){

    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);
    /* Terminate the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);
    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt_1);
    /* Release all resources */
    audio_pipeline_deinit(pipeline);
    audio_element_deinit(i2s_stream_writer);

    ESP_LOGI(TAG, "[ 10 ] Cleaning up AIR player resources");
    // Здесь должна быть очистка ресурсов pipeline, radio и т.д.
}
