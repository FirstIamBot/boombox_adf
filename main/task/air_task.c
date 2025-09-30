
#include "board.h"

#include "air_task.h"
#include "si4735.h"
#include "commons.h"

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

/*===========================================++++=== Bandwidth  SSB    ==================================
 0 = 1.2 kHz low-pass filter  (default).
 1 = 2.2 kHz low-pass filter.
 2 = 3.0 kHz low-pass filter.
 3 = 4.0 kHz low-pass filter.
 4 = 500 Hz band-pass filter 
  ====================================================================================================================*/
const char *  bandwidthSSB[] = {
    "1.2",
    "2.2",   
    "3.0",    
    "4.0",
    "0.5",
    "1.0", 
  };
/*===========================================++++=== Bandwidth  SSB   ==================================
  The bandwidth of the AM channel filter data type (bandwidthFM)
  0  Automatically select proper channel filter (Default) |
  1  Force wide (110 kHz) channel filter |
  2  Force narrow (84 kHz) channel filter |
  3  Force narrower (60 kHz) channel filter |
  4  Force narrowest (40 kHz) channel filter |
      
  Полоса пропускания AM (bandwidthAM)
 0 = 6 kHz Bandwidth
 1 = 4 kHz Bandwidth
 2 = 3 kHz Bandwidth
 3 = 2 kHz Bandwidth 
 4 = 1 kHz Bandwidth
 5 = 1.8 kHz Bandwidth
 6 = 2.5 kHz Bandwidth, gradual roll off*                                   
 7–15 = Reserved (Do not use).
  ====================================================================================================================*/
const char *  bandwidthAM[] = {
    "6.0",
    "4.0",
    "3.0",
    "2.0",
    "1.0",
    "1.8",
    "2.5",  
};

const char * bandwidthFM[] = {
    "AUT", // Automatic - default
    "110", 
    " 84",
    " 60",
    " 40"};
//======================================================= End Bandwidth AM & FM & SSB ===========================
//======================================================= The Step Definitions     ==============================
typedef struct
{
  uint8_t idx;      // SI473X device bandwidth index
  const char *desc; // bandwidth description
} Step;

Step stepAM[] = {
  {1, "1"},
  {5, "5"},
  {8, "8"},
  {10, "10"},
};

Step stepFM[] = {
  {1, "10"},
  {5, "50"},
  {10,"100"},
  {20,"200"},
};
//======================================================= End Step AM & FM & SSB =================================

//======================================================= THE Band Definitions     ============================
typedef struct // Band data
{
  const char *bandName; // Bandname
  uint8_t  bandType;    // Band type (FM, MW or SW)
  uint16_t prefmod;     // Pref. modulation
  uint16_t minimumFreq; // Minimum frequency of the band
  uint16_t maximumFreq; // maximum frequency of the band
  uint16_t currentFreq; // Default frequency or current frequency
  uint16_t currentStep; // Default step (increment and decrement)
} Band;

//   Band table
Band band[] = {  ////
  {   "LW", LW_BAND_TYPE,  AM,   150,   279,   164, 1}, //  LW          0  ////
  {   "MW", MW_BAND_TYPE,  AM,   520,  1710,   750, 1}, //  MW          1  ////
  {   "SW", SW_BAND_TYPE,  AM,  1710, 30000, 10000, 0},  // Whole SW    2  ////
  {   "FM", FM_BAND_TYPE,  FM,  7600, 10800, 10030, 2}, //  FM          3  ////
  {"2220M", LW_BAND_TYPE, LSB,   130,   140,   135, 1}, // Ham          4  ////
  { "630M", LW_BAND_TYPE, LSB,   420,   520,   475, 1}, // Ham  630M    5
  { "160M", SW_BAND_TYPE, LSB,  1800,  1920,  1910, 1}, // Ham  160M    6  ////
  { "120M", SW_BAND_TYPE,  AM,  2300,  2860,  2500, 1}, //      120M    7
  {  "90M", SW_BAND_TYPE,  AM,  3200,  3400,  3250, 1}, //       90M    8  ////
  {  "80M", SW_BAND_TYPE, LSB,  3500,  3810,  3540, 1}, // Ham   80M    9  ////
  {  "75M", SW_BAND_TYPE,  AM,  3900,  4000,  3925, 1}, //       75M   10  ////
  {  "60M", SW_BAND_TYPE, USB,  5330,  5410,  5375, 1}, // Ham   60M   11
  {  "49M", SW_BAND_TYPE,  AM,  5900,  6200,  6055, 1}, //       49M   12  ////
  {  "40M", SW_BAND_TYPE, LSB,  7000,  7200,  7100, 1}, // Ham   40M   13  ////
  {  "41M", SW_BAND_TYPE,  AM,  7200,  7600,  7580, 1}, //       41M   14  ////
  {  "31M", SW_BAND_TYPE,  AM,  9400,  9900,  9665, 1}, //       31M   15  ////
  {  "30M", SW_BAND_TYPE, USB, 10100, 10150, 10120, 1}, // Ham   30M   16
  {  "25M", SW_BAND_TYPE,  AM, 11600, 12100, 11680, 1}, //       25M   17  ////
  {  "22M", SW_BAND_TYPE,  AM, 13570, 13870, 13700, 1}, //       22M   18
  {  "20M", SW_BAND_TYPE, USB, 14000, 14350, 14200, 1}, // Ham   20M   19
  {  "19M", SW_BAND_TYPE,  AM, 15100, 15830, 15280, 1}, //       19M   20
  {  "17M", SW_BAND_TYPE, USB, 18000, 18170, 18100, 1}, // Ham   17M   21
  {  "16M", SW_BAND_TYPE,  AM, 17480, 17900, 17600, 1}, //       16M   22
  {  "15M", SW_BAND_TYPE,  AM, 18900, 19020, 18950, 1}, //       15M   23
  {  "15M", SW_BAND_TYPE, USB, 21000, 21450, 21250, 1}, // Ham   15M   24  ////
  {  "13M", SW_BAND_TYPE,  AM, 21450, 21850, 21500, 1}, //       13M   25
  {  "12M", SW_BAND_TYPE, USB, 24890, 24990, 24940, 1}, // Ham   12M   26
  {  "11M", SW_BAND_TYPE,  AM, 25670, 26100, 25800, 1}, //       11M   27
  {   "CB", SW_BAND_TYPE,  AM, 26200, 28000, 27000, 1}, // CB band     28  ////
  {  "10M", SW_BAND_TYPE, USB, 28000, 30000, 28500, 1} // Ham   10M    29
};
//=======================================================
extern QueueHandle_t xBoomboxToGuiQueue;
extern audio_pipeline_handle_t pipeline;
extern audio_element_handle_t i2s_stream_writer;
extern audio_event_iface_msg_t msg;
extern audio_event_iface_handle_t evt_1;
extern esp_periph_set_handle_t set; 

#define RESET_PIN CONFIG_RESET_PIN
//#define AUDIO_OUTPUT_MODE CONFIG_AUDIO_OUTPUT_MODE
#define RSSI_SEARCH 15
#define SNR_SEARCH  5

static const char *TAG = "AIR_TASK";
// Структура данных радио SI4735
SI4735_t * rx_radio;
char *stationName;
char *stationInfo;
char *programInfo;
char *rdsTime;
char *dataRDS;
static uint8_t statusRDS = 0;
//=============================================================================================================
// Test it with patch_init.h or patch_full.h. Do not try load both.
#include <patch_init.h> // SSB patch for whole SSBRX initialization string
// #include <patch_full.h>    // SSB patch for whole SSBRX full download
//=============================================================================================================
// Стуктура данных для сохранения временных(текущих) данных
//air_config_t air_radio_config;


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
    ESP_LOGI(TAG, "air_config_t:\n currentVOL= %d\n, currentStepFM= %d\n, currentStepAM= %d\n, currentMod= %d\n, currentFrequency= %d\n, currentBandType= %d\n, BandWidthFM= %d\n, BandWidthAM= %d\n, BandWidthSSB= %d\n, currentAGCgain= %d\n, onoffAGCgain= %d\n, rssi_thresh_seek= %d\n, snr_thresh_seek= %d",
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

   // FM Setup
    cnfg->currentFrequency = band[FM_BAND_TYPE].currentFreq;
    setTuneFrequencyAntennaCapacitor(rx_radio, 0); //0 = automatic capacitor tuning. valid range is 0 to 191.  
    setFmBandwidth(rx_radio, cnfg->BandWidthFM);      // Automatically select proper channel filter (Default)  
    setFMDeEmphasis(rx_radio, 1);     // 1 = 50 μs Europe, Australia, Japan. 2 = 75 μs USA (default)
    setFmBlendRssiStereoThreshold(rx_radio, 10); // 49 force stereo, set this to 0. force mono, set this to 127
    setFmBLendRssiMonoThreshold(rx_radio, 30);
    setFmSoftMuteMaxAttenuation(rx_radio, 10); //Default: 0x0002 Range: 
    setFmSoftMuteSnrAttenuation(rx_radio, 6);
    setSeekFmLimits(rx_radio, band[FM_BAND_TYPE].minimumFreq, band[FM_BAND_TYPE].maximumFreq);
    setSeekFmSpacing(rx_radio, stepFM[cnfg->currentStepFM].idx); // 1(10 kHz), 5 (50 kHz), 10 (100 kHz), and 20 (200 kHz). Default is 10
    setSeekFmSrnThreshold(rx_radio, cnfg->rssi_thresh_seek);
    setSeekFmRssiThreshold(rx_radio, cnfg->rssi_thresh_seek);
    // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 10kHz) 
    setFM(rx_radio, band[FM_BAND_TYPE].minimumFreq, band[FM_BAND_TYPE].maximumFreq, cnfg->currentFrequency, stepFM[cnfg->currentStepFM].idx);
    setVolume(rx_radio, cnfg->currentVOL);
    setAutomaticGainControl(rx_radio, cnfg->onoffAGCgain, cnfg->currentAGCgain); //MaxAGCgainFM 
    delay_ms(300);
    // Init Digital Output
    digitalOutputSampleRate(rx_radio, 48000);
    delay_ms(200);
    digitalOutputFormat(rx_radio, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
    delay_ms(200);
    //RDS
    RdsInit();
    setRdsConfig(rx_radio, 1, 2, 2, 2, 2); //  1, 2, 2, 2, 2     3, 3, 3, 3, 3
    setFifoCount(rx_radio, 1);
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

    // Стуктура данных для сохранения временных(текущих) данных
    air_config_t air_radio_config = {
        .currentVOL = 22,  // 0 = Minimum  64 - Maximum
        .currentStepFM = 1, // 0 - 10 KHz, 1 - 50KHz, 2 - 100 KHz
        .currentStepAM = 1,
        .currentMod = 1,    // 0 - AM, 1 - LSB, 2 - USB
        .currentFrequency = 1000,
        .currentBandType = 3, // FM_BAND_TYPE
        .BandWidthFM = 0, // AUT-0, 110-1
        .BandWidthAM = 0, // 6kHz - 0
        .BandWidthSSB = 0, // 1.2KHz - 0
        .currentAGCgain = 5,    // 0 = Minimum attenuation (max gain) 36 - Maximum attenuation (min gain)
        .onoffAGCgain = 0,  // 0 = AGC enabled; 1 = AGC disabled
        .rssi_thresh_seek = 25, // Минимальный уровень RSSI для принятия станции
        .snr_thresh_seek = 6   // Минимальный уровень SNR для принятия станции
    };

    // add pipeline and elements
    ESP_LOGI(TAG, "[ * ] Air player started");
    //export from pipeline i2s_stream_writer -  board.c ;

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
    
    ESP_ERROR_CHECK(i2c_master_init());
    // Set current source to AIR
    g_current_source = 2;
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

    #define DEFAULT_AUDIO_PIPELINE_CONFIG() {\
        .rb_size = 8 * 1024,\
    }
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
                  
       // setFM(rx_radio, 8750, 10800, 10210, 0);
        statusRadio(rx_radio);
        //delay_ms(200);
        setVolume(rx_radio, 90);

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
