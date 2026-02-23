// Подключение заголовочного файла с определениями структур и функций
#include "commons.h"
#include "esp_log.h"

static const char *TAG = "COMMON";

// Очередь для передачи данных от GUI к Boombox
QueueHandle_t xGuiToBoomboxQueue = NULL;
// Очередь для передачи данных от Boombox к GUI
QueueHandle_t xBoomboxToGuiQueue = NULL;

// Инициализация очереди для передачи данных от GUI к Boombox
void gui_boombox_queue_init(void)
{
    xGuiToBoomboxQueue = xQueueCreate(2, sizeof(Data_GUI_Boombox_t));
    if( xGuiToBoomboxQueue == NULL )
	{
        ESP_LOGE(TAG, "Error create xGuiToBoomboxQueue");
    }
}

// Инициализация очереди для передачи данных от Boombox к GUI
void boombox_gui_queue_init(void)
{
    xBoomboxToGuiQueue = xQueueCreate(10, sizeof(Data_Boombox_GUI_t));
    if( xBoomboxToGuiQueue == NULL )
	{
        ESP_LOGE(TAG, "Error create xBoomboxToGuiQueue");
    }
}
