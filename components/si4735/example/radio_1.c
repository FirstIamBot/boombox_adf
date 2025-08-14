#include <stdio.h>
#include "si4735.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "air_speaker.h"
//#include "audio_renderer.h"
#include "common.h"

#define AUDIO_OUTPUT_MODE CONFIG_AUDIO_OUTPUT_MODE

static const char *TAG = "MAIN  ";

static renderer_config_t *air_renderer_config()
{
    renderer_config_t *renderer_config = calloc(1, sizeof(renderer_config_t));

    renderer_config->bit_depth = I2S_BITS_PER_SAMPLE_16BIT;
    renderer_config->i2s_num = I2S_NUM_0;
    renderer_config->sample_rate = 48000; //44100
    renderer_config->sample_rate_modifier = 1.0;
    renderer_config->output_mode = AUDIO_OUTPUT_MODE;
    renderer_config->pin_cnfg.bck_io_num = CONFIG_I2S_BCK_PIN;
    renderer_config->pin_cnfg.ws_io_num = CONFIG_I2S_LRCK_PIN;
    renderer_config->pin_cnfg.data_in_num = CONFIG_I2S_DATA_PIN;
    renderer_config->pin_cnfg.data_out_num = -1;

    if(renderer_config->output_mode == DAC_BUILT_IN) {
        renderer_config->bit_depth = I2S_BITS_PER_SAMPLE_16BIT;
    }

    return renderer_config;
}


void task_handler(void *pvParameter)
{
    // Hardware ESP32 init
    ESP_ERROR_CHECK(i2c_master_init());
    // Стуктура данных для сохранения временных(текущих) данных
    air_config_t air_radio_config = {
        .currentVOL = 2,  // 0 = Minimum  64 - Maximum
        .currentStepFM = 1, // 1 - 10 KHz, 5 - 50KHz, 10 - 100 KHz
        .currentStepAM = 1,
        .currentMod = 3, // 3 = FM
        .currentFrequency = 10490,
        .currentBandType = 3, // FM_BAND_TYPE
        .BandWidthFM = 0, // AUT-0, 110-1
        .BandWidthAM = 0, // 6kHz - 0
        .BandWidthSSB = 0, // 1.2KHz - 0
        .currentAGCgain = 20,    // 0 = Minimum attenuation (max gain) 36 - Maximum attenuation (min gain)
        .onoffAGCgain = 1,  // 0 = AGC enabled; 1 = AGC disabled
    };;

    renderer_init(air_renderer_config());
    renderer_start();

    while(1){

        ESP_LOGI(TAG, " ************************** air_speaker_start(air_radio_config);");
        air_speaker_start(&air_radio_config);
        vTaskDelay(30000); 
        ESP_LOGI(TAG, " ************************** air_speaker_stop(); **************************");
        air_speaker_stop();
        if(air_radio_config.currentVOL <= 64){
            air_radio_config.currentVOL += 1;
        }
        else{
            air_radio_config.currentVOL = 0;
        }
        vTaskDelay(500);
    }
}



void app_main(void)
{ 
    xTaskCreatePinnedToCore(task_handler, "AirRadio", 1024*3 , NULL, 1, NULL, 1);   
}