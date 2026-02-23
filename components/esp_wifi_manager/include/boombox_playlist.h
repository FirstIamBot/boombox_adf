/**
 * @file boombox_playlist.h
 * @brief Playlist parser forward declarations for WiFi Manager
 * 
 * This is a wrapper to avoid direct dependency on main component
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the title of a playlist entry by index
 * @param index Index of the playlist entry
 * @return Pointer to title string, or NULL if index is invalid
 */
const char* playlist_get_title(int index);

/**
 * @brief Get the URL of a playlist entry by index
 * @param index Index of the playlist entry
 * @return Pointer to URL string, or NULL if index is invalid
 */
const char* playlist_get_url(int index);

/**
 * @brief Get the total number of entries in the playlist
 * @return Number of playlist entries
 */
int playlist_get_count(void);

/**
 * @brief Добавить запись в плейлист
 * @param title Заголовок станции
 * @param url URL потока
 * @return 0 при успехе, отрицательное значение при ошибке
 */
int playlist_add_entry(const char *title, const char *url);

/**
 * @brief Удалить запись из плейлиста по индексу
 * @param index Индекс записи
 * @return 0 при успехе, отрицательное значение при ошибке
 */
int playlist_delete_entry(int index);

/**
 * @brief Сохранить текущий плейлист в SPIFFS (/spiffs/playlist.pls)
 * @return 0 при успехе, отрицательное значение при ошибке
 */
int playlist_save(void);

/**
 * @brief Обновить запись плейлиста по индексу
 * @param index Индекс записи
 * @param title Новый заголовок (или NULL чтобы не менять)
 * @param url Новый URL (или NULL чтобы не менять)
 * @return 0 при успехе, отрицательное значение при ошибке
 */
int playlist_update_entry(int index, const char *title, const char *url);

/**
 * @brief Переместить запись внутри плейлиста
 * @param from Исходный индекс
 * @param to Целевой индекс
 * @return 0 при успехе, отрицательное значение при ошибке
 */
int playlist_move_entry(int from, int to);

#ifdef __cplusplus
}
#endif
