# Исправление проблемы нехватки памяти WiFi

## Проблема
Система испытывает нехватку памяти при переключении в режим точки доступа (AP):
```
W (85394) wifi:alloc eb len=752 type=4 fail
Guru Meditation Error: Core 0 panic'ed (LoadProhibited)
```

## Причина
Слишком высокие значения WiFi буферов в `sdkconfig`:
- `CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM = 32` (должно быть 8-16)
- `CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM = 32` (должно быть 8-16)
- `CONFIG_ESP_WIFI_MGMT_SBUF_NUM = 32` (должно быть 8-12)

Текущее потребление памяти: ~150KB только на WiFi буферы.

## Исправления в коде

### 1. Добавлены проверки безопасности
- ✅ NULL-проверки в `wifi_mgr_expand_template()`
- ✅ Проверка доступной памяти перед запуском AP (минимум 50KB)
- ✅ Обработка ошибок во всех вызовах `wifi_manager_start_ap()`
- ✅ Информативные сообщения об ошибках с указанием свободной памяти

### 2. Рекомендуемые настройки sdkconfig

Добавьте в `sdkconfig.defaults` или измените через `idf.py menuconfig`:

```ini
# WiFi Configuration - Оптимизированные настройки для режима APSTA
# Component config → Wi-Fi

# Static RX buffers (значение по умолчанию OK)
CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM=10

# Dynamic RX buffers - УМЕНЬШИТЬ
CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=16

# Dynamic TX buffers - УМЕНЬШИТЬ  
CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM=16

# Management short buffers - УМЕНЬШИТЬ
CONFIG_ESP_WIFI_MGMT_SBUF_NUM=12

# RX Management buffers (значение по умолчанию OK)
CONFIG_ESP_WIFI_RX_MGMT_BUF_NUM_DEF=5
```

### 3. Как применить изменения

#### Вариант А: ESP-IDF menuconfig (рекомендуется)
```bash
idf.py menuconfig
```
Затем перейдите:
```
Component config → Wi-Fi → WiFi
  - Dynamic RX buffer num: 16
  - Dynamic TX buffer num: 16
  - WiFi MGMT short buffer number: 12
```

#### Вариант Б: Редактирование sdkconfig вручную
1. Откройте `sdkconfig`
2. Найдите и измените:
   ```
   CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=16
   CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM=16
   CONFIG_ESP_WIFI_MGMT_SBUF_NUM=12
   ```
3. Сохраните файл

#### Вариант В: Добавление в sdkconfig.defaults.esp32
Добавьте в конец файла `sdkconfig.defaults.esp32`:
```ini
# Optimized WiFi buffers for APSTA mode
CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=16
CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM=16
CONFIG_ESP_WIFI_MGMT_SBUF_NUM=12
```

### 4. Пересборка проекта
```bash
idf.py fullclean
idf.py build
idf.py flash
```

## Ожидаемый результат
- Экономия памяти: ~50-70KB
- Стабильная работа в режиме APSTA
- Нет паники при переключении в режим AP
- Достаточная производительность для web-интерфейса и потоковой передачи

## Дополнительная оптимизация (если проблемы продолжаются)

### Уменьшение стека задач
```ini
# Component config → ESP Event System
CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE=2304  # вместо 4096

# Component config → Ethernet
CONFIG_ETH_DMA_RX_BUFFER_NUM=8  # вместо 10
CONFIG_ETH_DMA_TX_BUFFER_NUM=8  # вместо 10
```

### Мониторинг памяти
Добавьте в код периодический вывод:
```c
ESP_LOGI(TAG, "Free heap: %d bytes, Min: %d bytes", 
         esp_get_free_heap_size(),
         esp_get_minimum_free_heap_size());
```

## Проверка после исправления
После перепрошивки проверьте:
1. ✅ Устройство подключается к WiFi
2. ✅ После неудачи подключения запускается AP без краша
3. ✅ Web-интерфейс доступен в режиме AP
4. ✅ Нет предупреждений "alloc eb len=752 type=4 fail"

---

**Изменённые файлы:**
- `components/esp_wifi_manager/src/esp_wifi_manager_ap.c` - добавлены проверки безопасности
- `components/esp_wifi_manager/src/esp_wifi_manager_network.c` - обработка ошибок
- `components/esp_wifi_manager/src/esp_wifi_manager.c` - обработка ошибок

**Требуется изменить:**
- `sdkconfig` или `sdkconfig.defaults.esp32` - оптимизация WiFi буферов
