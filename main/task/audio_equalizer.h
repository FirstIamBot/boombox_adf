#pragma once

#include "audio_element.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Конфигурация эквалайзера
typedef struct {
    audio_element_handle_t i2s_element;    // I2S элемент для анализа
    uint32_t buffer_size;                  // Размер буфера для анализа
    uint32_t update_interval_ms;           // Интервал обновления дисплея в мс
    uint32_t fft_size;                     // Размер FFT (должен быть степенью 2)
    uint8_t num_bands;                     // Количество полос эквалайзера
    uint8_t display_height;                // Высота дисплея в символах
    uint8_t display_width;                 // Ширина каждой полосы
    bool show_frequency_labels;            // Показывать частотные метки
    bool show_db_scale;                    // Показывать шкалу в дБ
    bool enable_peak_hold;                 // Включить удержание пиков
    uint32_t peak_hold_time_ms;            // Время удержания пиков в мс
} audio_equalizer_config_t;

// Конфигурация по умолчанию
#define AUDIO_EQUALIZER_DEFAULT_CONFIG() {      \
    .i2s_element = NULL,                        \
    .buffer_size = 2048,                        \
    .update_interval_ms = 50,                   \
    .fft_size = 1024,                           \
    .num_bands = 16,                            \
    .display_height = 20,                       \
    .display_width = 4,                         \
    .show_frequency_labels = true,              \
    .show_db_scale = true,                      \
    .enable_peak_hold = true,                   \
    .peak_hold_time_ms = 1000                   \
}

/**
 * @brief Запустить ASCII эквалайзер для I2S потока
 * 
 * @param config Конфигурация эквалайзера
 * @param task_handle Указатель для сохранения handle задачи (может быть NULL)
 * @return esp_err_t ESP_OK в случае успеха
 */
esp_err_t audio_equalizer_start(audio_equalizer_config_t *config, TaskHandle_t *task_handle);

/**
 * @brief Остановить ASCII эквалайзер
 * 
 * @param task_handle Handle задачи эквалайзера
 * @return esp_err_t ESP_OK в случае успеха
 */
esp_err_t audio_equalizer_stop(TaskHandle_t task_handle);

/**
 * @brief Задача эквалайзера (внутренняя функция)
 * 
 * @param pvParameters Указатель на конфигурацию audio_equalizer_config_t
 */
void audio_equalizer_task(void *pvParameters);
