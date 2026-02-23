/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <string.h>
#include "gui.h"

#include "events_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_freertos_hooks.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "lvgl.h"
#include "custom.h"
#include "custom.h"

#include "encoder.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"

#include "disp_driver.h"
#include "touch_driver.h"
#include "commons.h"
#include "lv_tc.h"
#include "lv_tc_screen.h"
#include "esp_nvs_tc.h"
 /*********************
 *      DEFINES
 *********************/
// Тег для логирования
#define TAG "gui"
// Пины энкодера, определённые в конфиге
#define RE_A_GPIO   CONFIG_RE_A_GPIO    // 16
#define RE_B_GPIO   CONFIG_RE_B_GPIO    // 17
#define RE_BTN_GPIO CONFIG_RE_BTN_GPIO  // 9
// Длина очереди событий энкодера
#define EV_QUEUE_LEN 5

#define LV_LVGL_H_INCLUDE_SIMPLE 1
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**********************
 *  VARIABLES
 **********************/
Data_Boombox_GUI_t xResivedBoomboxToGUI; // Переменная для хранения полученных данных от Boombox к GUI
extern Data_GUI_Boombox_t xTransmitGUItoBoombox; // Внешняя Переменная для передачи данных от GUI к Boombox
// Указатель на устройство ввода — тачпад
lv_indev_t * indev_touchpad;
static lv_indev_drv_t touch_indev_drv;

// Указатель на устройство ввода — энкодер
lv_indev_t * indev_encoder;
static lv_indev_drv_t encoder_indev_drv;

// Глобальная структура интерфейса
lv_ui guider_ui;
// Семафор для синхронизации работы с lvgl
SemaphoreHandle_t xGuiSemaphore;

// Дескрипторы задач
TaskHandle_t TaskTFT;
TaskHandle_t TaskAudio;
TaskHandle_t TaskRadio;
TaskHandle_t *TaskBT;

extern int slider_vol;
rotary_encoder_event_t e;
int32_t new_value;

// Очередь событий энкодера и структура энкодера
static QueueHandle_t event_queue;
static rotary_encoder_t re;


/**************************************************************************************
*                             Прототипы функций
**************************************************************************************/
// Инициализация энкодера и очереди событий
void encoder_init(void) {
    // Создаём очередь событий для энкодера
    event_queue = xQueueCreate(EV_QUEUE_LEN, sizeof(rotary_encoder_event_t));
    // Инициализация библиотеки энкодера
    ESP_ERROR_CHECK(rotary_encoder_init(event_queue));
    // Добавляем энкодер
    memset(&re, 0, sizeof(rotary_encoder_t));
    re.pin_a = RE_A_GPIO;
    re.pin_b = RE_B_GPIO;
    re.pin_btn = RE_BTN_GPIO;
    ESP_ERROR_CHECK(rotary_encoder_add(&re));
}

// Чтение состояния энкодера и обработка событий
void encoder_read(lv_indev_drv_t * drv, lv_indev_data_t * data) {
    // Получаем событие из очереди
    xQueueReceive(event_queue, &e, 5);
    
    if (guider_ui.pageAirradio_slider_vol != NULL )
    {
        slider_vol = lv_slider_get_value(guider_ui.pageAirradio_slider_vol); 
    }

    switch (e.type)
    {
        case RE_ET_BTN_PRESSED:
            //ESP_LOGD(TAG, "Button pressed");
            data->state = LV_INDEV_STATE_PRESSED;
            break;
        case RE_ET_BTN_RELEASED:
            //ESP_LOGD(TAG, "Button released");
            break;
        case RE_ET_BTN_CLICKED:
            if(button_pres == 1 )
            {
                lv_obj_add_flag(guider_ui.pageAirradio_cont_vol, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.pageAirradio_slider_vol, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.pageAirradio_textprogress_vol, LV_OBJ_FLAG_HIDDEN);
                button_pres = 0;
            }
            else{
                lv_obj_clear_flag(guider_ui.pageAirradio_cont_vol, LV_OBJ_FLAG_HIDDEN);
		        lv_obj_clear_flag(guider_ui.pageAirradio_slider_vol, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.pageAirradio_textprogress_vol, LV_OBJ_FLAG_HIDDEN);
                button_pres = 1;
            }
            break;
        case RE_ET_BTN_LONG_PRESSED:
            rotary_encoder_disable_acceleration(&re);
            break;
        case RE_ET_CHANGED:  
            if(button_pres == 1 )// Регулировка громкости
            {
                if( e.diff != data->enc_diff){
                    data->enc_diff = e.diff;
                    new_value = slider_vol + data->enc_diff;                
                }
                if(new_value < 0) new_value = 0;
                if(new_value > 100) new_value = 100;
                lv_slider_set_value(guider_ui.pageAirradio_slider_vol, new_value, true);
                lv_textprogress_set_value(guider_ui.pageAirradio_textprogress_vol, new_value);
                xTransmitGUItoBoombox.eDataDescription = eslider_vol;
                xTransmitGUItoBoombox.ucValue = slider_vol;
                xTransmitGUItoBoombox.State = true;
                break;
            }
            else{                // Перестройка станций
                if ( e.diff > 0 ) {
                    data->key = LV_KEY_UP;
                    data->enc_diff = e.diff;
                    xTransmitGUItoBoombox.eDataDescription = eStepUP;
                    xTransmitGUItoBoombox.ucValue = 1;
                    xTransmitGUItoBoombox.State = true;
                }
                else if ( e.diff < 0 ){
                    data->key = LV_KEY_DOWN;
                    data->enc_diff = e.diff ;
                    xTransmitGUItoBoombox.eDataDescription = eStepDown;
                    xTransmitGUItoBoombox.ucValue = 1;
                    xTransmitGUItoBoombox.State = true;   
                }
            }
            data->enc_diff = e.diff = 0;
        default:
            break;
    }
    e.type = -1;     
}

/**
 * @brief Преобразует число в строку (char array)
 * @details Используется для экономии памяти по сравнению с sprintf и аналогами.
 * @param value  Число для преобразования
 * @param strValue Массив символов для результата
 * @param len Длина итоговой строки
 * @param dot Позиция разделителя
 * @param separator Символ-разделитель ('.' или ',')
 * @param remove_leading_zeros Удалять ведущие нули (по умолчанию true)
 */
void ConvertToChar(uint16_t value, char *strValue, uint8_t len, uint8_t dot, uint8_t separator, bool remove_leading_zeros)
{
    char d;
    for (int i = (len - 1); i >= 0; i--)
    {
        d = value % 10;
        value = value / 10;
        strValue[i] = d + 48;
    }
    strValue[len] = '\0';
    if (dot > 0)
    {
        for (int i = len; i >= dot; i--)
        {
            strValue[i + 1] = strValue[i];
        }
        strValue[dot] = separator;
    }

    if (remove_leading_zeros)
    {
        if (strValue[0] == '0')
        {
            strValue[0] = ' ';
            if (strValue[1] == '0')
                strValue[1] = ' ';
        }
    }
}

// Инициализация интерфейса и пользовательских событий
void awgui(void)
{
    setup_ui(&guider_ui);
    events_init(&guider_ui);
    custom_init(&guider_ui);
}

// Обновление интерфейса на основе новых данных от Boombox
void awgui_reload(Data_Boombox_GUI_t data){

    char valFreq[10];
    char tempebandIDx[4];
    static ModeBoombox_t last_mode = (ModeBoombox_t)(-1);
    lv_obj_t *active_scr = NULL;

    if (last_mode != data.eModeBoombox) {
        page_change(&guider_ui, data.eModeBoombox);
        last_mode = data.eModeBoombox;
    }
    active_scr = lv_scr_act();
    /*    */

    switch(data.eModeBoombox){
        case eAir:
            if (guider_ui.pageAirradio == NULL || !lv_obj_is_valid(guider_ui.pageAirradio) || active_scr != guider_ui.pageAirradio) {
                break;
            }
            if(data.eAirDescription.ucBand == 3){
                //tempebandIDx = data.ucBand;
                ConvertToChar(data.eAirDescription.ucFreq, valFreq, 5, 3, '.', true);
            }
            else {
                ConvertToChar(data.eAirDescription.ucFreq, valFreq, 5, 0, '.', true);
            }
            // Проверка, что объект был успешно создан
            if (guider_ui.pageAirradio_label_Freq != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_Freq))
            {
                lv_label_set_text(guider_ui.pageAirradio_label_Freq, valFreq);
            }
            if (guider_ui.pageAirradio_label_FreqRange  != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_FreqRange)){
                lv_label_set_text(guider_ui.pageAirradio_label_FreqRange, data.eAirDescription.vcFreqRange);
            }
            if (guider_ui.pageAirradio_label_mono != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_mono)){
                lv_label_set_text_fmt(guider_ui.pageAirradio_label_mono , data.eAirDescription.vcStereoMono);
            }
            if (guider_ui.pageAirradio_label_rssi_val != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_rssi_val))
            {
                lv_label_set_text_fmt(guider_ui.pageAirradio_label_rssi_val,"%3d" , data.eAirDescription.ucRSSI);
            }
            if (guider_ui.pageAirradio_label_snr_val  != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_snr_val)){
                lv_label_set_text_fmt(guider_ui.pageAirradio_label_snr_val,"%3d" , data.eAirDescription.ucSNR);
            }
            if (guider_ui.pageAirradio_label_wb_val != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_wb_val))
            {
               lv_label_set_text(guider_ui.pageAirradio_label_wb_val, data.eAirDescription.vcBW);
            }
            if (guider_ui.pageAirradio_label_step_val != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_step_val))
            {
               lv_label_set_text(guider_ui.pageAirradio_label_step_val, data.eAirDescription.vcStep);
            }
            if (guider_ui.pageAirradio_RDS != NULL && lv_obj_is_valid(guider_ui.pageAirradio_RDS))
            {
                lv_label_set_text(guider_ui.pageAirradio_RDS, data.eAirDescription.vcRDSdata);     // Текстовая информация от RDS
            }
            if (guider_ui.pageAirradio_label_band  != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_band))
            {
                lv_label_set_text(guider_ui.pageAirradio_label_band, data.eAirDescription.vcBand); // Текстовая информация от Band Name;
            }
            if (guider_ui.pageAirradio_label_stationIDx != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_stationIDx))
            {
                ConvertToChar(data.eAirDescription.ucStationIDx, tempebandIDx, 2, 0, '.', true);
                lv_label_set_text(guider_ui.pageAirradio_label_stationIDx, tempebandIDx); // Текстовая информация от Station ID
            }
            if (guider_ui.pageAirradio_label_air_id != NULL && lv_obj_is_valid(guider_ui.pageAirradio_label_air_id))
            {
                ESP_LOGD(TAG, "Web Radio IP: %s", data.eWebDescription.vcIP);
                lv_label_set_text(guider_ui.pageAirradio_label_air_id, data.eWebDescription.vcIP); // IP с источником (STA/AP)
            }
        break;
        case eBT:
            ESP_LOGD(TAG, "************** awgui_reload - eBT");
            if (guider_ui.Bluetooth == NULL || !lv_obj_is_valid(guider_ui.Bluetooth) || active_scr != guider_ui.Bluetooth) {
                break;
            }
            if (guider_ui.Bluetooth_label_bt_song != NULL && lv_obj_is_valid(guider_ui.Bluetooth_label_bt_song))
            {
                lv_label_set_text(guider_ui.Bluetooth_label_bt_song, data.eBtDescription.vcTitle); // Текстовая информация от 
            }
            if (guider_ui.Bluetooth_label_bt_artist != NULL && lv_obj_is_valid(guider_ui.Bluetooth_label_bt_artist))
            {
                lv_label_set_text(guider_ui.Bluetooth_label_bt_artist, data.eBtDescription.vcArtist); // Текстовая информация от 
            }
            if (guider_ui.Bluetooth_label_bt_album != NULL && lv_obj_is_valid(guider_ui.Bluetooth_label_bt_album))
            {
                lv_label_set_text(guider_ui.Bluetooth_label_bt_album, data.eBtDescription.vcAlbum); // Текстовая информация альбома
            }
        break;
        case eWeb:
            ESP_LOGI(TAG, "************* awgui_reload - eWeb");
            if (guider_ui.pageWebradio == NULL || !lv_obj_is_valid(guider_ui.pageWebradio) || active_scr != guider_ui.pageWebradio) {
                break;
            }
            if (guider_ui.pageWebradio_label_web_id != NULL && lv_obj_is_valid(guider_ui.pageWebradio_label_web_id))
            {
                ESP_LOGI(TAG, "Web Radio IP: %s", data.eWebDescription.vcIP);
                lv_label_set_text(guider_ui.pageWebradio_label_web_id, data.eWebDescription.vcIP); // IP с источником (STA/AP)
            }
            if (guider_ui.pageWebradio_label_web_title != NULL && lv_obj_is_valid(guider_ui.pageWebradio_label_web_title))
            {
                const char *station = (data.eWebDescription.vcStation != NULL) ? data.eWebDescription.vcStation : "Unknown";
                lv_label_set_text_fmt(guider_ui.pageWebradio_label_web_title, "%s", station); // Текстовая информация
            }
            if (guider_ui.pageWebradio_idWeb != NULL && lv_obj_is_valid(guider_ui.pageWebradio_idWeb))
            {
                lv_label_set_text_fmt(guider_ui.pageWebradio_idWeb, "%d", data.eWebDescription.ucStationIDx); // Текущая станция
            }
        break;
        default:
            ESP_LOGI(TAG, "************** awgui_reload - default");
        break;
    }
}

/* Колбэк, вызываемый по завершении калибровки тачскрина */
void tc_finish_cb(lv_event_t *event) {
   
    /* Загружаем основное приложение */
    awgui();

};

// Callback для esp_timer (вызывается в task контексте, безопасно для flash)
static void lv_tick_timer_cb(void *arg){
   (void)arg;
   lv_tick_inc(1); // 1ms
}

/*********************************************************************************************
*               Инициализация TFT и калибровка тачскрина
**********************************************************************************************/
void initCalcTFT( )
{
   // Инициализируем структуру данных от Boombox к GUI
   memset(&xResivedBoomboxToGUI, 0, sizeof(Data_Boombox_GUI_t));
   strncpy(xResivedBoomboxToGUI.eWebDescription.vcIP, "N/A", sizeof(xResivedBoomboxToGUI.eWebDescription.vcIP) - 1);
   
   lv_init();          
   lvgl_driver_init(); 

   /* Example for 1) */
   static lv_disp_draw_buf_t draw_buf;
   lv_color_t *buf1 = heap_caps_malloc((DLV_HOR_RES_MAX * DLV_VER_RES_MAX/10) * sizeof(lv_color_t), MALLOC_CAP_DMA);
   lv_color_t *buf2 = heap_caps_malloc((DLV_HOR_RES_MAX * DLV_VER_RES_MAX/10) * sizeof(lv_color_t), MALLOC_CAP_DMA);

   lv_disp_draw_buf_init(&draw_buf, buf1, buf2, (DLV_HOR_RES_MAX * DLV_VER_RES_MAX/10)); /*Initialize the display buffer*/
   /*---------------------------------------------------------------------------------------------------------------
   * Display
   * --------------------------------------------------------------------------------------------------------------*/
   static lv_disp_drv_t disp_drv;         /*A variable to hold the drivers. Must be static or global.*/
   lv_disp_drv_init(&disp_drv);           /*Basic initialization*/
   disp_drv.draw_buf = &draw_buf;         /*Set an initialized buffer*/
   disp_drv.flush_cb = disp_driver_flush; /*Set a flush callback to draw to the display*/
   disp_drv.hor_res = DLV_HOR_RES_MAX;                /*Set the horizontal resolution in pixels*/
   disp_drv.ver_res = DLV_VER_RES_MAX;                /*Set the vertical resolution in pixels*/
   lv_disp_drv_register(&disp_drv);       /*Register the driver and save the created display objects*/

    /*Initialize the (dummy) input device driver*/
    /*------------------------------------------------------------------------------------------------------------------
     * Encoder
    * ----------------------------------------------------------------------------------------------------------------*/ 
    //Initialize your encoder if you have
    encoder_init();
    //Register a encoder input device
    lv_indev_drv_init(&encoder_indev_drv);
    encoder_indev_drv.type = LV_INDEV_TYPE_ENCODER;
    encoder_indev_drv.read_cb = encoder_read;
    indev_encoder = lv_indev_drv_register(&encoder_indev_drv);
    
    /*------------------------------------------------------------------------------------------------------------------
     * Touchpad
     * ----------------------------------------------------------------------------------------------------------------*/
    /* Initialize the calibrated touch driver. */
    lv_tc_indev_drv_init(&touch_indev_drv, touch_driver_read);// xpt2046_read
    /* Register the driver. */
    indev_touchpad = lv_indev_drv_register(&touch_indev_drv);
    /* If using NVS: Register a calibration coefficients save callback. */
    lv_tc_register_coeff_save_cb(esp_nvs_tc_coeff_save_cb);
    /* Create the calibration screen. */
    lv_obj_t *tCScreen = lv_tc_screen_create();
    /* Register a callback for when the calibration finishes. An LV_EVENT_READY event is triggered. */
    lv_obj_add_event_cb(tCScreen, tc_finish_cb, LV_EVENT_READY, NULL);
    /* If using NVS: Init NVS and check for existing calibration data. */
    /*--------------------------------------------------------------
     * 
    ----------------------------------------------------------------*/
    if(esp_nvs_tc_coeff_init()) {
        /* Данные существуют: продолжить обычное приложение, не показывая экран калибровки */
         awgui();
    } else {
        /* Нет данных: загрузите экран калибровки, выполните калибровку */
        lv_disp_load_scr(tCScreen);
        lv_tc_screen_start(tCScreen);
    }
    
    // Используем esp_timer вместо tick hook (безопасно при работе с flash)
    const esp_timer_create_args_t lv_tick_timer_args = {
        .callback = lv_tick_timer_cb,
        .name = "lv_tick"
    };
    esp_timer_handle_t lv_tick_timer;
    esp_timer_create(&lv_tick_timer_args, &lv_tick_timer);
    esp_timer_start_periodic(lv_tick_timer, 1000); // 1ms = 1000us
}
/**************************************************************************************
 *                            Основная задача GUI
 *              Главная задача для работы GUI c калибровкой тачскрина
 **************************************************************************************/
void task_gui_calibrate(void *arg){

   // Создаём семафор для lvgl
    xGuiSemaphore = xSemaphoreCreateMutex();
    initCalcTFT();

    while (1)
    {
        /* Задержка на 20 мс - оптимальный баланс для 50 Hz обновления GUI */
        vTaskDelay(pdMS_TO_TICKS(20));
        
        /* Захватываем семафор с таймаутом (не блокируем бесконечно) */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, pdMS_TO_TICKS(50)))
        {
            lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
        
        /* Даём шанс другим задачам выполниться */
        taskYIELD();
        
       /*****************************************************************************************
            Отправка данных от GUI к Boombox очередь xGuiToBoomboxQueue
        ******************************************************************************************/
       if(xTransmitGUItoBoombox.State == true){      
            /* Не блокируем - если очередь полна, пропускаем */           
            if(pdTRUE != xQueueSend(xGuiToBoomboxQueue, &xTransmitGUItoBoombox, 0)) 
            {
                ESP_LOGW(TAG, "xGuiToBoomboxQueue full, skipping");
            }
            xTransmitGUItoBoombox.State = false;
        }
        /*****************************************************************************************
            Получаем данные из очереди xBoomboxToGuiQueue
            и обновляем интерфейс с помощью awgui_reload
         ******************************************************************************************/       
 
        /* Не блокируем - проверяем очередь без ожидания */
        if(pdTRUE == xQueueReceive(xBoomboxToGuiQueue, &xResivedBoomboxToGUI, 0))
        {
            awgui_reload(xResivedBoomboxToGUI);
        }
    }
}