#ifndef _INCLUDE_COMMON_H
#define _INCLUDE_COMMON_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
/***********************************************
 * 
 * For air_speaker
 * 
 * ******************************************* */
#define BANDIDx     1
#define MODIDx      2
#define STEPFM      3
#define STEPAM      4
#define BANDFM      5
#define BANDAM      6
#define BANDSSB     7
#define STEPUP      8
#define STEPDOWN    9
#define UP_SEEK     10
#define STEP_STATION_UP     11
#define STEP_STATION_DOWN   12
#define AGCGAIN     13
#define SLIDER_AGC  14
#define SLIDER_VOL  15
#define SET_FREQ    16
/***********************************************
 * 
 * 
 * ******************************************* */
#define LW_BAND_TYPE 0
#define MW_BAND_TYPE 1
#define SW_BAND_TYPE 2
#define FM_BAND_TYPE 3

#define AM          0
#define LSB         1
#define USB         2
#define CW          3
#define FM          4

#define AIR_SPEAKER 1
#define BT_SPEAKER 2
#define WEB_SPEKER 3
/***********************************************
 * 
 * 
 * ******************************************* */
typedef enum {
    UNINITIALIZED, INITIALIZED, RUNNING, STOPPED
} component_status_t;

//=================  Структура данных режимов роботы  Boombox =====
typedef enum
{
  eAir,
  eBT,
  eWeb,
} ModeBoombox_t;

//=================  Структура данных обработки нажатия кнопок lvgl ==============
typedef enum
{
    ebandIDx = 1,       // Выбор диапазона. ucValue = LW-0, MW-1, Sw-2, FM-3 
    eModIdx = 2,        // Выбор модуляции. ucValue = AM-0, FM-1, LSB-2, USB-3, SSB-4
    eStepFM = 3,        // Шаг перестройки FM
    eStepAM = 4,        // Шаг перестройки AM
    eBandWFM = 5,       // Полоса пропускания FM. 
                                        //  ucValue = 
                                        //  0  Automatically select proper channel filter (Default) |
                                        //  1  Force wide (110 kHz) channel filter |
                                        //  2  Force narrow (84 kHz) channel filter |
                                        //  3  Force narrower (60 kHz) channel filter |
                                        //  4  Force narrowest (40 kHz) channel filter |
    eBandWAM = 6,       // Полоса пропускания AM.
                                        //  ucValue = 
                                        //  0 = 6 kHz Bandwidth
                                        //  1 = 4 kHz Bandwidth
                                        //  2 = 3 kHz Bandwidth
                                        //  3 = 2 kHz Bandwidth
                                        //  4 = 1 kHz Bandwidth
                                        // 5 = 1.8 kHz Bandwidth
                                        // 6 = 2.5 kHz Bandwidth, gradual roll off
    eBandWSSB = 7,      // Полоса пропускания SSB
    eStepUP = 8,        // Переход на одну станцию в верх
    eStepDown = 9,      // Переход на одну станцию в верх
    eSeekUP = 10,       // Поиск станций вверх
    eStationStepUP = 11,    // Перестройка вверх с шагом
    eStationStepDOWN = 12,  // Перестройка вниз с шагом
    eAGCgain    = 13,
    eSlider_agc = 14,
    eslider_vol = 15,   // Громкость 
    eSetFreq = 16       // Установка частоты
} DataDescription_t;
//================  Структура данных помещаемых в очередь  для обработки в МК  =================
// 
typedef struct
{
    bool State;         //  Статус Структуры, false-изменений не было, true-изменений было
    ModeBoombox_t eModeBoombox; // режим работы Boombox 
    DataDescription_t eDataDescription;  // название Элемента
    int ucValue;        // Значение Элемента
} Data_GUI_Boombox_t;

typedef enum
{
    eEmpty = 0,
    eFreq = 1,          // Значение частоты
} AirWebDescription_t;
//================  Структура данных помещаемых в очередь  для вывода на GUI  =================

typedef struct
{
    bool State;         //  Статус Структуры, false-изменений не было, true-изменений не было
    uint8_t ucBand;     //  значение  диапазона AM/SW/SSB/FM   
    char *vcBand;       // Текстовое значение  диапазона AM/SW/SSB/FM  
    uint16_t ucFreq;    //  значение  частоты
    uint8_t ucSNR;      //  значение  SNR
    uint8_t ucRSSI;     //  значение  RSSI
    uint8_t ucslider_vol;// значение громкости
    char * vcFreqRange; // Текстовое значение kHz/MHz 
    char * vcStereoMono;// Текстовое значение стерео/моно
    char * vcBW;        // Текстовое значение полосы пропускания 
    char * vcStep;      // Текстовое значение шага перестройки
    char *vcRDSdata;    // Текстовая информация от RDS
    ModeBoombox_t eModeBoombox;
    AirWebDescription_t eDataDescription;  // название Элемента
} Data_Boombox_GUI_t;
//==================================================================================================

void gui_boombox_queue_init(void);
void boombox_gui_queue_init(void);

extern QueueHandle_t xGuiToBoomboxQueue;
extern QueueHandle_t xBoomboxToGuiQueue;

#endif 

/* COMMON_H_ */