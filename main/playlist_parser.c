#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"

#define MAX_ENTRIES  20      // Максимальное количество станций в плейлисте
#define MAX_TITLE    64      // Максимальная длина названия станции
#define MAX_URL      256     // Максимальная длина URL

static const char *TAG = "PARSER_PLAYLIST";

// Структура для хранения одной записи плейлиста
typedef struct {
    char title[MAX_TITLE];   // Название станции
    char url[MAX_URL];       // URL станции
} PlaylistEntry;

// Структура для хранения всего плейлиста
typedef struct {
    PlaylistEntry entries[MAX_ENTRIES]; // Массив записей
    int count;                         // Количество записей
} Playlist;

// Глобальная переменная для хранения плейлиста
static Playlist playlist = { .count = 0 };
void init_spiffs(void) {
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

// Функция парсинга файла плейлиста
// filename - путь к файлу .pls
// Возвращает количество найденных станций или -1 при ошибке
int parse_playlist(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;
    char line[512];
    int idx = 0;
    while (fgets(line, sizeof(line), f)) {
        // Поиск строки с названием станции
        if (strncmp(line, "Title", 5) == 0) {
            char *eq = strchr(line, '=');
            if (eq && idx < MAX_ENTRIES) {
                strncpy(playlist.entries[idx].title, eq + 1, MAX_TITLE - 1);
                // Удаление символа новой строки
                playlist.entries[idx].title[strcspn(playlist.entries[idx].title, "\r\n")] = 0;
            }
        // Поиск строки с URL станции
        } else if (strncmp(line, "File", 4) == 0) {
            char *eq = strchr(line, '=');
            if (eq && idx < MAX_ENTRIES) {
                strncpy(playlist.entries[idx].url, eq + 1, MAX_URL - 1);
                playlist.entries[idx].url[strcspn(playlist.entries[idx].url, "\r\n")] = 0;
                idx++; // Переход к следующей записи
            }
        }
    }
    fclose(f);
    playlist.count = idx;
    return idx;
}

// Получить название станции по индексу
const char* playlist_get_title(int index) {
    if (index < 0 || index >= playlist.count) return NULL;
    return playlist.entries[index].title;
}

// Получить URL станции по индексу
const char* playlist_get_url(int index) {
    if (index < 0 || index >= playlist.count) return NULL;
    return playlist.entries[index].url;
}

// Получить количество станций в плейлисте
int playlist_get_count() {
    return playlist.count;
}

void print_playlist(void) {
    ESP_LOGI(TAG, "Playlist contains %d entries:", playlist.count);
    for (int i = 0; i < playlist.count; ++i) {
        ESP_LOGI(TAG, "[%d] Title: %s | URL: %s", i, playlist.entries[i].title, playlist.entries[i].url);
    }
}

// Функция чтения и вывода содержимого index.html из SPIFFS
void read_and_print_index_html(void) {
    const char *filepath = "/spiffs/www/index.html";
    FILE *f = fopen(filepath, "r");
    
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s for reading", filepath);
        return;
    }
    
    ESP_LOGI(TAG, "========== Content of index.html ==========");
    
    // Читаем и выводим файл построчно
    char line[256];
    while (fgets(line, sizeof(line), f) != NULL) {
        // Удаляем символ новой строки для более чистого вывода
        line[strcspn(line, "\r\n")] = 0;
        ESP_LOGI(TAG, "%s", line);
    }
    
    ESP_LOGI(TAG, "==========================================");
    
    fclose(f);
    ESP_LOGI(TAG, "Successfully read and printed index.html");
}
