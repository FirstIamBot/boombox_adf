#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRIES  20      // Максимальное количество станций в плейлисте
#define MAX_TITLE    64      // Максимальная длина названия станции
#define MAX_URL      256     // Максимальная длина URL

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
