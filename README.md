# 📻 ESP32 Boombox ADF

Modern multi-source audio player based on ESP32 with ESP-ADF (Audio Development Framework). Supports FM/AM/SW radio, Bluetooth audio streaming, and Web radio with touch GUI and WebUI interface.

[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.4.0+-blue)](https://github.com/espressif/esp-idf)
[![ESP-ADF](https://img.shields.io/badge/ESP--ADF-latest-green)](https://github.com/espressif/esp-adf)
[![LVGL](https://img.shields.io/badge/LVGL-8.3.10-orange)](https://lvgl.io/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## ✨ Features

### 🎵 Audio Sources
- **📡 AIR Radio (Si4735)** - FM (64-108 MHz), AM, and SW bands with hardware seek
- **🔵 Bluetooth A2DP Sink** - Connect your phone as wireless speaker
- **🌐 Web Radio (HTTP)** - Stream internet radio stations from playlist

### 🖥️ User Interfaces
- **Touch GUI (LVGL)** - 320×240 color display with capacitive touch
- **WebUI** - Modern responsive web interface with real-time control
- **Rotary Encoder** - Volume and frequency control with button

### 🎛️ Audio Processing
- **PCM5102 I2S DAC** - High-quality 32-bit stereo amplifier
- **10-Band Equalizer** - Software equalizer with preset support (coming soon)
- **Hardware Volume Control** - 0-100 level via codec
- **I2S Audio Pipeline** - Low-latency digital audio path

### 🔧 System
- **WiFi Manager** - Captive portal for easy WiFi setup with BLE provisioning
- **NVS Configuration** - Persistent settings storage
- **Dual-Core FreeRTOS** - CPU0: WiFi/BT, CPU1: GUI tasks
- **4MB Flash + 4MB PSRAM** - Extended memory for audio buffers

## 🛠️ Hardware Requirements

### Main Components
- **MCU:** ESP32-WROVER-E (dual-core 240MHz, 4MB PSRAM)
- **Radio Chip:** Si4735-D60 (I2C, analog output)
- **Display:** ILI9341 320×240 (SPI)
- **Touch:** XPT2046 (SPI)
- **Encoder:** Rotary encoder with push button
- **Amplifier:** Any I2S-compatible PCM5102 amplifier

### Custom Board
This project uses `boombox_board_v1_0` custom board configuration. Pin mapping is defined in:
- [`components/boombox_board/boombox_board_v1_0/board.h`](components/boombox_board/boombox_board_v1_0/board.h)
- [`components/boombox_board/boombox_board_v1_0/board_pins_config.c`](components/boombox_board/boombox_board_v1_0/board_pins_config.c)

## 🚀 Quick Start

### Prerequisites
1. **ESP-IDF v5.4.0+** - Follow [official installation guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
2. **ESP-ADF** - Clone from [GitHub](https://github.com/espressif/esp-adf)
3. **Node.js 16+** (optional, for WebUI development)

### Installation

```bash
# Clone repository
git clone https://github.com/FirstIamBot/boombox_adf.git
cd boombox_adf

# Set ESP-IDF and ESP-ADF paths
export IDF_PATH=~/esp/esp-idf
export ADF_PATH=~/esp/esp-adf

# Install dependencies
. $IDF_PATH/export.sh

# Configure project (optional)
idf.py menuconfig

# Build firmware
idf.py build

# Flash to device
idf.py -p /dev/ttyUSB0 flash monitor
```

### First Boot

1. **WiFi Setup:**
   - Connect to `ESP32-WiFi-Manager` AP (default password: `esp32pwd`)
   - Open browser → Auto-redirect to captive portal
   - Select your WiFi network and enter password
   - Device will reboot and connect

2. **WebUI Access:**
   - Find device IP in serial monitor or router DHCP list
   - Open `http://<device-ip>` in browser
   - Control all playback modes from WebUI

3. **GUI Control:**
   - Touch display to navigate modes
   - Rotate encoder for volume/frequency
   - Press encoder button to confirm

## 📁 Project Structure

```
boombox_adf/
├── main/                          # Main application
│   ├── app_main.c                 # Entry point, system init
│   ├── boombox.c/h                # Main task, mode switching
│   ├── playlist_parser.c/h        # PLS playlist parser
│   └── task/                      # Mode-specific tasks
│       ├── air_task.c/h           # Si4735 radio control
│       ├── bt_task.c/h            # Bluetooth A2DP sink
│       └── http_task.c/h          # HTTP streaming player
├── components/
│   ├── audio_equalizer/           # 10-band equalizer
│   ├── boombox_board/             # Hardware abstraction
│   │   ├── boombox_board_v1_0/    # Custom board config
│   │   └── boombox_codec_driver/  # ES8388 driver
│   ├── commons/                   # Shared types, queues
│   ├── drv/                       # Display/touch drivers
│   ├── encoder/                   # Rotary encoder driver
│   ├── esp_wifi_manager/          # WiFi manager + WebUI
│   │   ├── frontend/              # Preact TypeScript UI
│   │   └── src/                   # Backend handlers
│   ├── gui/                       # LVGL interface
│   │   ├── generated/             # SquareLine Studio export
│   │   └── custom/                # Custom screens
│   ├── lvgl/                      # LVGL library 8.3.10
│   └── si4735/                    # Si4735 radio driver
├── spiffs_image/
│   └── playlist.pls               # Web radio stations list
├── managed_components/            # ESP Component Registry
├── partitions_bt_sink_example.csv # Flash layout
├── sdkconfig.defaults             # Default ESP-IDF config
└── CHANGELOG.md                   # Version history
```

## ⚙️ Configuration

### Playlist Configuration (`spiffs_image/playlist.pls`)

Edit playlist file to add your favorite stations:

```ini
[playlist]
NumberOfEntries=3

File1=http://stream.example.com:8000/radio
Title1=Example Radio Station
Length1=-1

File2=http://another.stream.com/music.mp3
Title2=Music Stream
Length2=-1
```

After editing, rebuild to update SPIFFS image:
```bash
idf.py build flash
```

### WiFi Manager

Web interface is embedded at compile time. To modify:

```bash
cd components/esp_wifi_manager/frontend
npm install
npm run dev          # Development server
npm run build        # Production build (embeds into firmware)
cd ../../..
idf.py build flash   # Rebuild firmware with new assets
```

### Display Calibration

Touch calibration runs on first boot. To re-calibrate:
1. Erase NVS: `idf.py erase-flash`
2. Reflash firmware
3. Follow on-screen instructions

## 🌐 WebUI API

See [`components/esp_wifi_manager/BOOMBOX_API.md`](components/esp_wifi_manager/BOOMBOX_API.md) for full API documentation.

### Quick Examples

**Get current status:**
```bash
curl http://<device-ip>/api/boombox/status
```

**Switch to FM radio:**
```bash
curl -X POST http://<device-ip>/api/boombox/command \
  -H "Content-Type: application/json" \
  -d '{"cmd": 1, "value": 0}'
```

**Set FM frequency (100.6 MHz):**
```bash
curl -X POST http://<device-ip>/api/boombox/command \
  -H "Content-Type: application/json" \
  -d '{"cmd": 16, "value": 10060}'
```

**Play/Pause:**
```bash
curl -X POST http://<device-ip>/api/boombox/command \
  -H "Content-Type: application/json" \
  -d '{"cmd": 17, "value": 1}'
```

## 🔧 Troubleshooting

### Build Issues

**Error: `components/esp_wifi_manager not found`**
```bash
# esp_wifi_manager should be in components/ as regular directory
# If you cloned with submodules, remove .git:
cd components/esp_wifi_manager
rm -rf .git
cd ../..
git add components/esp_wifi_manager/
```

**Linker error: Not enough IRAM**
- Reduce audio buffer sizes in `idf.py menuconfig`
- Disable debug logging: `Component config → Log output → Error`

### Runtime Issues

**Crash on mode switching:**
- Fixed in version 2026-02-23 (see [CHANGELOG.md](CHANGELOG.md))
- Ensure you have latest code: `git pull origin main`

**No audio output:**
1. Check I2S connections (BCK, WS, DOUT pins)
2. Check speaker/amplifier connections
3. Monitor serial output for codec init errors

**Touch not responsive:**
1. Check XPT2046 SPI connections
2. Run calibration: erase NVS and reflash
3. Verify touch pressure threshold in `menuconfig`

**WiFi won't connect:**
1. Check signal strength (需要 > -70 dBm)
2. Verify password in captive portal
3. Check router MAC filtering
4. Monitor logs: `idf.py monitor`

**WebUI not loading:**
1. Check device IP: `ping <device-ip>`
2. Clear browser cache (Ctrl+Shift+R)
3. Try incognito/private window
4. Verify assets embedded: check build log for `app.js.gz.S`

### Memory Issues

See [`memory_analysis_report.md`](memory_analysis_report.md) for detailed analysis.

**Quick fixes:**
- Enable PSRAM: `CONFIG_SPIRAM=y`
- Reduce ring buffers in audio elements
- Use external stack for tasks: `CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM=y`

## 📊 Memory Usage

| Region | Used | Total | Usage |
|--------|------|-------|-------|
| IRAM | 116 KB | 132 KB | 87.7% |
| DRAM | 129 KB | 201 KB | 64.2% |
| PSRAM | 52 KB | 4 MB | 1.3% |
| Flash | 2.3 MB | 4 MB | 57.5% |

See [`WIFI_MEMORY_FIX.md`](WIFI_MEMORY_FIX.md) for optimization strategies.

## 🗺️ Roadmap

- [ ] Software equalizer integration
- [ ] Spotify Connect support
- [ ] Alarm/timer functionality
- [ ] Multiple playlist support
- [ ] OTA firmware updates
- [ ] MQTT integration
- [ ] AirPlay receiver
- [ ] Hardware volume control API

## 📝 Changelog

See [CHANGELOG.md](CHANGELOG.md) for detailed version history.

**Latest (2026-02-23):**
- ✅ Fixed double-free crash on mode switching
- ✅ Fixed LVGL panic on invalid objects
- ✅ WebUI frequency formatting (100,60 MHz)
- ✅ Decimal frequency input (xxx.xx)

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing`)
3. Commit changes (`git commit -am 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing`)
5. Open Pull Request

## 📄 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [ESP-IDF](https://github.com/espressif/esp-idf) by Espressif
- [ESP-ADF](https://github.com/espressif/esp-adf) Audio Framework
- [LVGL](https://lvgl.io/) Graphics Library
- [Si4735 Library](https://github.com/pu2clr/SI4735) by Ricardo Lima Caratti
- [esp_wifi_manager](https://github.com/tuanpmt/esp_wifi_manager) by Tuan PM

## 📧 Contact

- **Author:** FirstIamBot
- **Repository:** https://github.com/FirstIamBot/boombox_adf
- **Issues:** https://github.com/FirstIamBot/boombox_adf/issues

---

**Made with ❤️ and ESP32**

