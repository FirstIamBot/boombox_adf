/*                                  */
//#include "board.h"

#include "air_task.h"

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
//=============================================================================================================
// Test it with patch_init.h or patch_full.h. Do not try load both.
#include <patch_init.h> // SSB patch for whole SSBRX initialization string
// #include <patch_full.h>    // SSB patch for whole SSBRX full download
//=============================================================================================================
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
/*===========================================++++=== Bandwidth  FM   ==================================*/
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
  {   "FM", FM_BAND_TYPE,  FM,  7600, 10800, 10280, 2}, //  FM          3  ////
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
#define RESET_PIN CONFIG_RESET_PIN
#define SAMPLE_RATE 44100


// Структура данных радио SI4735
SI4735_t * rx_radio;
extern QueueHandle_t xBoomboxToGuiQueue;
extern audio_pipeline_handle_t pipeline;
extern audio_element_handle_t i2s_stream_writer;
extern audio_event_iface_msg_t msg;
extern audio_event_iface_handle_t evt;
extern esp_periph_set_handle_t set; 
extern audio_source_t g_current_source;
// Структура данных для сохранения временных(текущих) данных
extern BoomBox_config_t xBoomBox_config; // Глобальная структура конфигурации Boombox  

static const char *TAG = "AIR_TASK";
char *stationName;
char *stationInfo;
char *programInfo;
char *rdsTime;
char *dataRDS;
static uint8_t statusRDS = 0;
static uint8_t count = 0;
//=============================================================================================================

// Функция для вывода данных структуры xDataBoomBox в консоль
void print_data_boombox_to_console(Data_Boombox_GUI_t *xDataBoomBox)
{
    if (xDataBoomBox == NULL) {
        ESP_LOGE(TAG, "xDataBoomBox is NULL");
        return;
    }

    ESP_LOGI(TAG, "================== Data_Boombox_GUI_t Debug ==================");
    ESP_LOGI(TAG, "State: %s", xDataBoomBox->State ? "true" : "false");
    ESP_LOGI(TAG, "eModeBoombox: %d", xDataBoomBox->eModeBoombox);
    ESP_LOGI(TAG, "ucFreq: %u", xDataBoomBox->ucFreq);
    ESP_LOGI(TAG, "vcFreqRange: %s", xDataBoomBox->vcFreqRange ? xDataBoomBox->vcFreqRange : "NULL");
    ESP_LOGI(TAG, "vcBand: %s", xDataBoomBox->vcBand ? xDataBoomBox->vcBand : "NULL");
    ESP_LOGI(TAG, "vcStereoMono: %s", xDataBoomBox->vcStereoMono ? xDataBoomBox->vcStereoMono : "NULL");
    ESP_LOGI(TAG, "ucBand: %u", xDataBoomBox->ucBand);
    ESP_LOGI(TAG, "ucSNR: %u dB", xDataBoomBox->ucSNR);
    ESP_LOGI(TAG, "ucRSSI: %u dBμV", xDataBoomBox->ucRSSI);
    ESP_LOGI(TAG, "vcStep: %s", xDataBoomBox->vcStep ? xDataBoomBox->vcStep : "NULL");
    ESP_LOGI(TAG, "vcBW: %s", xDataBoomBox->vcBW ? xDataBoomBox->vcBW : "NULL");
    ESP_LOGI(TAG, "vcRDSdata: %s", xDataBoomBox->vcRDSdata ? xDataBoomBox->vcRDSdata : "NULL");
    ESP_LOGI(TAG, "===========================================================");
}
// Детализированная функция для вывода данных структуры xDataGUI в консоль
void print_data_gui_detailed_to_console(Data_GUI_Boombox_t *xDataGUI)
{
    if (xDataGUI == NULL) {
        ESP_LOGE(TAG, "xDataGUI is NULL");
        return;
    }

    ESP_LOGI(TAG, "============== Data_GUI_Boombox_t Detailed Debug ==============");
    ESP_LOGI(TAG, "State: %s", xDataGUI->State ? "ACTIVE" : "INACTIVE");
    
    // Расшифровка режима
    const char* mode_str = "UNKNOWN";
    switch(xDataGUI->eModeBoombox) {
        case 0: mode_str = "AIR_RADIO"; break;
        case 1: mode_str = "BLUETOOTH"; break;
        case 2: mode_str = "HTTP_STREAM"; break;
        default: mode_str = "UNKNOWN"; break;
    }
    ESP_LOGI(TAG, "eModeBoombox: %d (%s)", xDataGUI->eModeBoombox, mode_str);
    
    // Расшифровка команды
    const char* cmd_str = "UNKNOWN";
    switch(xDataGUI->eDataDescription) {
        case BANDIDx: cmd_str = "BAND_SELECT"; break;
        case MODIDx: cmd_str = "MODULATION"; break;
        case STEPFM: cmd_str = "STEP_FM"; break;
        case STEPAM: cmd_str = "STEP_AM"; break;
        case BANDFM: cmd_str = "BANDWIDTH_FM"; break;
        case BANDAM: cmd_str = "BANDWIDTH_AM"; break;
        case BANDSSB: cmd_str = "BANDWIDTH_SSB"; break;
        case STEPUP: cmd_str = "FREQ_STEP_UP"; break;
        case STEPDOWN: cmd_str = "FREQ_STEP_DOWN"; break;
        case STEP_STATION_UP: cmd_str = "STATION_UP"; break;
        case STEP_STATION_DOWN: cmd_str = "STATION_DOWN"; break;
        case UP_SEEK: cmd_str = "SEEK_UP"; break;
        case AGCGAIN: cmd_str = "AGC_TOGGLE"; break;
        case SLIDER_AGC: cmd_str = "AGC_GAIN"; break;
        case SLIDER_VOL: cmd_str = "VOLUME"; break;
        case SET_FREQ: cmd_str = "SET_FREQUENCY"; break;
        default: cmd_str = "UNKNOWN"; break;
    }
    ESP_LOGI(TAG, "eDataDescription: %d (%s)", xDataGUI->eDataDescription, cmd_str);
    ESP_LOGI(TAG, "ucValue: %u", xDataGUI->ucValue);
    ESP_LOGI(TAG, "===============================================================");
}

void loadSSB(SI4735_t *rx, air_config_t *cnfg)
{
  uint16_t size_content = sizeof(ssb_patch_content)/sizeof(uint16_t);
  //const uint16_t size_content(ssb_patch_content );// see ssb_patch_content in patch_full.h or patch_init.h
  queryLibraryId(rx); // Исправлено: передаем указатель на SI4735_t
  patchPowerUp();
  delay_ms(50);
  uint32_t startload = xTaskGetTickCount();
  downloadPatch(ssb_patch_content, size_content);
  //ESP_LOGI(TAG, "SSB patch was loaded in:  =  %d ms", xTaskGetTickCount()-startload );
  // Parameters
  // AUDIOBW - SSB Audio bandwidth; 0 = 1.2kHz (default); 1=2.2kHz; 2=3kHz; 3=4kHz; 4=500Hz; 5=1kHz;
  // SBCUTFLT SSB - side band cutoff filter for band passand low pass filter ( 0 or 1)
  // AVC_DIVIDER  - set 0 for SSB mode; set 3 for SYNC mode.
  // AVCEN - SSB Automatic Volume Control (AVC) enable; 0=disable; 1=enable (default).
  // SMUTESEL - SSB Soft-mute Based on RSSI or SNR (0 or 1).
  // DSP_AFCDIS - DSP AFC Disable or enable; 0=SYNC MODE, AFC enable; 1=SSB MODE, AFC disable.
  setSSBConfig(rx, cnfg->BandWidthSSB, 0, 1, 0, 1, 1);
}
/*
  Функция инициализации радио SI4735
  Проблема при инициализации радио в том, что при смене диапазона (BandType)
  загрузка будет производится предыдущего диапазона.
  Поэтому при инициализации радио, нужно учитывать текущий диапазон.
  currentBandType = LW_BAND_TYPE, MW_BAND_TYPE, SW_BAND_TYPE
  брать частоту из band[]
*/
void radio_init(SI4735_t *rx, air_config_t *cnfg)
 {
  if(cnfg->currentBandType == LW_BAND_TYPE ){
    ESP_LOGI(TAG, "init %s_BAND_TYPE",band[cnfg->currentBandType].bandName); 
    //cnfg->currentFrequency = band[MW_BAND_TYPE].currentFreq;
    init_si4735(rx, RESET_PIN, -1, AM_CURRENT_MODE, SI473X_DIGITAL_AUDIO2, XOSCEN_RCLK, 0);  // Analog and digital audio outputs (LOUT/ROUT and DCLK, DFS, DIO), external RCLK
    // AM Setup
    setAmBandwidth(rx, 0, cnfg->BandWidthAM); // 0 = 6 kHz Bandwidth, 1 = Enable Noise Rejection Filter
    setAMDeEmphasis(rx, 1);
    setAmSoftMuteMaxAttenuation(rx, 10);
    setAMSoftMuteSnrThreshold(rx, 9);
    setSeekAmLimits(rx, band[LW_BAND_TYPE].minimumFreq, band[LW_BAND_TYPE].maximumFreq);
    setSeekAmSrnThreshold(rx, 11);
    setSeekAmRssiThreshold(rx, 42);
    // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 10kHz) 
    setAM(rx, band[LW_BAND_TYPE].minimumFreq, band[LW_BAND_TYPE].maximumFreq, cnfg->currentFrequency, stepAM[LW_BAND_TYPE].idx);
    setAutomaticGainControl(rx, cnfg->onoffAGCgain, cnfg->currentAGCgain); //MaxAGCgainFM 
    setVolume(rx, cnfg->currentVOL);
    delay_ms(300);
    // Init Digital Output
    digitalOutputSampleRate(rx, SAMPLE_RATE);
    delay_ms(200);
    digitalOutputFormat(rx, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
    delay_ms(200);
  }
  else if(cnfg->currentBandType == MW_BAND_TYPE){
    ESP_LOGI(TAG, "init %s_BAND_TYPE",band[cnfg->currentBandType].bandName); 
    //cnfg->currentFrequency = band[MW_BAND_TYPE].currentFreq;
    init_si4735(rx, RESET_PIN, -1, AM_CURRENT_MODE, SI473X_DIGITAL_AUDIO2, XOSCEN_RCLK, 0);  // Analog and digital audio outputs (LOUT/ROUT and DCLK, DFS, DIO), external RCLK
    // AM Setup
    setAmBandwidth(rx, 0, cnfg->BandWidthAM); // 0 = 6 kHz Bandwidth, 1 = Enable Noise Rejection Filter
    setAMDeEmphasis(rx, 1);
    setAmSoftMuteMaxAttenuation(rx, 10);
    setAMSoftMuteSnrThreshold(rx, 9);
    setSeekAmLimits(rx, band[MW_BAND_TYPE].minimumFreq, band[MW_BAND_TYPE].maximumFreq);
    setSeekAmSrnThreshold(rx, 11);
    setSeekAmRssiThreshold(rx, 42);
    // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 10kHz) 
    setAM(rx, band[MW_BAND_TYPE].minimumFreq, band[MW_BAND_TYPE].maximumFreq, cnfg->currentFrequency, stepAM[MW_BAND_TYPE].idx);
    setAutomaticGainControl(rx, cnfg->onoffAGCgain, cnfg->currentAGCgain); //MaxAGCgainFM 
    setVolume(rx, cnfg->currentVOL);
    delay_ms(300);
    // Init Digital Output
    digitalOutputSampleRate(rx, SAMPLE_RATE);
    delay_ms(200);
    digitalOutputFormat(rx, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
    delay_ms(200);
  }
  else if(cnfg->currentBandType == SW_BAND_TYPE){
    ESP_LOGI(TAG, "init %s_BAND_TYPE",band[cnfg->currentBandType].bandName); 
    init_si4735(rx, RESET_PIN, -1, AM_CURRENT_MODE, SI473X_DIGITAL_AUDIO2, XOSCEN_RCLK, 0);  // Analog and digital audio outputs (LOUT/ROUT and DCLK, DFS, DIO), external RCLK
    //cnfg->currentFrequency = band[SW_BAND_TYPE].currentFreq;
    // AM Setup
    setAmBandwidth(rx, 0, cnfg->BandWidthAM); // 0 = 6 kHz Bandwidth, 1 = Enable Noise Rejection Filter
    setAMDeEmphasis(rx, 1);
    setAmSoftMuteMaxAttenuation(rx, 10);
    setAMSoftMuteSnrThreshold(rx, 9);
    setSeekAmLimits(rx, band[SW_BAND_TYPE].minimumFreq, band[SW_BAND_TYPE].maximumFreq);
    setSeekAmSrnThreshold(rx, 11);
    setSeekAmRssiThreshold(rx, 42);
    // Starts defaul radio function and band (AM; ) 
    setAM(rx, band[cnfg->currentBandType].minimumFreq, band[cnfg->currentBandType].maximumFreq, cnfg->currentFrequency, stepAM[cnfg->currentBandType].idx);
    setVolume(rx, cnfg->currentVOL);
    setAutomaticGainControl(rx, cnfg->onoffAGCgain, cnfg->currentAGCgain); //MaxAGCgainFM 
    delay_ms(300);
    // Init Digital Output
    digitalOutputSampleRate(rx, SAMPLE_RATE);
    delay_ms(200);
    digitalOutputFormat(rx, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
    delay_ms(200);
  }
  else if(cnfg->currentBandType > 3) {
    if(band[cnfg->currentBandType].prefmod ==1 || band[cnfg->currentBandType].prefmod || 2){ // SSB mode LSB and USB modolation
      ESP_LOGI(TAG, "init %s_BAND_TYPE - ",band[cnfg->currentBandType].bandName); 
      init_si4735(rx, RESET_PIN, -1, AM_CURRENT_MODE, SI473X_DIGITAL_AUDIO2, XOSCEN_RCLK, 0);  // Analog and digital audio outputs (LOUT/ROUT and DCLK, DFS, DIO), external RCLK
      //cnfg->currentFrequency = band[cnfg->currentBandType].currentFreq;
      loadSSB(rx, cnfg);
      setTuneFrequencyAntennaCapacitor(rx, 0); // Set antenna tuning capacitor for SW.
      setSSB(rx, band[cnfg->currentBandType].minimumFreq, band[cnfg->currentBandType].maximumFreq, cnfg->currentFrequency, stepAM[cnfg->currentBandType].idx, band[cnfg->currentBandType].prefmod);
      // Init Digital Output
      digitalOutputSampleRate(rx, SAMPLE_RATE);
      delay_ms(200);
      // OSIZE Dgital Output Audio Sample Precision (0=16 bits, 1=20 bits, 2=24 bits, 3=8bits).
      // OMONO Digital Output Mono Mode (0=Use mono/stereo blend ).
      // OMODE Digital Output Mode (0=I2S, 6 = Left-justified, 8 = MSB at second DCLK after DFS pulse, 12 = MSB at first DCLK after DFS pulse).
      // OFALL Digital Output DCLK Edge (0 = use DCLK rising edge, 1 = use DCLK falling edge)
      digitalOutputFormat(rx, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
      delay_ms(200);
    }
    else if(band[cnfg->currentBandType].prefmod == 0) { // AM modolation
      init_si4735(rx, RESET_PIN, -1, AM_CURRENT_MODE, SI473X_DIGITAL_AUDIO2, XOSCEN_RCLK, 0);  // Analog and digital audio outputs (LOUT/ROUT and DCLK, DFS, DIO), external RCLK
      //cnfg->currentFrequency = band[cnfg->currentBandType].currentFreq;
      // AM Setup
      setAmBandwidth(rx, 0, cnfg->BandWidthAM); // 0 = 6 kHz Bandwidth, 1 = Enable Noise Rejection Filter
      setAMDeEmphasis(rx, 1);
      setAmSoftMuteMaxAttenuation(rx, 10);
      setAMSoftMuteSnrThreshold(rx, 9);
      setSeekAmLimits(rx, band[cnfg->currentBandType].minimumFreq, band[cnfg->currentBandType].maximumFreq);
      setSeekAmSrnThreshold(rx, 11);
      setSeekAmRssiThreshold(rx, 42);
      // Starts defaul radio function and band (AM; ) 
      setAM(rx, band[cnfg->currentBandType].minimumFreq, band[cnfg->currentBandType].maximumFreq, cnfg->currentFrequency, stepAM[cnfg->currentBandType].idx);
      setVolume(rx, cnfg->currentVOL);
      setAutomaticGainControl(rx, cnfg->onoffAGCgain, cnfg->currentAGCgain); //MaxAGCgainFM 
      delay_ms(300);
      // Init Digital Output
      digitalOutputSampleRate(rx, SAMPLE_RATE);
      delay_ms(200);
      digitalOutputFormat(rx, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
      delay_ms(200);
    }     
  }
  else if(cnfg->currentBandType == FM_BAND_TYPE){
    ESP_LOGI(TAG, "init %s_BAND_TYPE",band[cnfg->currentBandType].bandName); 
    //getDeviceI2CAddress(RESET_PIN);
    // Use SI473X_DIGITAL_AUDIO1       - Digital audio output (SI4735 device pins: 3/DCLK, 24/LOUT/DFS, 23/ROUT/DIO )
    // Use SI473X_DIGITAL_AUDIO2       - Digital audio output (SI4735 device pins: 3/DCLK, 2/DFS, 1/DIO)
    // Use SI473X_ANALOG_DIGITAL_AUDIO - Analog and digital audio outputs (24/LOUT/ 23/ROUT and 3/DCLK, 2/DFS, 1/DIO)
    // XOSCEN_RCLK                     - Use external source clock (active crystal or signal generator)
    init_si4735(rx, RESET_PIN, -1, FM_CURRENT_MODE, SI473X_DIGITAL_AUDIO2, XOSCEN_RCLK, 0);  // Analog and digital audio outputs (LOUT/ROUT and DCLK, DFS, DIO), external RCLK
    delay_ms(500); 
    // FM Setup
    //cnfg->currentFrequency = band[FM_BAND_TYPE].currentFreq;
    setTuneFrequencyAntennaCapacitor(rx, 0); //0 = automatic capacitor tuning. valid range is 0 to 191.  
    setFmBandwidth(rx, cnfg->BandWidthFM);      // Automatically select proper channel filter (Default)  
    setFMDeEmphasis(rx, 1);     // 1 = 50 μs Europe, Australia, Japan. 2 = 75 μs USA (default)
    setFmBlendRssiStereoThreshold(rx, 10); // 49 force stereo, set this to 0. force mono, set this to 127
    setFmBLendRssiMonoThreshold(rx, 30); //30
    setFmSoftMuteMaxAttenuation(rx, 6); // 6 Default: 0x0002 Range: 
    setFmSoftMuteSnrAttenuation(rx, 6);
    setSeekFmLimits(rx, band[FM_BAND_TYPE].minimumFreq, band[FM_BAND_TYPE].maximumFreq);
    setSeekFmSpacing(rx, stepFM[cnfg->currentStepFM].idx); // 1(10 kHz), 5 (50 kHz), 10 (100 kHz), and 20 (200 kHz). Default is 10
    setSeekFmSrnThreshold(rx, cnfg->snr_thresh_seek);
    setSeekFmRssiThreshold(rx, cnfg->rssi_thresh_seek);
    // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 10kHz) 
    setFM(rx, band[FM_BAND_TYPE].minimumFreq, band[FM_BAND_TYPE].maximumFreq, cnfg->currentFrequency, stepFM[cnfg->currentStepFM].idx);
    setVolume(rx, cnfg->currentVOL);
    setAutomaticGainControl(rx, cnfg->onoffAGCgain, cnfg->currentAGCgain); //MaxAGCgainFM 
    delay_ms(300);
    // Init Digital Output
    digitalOutputSampleRate(rx, SAMPLE_RATE);
    delay_ms(200);
    // OSIZE Dgital Output Audio Sample Precision (0=16 bits, 1=20 bits, 2=24 bits, 3=8bits).
    // OMONO Digital Output Mono Mode (0=Use mono/stereo blend ).
    // OMODE Digital Output Mode (0=I2S, 6 = Left-justified, 8 = MSB at second DCLK after DFS pulse, 12 = MSB at first DCLK after DFS pulse).
    // OFALL Digital Output DCLK Edge (0 = use DCLK rising edge, 1 = use DCLK falling edge)
    digitalOutputFormat(rx, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
    delay_ms(200);
    //RDS
    RdsInit();
    setRdsConfig(rx, 1, 2, 2, 2, 2); //  1, 2, 2, 2, 2     3, 3, 3, 3, 3
    setFifoCount(rx, 1);
  }
}

char *checkRDS(SI4735_t *rx){

  char *utcTime;
  char *utcDateTime;
  char *stationName;
  char *stationInformation;
  char *programInformation;

  uint16_t year, month, day, hour, minute;

/* */
  // Не выдаёт информацию по RDS
  getRdsStatus(rx, 0, 0 ,0);
  if (getRdsReceived(rx)) {
    if (getRdsSync(rx) && getRdsSyncFound(rx) ) {
      utcTime = getRdsTime(rx);              // returns NULL if no information
      stationName = getRdsText0A(rx);        // returns NULL if no information
      stationInformation = getRdsText2B(rx); // returns NULL if no information
      programInformation = getRdsText2A(rx); // returns NULL if no information
      if ( utcTime != NULL ){
        ESP_LOGI(TAG, "######  RDS utcTime(len-%d) : %s", strlen(utcTime), utcTime);
        return utcTime;
      }
      if ( stationName != NULL ){
        ESP_LOGI(TAG, "######  RDS stationName(len-%d) : %s", strlen(stationName), stationName);
        //return stationName;
      }
      if ( stationInformation != NULL ){
        ESP_LOGI(TAG, "######  RDS stationInformation(len-%d) : %s", strlen(stationInformation), stationInformation);
        //return stationName;
      }
      if ( programInformation != NULL ){
        ESP_LOGI(TAG, "######  RDS stationName(len-%d) : %s", strlen(programInformation), programInformation);
        //return stationName;
      }
    } 
  }

/*  
  #define INRERVAL_RDS 1000
  static uint32_t previousMillis = 0;

  if(get->currentBandType == FM_BAND_TYPE ){
    previousMillis = xTaskGetTickCount();
    dataRDS = (char *)malloc(240 );
    if((getRdsAllData(&radio, &stationName, &stationInfo, &programInfo, &rdsTime) != NULL) ){ // && (xTaskGetTickCount() - previousMillis > INRERVAL_RDS )
      if(stationName != NULL){
        removeUnwantedChar( stationName, strlen(stationName));
        memcpy(dataRDS, stationName, strlen(stationName));
        ESP_LOGI(TAG, " stationName -  %s", stationName);
      }
      if(stationInfo != NULL){
        removeUnwantedChar( stationInfo, strlen(stationInfo));
        ESP_LOGI(TAG, " stationInfo -  %s", stationInfo);

        memcpy(dataRDS, stationInfo, strlen(stationInfo));
      }
      if(programInfo != NULL){
        removeUnwantedChar( programInfo, strlen(programInfo));
        ESP_LOGI(TAG, " programInfo -  %s", programInfo);
        memcpy(dataRDS, programInfo, strlen(programInfo));
      }
      if(rdsTime != NULL){
        removeUnwantedChar( rdsTime, strlen(rdsTime));
        ESP_LOGI(TAG, " rdsTime -  %s", rdsTime);
        memcpy(dataRDS, rdsTime, strlen(rdsTime));
      }
      if(stationName != NULL || stationInfo != NULL || programInfo != NULL || rdsTime != NULL ){
        removeUnwantedChar( dataRDS, strlen(dataRDS));
        get_data->vcRDSdata = dataRDS;
        ESP_LOGI(TAG, " dataRDS -  %s", dataRDS);
      } 
    }
    free(dataRDS);
  }
  */
 /*  
  char *strRDS = "фівапролджє.юбьтимсчяйцукенгшщзхї 132+7-,,Ю,Ю..ю!№;";
  char *prtRDS;
  char bufst[120];
  ESP_LOGI(TAG, " ######################  strRDS -  %s", strRDS);
  prtRDS = strRDS;
  ESP_LOGI(TAG, " ######################  prtRDS -  %s", prtRDS);
  strcat(bufst, prtRDS);
  ESP_LOGI(TAG, " ######################  bufst -  %s", bufst);
  get_data->vcRDSdata = bufst;

  char bufst[120] = {'q','w','e','r','t','y','u','i','o','p','a','s','d','f','g','h','j','k','l','z','x','c','v','b','n','m'};
  char *strRDS ;

  //char *prtRDS = malloc(strlen(bufst)+1);
  char *prtRDS;
  prtRDS= malloc(strlen(bufst)+1);

  ESP_LOGI(TAG, " ########  bufst -  %s", bufst);
  memcpy(prtRDS, bufst, strlen(bufst));
  ESP_LOGI(TAG, " ########  prtRDS -  %s", prtRDS);
  
  get_data->vcRDSdata = prtRDS;

  if(statusRDS == 1 ){
    statusRDS = 0;
    get_data->vcRDSdata = "                            ";
  }
  free(prtRDS);
 */
/*
  rdsBeginQuery(rx);
  if (!getRdsReceived(rx))  return 0;
  if (getRdsSync(&radio) && getRdsSyncFound(&radio)) return 0; //  if (!getRdsSync(rx) || getNumRdsFifoUsed(rx) == 0) return 0; //  
  utcTime = getRdsTime(rx);              // returns NULL if no information
  stationName = getRdsText0A(rx);        // returns NULL if no information
  stationInformation = getRdsText2B(rx); // returns NULL if no information
  programInformation = getRdsText2A(rx); // returns NULL if no information

  getRdsDateTime(rx, &year, &month, &day, &hour, &minute);
  
  utcDateTime = getRdsDateTimeStr(rx);
  ESP_LOGI(TAG, "pointer stationName-------- RDS: %p   strlen stationName  - %d", stationName, strlen(stationName));
  ESP_LOGI(TAG, "pointer stationInformation ----------- RDS: %p", stationInformation);
  ESP_LOGI(TAG, "pointer programInformation ----------- RDS: %p", programInformation);
  ESP_LOGI(TAG, "pointer utcTime ---------------------- RDS: %p", utcTime);

  if (stationName != 0) ESP_LOGI(TAG, "RDS stationName : %s", stationName);
  if (stationInformation != 0) ESP_LOGI(TAG, "RDS stationInformation : %s", stationInformation);
  if (programInformation != 0) ESP_LOGI(TAG, "RDS  programInformation : %s", programInformation);
  if (!utcDateTime) return 0;
  ESP_LOGI(TAG, "RDS: %s", utcDateTime);

  return utcDateTime;
*/
return 0;
}

void scan_radio_band(SI4735_t *rx, air_config_t *cnfg) {

    uint16_t freq = 0;
    uint16_t min_freq = 0;
    uint16_t max_freq = 0;
    uint8_t step = 0;
    uint8_t count = 0;

    for(int i = 0; i < MAX_FOUND_STATIONS; i++) {
        cnfg->air_FM_station.stations[i] = 0; // Инициализация массива частот станций
    }
    for(int i = 0; i < MAX_FOUND_STATIONS; i++) {
        cnfg->air_LW_station.stations[i] = 0; // Инициализация массива частот станций
    } 
    for(int i = 0; i < MAX_FOUND_STATIONS; i++) {
        cnfg->air_MW_station.stations[i] = 0; // Инициализация массива частот станций
    } 
    for(int i = 0; i < MAX_FOUND_STATIONS; i++) {
        cnfg->air_SW_station.stations[i] = 0; // Инициализация массива частот станций
    }  

    if(cnfg->currentBandType == FM_BAND_TYPE) {
      step = stepFM[FM_BAND_TYPE].idx;
      freq = band[FM_BAND_TYPE].minimumFreq;
      max_freq = band[FM_BAND_TYPE].maximumFreq;
      cnfg->air_FM_station.currentStationIndex = 0; // сброс индекса текущей станции
    }
    else if(cnfg->currentBandType == LW_BAND_TYPE) {
      step = stepAM[LW_BAND_TYPE].idx;
      freq = band[LW_BAND_TYPE].minimumFreq;
      max_freq = band[LW_BAND_TYPE].maximumFreq;
      cnfg->air_LW_station.currentStationIndex = 0;
    }
    else if(cnfg->currentBandType == MW_BAND_TYPE) {
      step = stepAM[MW_BAND_TYPE].idx;
      freq = band[MW_BAND_TYPE].minimumFreq;
      max_freq = band[MW_BAND_TYPE].maximumFreq;
      cnfg->air_MW_station.currentStationIndex = 0;
    }
    else if(cnfg->currentBandType == SW_BAND_TYPE) {
      step = stepAM[SW_BAND_TYPE].idx;
      freq = band[SW_BAND_TYPE].minimumFreq;
      max_freq = band[SW_BAND_TYPE].maximumFreq;
      cnfg->air_SW_station.currentStationIndex = 0;
    }
    
    for (; freq <= max_freq && count < MAX_FOUND_STATIONS; freq += step) {
        setFrequency(rx, freq);
        vTaskDelay(pdMS_TO_TICKS(80)); // задержка для стабилизации

        getCurrentReceivedSignalQuality(rx, 0);
        uint8_t rssi = getCurrentRSSI(rx);
        uint8_t snr  = getCurrentSNR(rx);
        // порог для обнаружения станции
        if (rssi > cnfg->rssi_thresh_seek && snr > cnfg->snr_thresh_seek) {
            if(cnfg->currentBandType == FM_BAND_TYPE){
              cnfg->air_FM_station.stations[count] = freq;
            }
            if(cnfg->currentBandType == LW_BAND_TYPE){
              cnfg->air_LW_station.stations[count] = freq;
            }
            if(cnfg->currentBandType == MW_BAND_TYPE){
              cnfg->air_MW_station.stations[count] = freq;
            }
            if(cnfg->currentBandType == SW_BAND_TYPE){
              cnfg->air_SW_station.stations[count] = freq;
            }
            count++;
        }
    }
}

void seek_radio_band(SI4735_t *rx, air_config_t *cnfg) {

    uint16_t freq = 0;
    uint16_t last_freq = 0;
    uint16_t min_freq = 0;
    uint16_t max_freq = 0;
    uint8_t step = 0;
    //uint8_t count = 0;
    // Инициализация массива частот станций
    for(int i = 0; i < MAX_FOUND_STATIONS; i++) {
        cnfg->air_FM_station.stations[i] = 0; 
    }
    for(int i = 0; i < MAX_FOUND_STATIONS; i++) {
        cnfg->air_LW_station.stations[i] = 0; // Инициализация массива частот станций
    } 
    for(int i = 0; i < MAX_FOUND_STATIONS; i++) {
        cnfg->air_MW_station.stations[i] = 0; // Инициализация массива частот станций
    } 
    for(int i = 0; i < MAX_FOUND_STATIONS; i++) {
        cnfg->air_SW_station.stations[i] = 0; // Инициализация массива частот станций
    }  

    if(cnfg->currentBandType == FM_BAND_TYPE) {
      max_freq = band[FM_BAND_TYPE].maximumFreq;
      cnfg->air_FM_station.currentStationIndex = 0; // сброс индекса текущей станции
      setFrequency(rx, band[FM_BAND_TYPE].minimumFreq);
    }
    else if(cnfg->currentBandType == LW_BAND_TYPE) {
      max_freq = band[LW_BAND_TYPE].maximumFreq;
      cnfg->air_LW_station.currentStationIndex = 0;
    }
    else if(cnfg->currentBandType == MW_BAND_TYPE) {
      max_freq = band[MW_BAND_TYPE].maximumFreq;
      cnfg->air_MW_station.currentStationIndex = 0;
    }
    else if(cnfg->currentBandType == SW_BAND_TYPE) {
      max_freq = band[SW_BAND_TYPE].maximumFreq;
      cnfg->air_SW_station.currentStationIndex = 0;
    }
    
    while (count < MAX_FOUND_STATIONS ) {
      last_freq = freq;      
      freq = seekNextStation(rx_radio);
      if (freq == 0 || freq > max_freq) break; // Прервать, если частота недействительна или превышает максимальную
      if (freq <= last_freq) break; // Прервать, если частота не изменилась (избежать зацикливания)
        getCurrentReceivedSignalQuality(rx, 0);
        uint8_t rssi = getCurrentRSSI(rx);
        uint8_t snr  = getCurrentSNR(rx);

        if (rssi >= cnfg->rssi_thresh_seek && snr >= cnfg->snr_thresh_seek){
          if(cnfg->currentBandType == FM_BAND_TYPE) {
              cnfg->air_FM_station.stations[count] = freq;
          }
          else if(cnfg->currentBandType == LW_BAND_TYPE) {
              cnfg->air_LW_station.stations[count] = freq;
          }
          else if(cnfg->currentBandType == MW_BAND_TYPE) {
              cnfg->air_MW_station.stations[count] = freq;
          }
          else if(cnfg->currentBandType == SW_BAND_TYPE) {
              cnfg->air_SW_station.stations[count] = freq;
          }
          count++;
        }
    }
// Вывести все найденные FM станции
ESP_LOGI(TAG, "FM станции (кол-во: %d):", MAX_FOUND_STATIONS);
for (int i = 0; i < MAX_FOUND_STATIONS; i++) {
    if (cnfg->air_FM_station.stations[i] != 0) {
        ESP_LOGI(TAG, "[%2d] %u", i, cnfg->air_FM_station.stations[i]);
    }
}
}

void set_radio(SI4735_t *rx, Data_GUI_Boombox_t *set_data, air_config_t *set)
{
  static uint8_t amountStation;
  uint16_t last_frequency = 0;
  uint16_t frequency = 0;
  uint8_t mode = 0;

  ESP_LOGD(TAG, "set_radio();");
  if(set_data->eDataDescription == BANDIDx){
    //ESP_LOGI(TAG, "******* Air Radio - BANDIDx = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);

    if(set_data->ucValue == LW_BAND_TYPE){
      set->currentBandType = LW_BAND_TYPE;
      set->currentMod = band[set->currentBandType].prefmod;           // Устанока предпочтительной модуляции
      set->currentStepAM = band[set->currentBandType].currentStep ;
      set->currentFrequency = band[set->currentBandType].currentFreq;
      
      radio_init(rx, set);
    }
    if(set_data->ucValue == MW_BAND_TYPE){
      set->currentBandType = MW_BAND_TYPE;
      set->currentMod = band[set->currentBandType].prefmod;           // Устанока предпочтительной модуляции
      set->currentStepAM = band[set->currentBandType].currentStep ;
      set->currentFrequency = band[set->currentBandType].currentFreq;
      
      radio_init(rx, set);
    }
    if(set_data->ucValue == SW_BAND_TYPE){
      set->currentBandType = SW_BAND_TYPE;
      set->currentMod = band[set->currentBandType].prefmod;           // Устанока предпочтительной модуляции
      set->currentStepAM = band[set->currentBandType].currentStep ;
      set->currentFrequency = band[set->currentBandType].currentFreq;

      radio_init(rx, set);
    }
    if(set_data->ucValue > 3 ){
      ESP_LOGD(TAG, "################## Air Radio - BANDIDx = %d (BAND type), ucValue = %d", set_data->eDataDescription, set_data->ucValue);
      set->currentBandType = set_data->ucValue; // BAND_TYPE
      set->currentMod = band[set->currentBandType].prefmod;           // Устанока предпочтительной модуляции
      set->currentStepAM = band[set->currentBandType].currentStep ;
      set->currentFrequency = band[set->currentBandType].currentFreq;

      radio_init(rx, set);
    }
    if(set_data->ucValue == FM_BAND_TYPE){
      // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 10kHz) 
      set->currentBandType = FM_BAND_TYPE;
      set->currentStepAM = band[set->currentBandType].currentStep ;
      set->currentFrequency = band[set->currentBandType].currentFreq;

      radio_init(rx, set);
    }
  }
  else if(set_data->eDataDescription == MODIDx){          // Установка предпочтительной модуляции (AM, LSB, USB) и включения режима SSB 
    ESP_LOGD(TAG, "******* Air Radio - MODIDx = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    set->currentMod = set_data->ucValue;
    if(set->currentMod == 0){
      // Starts defaul radio function and AM modulation
      setAM(rx, band[set->currentBandType].minimumFreq, band[set->currentBandType].maximumFreq, set->currentFrequency, stepAM[set->currentBandType].idx);
      // Init Digital Output
      digitalOutputSampleRate(rx, SAMPLE_RATE);
      delay_ms(200);
      digitalOutputFormat(rx, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
      delay_ms(200);
    }
    else if(set->currentMod == 1 || set->currentMod == 2){
      // Starts defaul radio function and SSB modulation 
      setSSBband(rx, set->currentMod);
      // Init Digital Output
      digitalOutputSampleRate(rx, SAMPLE_RATE);
      delay_ms(200);
      digitalOutputFormat(rx, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
      delay_ms(200);
    }
  }
  else if(set_data->eDataDescription == STEPFM){
    set->currentStepFM = set_data->ucValue;
  }
  else if(set_data->eDataDescription == STEPAM){
    set->currentStepAM = set_data->ucValue;
  }
  else if(set_data->eDataDescription == BANDFM){
    set->BandWidthFM = set_data->ucValue;
    setFmBandwidth(rx, set->BandWidthFM );
  }
  else if(set_data->eDataDescription == BANDAM){
    ESP_LOGI(TAG, "******* Air Radio - BANDAM = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    if(set->currentBandType == LW_BAND_TYPE){
      set->BandWidthAM = set_data->ucValue;
      setAmBandwidth(rx, set->BandWidthAM , 1);
    }
    if(set->currentBandType == MW_BAND_TYPE){
      set->BandWidthAM = set_data->ucValue;
      setAmBandwidth(rx, set->BandWidthAM , 1);
    }
    if(set->currentBandType == SW_BAND_TYPE){
      set->BandWidthAM = set_data->ucValue;
      setAmBandwidth(rx, set->BandWidthAM , 1); 
    }
    if(set->currentBandType > 3){
      set->BandWidthAM = set_data->ucValue;
      setAmBandwidth(rx, set->BandWidthAM , 1);
    }
  }
  else if(set_data->eDataDescription == BANDSSB){
    ESP_LOGI(TAG, "******* Air Radio - BANDSSB = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    set->BandWidthSSB = set_data->ucValue;
    setSSBAudioBandwidth(rx, set->BandWidthSSB );
  }
  else if(set_data->eDataDescription == STEPUP){
    ESP_LOGD(TAG, "******* Air Radio - STEPUP = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    if((set->currentBandType == LW_BAND_TYPE) || (set->currentBandType == MW_BAND_TYPE) || set->currentBandType == SW_BAND_TYPE){
      set->currentFrequency += stepAM[set->currentStepAM].idx;
      //**************
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType > 3){
      //**************
      set->currentFrequency += stepAM[set->currentStepAM].idx;
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType == FM_BAND_TYPE){
      clearRDSbuffer();
      //**************
      set->currentFrequency += stepFM[set->currentStepFM].idx;
      setFrequency(rx, set->currentFrequency);

    }
  }
  else if(set_data->eDataDescription == STEPDOWN){
    ESP_LOGD(TAG, "******* Air Radio - STEPDOWN = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    if((set->currentBandType == LW_BAND_TYPE) || (set->currentBandType == MW_BAND_TYPE) || set->currentBandType == SW_BAND_TYPE){
      //**************
      set->currentFrequency -= stepAM[set->currentStepAM].idx;
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType > 3){
      //**************
      set->currentFrequency -= stepAM[set->currentStepAM].idx;
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType == FM_BAND_TYPE){
      clearRDSbuffer();
      //**************
      set->currentFrequency -= stepFM[set->currentStepFM].idx;
      setFrequency(rx, set->currentFrequency);
    }
  }
  else if(set_data->eDataDescription == STEP_STATION_UP){
    ESP_LOGD(TAG, "******* Air Radio - STEP_STATION_UP = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    if(set->currentBandType == LW_BAND_TYPE){
      if(set->air_LW_station.currentStationIndex >= count){
        set->air_LW_station.currentStationIndex = 0;
      } else {
        set->air_LW_station.currentStationIndex++;
      }
      set->currentFrequency = set->air_LW_station.stations[set->air_LW_station.currentStationIndex];
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType == MW_BAND_TYPE){
      if(set->air_MW_station.currentStationIndex >= count){
        set->air_MW_station.currentStationIndex = 0;
      } else {
        set->air_MW_station.currentStationIndex++;
      }
      set->currentFrequency = set->air_MW_station.stations[set->air_MW_station.currentStationIndex];
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType == SW_BAND_TYPE){
      if(set->air_SW_station.currentStationIndex >= count){
        set->air_SW_station.currentStationIndex = 0;
      } else {
        set->air_SW_station.currentStationIndex++;
      }
      set->currentFrequency = set->air_SW_station.stations[set->air_SW_station.currentStationIndex];
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType == FM_BAND_TYPE){
      clearRDSbuffer();
      if(set->air_FM_station.currentStationIndex >= count){
        set->air_FM_station.currentStationIndex = 0;
      } else {
        set->air_FM_station.currentStationIndex++;
      }
      set->currentFrequency = set->air_FM_station.stations[set->air_FM_station.currentStationIndex];
      setFrequency(rx, set->currentFrequency);
    }
  }
  else if(set_data->eDataDescription == STEP_STATION_DOWN){
    ESP_LOGD(TAG, "******* Air Radio - STEP_STATION_DOWN = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    if(set->currentBandType == LW_BAND_TYPE){
      if(set->air_LW_station.currentStationIndex == 0){
        set->air_LW_station.currentStationIndex = count;
      } else {
        set->air_LW_station.currentStationIndex--;
      }
      set->currentFrequency = set->air_LW_station.stations[set->air_LW_station.currentStationIndex];
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType == MW_BAND_TYPE){
      if(set->air_MW_station.currentStationIndex == 0){
        set->air_MW_station.currentStationIndex = count;
      } else {
        set->air_MW_station.currentStationIndex--;
      }
      set->currentFrequency = set->air_MW_station.stations[set->air_MW_station.currentStationIndex];
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType == SW_BAND_TYPE){
      if(set->air_SW_station.currentStationIndex == 0){
        set->air_SW_station.currentStationIndex = count;
      } else {
        set->air_SW_station.currentStationIndex--;
      }
      set->currentFrequency = set->air_SW_station.stations[set->air_SW_station.currentStationIndex];
      setFrequency(rx, set->currentFrequency);
    }
    if(set->currentBandType == FM_BAND_TYPE){
      clearRDSbuffer();
      if(set->air_FM_station.currentStationIndex == 0){
        set->air_FM_station.currentStationIndex = count;
      } else {
        set->air_FM_station.currentStationIndex--;
      }
      set->currentFrequency = set->air_FM_station.stations[set->air_FM_station.currentStationIndex];
      setFrequency(rx, set->currentFrequency);
    }
  }
  else if(set_data->eDataDescription == UP_SEEK){
    ESP_LOGD(TAG, "******* Air Radio - UP_SEEK = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    //scan_radio_band(rx, set);
    seek_radio_band(rx, set);
    if(set->currentBandType == FM_BAND_TYPE) {
      set->currentFrequency = set->air_FM_station.stations[0];
      setFrequency(rx, set->currentFrequency);
    }
    else if(set->currentBandType == LW_BAND_TYPE) {
      set->currentFrequency = set->air_LW_station.stations[0];
      setFrequency(rx, set->currentFrequency);
    }
    else if(set->currentBandType == MW_BAND_TYPE) {
      set->currentFrequency = set->air_MW_station.stations[0];
      setFrequency(rx, set->currentFrequency);
    }
    else if(set->currentBandType == SW_BAND_TYPE) {
      set->currentFrequency = set->air_SW_station.stations[0];
     setFrequency(rx, set->currentFrequency);
    }
  }
  else if(set_data->eDataDescription == AGCGAIN){
    ESP_LOGD(TAG, "******* Air Radio - AGCGAIN = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    set->onoffAGCgain = set_data->ucValue;
    setAutomaticGainControl(rx, set->onoffAGCgain, set->currentAGCgain); //MaxAGCgainFM
  }
  else if(set_data->eDataDescription == SLIDER_AGC){
    ESP_LOGD(TAG, "******* Air Radio - SLIDER_AGC = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    set->currentAGCgain = set_data->ucValue;
    setAutomaticGainControl(rx, set->onoffAGCgain, set->currentAGCgain); //MaxAGCgainFM
  }
  else if(set_data->eDataDescription == SLIDER_VOL){
    ESP_LOGD(TAG, "******* Air Radio - SLIDER_VOL = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    set->currentVOL = set_data->ucValue*0.63;
    setVolume(rx, set->currentVOL);
  }
  else if(set_data->eDataDescription == SET_FREQ){
    ESP_LOGD(TAG, "******* Air Radio - SET_FREQ = %d, ucValue = %d", set_data->eDataDescription, set_data->ucValue);
    set->currentFrequency = set_data->ucValue;
    if((set->currentBandType == LW_BAND_TYPE) || (set->currentBandType == MW_BAND_TYPE)){
      //**************
      setFrequency(rx, set->currentFrequency);
      setAM(rx, band[set->currentBandType].minimumFreq, band[set->currentBandType].maximumFreq, set->currentFrequency, stepAM[set->currentBandType].idx);
    }
    if(set->currentBandType == SW_BAND_TYPE){
      //**************
      setFrequency(rx, set->currentFrequency);
      setAM(rx, band[set->currentBandType].minimumFreq, band[set->currentBandType].maximumFreq, set->currentFrequency, stepAM[set->currentBandType].idx);
    }
    if(set->currentBandType > 3){
      //**************
      setFrequency(rx, set->currentFrequency);
      setAM(rx, band[set->currentBandType].minimumFreq, band[set->currentBandType].maximumFreq, set->currentFrequency, stepAM[set->currentBandType].idx);
      //Добавить для SSB выбор по модуляции
    }
    if(set->currentBandType == FM_BAND_TYPE){
      clearRDSbuffer();
      //**************
      setFrequency(rx, set->currentFrequency);
      setFM(rx, band[FM_BAND_TYPE].minimumFreq, band[FM_BAND_TYPE].maximumFreq, set->currentFrequency, stepFM[set->currentStepFM].idx);
      //RDS
      RdsInit();
      setRdsConfig(rx, 1, 2, 2, 2, 2);// 3,2,3,3,3  1, 2, 2, 2, 2 3, 3, 3, 3, 3 
      setFifoCount(rx, 1);  
    }
    setVolume(rx, set->currentVOL);
    // Init Digital Output
    digitalOutputSampleRate(rx, SAMPLE_RATE);
    delay_ms(200);
    digitalOutputFormat(rx, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
    delay_ms(200);

  }
}

/**
 * @brief Заполняет структуру GUI и конфигурации текущими параметрами радиоприёмника.
 *
 * @param rx        Указатель на структуру радиомодуля SI4735
 * @param get_data  Указатель на структуру данных для отображения на GUI
 * @param get       Указатель на структуру конфигурации радиомодуля
 *
 * Получает текущую частоту, качество сигнала, режим (FM/AM/SSB), стерео/моно,
 * диапазон, индекс станции, SNR, RSSI, шаг частоты, полосу пропускания, данные RDS
 * и другие параметры, и записывает их в структуру get_data для GUI.
 */
void get_radio(SI4735_t *rx, Data_Boombox_GUI_t *get_data, air_config_t *get)
{
  char strfreq[10];

  uint8_t n = 5, d = 3;
  
  ESP_LOGD(TAG, "get_radio();"); // Логирование вызова функции
  // Получение текущей частоты и качества сигнала
  get->currentFrequency = getFrequency(rx);
  getCurrentReceivedSignalQuality(rx,0);

  // Определение диапазона (FM или AM/SSB) и заполнение соответствующих полей
  if (isCurrentTuneFM(rx))
	{
  // Форматирование частоты для FM диапазона
  if (get->currentFrequency < 10000)
    {
      n = 4;
      d = 2;
    }
    get_data->ucFreq = get->currentFrequency;
    convertToChar(get->currentFrequency, strfreq, n, d, '.',true);
    get_data->vcFreqRange = "MHz";

  // Определение режима стерео/моно для FM
  if( getCurrentPilot(rx) == 0 )
    {
      get_data->vcStereoMono = " mono ";
    } 
    else
    {
      get_data->vcStereoMono = "stereo";
    }
  get_data->vcBand = " FM "; // Диапазон
  get_data->ucStationIDx = get->air_FM_station.currentStationIndex+1; // Индекс станции FM
	}
  else {
    // Для AM/SSB диапазонов
  // Форматирование частоты для AM/SSB диапазонов
  if (get->currentBandType == MW_BAND_TYPE || get->currentBandType == LW_BAND_TYPE || get->currentBandType == SW_BAND_TYPE){
      convertToChar(get->currentFrequency, strfreq, 5, 0, '.', true);
    }
    else {
      convertToChar(get->currentFrequency, strfreq, 5, 2, '.', true);
    }
    get_data->ucFreq = get->currentFrequency;
    get_data->vcFreqRange = "KHz";
  // Определение модуляции: AM или SSB (USB/LSB)
  //get_data->vcStereoMono = "      ";
  if(get->currentMod == 0) // AM (AM modulation) mode
    {
      get_data->vcStereoMono = " AM";
    }
    else // SSB (LSB, USB modulation) mode
    {
      if(get->currentMod == 1)
      {
        get_data->vcStereoMono = " USB ";
      }
      else
      {
        get_data->vcStereoMono = " LSB ";
      }
    }
  }
  // Установка диапазона и индекса станции для MW/LW/SW
  if(get->currentBandType == MW_BAND_TYPE){
    get_data->vcBand = " MW ";
    get_data->ucStationIDx = get->air_MW_station.currentStationIndex+1;
  }
  if(get->currentBandType == LW_BAND_TYPE){
    get_data->vcBand = " LW ";
    get_data->ucStationIDx = get->air_LW_station.currentStationIndex+1;
  }
  if(get->currentBandType == SW_BAND_TYPE){
    get_data->vcBand = " SW ";
    get_data->ucStationIDx = get->air_SW_station.currentStationIndex+1;
  }
  if(get->currentBandType > 3){
    get_data->vcBand = band[get->currentBandType].bandName;
  }
  // Получение SNR и RSSI
  get_data->ucSNR = getCurrentSNR(rx);
  get_data->ucRSSI = getCurrentRSSI(rx);
  /*
   * Вывод данных о шаге частоты (vcStep) и полосе пропускания (vcBW)
   * в зависимости от типа диапазона частот
   */
  if (get->currentBandType == MW_BAND_TYPE || get->currentBandType == LW_BAND_TYPE || get->currentBandType == SW_BAND_TYPE){ 
    get_data->vcStep = (char *)stepAM[get->currentStepAM].desc;  // Шаг частоты AM
    get_data->vcBW = (char *)bandwidthAM[get->BandWidthAM];      // Полоса пропускания AM
  }
  else if( get->currentBandType > 3){
    get_data->vcStep = (char *)stepAM[get->currentStepAM].desc;  
    if(get->currentMod == 0) // AM (AM modulation) mode
    {
      get_data->vcBW = (char *)bandwidthAM[get->BandWidthAM];
    }
    else // SSB (LSB, USB modulation) mode
    {
       get_data->vcBW = (char *)bandwidthSSB[get->BandWidthSSB];
    }
  }
  else if(get->currentBandType == FM_BAND_TYPE ){
    get_data->vcBW = (char *)bandwidthFM[get->BandWidthFM];      // Полоса пропускания FM
    get_data->vcStep =  (char *)stepFM[get->currentStepFM].desc; // Шаг частоты FM
  }
  get_data->ucBand = get->currentBandType; // Тип диапазона для GUI

  // Проверка и получение данных RDS
  rdsTime = checkRDS(rx);
  if(rdsTime == 0){
    ESP_LOGD(TAG, " No data RDS  ");
    get_data->vcRDSdata = "   ";
  }
  else{
    get_data->vcRDSdata = rdsTime;
  }
  
  get_data->eModeBoombox = eAir; // Установка режима работы
  get_data->State = true;         // Флаг валидности данных

}

void clearRDSbuffer(){
  free(dataRDS);
  rdsTime = stationName = stationInfo = programInfo = NULL;
  statusRDS = 1;
}

//=======================================================================================
void init_air_player(BoomBox_config_t  *init_air_config)
{
    ESP_LOGI(TAG, "init_air_player(BoomBox_config_t  *init_air_config)");  
  
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
    // ************* Si4735 init ********************* 
    // init  i2c шины
    ESP_ERROR_CHECK(i2c_master_init());
    // Выделяем память для структуры SI4735
    rx_radio = (SI4735_t*)malloc(sizeof(SI4735_t));
    if (rx_radio == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for SI4735 control structure");
        return;
    }
    radio_init(rx_radio, &init_air_config->air_radio_config);
    // ***********************************************************************************************************
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
    /*
    i2s_stream_cfg_t i2s_cfg = {
        .type = AUDIO_STREAM_WRITER,
        .transmit_mode = I2S_COMM_MODE_STD,
        
        // Конфигурация канала
        .chan_cfg = {
            .id = I2S_NUM_0,
            .role = I2S_ROLE_MASTER,
            .dma_desc_num = 3,
            .dma_frame_num = 312,
            .auto_clear = 1,
        },
        
        // Стандартная конфигурация I2S
        .std_cfg = {
            // Конфигурация тактирования
            .clk_cfg = {
                .sample_rate_hz = 44100,
                .clk_src = I2S_CLK_SRC_DEFAULT,
                .mclk_multiple = I2S_MCLK_MULTIPLE_256,
            },
            
            // Конфигурация слотов данных
            .slot_cfg = {
                .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
                .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
                .slot_mode = I2S_SLOT_MODE_STEREO,
                .slot_mask = (I2S_SLOT_MODE_STEREO == I2S_SLOT_MODE_MONO) ? 
                            I2S_STD_SLOT_RIGHT : I2S_STD_SLOT_BOTH,
                .ws_width = I2S_DATA_BIT_WIDTH_16BIT,
                .ws_pol = 0,
                .bit_shift = 1,
                .msb_right = (I2S_DATA_BIT_WIDTH_16BIT <= I2S_DATA_BIT_WIDTH_16BIT) ? 1 : 0,
            },
            
            // Конфигурация GPIO
            .gpio_cfg = {
                .invert_flags = {
                    .mclk_inv = 0,
                    .bclk_inv = 0,
                },
            },
        },
        
        // Дополнительные параметры
        .use_alc = 0,
        .volume = 0,
        .out_rb_size = (8 * 1024),
        .task_stack = (3584),
        .task_core = (0),
        .task_prio = (23),
        .stack_in_ext = 0,
        .multi_out_num = 0,
        .uninstall_drv = 1,
        .need_expand = 0,
        .expand_src_bits = I2S_DATA_BIT_WIDTH_16BIT,
        .buffer_len = (3600),
    };
    */    
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
    evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[5.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[ 6 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);
 
    ESP_LOGI(TAG, "[ 7 ] Listen for all pipeline events");

}

void deinit_air_player()
{
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
    radio_deinit(rx_radio);

}

// Вывод статуса радио
void statusRadio(SI4735_t * rx)
{ 
  getCurrentReceivedSignalQuality(rx, 0 );
  printf("Frequency = %d Hz, Volume = %d, SNR = %d dB, RSSI = %d dBμV", 
            getFrequency(rx), getVolume(rx), getCurrentSNR(rx), getCurrentRSSI(rx));
}

// Основная функция проигрывателя AIR радио
void air_player( Data_GUI_Boombox_t *xDataGUI,  Data_Boombox_GUI_t *xDataBoomBox)
{
    //print_data_gui_detailed_to_console(xDataGUI);// Вывод данных xDataGUI в консоль для отладки
    if(xDataGUI->State == true){
      ESP_LOGW(TAG, "xDataGUI.State: %d", xDataGUI->State);
      set_radio(rx_radio, xDataGUI, &xBoomBox_config.air_radio_config);
      xDataGUI->State = false;
    }
    
    get_radio(rx_radio, xDataBoomBox, &xBoomBox_config.air_radio_config);
    //print_data_boombox_to_console(xDataBoomBox);// Вывод данных xDataBoomBox в консоль для отладки

}
