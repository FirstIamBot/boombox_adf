# Boombox WiFi Manager API

## Описание

Компонент `esp_wifi_manager_boombox` предоставляет REST API для просмотра и управления данными Boombox через WiFi.

## Web UI Interface

Начиная с версии 2.0, функциональность управления Boombox интегрирована в основной Web UI интерфейс WiFi Manager. Откройте веб-браузер и перейдите по адресу вашего устройства:

```
http://<device-ip>/         # Главная страница WebUI
```

В WebUI доступны две вкладки:
- **📶 WiFi** - управление WiFi сетями
- **📻 Boombox** - управление аудиоплеером Boombox

Страница Boombox включает:
- Отображение текущего статуса (режим, частота, уровень сигнала, RDS)
- Регулятор громкости (0-64)
- Выбор диапазона (LW/MW/SW/FM) для режима Air Radio
- Поиск станций вверх/вниз
- Ручная установка частоты
- Управление воспроизведением (Stop/Play/Pause/Next)
- Автоматическое обновление статуса каждые 2 секунды

> **Примечание:** Старая HTML страница `/boombox_test.html` больше не доступна. Используйте главную страницу WebUI с интегрированными табами.

## API Endpoints

### 1. GET /api/boombox/status

Получить текущее состояние Boombox, включая последнюю полученную команду и текущий статус плеера.

**Пример запроса:**
```bash
curl http://<device-ip>/api/boombox/status
```

**Пример ответа:**
```json
{
  "lastCommand": {
    "hasChanges": true,
    "mode": "Air",
    "control": "Volume",
    "value": 50
  },
  "currentStatus": {
    "hasUpdate": true,
    "mode": "Air",
    "air": {
      "band": "FM",
      "stationIndex": 5,
      "frequency": 10030,
      "snr": 25,
      "rssi": -65,
      "volume": 50,
      "freqRange": "MHz",
      "stereoMono": "Stereo",
      "bandwidth": "110kHz",
      "step": "10kHz",
      "rds": "Radio Station Name"
    }
  }
}
```

**Поля в режиме Air (Эфирное радио):**
- `band` - диапазон (LW/MW/SW/FM)
- `stationIndex` - индекс текущей станции
- `frequency` - частота в kHz
- `snr` - отношение сигнал/шум
- `rssi` - уровень сигнала
- `volume` - громкость (0-64)
- `freqRange` - диапазон частот (kHz/MHz)
- `stereoMono` - режим стерео/моно
- `bandwidth` - полоса пропускания
- `step` - шаг перестройки
- `rds` - данные RDS

**Поля в режиме Web (Интернет радио):**
- `uri` - URL потока
- `station` - название станции
- `stationIndex` - индекс станции в плейлисте

**Поля в режиме Bluetooth:**
- `title` - название трека
- `artist` - исполнитель
- `album` - альбом

---

### 2. GET /api/boombox/config

Получить сохраненную конфигурацию Boombox из NVS.

**Пример запроса:**
```bash
curl http://<device-ip>/api/boombox/config
```

**Пример ответа:**
```json
{
  "mode": "Air",
  "currentSource": 1,
  "volume": 50,
  "airConfig": {
    "bandType": 3,
    "modulation": 1,
    "stepFM": 1,
    "stepAM": 1,
    "frequency": 10030,
    "volume": 32,
    "bandwidthFM": 0,
    "bandwidthAM": 0,
    "bandwidthSSB": 0,
    "agcGain": 10,
    "agcEnabled": 1,
    "rssiThreshold": 15,
    "snrThreshold": 10,
    "fmStations": [10030, 10100, 10330, 10550],
    "currentFMStation": 0
  }
}
```

**Поля конфигурации:**
- `mode` - режим работы (Air/Bluetooth/Web)
- `currentSource` - текущий источник (1=AIR, 2=BT, 3=WEB)
- `volume` - общая громкость
- `airConfig` - конфигурация эфирного радио:
  - `bandType` - тип диапазона (0=LW, 1=MW, 2=SW, 3=FM)
  - `modulation` - модуляция (0=AM, 1=LSB, 2=USB, 4=FM)
  - `stepFM/stepAM` - шаг настройки
  - `frequency` - текущая частота
  - `bandwidthFM/AM/SSB` - ширина полосы пропускания
  - `agcGain` - усиление AGC (0-36)
  - `agcEnabled` - AGC вкл/выкл (1/0)
  - `rssiThreshold` - порог RSSI для поиска станций
  - `snrThreshold` - порог SNR для поиска станций
  - `fmStations` - массив найденных FM станций
  - `currentFMStation` - индекс текущей FM станции

---

### 3. POST /api/boombox/control

Отправить команду управления в Boombox.

**Пример запроса (изменить громкость):**
```bash
curl -X POST http://<device-ip>/api/boombox/control \
  -H "Content-Type: application/json" \
  -d '{"mode":"Air", "control":15, "value":50}'
```

**Параметры:**
- `mode` - режим (опционально): "Air", "Bluetooth", "Web"
- `control` - код команды управления (см. таблицу ниже)
- `value` - значение команды

**Коды команд управления:**
| Код | Название | Описание |
|-----|----------|----------|
| 1 | BandIndex | Выбор диапазона (0=LW, 1=MW, 2=SW, 3=FM) |
| 2 | ModulationIndex | Выбор модуляции (0=AM, 1=LSB, 2=USB, 4=FM) |
| 3 | StepFM | Шаг перестройки FM |
| 4 | StepAM | Шаг перестройки AM |
| 5 | BandwidthFM | Полоса пропускания FM |
| 6 | BandwidthAM | Полоса пропускания AM |
| 7 | BandwidthSSB | Полоса пропускания SSB |
| 8 | StepUp | Перейти на станцию выше |
| 9 | StepDown | Перейти на станцию ниже |
| 10 | SeekUp | Поиск станций вверх |
| 11 | StationStepUp | Перестройка вверх с шагом |
| 12 | StationStepDown | Перестройка вниз с шагом |
| 13 | AGCGain | Вкл/выкл AGC |
| 14 | SliderAGC | Регулировка AGC (0-36) |
| 15 | Volume | Громкость (0-64) |
| 16 | SetFrequency | Установка частоты |
| 17 | PlayControl | Управление воспроизведением (0=STOP, 1=PLAY, 2=PAUSE, 3=FORWARD, 4=NEXT) |

**Пример ответа:**
```json
{
  "status": "ok",
  "message": "Command sent successfully"
}
```

## Примеры использования

### JavaScript / Fetch API

```javascript
// Получить статус
async function getStatus() {
  const response = await fetch('http://192.168.4.1/api/boombox/status');
  const data = await response.json();
  console.log('Boombox status:', data);
  return data;
}

// Установить громкость на 50
async function setVolume(volume) {
  const response = await fetch('http://192.168.4.1/api/boombox/control', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({
      mode: 'Air',
      control: 15, // Volume
      value: volume
    })
  });
  return await response.json();
}

// Переключить на FM диапазон
async function switchToFM() {
  const response = await fetch('http://192.168.4.1/api/boombox/control', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({
      mode: 'Air',
      control: 1, // BandIndex
      value: 3    // FM_BAND_TYPE
    })
  });
  return await response.json();
}

// Поиск станций вверх
async function seekUp() {
  const response = await fetch('http://192.168.4.1/api/boombox/control', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({
      control: 10, // SeekUp
      value: 1
    })
  });
  return await response.json();
}
```

### Python

```python
import requests
import json

# Получить статус
def get_status():
    response = requests.get('http://192.168.4.1/api/boombox/status')
    return response.json()

# Установить громкость
def set_volume(volume):
    data = {
        'mode': 'Air',
        'control': 15,  # Volume
        'value': volume
    }
    response = requests.post('http://192.168.4.1/api/boombox/control', 
                           json=data)
    return response.json()

# Переключить режим
def switch_mode(mode):
    # mode: "Air", "Bluetooth", "Web"
    data = {
        'mode': mode,
        'control': 0,
        'value': 0
    }
    response = requests.post('http://192.168.4.1/api/boombox/control', 
                           json=data)
    return response.json()

# Пример использования
if __name__ == '__main__':
    # Получить текущий статус
    status = get_status()
    print("Current status:", json.dumps(status, indent=2))
    
    # Установить громкость на 40
    result = set_volume(40)
    print("Set volume:", result)
```

### curl примеры

```bash
# Получить статус
curl http://192.168.4.1/api/boombox/status

# Получить конфигурацию
curl http://192.168.4.1/api/boombox/config

# Установить громкость на 45
curl -X POST http://192.168.4.1/api/boombox/control \
  -H "Content-Type: application/json" \
  -d '{"control":15,"value":45}'

# Переключить на FM диапазон
curl -X POST http://192.168.4.1/api/boombox/control \
  -H "Content-Type: application/json" \
  -d '{"control":1,"value":3}'

# Установить частоту 100.3 MHz (10030 kHz)
curl -X POST http://192.168.4.1/api/boombox/control \
  -H "Content-Type: application/json" \
  -d '{"control":16,"value":10030}'

# Начать воспроизведение
curl -X POST http://192.168.4.1/api/boombox/control \
  -H "Content-Type: application/json" \
  -d '{"control":17,"value":1}'
```

## CORS Support

API поддерживает CORS (Cross-Origin Resource Sharing), что позволяет обращаться к нему из веб-приложений, размещенных на других доменах.

## Безопасность

По умолчанию API доступен без аутентификации. Если требуется защита, используйте конфигурацию WiFi Manager для включения HTTP Basic Auth.

## Интеграция

Компонент автоматически регистрируется при инициализации WiFi Manager. Никаких дополнительных действий не требуется - просто инициализируйте WiFi Manager как обычно:

```c
wifi_manager_config_t wifi_config = {
    .auto_reconnect = true,
    .http = {
        .enable = true,
    },
};

ESP_ERROR_CHECK(wifi_manager_init(&wifi_config));
```

После этого все endpoints будут доступны по указанным выше путям.
