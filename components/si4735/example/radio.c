#include <stdio.h>
#include "si4735.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "si4735.h"

#include "driver/i2s.h"

#define I2S_BCK_IO 33
#define I2S_WS_IO 25
#define I2S_DI_IO 32
#define I2S_NUM 0

#define RESET_PIN CONFIG_RESET_PIN

// Стуктура данных для сохранения временных(текущих) данных
typedef struct {

    uint8_t currentBandType;    //  = FM_BAND_TYPE ; = ebandIDx (LW, MW, SW)
    uint8_t currentMod;         //  = eModIdx (AM, SSB, FM)
    uint16_t currentStepFM;     //  = eStepFM, eStepAM, eBandWSSB
    uint16_t currentStepAM;     //  = eStepFM, eStepAM, eBandWSSB
    uint16_t currentFrequency;
    uint8_t currentVOL;         // = eslider_vol
    uint8_t BandWidthFM;        // = eBandWFM
    uint8_t BandWidthAM;        // = eBandWAM
    uint8_t BandWidthSSB;       // = eBandWSSB
    uint8_t currentAGCgain;     // = eSlider_agc
    uint8_t onoffAGCgain;       // 0 = AGC enabled; 1 = AGC disabled

} air_config_t;


SI4735_t * ex_radio;
uint16_t WorkFrequency;//!<  current frequency
uint8_t currentVOL = 10;
uint16_t currentStep = 10; 

static const char *TAG = "MAIN  ";

// Стуктура данных для сохранения временных(текущих) данных
air_config_t air_radio_config;


void air_player_init(air_config_t cnfg){
   
    //getDeviceI2CAddress(RESET_PIN);
    // Use SI473X_DIGITAL_AUDIO1       - Digital audio output (SI4735 device pins: 3/DCLK, 24/LOUT/DFS, 23/ROUT/DIO )
    // Use SI473X_DIGITAL_AUDIO2       - Digital audio output (SI4735 device pins: 3/DCLK, 2/DFS, 1/DIO)
    // Use SI473X_ANALOG_DIGITAL_AUDIO - Analog and digital audio outputs (24/LOUT/ 23/ROUT and 3/DCLK, 2/DFS, 1/DIO)
    // XOSCEN_RCLK                     - Use external source clock (active crystal or signal generator)
    init_si4735(&ex_radio, RESET_PIN, -1, FM_CURRENT_MODE, SI473X_DIGITAL_AUDIO2, XOSCEN_RCLK, 0);  // Analog and digital audio outputs (LOUT/ROUT and DCLK, DFS, DIO), external RCLK
    delay_ms(500); 
    // FM Setup
    setTuneFrequencyAntennaCapacitor(&ex_radio, 0); //0 = automatic capacitor tuning. valid range is 0 to 191.  
    setFmBandwidth(&ex_radio, cnfg.BandWidthFM);      // Automatically select proper channel filter (Default)  
    setFMDeEmphasis(&ex_radio, 1);     // 1 = 50 μs Europe, Australia, Japan. 2 = 75 μs USA (default)

    setFmBlendRssiStereoThreshold(&ex_radio, 10); // 49 force stereo, set this to 0. force mono, set this to 127
    setFmBLendRssiMonoThreshold(&ex_radio, 30);

    setFmSoftMuteMaxAttenuation(&ex_radio, 10); //Default: 0x0002 Range: 
    setFmSoftMuteSnrAttenuation(&ex_radio, 6);

    setSeekFmLimits(&ex_radio, 8750, 10800);
    setSeekFmSpacing(&ex_radio, cnfg.currentStepFM); // 1(10 kHz), 5 (50 kHz), 10 (100 kHz), and 20 (200 kHz). Default is 10
    setSeekFmSrnThreshold(&ex_radio, 6);
    setSeekFmRssiThreshold(&ex_radio, 20);
    // AM Setup
    setAmBandwidth(&ex_radio, 0, cnfg.currentStepAM); // 0 = 6 kHz Bandwidth, 1 = Enable Noise Rejection Filter
    setAMDeEmphasis(&ex_radio, 1);
    setAmSoftMuteMaxAttenuation(&ex_radio, 10);
    setAMSoftMuteSnrThreshold(&ex_radio, 9);
    setSeekAmLimits(&ex_radio, 520, 1710);
    setSeekAmSrnThreshold(&ex_radio, 11);
    setSeekAmRssiThreshold(&ex_radio, 42);
    // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 10kHz) 
    setFM(&ex_radio, 8750, 10800, 8750, cnfg.currentStepFM); 
    delay_ms(300);
    // Init Digital Output
    digitalOutputSampleRate(&ex_radio, 48000);
    delay_ms(200);
    // OSIZE Dgital Output Audio Sample Precision (0=16 bits, 1=20 bits, 2=24 bits, 3=8bits).
    // OMONO Digital Output Mono Mode (0=Use mono/stereo blend ).
    // OMODE Digital Output Mode (0=I2S, 6 = Left-justified, 8 = MSB at second DCLK after DFS pulse, 12 = MSB at first DCLK after DFS pulse).
    // OFALL Digital Output DCLK Edge (0 = use DCLK rising edge, 1 = use DCLK falling edge)
    digitalOutputFormat(&ex_radio, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
    delay_ms(200);
    //RDS
    RdsInit();
    setRdsConfig(&ex_radio, 3, 3, 3, 3, 3);
    setFifoCount(&ex_radio, 1);   

    setAutomaticGainControl(&ex_radio, cnfg.onoffAGCgain, cnfg.currentAGCgain); //MaxAGCgainFM 
    setVolume(&ex_radio, cnfg.currentVOL);
}

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

void si4735task(void *pvParameter){
    
    uint8_t n = 0;
    uint8_t mode = 1;  // mode = 0 Fixed Frequency , mode = 1  Scan Frequency 
    uint8_t i=0;

    uint16_t station[50];
    uint16_t last_frequency = 0;

    // Si4735 init
    air_player_init(air_radio_config);

    while(1){
                  
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
            setFM(&ex_radio, 6400, 10800, WorkFrequency, air_radio_config.currentStepFM);
            statusRadio(&ex_radio);
            air_radio_config.currentFrequency = WorkFrequency;
            delay_ms(200);
            // Init Digital Output
            digitalOutputSampleRate(&ex_radio, 48000);
            delay_ms(200);
            // OSIZE Dgital Output Audio Sample Precision (0=16 bits, 1=20 bits, 2=24 bits, 3=8bits).
            // OMONO Digital Output Mono Mode (0=Use mono/stereo blend ).
            // OMODE Digital Output Mode (0=I2S, 6 = Left-justified, 8 = MSB at second DCLK after DFS pulse, 12 = MSB at first DCLK after DFS pulse).
            // OFALL Digital Output DCLK Edge (0 = use DCLK rising edge, 1 = use DCLK falling edge)
            digitalOutputFormat(&ex_radio, 0 /* OSIZE */, 0 /* OMONO */, 0 /* OMODE */, 0 /* OFALL*/);
            //delay_ms(200);   
            vTaskDelay(1500); 
        }
        else if (mode == 1) // scan station
        {
            ESP_LOGI(TAG, "seekNextStation = %d", seekNextStation(&ex_radio));
            getCurrentReceivedSignalQuality(&ex_radio, 0 );
            //if(getCurrentRSSI(&ex_radio) > 30 && getCurrentSNR(&ex_radio) > 9){
            if(getCurrentRSSI(&ex_radio) > 25 || getCurrentSNR(&ex_radio) > 9){
                station[i] = getFrequency(&ex_radio);
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
    }
}

void app_main(void)
{
    // Hardware ESP32 init

    ESP_ERROR_CHECK(i2c_master_init());

    air_radio_config.currentVOL = 12;   // 0 = Minimum  64 - Maximum
    air_radio_config.currentStepFM = 1; // 1 - 10 KHz, 5 - 50KHz, 10 - 100 KHz
    air_radio_config.currentStepAM = 1;
    air_radio_config.currentMod = 3;  // 3 - FM
    air_radio_config.currentFrequency = 10490;
    air_radio_config.currentBandType = 3; // FM_BAND_TYPE
    air_radio_config.BandWidthFM = 0; // AUT-0, 110-1
    air_radio_config.BandWidthAM = 0; // 6kHz - 0
    air_radio_config.BandWidthSSB = 0; // 1.2KHz - 0
    air_radio_config.currentAGCgain = 20;    // 0 = Minimum attenuation (max gain) 36 - Maximum attenuation (min gain)
    air_radio_config.onoffAGCgain = 1;  // 0 = AGC enabled; 1 = AGC disabled

    xTaskCreatePinnedToCore(&si4735task, "si4735 test", 1024 * 8, NULL, 1, NULL, 0);  
}