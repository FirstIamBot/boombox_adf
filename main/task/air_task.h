//#pragma once

#include "board.h"


typedef struct {
    uint16_t freq_khz; // Частота в кГц
    int rssi;          // Уровень сигнала rssi
    int snr;           // Уровень snr
    uint16_t stations[50];
} si4735_station_t;



// Структура данных для сохранения временных(текущих) данных в Flash
typedef struct {
    uint8_t currentBandType;    // Тип текущего диапазона (FM, LW, MW, SW)  
    uint8_t currentMod;       // Текущий режим (AM, SSB, FM)  
    uint16_t currentStepFM;   // Шаг настройки FM  
    uint16_t currentStepAM;  // Шаг настройки AM   
    uint16_t currentFrequency;// Текущая частота
    uint8_t currentVOL;       // Текущий уровень громкости  
    uint8_t BandWidthFM;     // Ширина полосы FM   
    uint8_t BandWidthAM;    // Ширина полосы AM    
    uint8_t BandWidthSSB;   // Ширина полосы SSB     
    uint8_t currentAGCgain;  // Текущее значение AGC     
    uint8_t onoffAGCgain;     // Включение/выключение AGC (0 = включено, 1 = выключено)
    uint8_t rssi_thresh_seek;  // Минимальный уровень RSSI для принятия станции
    uint8_t snr_thresh_seek;  //Минимальный уровень SNR для принятия станции
} air_config_t;




void air_player( );