#include "audio_equalizer.h"
#include "esp_log.h"
#include "audio_mem.h"
#include "audio_common.h"
#include <string.h>
#include <math.h>
#include <inttypes.h>
static const char *TAG = "AUDIO_EQUALIZER";

// Структура для передачи данных в задачу
typedef struct {
    audio_equalizer_config_t config;
    TaskHandle_t *task_handle_ptr;
    volatile bool should_stop;
    float *band_levels;
    float *peak_levels;
    uint32_t *peak_hold_time;
} audio_equalizer_task_params_t;

static audio_equalizer_task_params_t *g_equalizer_params = NULL;

// Сложная реализация FFT (быстрое преобразование Фурье, комплексное, с вычислением амплитуд)
// Используется алгоритм Cooley-Tukey для размера, равного степени двойки
static void complex_fft(const float *input, float *output, int fft_size)
{
    typedef struct { float re, im; } complex_t;
    complex_t buffer[fft_size];
    // Копируем входные данные в буфер, преобразуем в комплексные числа
    for (int i = 0; i < fft_size; i++) {
        buffer[i].re = input[i];
        buffer[i].im = 0.0f;
    }
    // Битовое реверсирование индексов
    int j = 0;
    for (int i = 0; i < fft_size; i++) {
        if (i < j) {
            complex_t tmp = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = tmp;
        }
        int m = fft_size >> 1;
        while (m >= 1 && j >= m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    // Основной цикл FFT
    for (int s = 1; s <= log2f(fft_size); s++) {
        int m = 1 << s;
        float theta = -2.0f * M_PI / m;
        complex_t wm = { cosf(theta), sinf(theta) };
        for (int k = 0; k < fft_size; k += m) {
            complex_t w = { 1.0f, 0.0f };
            for (int l = 0; l < m / 2; l++) {
                int i1 = k + l;
                int i2 = k + l + m / 2;
                complex_t t = {
                    w.re * buffer[i2].re - w.im * buffer[i2].im,
                    w.re * buffer[i2].im + w.im * buffer[i2].re
                };
                complex_t u = buffer[i1];
                buffer[i1].re = u.re + t.re;
                buffer[i1].im = u.im + t.im;
                buffer[i2].re = u.re - t.re;
                buffer[i2].im = u.im - t.im;
                // Обновляем w
                float wre = w.re * wm.re - w.im * wm.im;
                float wim = w.re * wm.im + w.im * wm.re;
                w.re = wre;
                w.im = wim;
            }
        }
    }
    // Вычисляем амплитуды
    for (int i = 0; i < fft_size; i++) {
        output[i] = sqrtf(buffer[i].re * buffer[i].re + buffer[i].im * buffer[i].im);
    }
}

// Использовать сложную FFT для анализа аудиосигнала и вычисления уровней по полосам
static void advanced_fft_magnitude(int16_t *input, float *output, int size, int num_bands)
{
    // Преобразуем входные данные в float
    float fft_in[size];
    for (int i = 0; i < size; i++) {
        fft_in[i] = (float)input[i] / 32768.0f;
    }
    float fft_out[size];
    complex_fft(fft_in, fft_out, size);

    // Разделяем спектр на полосы и вычисляем среднюю амплитуду в каждой
    int bins_per_band = size / 2 / num_bands;
    for (int band = 0; band < num_bands; band++) {
        float sum = 0.0f;
        int start = band * bins_per_band;
        int end = start + bins_per_band;
        for (int i = start; i < end && i < size / 2; i++) {
            sum += fft_out[i];
        }
        float magnitude = sum / bins_per_band;
        // Преобразуем в логарифмическую шкалу и нормализуем
        output[band] = 20.0f * log10f(magnitude + 1e-10f);
        output[band] = (output[band] + 60.0f) / 60.0f;
        if (output[band] < 0) output[band] = 0;
        if (output[band] > 1) output[band] = 1;
    }
}

static void display_equalizer(audio_equalizer_task_params_t *params) {
    audio_equalizer_config_t *config = &params->config;
    printf("\033[2J\033[H");
    printf("AUDIO EQUALIZER LEVELS\n");
    printf("Band\tLevel\tPeak\n");
    for (int band = 0; band < config->num_bands; band++) {
        printf("%2d\t%6.2f\t%6.2f\n", band + 1, params->band_levels[band] * 100.0f, params->peak_levels[band] * 100.0f);
    }
    printf("Bands: %d | Update: %ums | FFT: %u\n", config->num_bands, (unsigned int)config->update_interval_ms, (unsigned int)config->fft_size);
    float avg_level = 0;
    for (int i = 0; i < config->num_bands; i++) {
        avg_level += params->band_levels[i];
    }
    avg_level /= config->num_bands;
    printf("Avg Level: %.1f%% | Peak Hold: %s\n", avg_level * 100, config->enable_peak_hold ? "ON" : "OFF");
    fflush(stdout);
}

void audio_equalizer_task(void *pvParameters) {
    audio_equalizer_task_params_t *params = (audio_equalizer_task_params_t *)pvParameters;
    audio_equalizer_config_t *config = &params->config;
    ESP_LOGI(TAG, "=== Audio Equalizer Started ===");
    ESP_LOGI(TAG, "Bands: %d, FFT Size: %" PRIu32 ", Update: %" PRIu32 "ms", config->num_bands, config->fft_size, config->update_interval_ms);
    int16_t *audio_buffer = audio_malloc(config->buffer_size);
    float *fft_output = audio_malloc(config->num_bands * sizeof(float));
    if (!audio_buffer || !fft_output) {
        ESP_LOGE(TAG, "Failed to allocate buffers");
        goto cleanup;
    }
    uint32_t last_update = xTaskGetTickCount();
    printf("\033[?25l");
    while (!params->should_stop) {
        int samples = config->buffer_size / sizeof(int16_t);
        static float phase = 0.0f;
        for (int i = 0; i < samples; i += 2) {
            float sample1 = sin(phase * 2.0f * M_PI) * 8192;
            float sample2 = sin(phase * 4.0f * M_PI) * 4096;
            float sample3 = sin(phase * 8.0f * M_PI) * 2048;
            int16_t combined = (int16_t)(sample1 + sample2 + sample3);
            audio_buffer[i] = combined;
            audio_buffer[i + 1] = combined;
            phase += 1.0f / 44100.0f;
            if (phase >= 1.0f) phase -= 1.0f;
        }
        advanced_fft_magnitude(audio_buffer, fft_output, samples, config->num_bands);
        uint32_t current_time = xTaskGetTickCount();
        for (int band = 0; band < config->num_bands; band++) {
            float new_level = fft_output[band];
            params->band_levels[band] = params->band_levels[band] * 0.7f + new_level * 0.3f;
            if (config->enable_peak_hold) {
                if (new_level > params->peak_levels[band]) {
                    params->peak_levels[band] = new_level;
                    params->peak_hold_time[band] = current_time;
                } else {
                    if ((current_time - params->peak_hold_time[band]) * portTICK_PERIOD_MS > config->peak_hold_time_ms) {
                        params->peak_levels[band] *= 0.95f;
                    }
                }
            }
        }
        if ((current_time - last_update) * portTICK_PERIOD_MS >= config->update_interval_ms) {
            display_equalizer(params);
            last_update = current_time;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    printf("\033[?25h");
cleanup:
    if (audio_buffer) audio_free(audio_buffer);
    if (fft_output) audio_free(fft_output);
    ESP_LOGI(TAG, "=== Audio Equalizer Stopped ===");
    if (params->task_handle_ptr) {
        *params->task_handle_ptr = NULL;
    }
    if (g_equalizer_params == params) {
        g_equalizer_params = NULL;
    }
    if (params->band_levels) audio_free(params->band_levels);
    if (params->peak_levels) audio_free(params->peak_levels);
    if (params->peak_hold_time) audio_free(params->peak_hold_time);
    audio_free(params);
    vTaskDelete(NULL);
}

esp_err_t audio_equalizer_start(audio_equalizer_config_t *config, TaskHandle_t *task_handle) {
    if (!config || !config->i2s_element) {
        ESP_LOGE(TAG, "Invalid equalizer configuration");
        return ESP_ERR_INVALID_ARG;
    }
    if (g_equalizer_params) {
        ESP_LOGW(TAG, "Equalizer already running");
        return ESP_ERR_INVALID_STATE;
    }
    audio_equalizer_task_params_t *params = audio_malloc(sizeof(audio_equalizer_task_params_t));
    if (!params) {
        ESP_LOGE(TAG, "Failed to allocate memory for task parameters");
        return ESP_ERR_NO_MEM;
    }
    params->band_levels = audio_malloc(config->num_bands * sizeof(float));
    params->peak_levels = audio_malloc(config->num_bands * sizeof(float));
    params->peak_hold_time = audio_malloc(config->num_bands * sizeof(uint32_t));
    if (!params->band_levels || !params->peak_levels || !params->peak_hold_time) {
        ESP_LOGE(TAG, "Failed to allocate memory for band data");
        if (params->band_levels) audio_free(params->band_levels);
        if (params->peak_levels) audio_free(params->peak_levels);
        if (params->peak_hold_time) audio_free(params->peak_hold_time);
        audio_free(params);
        return ESP_ERR_NO_MEM;
    }
    memcpy(&params->config, config, sizeof(audio_equalizer_config_t));
    params->task_handle_ptr = task_handle;
    params->should_stop = false;
    memset(params->band_levels, 0, config->num_bands * sizeof(float));
    memset(params->peak_levels, 0, config->num_bands * sizeof(float));
    memset(params->peak_hold_time, 0, config->num_bands * sizeof(uint32_t));
    g_equalizer_params = params;
    BaseType_t result = xTaskCreate(
        audio_equalizer_task,
        "audio_equalizer",
        8192,
        params,
        4,
        task_handle
    );
    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create equalizer task");
        audio_free(params->band_levels);
        audio_free(params->peak_levels);
        audio_free(params->peak_hold_time);
        audio_free(params);
        g_equalizer_params = NULL;
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Audio equalizer task started successfully");
    return ESP_OK;
}

esp_err_t audio_equalizer_stop(TaskHandle_t task_handle) {
    if (!task_handle || !g_equalizer_params) {
        ESP_LOGW(TAG, "Equalizer not running or invalid handle");
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGI(TAG, "Stopping audio equalizer...");
    g_equalizer_params->should_stop = true;
    uint32_t timeout = 5000 / portTICK_PERIOD_MS;
    while (g_equalizer_params && timeout > 0) {
        vTaskDelay(pdMS_TO_TICKS(100));
        timeout -= 100 / portTICK_PERIOD_MS;
    }
    if (g_equalizer_params) {
        ESP_LOGW(TAG, "Task did not finish within timeout, force delete");
        vTaskDelete(task_handle);
        if (g_equalizer_params) {
            if (g_equalizer_params->band_levels) audio_free(g_equalizer_params->band_levels);
            if (g_equalizer_params->peak_levels) audio_free(g_equalizer_params->peak_levels);
            if (g_equalizer_params->peak_hold_time) audio_free(g_equalizer_params->peak_hold_time);
            audio_free(g_equalizer_params);
            g_equalizer_params = NULL;
        }
    }
    printf("\033[2J\033[H\033[?25h");
    ESP_LOGI(TAG, "Audio equalizer stopped");
    return ESP_OK;
}

