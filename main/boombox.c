#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "bt_task.h"
#include "http_task.h"
#include "air_task.h"

#include "commons.h"

static const char *TAG = "BOOMBOX_TASK";

Data_GUI_Boombox_t xResiveGUItoBoombox;// Переменная для хранения полученных данных от GUI к Boombox
Data_Boombox_GUI_t xTransmitBoomboxToGUI; // Переменная для передачи данных от Boombox к GUI

extern QueueHandle_t xGuiToBoomboxQueue;
extern QueueHandle_t xBoomboxToGuiQueue;

// Глобальная переменная для выбора источника
//extern audio_source_t g_current_source; // Изначально запускаем AIR радио;

void boombox_task(void *pvParameters)
{

    //g_current_source = SOURCE_AIR;
    
    while (1) {
        /*****************************************************************************************
        *   Receive data from GUI
        ******************************************************************************************/
        if(pdTRUE == xQueueReceive(xGuiToBoomboxQueue, &xResiveGUItoBoombox, pdPASS))
        {
            // Обработка полученных данных
            g_current_source = (audio_source_t)xResiveGUItoBoombox.eModeBoombox;
        }
        /******************************************************************************************
         * Обработка данных от GUI 
         ******************************************************************************************/
        if (g_current_source == SOURCE_BLUETOOTH) {
            ESP_LOGI(TAG, "Switching to Bluetooth player");
            //bt_player();
        } else if (g_current_source == SOURCE_HTTP) {
            ESP_LOGI(TAG, "Switching to HTTP player");
           // http_player();
        } else if (g_current_source == SOURCE_AIR) {
            ESP_LOGI(TAG, "Switching to AIR radio player");
            //air_player();
        } else {
            ESP_LOGW(TAG, "Unknown source, waiting...");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        /******************************************************************************************
         *  Отправка данных от Boombox к  GUI очередь xBoomboxToGuiQueue
        ******************************************************************************************/
       if(xTransmitBoomboxToGUI.State == true ){
            if(pdTRUE == xQueueSend(xBoomboxToGuiQueue, &xTransmitBoomboxToGUI, pdPASS))
            {
                ESP_LOGE(TAG, "Error to xBoomboxToGuiQueue");
            }
            xTransmitBoomboxToGUI.State = false;
       }

       // После завершения работы плеера можно добавить логику ожидания или переключения
       vTaskDelay(pdMS_TO_TICKS(50));
    }
}