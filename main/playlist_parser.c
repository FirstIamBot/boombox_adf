#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"

#include "boombox_playlist.h"
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>

// Helper: trim leading/trailing whitespace and remove leading ':'
static void sanitize_str(const char *src, char *dst, size_t dst_len)
{
    if (!src || !dst || dst_len == 0) return;
    // skip leading whitespace
    while (*src && isspace((unsigned char)*src)) src++;
    // skip leading ':' characters
    while (*src == ':') src++;
    // copy
    size_t i = 0;
    while (*src && i + 1 < dst_len) dst[i++] = *src++;
    dst[i] = '\0';
    // trim trailing whitespace
    while (i > 0 && isspace((unsigned char)dst[i-1])) dst[--i] = '\0';
}

// Helper: ensure scheme present (prepend http:// if missing)
static void ensure_scheme(char *s, size_t len)
{
    if (!s) return;
    if (strstr(s, "://") == NULL) {
        const char *scheme = "http://";
        size_t sl = strlen(scheme);
        size_t cur = strlen(s);
        if (cur + sl + 1 < len) {
            memmove(s + sl, s, cur + 1);
            memcpy(s, scheme, sl);
        }
    }
}

// Helper: simple URL validity check (has scheme and host)
static bool is_valid_url(const char *s)
{
    if (!s) return false;
    const char *p = strstr(s, "://");
    if (!p) return false;
    p += 3; // after ://
    if (*p == '\0') return false;
    // host must contain at least one alnum
    const char *q = p;
    while (*q && *q != '/' && *q != ':') {
        if (isalnum((unsigned char)*q)) return true;
        q++;
    }
    return false;
}

// Helper: append invalid entry to a file for inspection
static void record_invalid(const char *title, const char *url, const char *reason)
{
    const char *invalid_path = "/spiffs/playlist_invalid.txt";
    FILE *inv = fopen(invalid_path, "a");
    if (!inv) return;
    fprintf(inv, "Title=%s\nURL=%s\nReason=%s\n---\n", title ? title : "", url ? url : "", reason ? reason : "");
    fclose(inv);
}

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
    char pending_title[MAX_TITLE] = {0};
    char tmp_url[MAX_URL];
    char sanitized[MAX_URL];

    while (fgets(line, sizeof(line), f)) {
        // Поиск строки с названием станции
        if (strncmp(line, "Title", 5) == 0) {
            char *eq = strchr(line, '=');
            if (eq && idx < MAX_ENTRIES) {
                // store pending title until we have a valid URL
                strncpy(pending_title, eq + 1, MAX_TITLE - 1);
                pending_title[strcspn(pending_title, "\r\n")] = 0;
            }
        // Поиск строки с URL станции
        } else if (strncmp(line, "File", 4) == 0) {
            char *eq = strchr(line, '=');
            if (eq && idx < MAX_ENTRIES) {
                strncpy(tmp_url, eq + 1, MAX_URL - 1);
                tmp_url[strcspn(tmp_url, "\r\n")] = 0;

                // sanitize and ensure scheme
                sanitize_str(tmp_url, sanitized, sizeof(sanitized));
                ensure_scheme(sanitized, sizeof(sanitized));

                if (is_valid_url(sanitized)) {
                    // commit entry
                    strncpy(playlist.entries[idx].title, pending_title, MAX_TITLE - 1);
                    playlist.entries[idx].title[MAX_TITLE - 1] = '\0';
                    strncpy(playlist.entries[idx].url, sanitized, MAX_URL - 1);
                    playlist.entries[idx].url[MAX_URL - 1] = '\0';
                    idx++; // next entry
                    // clear pending title
                    pending_title[0] = '\0';
                } else {
                    ESP_LOGW(TAG, "Skipping invalid URL in playlist: '%s' (sanitized: '%s')", tmp_url, sanitized);
                    record_invalid(pending_title[0] ? pending_title : "(no title)", tmp_url, "invalid url/sanitized");
                    // clear pending title
                    pending_title[0] = '\0';
                }
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

// Добавить запись в плейлист (в RAM) и сохранить в SPIFFS
int playlist_add_entry(const char *title, const char *url) {
    if (!title || !url) return -1;
    if (playlist.count >= MAX_ENTRIES) return -2; // full

    strncpy(playlist.entries[playlist.count].title, title, MAX_TITLE - 1);
    playlist.entries[playlist.count].title[MAX_TITLE - 1] = '\0';
    strncpy(playlist.entries[playlist.count].url, url, MAX_URL - 1);
    playlist.entries[playlist.count].url[MAX_URL - 1] = '\0';
    playlist.count++;

    // Persist immediately
    return playlist_save();
}

// Удалить запись по индексу и сохранить
int playlist_delete_entry(int index) {
    if (index < 0 || index >= playlist.count) return -1;
    for (int i = index; i < playlist.count - 1; ++i) {
        strncpy(playlist.entries[i].title, playlist.entries[i+1].title, MAX_TITLE);
        strncpy(playlist.entries[i].url, playlist.entries[i+1].url, MAX_URL);
    }
    // Clear last
    playlist.entries[playlist.count - 1].title[0] = '\0';
    playlist.entries[playlist.count - 1].url[0] = '\0';
    playlist.count--;

    return playlist_save();
}

// Сохранить плейлист в /spiffs/playlist.pls в формате PLS
int playlist_save(void) {
    const char *filepath = "/spiffs/playlist.pls";
    FILE *f = fopen(filepath, "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open %s for writing: %d", filepath, errno);
        return -1;
    }

    fprintf(f, "[playlist]\n");
    fprintf(f, "NumberOfEntries=%d\n", playlist.count);
    for (int i = 0; i < playlist.count; ++i) {
        // PLS fields are 1-based
        fprintf(f, "File%d=%s\n", i + 1, playlist.entries[i].url);
        fprintf(f, "Title%d=%s\n", i + 1, playlist.entries[i].title);
    }
    fprintf(f, "Version=2\n");
    fclose(f);

    ESP_LOGI(TAG, "Playlist saved (%d entries) to %s", playlist.count, filepath);
    return 0;
}

// Обновить существующую запись
int playlist_update_entry(int index, const char *title, const char *url) {
    if (index < 0 || index >= playlist.count) return -1;
    if (title) {
        strncpy(playlist.entries[index].title, title, MAX_TITLE - 1);
        playlist.entries[index].title[MAX_TITLE - 1] = '\0';
    }
    if (url) {
        strncpy(playlist.entries[index].url, url, MAX_URL - 1);
        playlist.entries[index].url[MAX_URL - 1] = '\0';
    }
    return playlist_save();
}

// Переместить запись в плейлисте (shift others accordingly)
int playlist_move_entry(int from, int to) {
    if (from < 0 || from >= playlist.count) return -1;
    if (to < 0) to = 0;
    if (to >= playlist.count) to = playlist.count - 1;
    if (from == to) return 0;

    PlaylistEntry tmp;
    strncpy(tmp.title, playlist.entries[from].title, MAX_TITLE);
    strncpy(tmp.url, playlist.entries[from].url, MAX_URL);

    if (from < to) {
        for (int i = from; i < to; ++i) {
            strncpy(playlist.entries[i].title, playlist.entries[i+1].title, MAX_TITLE);
            strncpy(playlist.entries[i].url, playlist.entries[i+1].url, MAX_URL);
        }
    } else {
        for (int i = from; i > to; --i) {
            strncpy(playlist.entries[i].title, playlist.entries[i-1].title, MAX_TITLE);
            strncpy(playlist.entries[i].url, playlist.entries[i-1].url, MAX_URL);
        }
    }

    strncpy(playlist.entries[to].title, tmp.title, MAX_TITLE);
    strncpy(playlist.entries[to].url, tmp.url, MAX_URL);

    return playlist_save();
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
