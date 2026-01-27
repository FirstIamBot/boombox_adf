#ifndef SERVER_TASK_H
#define SERVER_TASK_H

#include "esp_http_server.h"
#include "board.h"
#include "commons.h"


// Инициализация веб-сервера
void init_web_server(void);

// Остановка веб-сервера
void deinit_web_server(void);

// Запуск задачи веб-сервера
void web_server_task(void *pvParameters);

#endif // WEB_SERVER_TASK_H