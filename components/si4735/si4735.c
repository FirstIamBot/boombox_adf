/**
 * @mainpage SI47XX Arduino Library implementation
 *
 * This is a library for the SI473X and SI474X, BROADCAST AM/FM/SW RADIO RECEIVER, IC from Silicon Labs for the
 * Arduino development environment.  It works with I2C protocol and provides an easy-to-use interface for controlling the SI47XX IC family.<br>
 *
 * This library was built based on [Si47XX PROGRAMMING GUIDE-AN332 (REV 1.0)](https://www.silabs.com/documents/public/application-notes/AN332.pdf) document from Silicon Labs.
 *
 * It also can be used on **all members of the SI473X and SI474X family**, though the features you can use depend on which IC version you have.
 * The functionality of each IC is outlined in the comparison matrix in table 1 (Product Family Function); pages 2 and 3 of the programming guide.
 * If you need to build a prototype based on SI47XX device, see <https://pu2clr.github.io/SI4735/><br>
 *
 * This library has more than 30 examples, and it can be freely distributed using the MIT Free Software model. [Copyright (c) 2019 Ricardo Lima Caratti](https://pu2clr.github.io/SI4735/#mit-license).
 * Contact: pu2clr@gmail.com
 *
 * @details This library uses the I²C communication protocol and implements most of the functions offered by Si47XX (BROADCAST AM / FM / SW / LW RADIO RECEIVER) IC family from Silicon Labs.
 * @details The main features of this library are listed below.
 * @details 1. Open Source. It is free. You can use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software. See [MIT License](https://pu2clr.github.io/SI4735/#mit-license) to learn more.
 * @details 2. Built Based on [Si47XX PROGRAMMING GUIDE](https://www.silabs.com/documents/public/application-notes/AN332.pdf).
 * @details 3. C++ Language and Object-oriented programming. You can easily extend the SI4735 class if you need new functionality.
 * @details 4. Available on Arduino IDE (Manage Libraries).
 * @details 5. Cross-platform. You can compile and run this library on most of boards which Arduino IDE supports (Examples: ATtiny85, boards based on ATmega328 and ATmega-32u4, ATmega2560, 32 ARM Cortex, Arduino DUE, ESP32 and more). See [Boards where this library has been successfully tested](https://pu2clr.github.io/SI4735/#boards-where-this-library-has-been-successfully-tested).
 * @details 6. Simplifies projects based on SS4735-D60, SI4732-A10, SI4730-D60, and other SI473X devices;
 * @details 7. I²C communication and Automatic I²C bus address detection.
 * @details 8. More than 120 functions implemented. You can customize almost every feature available on Si47XX family.
 * @details 9. RDS support.
 * @details 10. SSB (Single Side Band) patch support (SI4735-D60 and SI4732-A10).
 * @details 11. Digital Audio (__Attention__: Crystal and digital audio mode cannot be used at the same time).
 * @details 12. More than 30 example available.  See <https://github.com/pu2clr/SI4735/tree/master/examples><br>
 *
 *  Some texts were extracted directly from the Silicon Labs documentation. The name of the Silicon Labs document and pages are described in the source code comments.
 *
 * @see [General Documentation](https://pu2clr.github.io/SI4735/)
 * @see [Schematics](https://pu2clr.github.io/SI4735/extras/schematic/)
 * @see Si47XX PROGRAMMING GUIDE AN332 (REV 1.0): https://www.silabs.com/documents/public/application-notes/AN332.pdf
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; AMENDMENT FOR SI4735-D60 SSB AND NBFM PATCHES
 *
 * @author PU2CLR - Ricardo Lima Caratti
 * @date  2019-2022
 * @copyright MIT Free Software model. See [Copyright (c) 2019 Ricardo Lima Caratti](https://pu2clr.github.io/SI4735/#mit-license).
 */

#include <si4735.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"

static const char *TAG = "si4735 ";
/**
 * @brief I2C initial defines and function
 * @details 
 * IMPORTANT: 
 * @defgroup
 */
#define I2C_MASTER_WRITE 0
#define I2C_MASTER_READ 1

#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          100000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000                 /* 1000 */
#define WRITE_BIT  I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT   I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN   0x1     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS  0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL    0x0         /*!< I2C ack value */
#define NACK_VAL   0x1         /*!< I2C nack value */

/************************************************************************************************
 *                                  VARIABLE
 ***********************************************************************************************/
char rds_buffer2A[65]; //!<  RDS Radio Text buffer - Program Information
char rds_buffer2B[33]; //!<  RDS Radio Text buffer - Station Informaation
char rds_buffer0A[9];  //!<  RDS Basic tuning and switching information (Type 0 groups)
char rds_time[25]; //!<  RDS date time received information

int rdsTextAdress2A; //!<  rds_buffer2A current position
int rdsTextAdress2B; //!<  rds_buffer2B current position
int rdsTextAdress0A; //!<  rds_buffer0A current position

bool rdsEndGroupA = false;
bool rdsEndGroupB = false;

int16_t deviceAddress = SI473X_ADDR_SEN_LOW; //!<  Stores the current I2C bus address.

// Delays
uint16_t maxDelaySetFrequency = MAX_DELAY_AFTER_SET_FREQUENCY; //!< Stores the maximum delay after set frequency command (in ms).
uint16_t maxDelayAfterPouwerUp = MAX_DELAY_AFTER_POWERUP;  //!< Stores the maximum delay you have to setup after a power up command (in ms).
unsigned long maxSeekTime = MAX_SEEK_TIME; //!< Stores the maximum time (ms) for a seeking process. Defines the maximum seeking time.

uint8_t lastTextFlagAB;

uint8_t currentClockType = XOSCEN_CRYSTAL; //!< Stores the current clock type used (Crystal or REF CLOCK)
uint8_t currentAudioMode = SI473X_DIGITAL_AUDIO2; //!< Current audio mode used (ANALOG or DIGITAL or both)
uint8_t currentSsbStatus = 0;
uint8_t lastMode = -1; //!<  Stores the last mode used.
uint8_t ctsIntEnable = 0;
uint8_t gpo2Enable = 0;


uint16_t refClock = 32768; // 32768; !< Frequency of Reference Clock in Hz.
uint16_t refClockPrescale = 1; //!< Prescaler for Reference Clock (divider).
//uint8_t refClockSourcePin = 1; //!< 0 = RCLK pin is clock source; 1 = DCLK pin is clock source.

int8_t audioMuteMcuPin = -1;

// I2C Master Driver handles for ESP-IDF 5.4
static i2c_master_bus_handle_t bus_handle = NULL;
static i2c_master_dev_handle_t dev_handle = NULL;

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init(void)
{
    esp_err_t ret = ESP_OK;
    
    // Проверяем, не инициализирован ли уже I2C
    if (bus_handle != NULL) {
        ESP_LOGW(TAG, "I2C master already initialized");
        return ESP_OK;
    }
    
    // Конфигурация I2C шины
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT, 
        .i2c_port = I2C_MASTER_NUM,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,// Включить внутренний подтягивающий резистор
    };
    
    // Создаем шину I2C
    ret = i2c_new_master_bus(&i2c_mst_config, &bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C master bus: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Конфигурация устройства SI4735
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = deviceAddress,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    
    // Добавляем устройство на шину
    ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add I2C device: %s", esp_err_to_name(ret));
        i2c_del_master_bus(bus_handle);
        bus_handle = NULL;
        return ret;
    }
    return ESP_OK;
}

esp_err_t i2c_master_deinit(void)
{
    esp_err_t ret = ESP_OK;
    
    // Удаляем устройство с шины
    if (dev_handle != NULL) {
        ret = i2c_master_bus_rm_device(dev_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to remove I2C device: %s", esp_err_to_name(ret));
        }
        dev_handle = NULL;
    }
    
    // Удаляем шину I2C
    if (bus_handle != NULL) {
        ret = i2c_del_master_bus(bus_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to delete I2C master bus: %s", esp_err_to_name(ret));
        }
        bus_handle = NULL;
    }
    
    ESP_LOGW(TAG, "I2C master deinitialized");
    return ret;
}


/**
 * @brief Логирует информацию о I2C команде и данных для SI4735
 * 
 * @param reg_addr Адрес регистра/команды
 * @param data Указатель на данные команды
 */
static void log_i2c_command(uint8_t reg_addr, uint8_t *data)
{
    uint16_t propCMD, propDATA;

    data[0] = reg_addr; // First byte is the register address
    propCMD =  data[2]<<8 | data[3];   
    propDATA =  data[4]<<8 | data[5];

    ESP_LOGI(TAG, "**** I2C  deviceAddress = %x", deviceAddress);

    switch (reg_addr)
    {
        case SET_PROPERTY:
            ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (SET_PROPERTY)", reg_addr);
                switch (propCMD){
                    case REFCLK_FREQ:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (REFCLK_FREQ)", propCMD);
                        break;
                    case REFCLK_PRESCALE:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (REFCLK_PRESCALE)", propCMD);
                        break;
                    case FM_SEEK_FREQ_SPACING:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_SEEK_FREQ_SPACING)",propCMD);
                        break;
                    case RX_VOLUME:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (RX_VOLUME)",propCMD);
                        break;
                    case FM_SEEK_BAND_BOTTOM:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_SEEK_BAND_BOTTOM)",propCMD);
                        break;
                    case FM_SEEK_BAND_TOP:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_SEEK_BAND_TOP)",propCMD);
                        break;
                    case FM_BLEND_RSSI_MONO_THRESHOLD:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_BLEND_RSSI_MONO_THRESHOLD)",propCMD);
                        break;
                    case FM_BLEND_RSSI_STEREO_THRESHOLD:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_BLEND_RSSI_STEREO_THRESHOLD)",propCMD);
                        break;
                    case AM_CHANNEL_FILTER:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (AM_CHANNEL_FILTER)",propCMD);
                        break;  
                    case AM_SOFT_MUTE_MAX_ATTENUATION:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (AM_SOFT_MUTE_MAX_ATTENUATION)",propCMD);
                        break;  
                    case AM_SOFT_MUTE_SNR_THRESHOLD:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (AM_SOFT_MUTE_SNR_THRESHOLD)",propCMD);
                        break;    
                    case AM_SEEK_RSSI_THRESHOLD:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (AM_SEEK_RSSI_THRESHOLD)",propCMD);
                        break;
                    case AM_SEEK_SNR_THRESHOLD:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (AM_SEEK_SNR_THRESHOLD)",propCMD);
                        break;
                    case AM_SEEK_BAND_BOTTOM:
                        ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (AM_SEEK_BAND_BOTTOM)", reg_addr);
                        break;
                    case AM_SEEK_BAND_TOP:
                        ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (AM_SEEK_BAND_TOP)", reg_addr);
                        break;
                    case AM_DEEMPHASIS:
                        ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (AM_DEEMPHASIS)", reg_addr);
                        break;                      
                    case FM_SEEK_TUNE_RSSI_THRESHOLD:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_SEEK_TUNE_RSSI_THRESHOLD)",propCMD);
                        break;
                    case FM_SEEK_TUNE_SNR_THRESHOLD:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_SEEK_TUNE_SNR_THRESHOLD)",propCMD);
                        break;                    
                    case FM_DEEMPHASIS:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_DEEMPHASIS)",propCMD);
                        break;
                    case DIGITAL_OUTPUT_SAMPLE_RATE:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (DIGITAL_OUTPUT_SAMPLE_RATE)",propCMD);
                        break;
                    case DIGITAL_OUTPUT_FORMAT:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (DIGITAL_OUTPUT_FORMAT)",propCMD);
                        break;
                    case FM_RDS_INT_FIFO_COUNT:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_RDS_INT_FIFO_COUNT)",propCMD);
                        break;
                    case FM_RDS_INT_SOURCE:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_RDS_INT_SOURCE)",propCMD);
                        break;     
                    case FM_RDS_CONFIG:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_RDS_CONFIG)",propCMD);
                        break;
                    case FM_SOFT_MUTE_MAX_ATTENUATION:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_SOFT_MUTE_MAX_ATTENUATION)",propCMD);
                        break;     
                    case FM_SOFT_MUTE_SNR_THRESHOLD:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_SOFT_MUTE_SNR_THRESHOLD)",propCMD);
                        break;   
                    case FM_CHANNEL_FILTER:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_CHANNEL_FILTER)",propCMD);
                        break;
                    case FM_DISABLE_DEBUG:
                        ESP_LOGI(TAG, "**** propCMD = 0x%4X (FM_DISABLE_DEBUG)",propCMD);
                        break;           
                    default:
                        ESP_LOGI(TAG, "**** UNKNOW propCMD = 0x%X", propCMD);
                    break;
                }
                ESP_LOGI(TAG, "**** propDATA = 0x%4X", propDATA);
                break;
        case POWER_UP:
            ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (POWER_UP)", reg_addr);
            break;
        case POWER_DOWN:
            ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (POWER_DOWN)", reg_addr);
            break;
        case FM_TUNE_FREQ:
            ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (FM_TUNE_FREQ)", reg_addr);
            break;
        case FM_SEEK_START:
            ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (FM_SEEK_START)", reg_addr);
            break;
        case FM_AGC_OVERRIDE:
            ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (FM_AGC_OVERRIDE)", reg_addr);
            break;
        case GPIO_CTL:
            ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (GPIO_CTL)", reg_addr);
            break;
          case GPIO_SET:
            ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X (GPIO_SET)", reg_addr);
            break; 
        default:
            ESP_LOGI(TAG, "**** I2C  reg_addr= 0x%2X ", reg_addr);
            break;
    }
}

/**
 * @brief Read a sequence of bytes from a registers
 */
esp_err_t register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    if (dev_handle == NULL) {
        ESP_LOGE(TAG, "I2C device not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    return i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS);
}

esp_err_t register_read_block(uint8_t *write_data, size_t len_write, uint8_t *data, size_t len_read)
{
    if (dev_handle == NULL) {
        ESP_LOGE(TAG, "I2C device not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    return i2c_master_transmit_receive(dev_handle, write_data, len_write, data, len_read, I2C_MASTER_TIMEOUT_MS);
}

/**
 * @brief Write a byte to a register
 */
esp_err_t register_write_byte(uint8_t reg_addr, uint8_t data)
{
    if (dev_handle == NULL) {
        ESP_LOGE(TAG, "I2C device not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    uint8_t write_buf[2] = {reg_addr, data};

    if(write_buf[0] == POWER_DOWN){
        ESP_LOGD(TAG, "**** I2C  data[0]= 0x%X (POWER_DOWN)", write_buf[0]);
    }
    ESP_LOGD(TAG, "**** I2C  data[1]=0x%X", write_buf[1]);

    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS);
}

/**
 * @brief Write a block 
 */
esp_err_t register_write_block(uint8_t *data, size_t len)
{
    if (dev_handle == NULL) {
        ESP_LOGE(TAG, "I2C device not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret; 

    ret = i2c_master_transmit(dev_handle, data, len, I2C_MASTER_TIMEOUT_MS);
    
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Write OK");
    } else if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGW(TAG, "Bus is busy");
    } else if (ret == ESP_ERR_INVALID_ARG) {
        ESP_LOGW(TAG, "Invalid  Argument");
    } else {
        ESP_LOGW(TAG, "Write Failed");
    }

    return ret;
}

/**
 * @ingroup group10 Generic send property
 *
 * @brief Sends (sets) property to the SI47XX
 *
 * @details This method is used for others to send generic properties and params to SI47XX
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 68, 124 and  133.
 * @see setProperty, sendCommand, getProperty, getCommandResponse
 *
 * @param propertyNumber property number (example: RX_VOLUME)
 * @param parameter   property value that will be seted
 */
void sendProperty(SI4735_t *cntrl_data, uint16_t propertyNumber, uint16_t parameter)
{
    uint8_t write_buf[6] = {0};
    esp_err_t ret;

    cntrl_data->property.value = propertyNumber;
    cntrl_data->param.value = parameter;

    write_buf[0] = SET_PROPERTY;
    write_buf[1] = 0x00;
    write_buf[2] = cntrl_data->property.raw.byteHigh;
    write_buf[3] = cntrl_data->property.raw.byteLow;
    write_buf[4] = cntrl_data->param.raw.byteHigh;
    write_buf[5] = cntrl_data->param.raw.byteLow;

    //log_i2c_command(propertyNumber, write_buf);

    ret = i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS);
    
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Write OK");
    } else if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGW(TAG, "Bus is busy");
    } else if (ret == ESP_ERR_INVALID_ARG) {
        ESP_LOGW(TAG, "Invalid  Argument");
    } else {
        ESP_LOGW(TAG, "Write Failed");
    }

    delay_ms(1); //delayMicroseconds(550);
}

void initResetPin(SI4735_t *cntrl_data){

    gpio_config_t out_conf = {
        .intr_type = GPIO_INTR_DISABLE,        // Отключить прерывания
        .mode = GPIO_MODE_OUTPUT,              // Режим выхода
        .pin_bit_mask = (1ULL << cntrl_data->resetPin), // Маска пина
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // Отключить подтяжку вниз
        .pull_up_en = GPIO_PULLUP_DISABLE      // Отключить подтяжку вверх
    };
    
    esp_err_t ret = gpio_config(&out_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure reset pin GPIO %d: %s", 
                 cntrl_data->resetPin, esp_err_to_name(ret));
    }
}

// Функция для установки уровня сигнала на пине RESET
esp_err_t setResetPin(SI4735_t *cntrl_data, uint32_t level)
{
    return gpio_set_level(cntrl_data->resetPin, level);
}

// Функция для деинициализации GPIO
esp_err_t deinitResetPin(SI4735_t *cntrl_data)
{
    return gpio_reset_pin(cntrl_data->resetPin);
}

void delay_ms(int ms) {
	int _ms = ms + (portTICK_PERIOD_MS - 1);
	TickType_t xTicksToDelay = _ms / portTICK_PERIOD_MS;
	ESP_LOGD(TAG, " delay :  ms=%d _ms=%d",ms, _ms);
	//ESP_LOGD(TAG, "portTICK_PERIOD_MS=%"PRIu32" xTicksToDelay=%"PRIu32, portTICK_PERIOD_MS, xTicksToDelay);
	vTaskDelay(xTicksToDelay);
}


int scan_i2c_devices()
{
    uint8_t address;
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
    for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
            fflush(stdout);
            address = i + j;
            esp_err_t ret = i2c_master_probe(bus_handle, address, I2C_MASTER_TIMEOUT_MS);
            if (ret == ESP_OK) {
                printf("%02x ", address);
            } else if (ret == ESP_ERR_TIMEOUT) {
                printf("UU ");
            } else {
                printf("-- ");
            }
        }
        printf("\r\n");
        //return address
    }
    return 0;
}

void try_different_addresses(SI4735_t *cntrl_data)
{
    uint8_t addresses[] = {0x11, 0x63, 0x60, 0x61}; // Возможные адреса SI4735
    
    for (int i = 0; i < sizeof(addresses); i++) {
        deviceAddress = addresses[i];
        ESP_LOGI(TAG, "Trying address 0x%02X", deviceAddress);
        
        esp_err_t ret = i2c_master_probe(bus_handle, deviceAddress, I2C_MASTER_TIMEOUT_MS);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "SI4735 found at address 0x%02X", deviceAddress);
            return;
        }
        else if (ret == ESP_ERR_TIMEOUT) {
            ESP_LOGW(TAG, "Address 0x%02X is busy or not responding", deviceAddress);
        } else {
            ESP_LOGE(TAG, "Failed to probe address 0x%02X: %s", deviceAddress, esp_err_to_name(ret));
        }
    }
    ESP_LOGE(TAG, "SI4735 not found at any known address");
}


/********************************************************************************************************************************************************************
*
*
*
*
*
*********************************************************************************************************************************************************************/

void init_si4735(SI4735_t *cntrl_data, uint8_t resetPin, uint8_t ctsIntEnable, uint8_t defaultFunction, uint8_t audioMode, uint8_t clockType, uint8_t gpo2ENABLE)
{
    // Добавьте проверку в начало функции:
    if (cntrl_data == NULL) {
        ESP_LOGE(TAG, "cntrl_data is NULL!");
        return;
    }

    ESP_LOGD(TAG, " init_si4735");
    //ESP_ERROR_CHECK(i2c_master_init());
    delay_ms(500); 
    cntrl_data->resetPin = resetPin; // Set the reset pin number
    initResetPin(cntrl_data);

    ctsIntEnable = (ctsIntEnable != 0) ? 1 : 0; // Keeps old versions of the sketches running
    gpo2Enable = gpo2ENABLE;
    currentAudioMode = audioMode;
    // Set the initial SI473X behavior
    // CTSIEN   interruptEnable -> Interrupt anabled or disable;
    // GPO2OEN  1 -> GPO2 Output Enable;
    // PATCH    0 -> Boot normally;
    // XOSCEN   clockType -> Use external crystal oscillator (0 = XOSCEN_CRYSTAL) or reference clock (1 = XOSCEN_RCLK);
    // FUNC     defaultFunction  0 = FM Receive; 1 = AM (LW/MW/SW) Receiver.
    // OPMODE   SI473X_ANALOG_AUDIO or SI473X_DIGITAL_AUDIO.
    setPowerUp(cntrl_data, ctsIntEnable, gpo2Enable, 0, clockType, defaultFunction, audioMode);
    if (audioMuteMcuPin >= 0)
        setHardwareAudioMute(true); // If you are using external citcuit to mute the audio, it turns the audio mute
    reset(cntrl_data);
    // Сканирование I2C устройств
    //scan_i2c_devices();
    // Попробовать разные адреса
    //try_different_addresses(cntrl_data);
    
    radioPowerUp(cntrl_data);
    delay_ms(1000); 
    getFirmware(cntrl_data);
    cntrl_data->gpio_ien.raw = 0;
}

void radio_deinit(SI4735_t *cntrl_data)
{
    ESP_LOGI(TAG, " deinit_si4735");
    powerDown();
    delay_ms(20);
    gpio_set_level(cntrl_data->resetPin, 0);// Set LOW RESET pin
    i2c_master_deinit();
}

void setPowerUp(SI4735_t *cntrl_data, uint8_t CTSIEN, uint8_t GPO2OEN, uint8_t PATCH, uint8_t XOSCEN, uint8_t FUNC, uint8_t OPMODE)
{
    cntrl_data->powerUp.arg.CTSIEN = CTSIEN;   // 1 -> Interrupt enabled;
    cntrl_data->powerUp.arg.GPO2OEN = GPO2OEN; // 1 -> GPO2 Output Enable;
    cntrl_data->powerUp.arg.PATCH = PATCH;     // 0 -> Boot normally;
    cntrl_data->powerUp.arg.XOSCEN = XOSCEN;   // 1 -> Use external crystal oscillator;
    cntrl_data->powerUp.arg.FUNC = FUNC;       // 0 = FM Receive; 1 = AM/SSB (LW/MW/SW) Receiver.
    cntrl_data->powerUp.arg.OPMODE = OPMODE;   // 0x5 = 00000101 = Analog audio outputs (LOUT/ROUT).
    // Set the current tuning frequancy mode 0X20 = FM and 0x40 = AM (LW/MW/SW)
    // See See Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 55 and 124
    currentClockType = XOSCEN;

    if (FUNC == 0)
    {
        cntrl_data->currentTune = FM_TUNE_FREQ;
        cntrl_data->set_frequency.arg.FREEZE = 1;
    }
    else
    {
        cntrl_data->currentTune = 0x40; //AM_TUNE_FREQ;
        cntrl_data->set_frequency.arg.FREEZE = 0;
    }
    cntrl_data->set_frequency.arg.FAST = 1;
    cntrl_data->set_frequency.arg.DUMMY1 = 0;
    cntrl_data->set_frequency.arg.ANTCAPH = 0;
    cntrl_data->set_frequency.arg.ANTCAPL = 1;
}

/**
 * @ingroup group06 RESET
 *
 * @brief Reset the SI473X
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0);
 */
void reset(SI4735_t *cntrl_data)
{

    delay_ms(500);
    // Установить LOW уровень (активный сброс)
    esp_err_t ret = gpio_set_level(cntrl_data->resetPin, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set reset pin LOW: %s", esp_err_to_name(ret));
    }
    delay_ms(500);
    // Установить HIGH уровень (снять сброс)
    ret = gpio_set_level(cntrl_data->resetPin, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set reset pin HIGH: %s", esp_err_to_name(ret));
    }
    delay_ms(1000);
}

/**
 * @ingroup group18 MCU External Audio Mute
 *
 * @brief Sets the Hardware Audio Mute
 * @details Turns the Hardware audio mute on or off
 *
 * @see setAudioMuteMcuPin
 *
 * @param on  True or false
 */
 void setHardwareAudioMute(bool on)
{
    //digitalWrite(audioMuteMcuPin, on);
   delay_ms(1);//delayMicroseconds(300);
}
/**
 * @ingroup group07 Device Power Up
 *
 * @brief Powerup the Si47XX
 *
 * @details Before call this function call the setPowerUp to set up the parameters.
 *
 * @details Parameters you have to set up with setPowerUp
 *
 * | Parameter | Description |
 * | --------- | ----------- |
 * | CTSIEN    | Interrupt anabled or disabled |
 * | GPO2OEN   | GPO2 Output Enable or disabled |
 * | PATCH     | Boot normally or patch |
 * | XOSCEN    | 0 (XOSCEN_RCLK) = external active crystal oscillator. 1 (XOSCEN_CRYSTAL) = passive crystal oscillator;  |
 * | FUNC      | defaultFunction = 0 = FM Receive; 1 = AM (LW/MW/SW) Receiver |
 * | OPMODE    | SI473X_ANALOG_AUDIO (B00000101) or SI473X_DIGITAL_AUDIO (B00001011) |
 *
 * ATTENTION: The document AN383; "Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES"; rev 0.8; page 6; there is the following note:
 *            Crystal and digital audio mode cannot be used at the same time. Populate R1 and remove C10, C11, and X1 when using digital audio.
 *
 * @see setMaxDelaySetFrequency()
 * @see MAX_DELAY_AFTER_POWERUP
 * @see XOSCEN_CRYSTAL
 * @see XOSCEN_RCLK
 * @see  setPowerUp()
 * @see  Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 64, 129
 */
void radioPowerUp(SI4735_t *cntrl_data)
{
    uint8_t write_buf[3];

    delay_ms(1);
    write_buf[0] = POWER_UP; // 0x01; // 
    write_buf[1] = cntrl_data->powerUp.raw[0]; // 0x80; // 
    write_buf[2] = cntrl_data->powerUp.raw[1]; // 0xB0; // 
    ESP_LOGD(TAG, " radioPowerUp  POWER_UP");
    ESP_ERROR_CHECK(register_write_block(write_buf, 3));
    // Delay at least 500 ms between powerup command and first tune command to wait for
    // the oscillator to stabilize if XOSCEN is set and crystal is used as the RCLK.
    delay_ms(500);
    
    if (currentClockType == XOSCEN_RCLK)
    {
        setRefClock(cntrl_data, refClock);
        setRefClockPrescaler(cntrl_data, refClockPrescale, 1);
    }
    // Turns the external mute circuit off
    if (audioMuteMcuPin >= 0)
        setHardwareAudioMute(false);
}

/**
 * @ingroup group07
 * @brief Sets the frequency of the REFCLK from the output of the prescaler
 * @details The REFCLK range is 31130 to 34406 Hz (32768 ±5% Hz) in 1 Hz steps, or 0 (to disable AFC). For example, an RCLK of 13 MHz would require a prescaler value of 400 to divide it to 32500 Hz REFCLK.
 * @details The reference clock frequency property would then need to be set to 32500 Hz.
 * @details RCLK frequencies between 31130 Hz and 40 MHz are supported, however, there are gaps in frequency coverage for prescaler values ranging from 1 to 10, or frequencies up to 311300 Hz. See table below.
 *
 * Table REFCLK Prescaler
 * | Prescaler  | RCLK Low (Hz) | RCLK High (Hz)   |
 * | ---------- | ------------- | ---------------- |
 * |    1       |   31130       |   34406          |
 * |    2       |   62260       |   68812          |
 * |    3       |   93390       |  103218          |
 * |    4       |  124520       |  137624          |
 * |    5       |  155650       |  172030          |
 * |    6       |  186780       |  206436          |
 * |    7       |  217910       |  240842          |
 * |    8       |  249040       |  275248          |
 * |    9       |  280170       |  309654          |
 * |   10       |  311300       |  344060          |
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 34 and 35
 *
 * @param refclk The allowed REFCLK frequency range is between 31130 and 34406 Hz (32768 ±5%), or 0 (to disable AFC).
 */
void setRefClock(SI4735_t *cntrl_data, uint16_t refclk)
{
    ESP_LOGD(TAG, " setRefClock REFCLK_FREQ");
    sendProperty(cntrl_data, REFCLK_FREQ, refclk);
    cntrl_data->refClock = refclk;
}

/**
 * @ingroup group07
 * @brief Sets the number used by the prescaler to divide the external RCLK down to the internal REFCLK.
 * @details The range may be between 1 and 4095 in 1 unit steps.
 * @details For example, an RCLK of 13 MHz would require a prescaler value of 400 to divide it to 32500 Hz. The reference clock frequency property would then need to be set to 32500 Hz.
 * @details ATTENTION by default, this function assumes you are using the RCLK pin as clock source.
 * @details Example: The code below shows the setup for an active 4.9152MHz crystal
 * @code
 *   setRefClock(rx, 32768);
 *   setRefClockPrescaler(rx, 150, 0); // will work with 4915200Hz active crystal => 4.9152MHz => (32768 x 150)
 *   setup(rx, RESET_PIN, 0, POWER_UP_AM, SI473X_ANALOG_AUDIO, XOSCEN_RCLK);
 * @endcode
 * @details Example: The code below shows the setup for an active 13MHz crystal
 * @code
 *   setRefClock(rx, 32500);
 *   setRefClockPrescaler(rx, 400, 0); // 32500 x 400 = 13000000 (13MHz)
 *   setup(rx, RESET_PIN, 0, POWER_UP_AM, SI473X_ANALOG_AUDIO, XOSCEN_RCLK);
 * @endcode
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 34 and 35
 *
 * @param prescale  Prescaler for Reference Clock value; Between 1 and 4095 in 1 unit steps. Default is 1.
 * @param rclk_sel  0 = RCLK pin is clock source (default); 1 = DCLK pin is clock source
 */
void setRefClockPrescaler(SI4735_t *cntrl_data, uint16_t prescale, uint8_t rclk_sel)
{
    ESP_LOGD(TAG, " setRefClockPrescaler REFCLK_PRESCALE");
    sendProperty(cntrl_data, REFCLK_PRESCALE, prescale); //| (rclk_sel << 13)); // Sets the D12 to rclk_sel
    cntrl_data->refClockPrescale = prescale;
    cntrl_data->refClockSourcePin = rclk_sel;
}

/**
 * @ingroup group13 Audio volume
 *
 * @brief Sets volume level (0  to 63)
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 62, 123, 170, 173 and 204
 *
 * @param uint8_t volume (domain: 0 - 63)
 */
void setVolume(SI4735_t *cntrl_data, uint8_t volume)
{
    ESP_LOGD(TAG, " setVolume RX_VOLUME");
    cntrl_data->volume = volume;
    sendProperty(cntrl_data, RX_VOLUME, cntrl_data->volume);
}
/**
 * @ingroup group07 Firmware Information
 *
 * @brief Gets firmware information
 * @details The firmware information will be stored in firmwareInfo member variable
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 66, 131
 * @see firmwareInfo
 */
void getFirmware(SI4735_t *cntrl_data)
{
    ESP_LOGD(TAG, "I2C getFirmware");
    ESP_ERROR_CHECK(register_read(GET_REV, cntrl_data->firmware_information.raw, sizeof(cntrl_data->firmware_information.raw)));
    for( uint8_t i=0; i<9; ++i){
        ESP_LOGD(TAG, "firmwareInfo.raw[%d]=0x%X", i, cntrl_data->firmware_information.raw[i]);
    }
}

/**
 * @ingroup   group08 Internal Antenna Tuning capacitor
 *
 * @brief Selects the tuning capacitor value.
 *
 * @details On FM mode, the Antenna Tuning Capacitor is valid only when using TXO/LPI pin as the antenna input.
 * This selects the value of the antenna tuning capacitor manually, or automatically if set to zero.
 * The valid range is 0 to 191. Automatic capacitor tuning is recommended.
 * For example, if the varactor is set to a value of 5 manually, when read back the value will be 1.
 * @details on AM mode, If the value is set to anything other than 0, the tuning capacitance is manually set as 95 fF x ANTCAP + 7 pF.
 * ANTCAP manual range is 1–6143. Automatic capacitor tuning is recommended. In SW mode, ANTCAPH[15:8] (high byte) needs to be set to 0 and ANTCAPL[7:0] (low byte) needs to be set to 1.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 71 and 136
 *
 * @param capacitor If zero, the tuning capacitor value is selected automatically.
 *                  If the value is set to anything other than 0:
 *                  AM - the tuning capacitance is manually set as 95 fF x ANTCAP + 7 pF.
 *                       ANTCAP manual range is 1–6143;
 *                  FM - the valid range is 0 to 191.
 *                  According to Silicon Labs, automatic capacitor tuning is recommended (value 0).
 */
void setTuneFrequencyAntennaCapacitor(SI4735_t *cntrl_data, uint16_t capacitor)
{
    cntrl_data->antenna_capacitor.value = capacitor;
    cntrl_data->set_frequency.arg.DUMMY1 = 0;

    if (cntrl_data->currentTune != AM_TUNE_FREQ)
    {
        // For FM, the capacitor value has just one byte
        cntrl_data->set_frequency.arg.ANTCAPH = (capacitor <= 191) ? cntrl_data->antenna_capacitor.raw.ANTCAPL : 0;
    }
    else
    {
        if (capacitor <= 6143)
        {
            cntrl_data->set_frequency.arg.FREEZE = 0; // This parameter is not used for AM
            cntrl_data->set_frequency.arg.ANTCAPH = cntrl_data->antenna_capacitor.raw.ANTCAPH;
            cntrl_data->set_frequency.arg.ANTCAPL = cntrl_data->antenna_capacitor.raw.ANTCAPL;
        }
    }
    // Tune the device again with the current frequency.
    ESP_LOGD(TAG, " setTuneFrequencyAntennaCapacitor");
    setFrequency(cntrl_data, cntrl_data->frequency.value);
}

/**
 * @ingroup   group08 Tune Frequency
 *
 * @brief Set the frequency to the corrent function of the Si4735 (FM, AM or SSB)
 *
 * @details You have to call setup or setPowerUp before call setFrequency.
 *
 * @see maxDelaySetFrequency()
 * @see MAX_DELAY_AFTER_SET_FREQUENCY
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 70, 135
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 13
 *
 * @param uint16_t  freq is the frequency to change. For example, FM => 10390 = 103.9 MHz; AM => 810 = 810 kHz.
 */
void setFrequency(SI4735_t *cntrl_data, uint16_t freq)
{
    uint8_t write_buf[6] = {0};
    //waitToSend(); // Wait for the si473x is ready.
    cntrl_data->frequency.value = freq;
    cntrl_data->set_frequency.arg.FREQH = cntrl_data->frequency.raw.FREQH;
    cntrl_data->set_frequency.arg.FREQL = cntrl_data->frequency.raw.FREQL;

    if (currentSsbStatus != 0)
    {
        cntrl_data->set_frequency.arg.DUMMY1 = 0;
        cntrl_data->set_frequency.arg.USBLSB = currentSsbStatus; // Set to LSB or USB
        cntrl_data->set_frequency.arg.FAST = 1;                  // Used just on AM and FM
        cntrl_data->set_frequency.arg.FREEZE = 0;                // Used just on FM
    }

    write_buf[1] = cntrl_data->set_frequency.raw[0];
    write_buf[2] = cntrl_data->set_frequency.arg.FREQH;
    write_buf[3] = cntrl_data->set_frequency.arg.FREQL;
    write_buf[4] = cntrl_data->set_frequency.arg.ANTCAPH;

    ESP_LOGD(TAG, " setFrequency");
    ESP_LOGD(TAG, " cntrl_data->set_frequency.arg = 0x%x%x(%d)", cntrl_data->set_frequency.arg.FREQH,cntrl_data->set_frequency.arg.FREQL, cntrl_data->frequency.value);

    if (cntrl_data->currentTune == FM_TUNE_FREQ){
         // if FM
        write_buf[0] = FM_TUNE_FREQ;
        ESP_ERROR_CHECK(register_write_block(write_buf, 5));
    }
    else if (cntrl_data-> currentTune == AM_TUNE_FREQ) {// if AM or SSB
        write_buf[0] = AM_TUNE_FREQ;
        write_buf[5] = cntrl_data->set_frequency.arg.ANTCAPL;
        ESP_ERROR_CHECK(register_write_block( write_buf, 6));
    }
    delay_ms(maxDelaySetFrequency); // For some reason I need to delay here.
}

/**
 * @brief Sets the Bandwith on FM mode
 * @details Selects bandwidth of channel filter applied at the demodulation stage. Default is automatic which means the device automatically selects proper channel filter. <BR>
 * @details | Filter  | Description |
 * @details | ------- | -------------|
 * @details |0        | Automatically select proper channel filter (Default) |
 * @details |1        | Force wide (110 kHz) channel filter |
 * @details |2        | Force narrow (84 kHz) channel filter |
 * @details |3        | Force narrower (60 kHz) channel filter |
 * @details |4        | Force narrowest (40 kHz) channel filter |
 *
 * @param filter_value
 */
 void setFmBandwidth(SI4735_t *cntrl_data, uint8_t filter_value )
{
    ESP_LOGD(TAG, " setFmBandwidth FM_CHANNEL_FILTER");
    sendProperty(cntrl_data, FM_CHANNEL_FILTER, filter_value);
}

/**
 * @ingroup group08
 * @brief Sets the FM Receive de-emphasis to 50 or 75 μs.
 * @details valid parameters are 1 = 50 μs. Usedin Europe, Australia, Japan; 2 = 75 μs. Used in USA (default)
 *
 * @param parameter 1 or 2 (default 1 - USA)
 */
 void setFMDeEmphasis(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setFMDeEmphasis FM_DEEMPHASIS");
    sendProperty(cntrl_data, FM_DEEMPHASIS, parameter);
}

/**
 * @ingroup group12 FM Mono Stereo audio setup
 *
 * @brief Sets RSSI threshold for stereo blend. (Full stereo above threshold, blend below threshold.)
 *
 * @details To force stereo, set this to 0. To force mono, set this to 127. Default value is 49 dBμV.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 59.
 *
 * @param parameter valid values: 0 to 127
 */
void setFmBlendRssiStereoThreshold(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setFmBlendRssiStereoThreshold");
    sendProperty(cntrl_data, FM_BLEND_RSSI_STEREO_THRESHOLD, parameter);
}

/**
 * @ingroup group12 FM Mono Stereo audio setup
 *
 * @brief Sets RSSI threshold for mono blend (Full mono below threshold, blend above threshold).
 *
 * @details To force stereo, set this to 0. To force mono, set this to 127. Default value is 30 dBμV.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 59.
 *
 * @param parameter valid values: 0 to 127
 */
void setFmBLendRssiMonoThreshold(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setFmBLendRssiMonoThreshold");
    sendProperty(cntrl_data, FM_BLEND_RSSI_MONO_THRESHOLD, parameter);
}

/**
 * @ingroup group08
 * @brief Sets the Fm Soft Mute Max Attenuation
 *
 * @details This function can be useful to disable Soft Mute on FM mode. The value 0 disable soft mute.
 * @details Specified in units of dB. Default maximum attenuation is 8 dB. It works for AM and SSB.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 97.
 *
 * @param smattn Maximum attenuation to apply when in soft mute
 */
void setFmSoftMuteMaxAttenuation(SI4735_t *cntrl_data, int8_t smattn )
{
    ESP_LOGD(TAG, " setFmSoftMuteMaxAttenuation");
    sendProperty(cntrl_data, FM_SOFT_MUTE_MAX_ATTENUATION, smattn);
}

/**
 * @ingroup group08
 * @brief Sets the Fm Soft Mute Max Attenuation
 *
 * @details This function can be useful to disable Soft Mute on FM mode. The value 0 disable soft mute.
 * @details Specified in units of dB. Default maximum attenuation is 8 dB. It works for AM and SSB.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 97.
 *
 * @param smattn Maximum attenuation to apply when in soft mute
 */
void setFmSoftMuteSnrAttenuation(SI4735_t *cntrl_data, uint8_t smattn )
{
    ESP_LOGD(TAG, " setFmSoftMuteSnrAttenuation");
    sendProperty(cntrl_data, FM_SOFT_MUTE_SNR_THRESHOLD, smattn);
}

/**
 * @ingroup group08 Seek
 *
 * @brief Sets the bottom frequency and top frequency of the FM band for seek. Default is 8750 to 10790.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 100 and  101
 *
 * @param uint16_t bottom - the bottom of the FM(VHF) mode for seek
 * @param uint16_t    top - the top of the FM(VHF) mode for seek
 */
void setSeekFmLimits(SI4735_t *cntrl_data, uint16_t bottom, uint16_t top)
{
    ESP_LOGD(TAG, " setSeekFmLimits - bottom FM_SEEK_BAND_BOTTOM");
    sendProperty(cntrl_data, FM_SEEK_BAND_BOTTOM, bottom);
    ESP_LOGD(TAG, " setSeekFmLimits - top FM_SEEK_BAND_TOP");
    sendProperty(cntrl_data, FM_SEEK_BAND_TOP, top);
}

/**
 * @ingroup group08 Seek
 *
 * @brief Selects frequency spacingfor FM seek. Default is 100 kHz (value 10) spacing. There are only 3 valid values: 5, 10, and 20.
 * @details Although the guide does not mention it, the value 1 (10 kHz) seems to work better
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 101
 *
 * @param uint16_t spacing - step in kHz
 */
void setSeekFmSpacing(SI4735_t *cntrl_data, uint16_t spacing)
{
    ESP_LOGD(TAG, " setSeekFmSpacing FM_SEEK_FREQ_SPACING");
    sendProperty(cntrl_data, FM_SEEK_FREQ_SPACING, spacing);
}

/**
 * @ingroup group08 Seek
 * @brief Set the Seek Fm Srn Threshold object
 * @deprecated Use setSeekFmSNRThreshold instead.
 * @see setSeekFmSNRThreshold
 * @param value
 */
void  setSeekFmSrnThreshold(SI4735_t *cntrl_data, uint16_t value) 
{
    ESP_LOGD(TAG, " setSeekFmSrnThreshol FM_SEEK_TUNE_SNR_THRESHOLD"); 
    sendProperty(cntrl_data, FM_SEEK_TUNE_SNR_THRESHOLD, value);
} // Wrong name. Will be removed later

/**
 * @ingroup group08 Seek
 *  
 * @brief Sets the RSSI threshold for a valid FM Seek/Tune.
 *
 * @details RSSI threshold which determines if a valid channel has been found during seek/tune. Specified in units of dBμV in 1 dBμV steps (0–127). Default is 20 dBμV.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 102
 */
void setSeekFmRssiThreshold(SI4735_t *cntrl_data, uint16_t value)
{
    ESP_LOGD(TAG, " setSeekFmRssiThreshold FM_SEEK_TUNE_RSSI_THRESHOLD");
    sendProperty(cntrl_data, FM_SEEK_TUNE_RSSI_THRESHOLD, value);
}

/**
 * @ingroup group12 FM Mono Stereo audio setup
 *
 * @brief Sets SNR threshold for stereo blend (Full stereo above threshold, blend below threshold).
 *
 * @details To force stereo, set this to 0. To force mono, set this to 127. Default value is 27 dB.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 59.
 *
 * @param parameter valid values: 0 to 127
 */
void setFmBlendSnrStereoThreshold(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setFmBlendSnrStereoThreshold");
    sendProperty(cntrl_data, FM_BLEND_SNR_STEREO_THRESHOLD, parameter);
}

/**
 * @ingroup group12 FM Mono Stereo audio setup
 *
 * @brief Sets RSSI threshold for mono blend (Full mono below threshold, blend above threshold).
 *
 * @details To force stereo set this to 0. To force mono set this to 127. Default value is 30 dBμV.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 56.
 *
 * @param parameter valid values: 0 to 127
 */
void setFmBlendMonoThreshold(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setFmBlendMonoThreshold");
    sendProperty(cntrl_data, FM_BLEND_MONO_THRESHOLD, parameter);
}

/**
 * @ingroup group12 FM Mono Stereo audio setup
 *
 * @brief Sets SNR threshold for mono blend (Full mono below threshold, blend above threshold).
 *
 * @details To force stereo, set this to 0. To force mono, set this to 127. Default value is 14 dB.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 59.
 *
 * @param parameter valid values: 0 to 127
 */
void setFmBLendSnrMonoThreshold(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setFmBLendSnrMonoThreshold");
    sendProperty(cntrl_data, FM_BLEND_SNR_MONO_THRESHOLD, parameter);
}

/**
 * @ingroup group12 FM Mono Stereo audio setup
 *
 * @brief Sets RSSI threshold for stereo blend (Full stereo above threshold, blend below threshold).
 *
 * @details To force stereo, set this to 0. To force mono, set this to 127.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 90.
 *
 * @param parameter  valid values: 0 to 127
 */
void setFmBlendStereoThreshold(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setFmBlendStereoThreshold");
    sendProperty(cntrl_data, FM_BLEND_STEREO_THRESHOLD, parameter);
}

/**
 * @ingroup group12 FM Mono Stereo audio setup
 *
 * @brief Sets multipath threshold for stereo blend (Full stereo below threshold, blend above threshold).
 *
 * @details To force stereo, set this to 100. To force mono, set this to 0. Default value is 20.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 60.
 *
 * @param parameter valid values: 0 to 100
 */
void setFmBlendMultiPathStereoThreshold(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setFmBlendMultiPathStereoThreshold");
    sendProperty(cntrl_data, FM_BLEND_MULTIPATH_STEREO_THRESHOLD, parameter);
}

/**
 * @ingroup group12 FM Mono Stereo audio setup
 *
 * @brief Sets Multipath threshold for mono blend (Full mono above threshold, blend below threshold).
 *
 * @details To force stereo, set to 100. To force mono, set to 0. The default is 60.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 60.
 *
 * @param parameter valid values: 0 to 100
 */
void setFmBlendMultiPathMonoThreshold(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setFmBlendMultiPathMonoThreshold");
    sendProperty(cntrl_data, FM_BLEND_MULTIPATH_MONO_THRESHOLD, parameter);
}

/**
 * @brief Set the Fm Noise Blank Threshold
 * @details Sets the threshold for detecting impulses in dB above the noise floor. The CTS bit (and optional interrupt) is set when it is safe to send the next command.
 * @param parameter (from 0 to 90. default is 10)
 */
 void setFmNoiseBlankThreshold(SI4735_t *cntrl_data, uint16_t parameter)
{
    ESP_LOGD(TAG, " setFmNoiseBlankThreshold");
    sendProperty(cntrl_data, FM_NB_DETECT_THRESHOLD, parameter);
}

/**
 * @brief Set the Fm Noise Blank
 * @details Sets Noise blanking rate in 100 Hz units
 * @details Sets the Interval in micro-seconds that original samples are replaced by sample-hold clean samples.
 * @details Sets the bandwidth of the noise floor estimator.
 *
 * @details ATTENTION: It works on SI474X. It may not work on SI473X devices.
 *
 * @param nb_rate Noise blanking rate in 100 Hz units. Default value is 64.
 * @param nb_interval Interval in micro-seconds that original samples are replaced by interpolated clean samples. Default value is 55 μs.
 * @param nb_irr_filter Sets the bandwidth of the noise floor estimator. Default value is 300.
 */
 void setFmNoiseBlank(SI4735_t *cntrl_data, uint16_t nb_rate, uint16_t nb_interval, uint16_t nb_irr_filter)
{
    ESP_LOGD(TAG, " setFmNoiseBlank 3-bank FM_NB_RATE");
    sendProperty(cntrl_data, FM_NB_RATE, nb_rate);
    sendProperty(cntrl_data, FM_NB_INTERVAL, nb_interval);
    sendProperty(cntrl_data, FM_NB_IIR_FILTER, nb_irr_filter);
}

/**
 * @ingroup group08 Set mode and Band
 *
 * @brief Sets the radio to AM (LW/MW/SW) function.
 *
 * @details The example below sets the band from 550kHz to 1750kHz on AM mode. The band will start on 810kHz and step is 10kHz.
 *
 * @code
 * si4735.setAM(520, 1750, 810, 10);
 * @endcode
 *
 * @see setFM()
 * @see setSSB()
 *
 * @param fromFreq minimum frequency for the band
 * @param toFreq maximum frequency for the band
 * @param initialFreq initial frequency
 * @param step step used to go to the next channel
 */
void setAM(SI4735_t *cntrl_data, uint16_t fromFreq, uint16_t toFreq, uint16_t initialFreq, uint16_t step)
{
    cntrl_data->AvcAmMaxGain = DEFAULT_CURRENT_AVC_AM_MAX_GAIN;

    if (initialFreq < fromFreq || initialFreq > toFreq)
        initialFreq = fromFreq;

    if (lastMode != AM_CURRENT_MODE)
    {
        powerDown();
        setPowerUp(cntrl_data, ctsIntEnable, 0, 0, currentClockType, AM_CURRENT_MODE, currentAudioMode);
        radioPowerUp(cntrl_data);
        setAvcAmMaxGain(cntrl_data, cntrl_data->AvcAmMaxGain); // Set AM Automatic Volume Gain (default value is DEFAULT_CURRENT_AVC_AM_MAX_GAIN)
        //setVolume(cntrl_data, volume);                    // Set to previus configured volume
    }
    currentSsbStatus = 0;
    lastMode = AM_CURRENT_MODE;
    setFrequency(cntrl_data, initialFreq);
}

/**
 * @ingroup group08 Automatic Volume Control
 *
 * @brief Sets the gain for automatic volume control.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 152
 * @see setAvcAmMaxGain()
 *
 * @param uint8_t gain  Select a value between 12 and 90.  Defaul value 48dB.
 */
void setAvcAmMaxGain(SI4735_t *cntrl_data, uint8_t gain)
{
    if (gain < 12 || gain > 90)
        return;
    cntrl_data->AvcAmMaxGain = gain;
    sendProperty(cntrl_data, AM_AUTOMATIC_VOLUME_CONTROL_MAX_GAIN, gain * 340);
}

/**
 * @ingroup group08 Seek
 *
 * @brief Selects frequency spacingfor AM seek. Default is 10 kHz spacing.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 163, 229 and 283
 *
 * @param uint16_t spacing - step in kHz
 */
void setSeekAmSpacing(SI4735_t *cntrl_data, uint16_t spacing)
{
    ESP_LOGD(TAG, " setSeekAmSpacing AM_SEEK_FREQ_SPACING");
    sendProperty(cntrl_data, AM_SEEK_FREQ_SPACING, spacing);
}

/**
 * @brief Set the Am Noise Blank
 *
 * @details Sets Noise blanking rate in 100 Hz units
 * @details Sets the Interval in micro-seconds that original samples are replaced by sample-hold clean samples.
 * @details Sets the bandwidth of the noise floor estimator.
 *
 * @details ATTENTION: It works on SI474X. It may not work on SI473X devices.
 *
 * @param nb_rate Noise blanking rate in 100 Hz units. Default value is 64.
 * @param nb_interval Interval in micro-seconds that original samples are replaced by interpolated clean samples. Default value is 55 μs.
 * @param nb_irr_filter Sets the bandwidth of the noise floor estimator. Default value is 300.
 *
 */
 void setAmNoiseBlank(SI4735_t *cntrl_data, uint16_t nb_rate, uint16_t nb_interval, uint16_t nb_irr_filter)
{
    ESP_LOGD(TAG, " setAmNoiseBlank - 3 bank");
    sendProperty(cntrl_data, AM_NB_RATE, nb_rate);
    sendProperty(cntrl_data, AM_NB_INTERVAL, nb_interval);
    sendProperty(cntrl_data, AM_NB_IIR_FILTER, nb_irr_filter);
}

/**
 * @ingroup group08
 * @brief Sets the AM attenuation slope during soft mute
 * @details Configures attenuation slope during soft mute in dB attenuation per dB SNR below the soft mute SNR threshold.
 * @details Soft mute attenuation is the minimum of SMSLOPEx(SMTHR–SNR) and SMATTN.
 * @details The default slope is 1 dB/dB for AMRX component 5.0 or later and 2 dB/dB for AMRX component 3.0 or earlier.
 *
 * @see setAmSoftMuteMaxAttenuation
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0);
 * @param parameter  the valid values are 1–5 (default 1).
 */
 void setAMSoftMuteSlop(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setAMSoftMuteSlop");
    sendProperty(cntrl_data, AM_SOFT_MUTE_SLOPE, parameter);
}

/**
 * @ingroup group08
 * @brief Sets the attack and decay rates when entering or leaving soft mute.
 * @details The value specified is multiplied by 4.35 dB/s to come up with the actual attack rate
 * @details The default rate is 278 dB/s.
 * @see setAmSoftMuteMaxAttenuation
 * @see Si47XX PRORAMMING GUIDE; AN332 (REV 1.0);
 * @param parameter  The valid values are 1-255  ( Default is ~64 - [64 x 4.35 = 278] )
 */
 void setAMSoftMuteRate(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setAMSoftMuteRate");
    sendProperty(cntrl_data, AM_SOFT_MUTE_RATE, parameter);
}

/**
 * @ingroup group08
 * @brief Sets the soft mute attack rate.
 * @details Smaller values provide slower attack and larger values provide faster attack.
 * @see setAmSoftMuteMxAttenuation
 * @see Si47XX PROAMMING GUIDE; AN332 (REV 1.0);
 * @param parameter  1–32767 (The default is 8192 (approximately 8000 dB/s)
 */
 void setAMSoftMuteAttackRate(SI4735_t *cntrl_data, uint16_t parameter)
{
    ESP_LOGD(TAG, " setAMSoftMuteAttackRate");
    sendProperty(cntrl_data, AM_SOFT_MUTE_ATTACK_RATE, parameter);
}

/**
 * @ingroup group08
 * @brief Sets the AGC attack rate.
 * @details Large values provide slower attack, and smaller values provide faster attack..
 * @see setAmAgcAttackRate
 * @see Si47XX PROAMMING GUIDE; AN332 (REV 1.2); page 167
 * @param parameter Range: 4–248 (The default is 0x04)
 */
 void setAmAgcAttackRate(SI4735_t *cntrl_data, uint16_t parameter)
{
    ESP_LOGD(TAG, " setAmAgcAttackRate");
    sendProperty(cntrl_data, AM_AGC_ATTACK_RATE, parameter);
}

/**
 * @ingroup group08
 * @brief Sets the AGC release rate.
 * @details  Larger values provide slower release, and smaller values provide faster release.
 * @see setAmAgcReleaseRate
 * @see Si47XX PROAMMING GUIDE; AN332 (REV 1.2); page 168
 * @param parameter Range: 4–248 (The default is 0x8C)
 */
 void setAmAgcReleaseRate(SI4735_t *cntrl_data, uint16_t parameter)
{
    ESP_LOGD(TAG, " setAmAgcReleaseRate");
    sendProperty(cntrl_data, AM_AGC_RELEASE_RATE, parameter);
}

/** 
 * @ingroup group08 Set bandwidth
 *
 * @brief Selects the bandwidth of the channel filter for AM reception.
 *
 * @details The choices are 6, 4, 3, 2, 2.5, 1.8, or 1 (kHz). The default bandwidth is 2 kHz. It works only in AM / SSB (LW/MW/SW)
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 125, 151, 277, 181.
 *
 * @param AMCHFLT the choices are:   0 = 6 kHz Bandwidth
 *                                   1 = 4 kHz Bandwidth
 *                                   2 = 3 kHz Bandwidth
 *                                   5 = 1.8 kHz Bandwidth
 *                                   6 = 2.5 kHz Bandwidth, gradual roll off*                                   
 *                                   3 = 2 kHz Bandwidth
 *                                   4 = 1 kHz Bandwidth
 *                                   7–15 = Reserved (Do not use).
 * @param AMPLFLT Enables the AM Power Line Noise Rejection Filter.
 */
void setAmBandwidth(SI4735_t *cntrl_data, uint8_t AMCHFLT, uint8_t AMPLFLT)
{
    uint8_t write_buf[5] = {0};

    if (cntrl_data->currentTune != AM_TUNE_FREQ) // Only for AM/SSB mode
        return;

    if (AMCHFLT > 6)
        return;

    cntrl_data->bandwidth_config.raw[0] = cntrl_data->bandwidth_config.raw[1] = 0;

    cntrl_data->property.value = AM_CHANNEL_FILTER;
    cntrl_data->bandwidth_config.param.AMCHFLT = AMCHFLT;
    cntrl_data->bandwidth_config.param.AMPLFLT = AMPLFLT;

    write_buf[0] = 0x00;
    write_buf[1] = cntrl_data->property.raw.byteHigh;
    write_buf[2] = cntrl_data->property.raw.byteLow;
    write_buf[3] = cntrl_data->bandwidth_config.raw[1];
    write_buf[4] = cntrl_data->bandwidth_config.raw[0];
    
    ESP_LOGD(TAG, " setBandwidth");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));
    
}

/**
 * @ingroup group08
 * @brief Sets the AM Receive de-emphasis to 50 or disable.
 * @details valid parameters are 1 = 50 μs. Usedin urope, Australia, Japan; 2 = 75 μs. Used in USA (default)
 *
 * @param parameter 1 = enable or 0 = disable
 */
void setAMDeEmphasis(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setAMDeEmphasis");
    sendProperty(cntrl_data, AM_DEEMPHASIS, parameter);
}

/**
 * @ingroup group17
 * @brief Sets the Am Soft Mute Max Attenuation
 *
 * @details This function can be useful to disable Soft Mute. The value 0 disable soft mute.
 * @details Specified in units of dB. Default maximum attenuation is 8 dB. It works for AM and SSB.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 158.
 *
 * @param smattn Maximum attenuation to apply when in soft mute
 */
void setAmSoftMuteMaxAttenuation(SI4735_t *cntrl_data, uint8_t smattn )
{
    sendProperty(cntrl_data, AM_SOFT_MUTE_MAX_ATTENUATION, smattn);
}

/**
 * @ingroup group08
 * @brief Sets the SNR threshold to engage soft mute
 * @details Whenever the SNR for a tuned frequency drops below this threshold the AM reception will go in soft mute,
 * @details provided soft mute max attenuation property is non-zero. The default value is 8dB
 * @see setAmSoftMuteMxAttenuation
 * @see Si47XX PROAMMING GUIDE; AN332 (REV 1.0);
 * @param parameter  0-63 (default is 8)
 */
void setAMSoftMuteSnrThreshold(SI4735_t *cntrl_data, uint8_t parameter)
{
    ESP_LOGD(TAG, " setAMSoftMuteSnrThreshold");
    sendProperty(cntrl_data, AM_SOFT_MUTE_SNR_THRESHOLD, parameter);
}

/*
 * @ingroup group08 Seek
 *
 * @brief Sets the bottom frequency and top frequency of the AM band for seek. Default is 520 to 1710.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 127, 161, and 162
 *
 * @param uint16_t bottom - the bottom of the AM (MW/SW) mode for seek
 * @param uint16_t    top - the top of the AM (MW/SW) mode for seek
 */
void setSeekAmLimits(SI4735_t *cntrl_data, uint16_t bottom, uint16_t top)
{
    ESP_LOGD(TAG, " setSeekAmLimits - bottom AM_SEEK_BAND_BOTTOM");
    sendProperty(cntrl_data, AM_SEEK_BAND_BOTTOM, bottom);
    ESP_LOGD(TAG, " setSeekAmLimits - top AM_SEEK_BAND_TOP");
    sendProperty(cntrl_data, AM_SEEK_BAND_TOP, top);
}

/**
 * @ingroup group08 Seek
 * @brief Set the Seek Am Srn Threshold object
 * @deprecated Use setSeekAmSNRThreshold instead.
 * @see setSeekAmSNRThreshold
 * @param value
 */
void  setSeekAmSrnThreshold(SI4735_t *cntrl_data, uint16_t value) 
{ 
    ESP_LOGD(TAG, " setSeekAmSrnThreshold AM_SEEK_SNR_THRESHOLD");
    sendProperty(cntrl_data, AM_SEEK_SNR_THRESHOLD, value); 
} // Wrong name! Will be removed later

/**
 * @ingroup group08 Seek
 *
 * @brief Sets the RSSI threshold for a valid AM Seek/Tune.
 *
 * @details If the value is zero then RSSI threshold is not considered when doing a seek. Default value is 25 dBμV.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 127
 */
void setSeekAmRssiThreshold(SI4735_t *cntrl_data, uint16_t value)
{
    ESP_LOGD(TAG, " setSeekAmRssiThreshold AM_SEEK_RSSI_THRESHOLD");
    sendProperty(cntrl_data, AM_SEEK_RSSI_THRESHOLD, value);
}

/**
 * @ingroup group08 Set mode and Band
 *
 * @brief Sets the radio to FM function.
 *
 * @details Defines the band range you want to use for the FM mode.
 *
 * @details The example below sets the band from 64MHz to 108MHzkHz on FM mode. The band will start on 103.9MHz and step is 100kHz.
 * On FM mode, the step 10 means 100kHz. If you want a 1MHz step, use 100.
 *
 * @code
 * si4735.setFM(6400, 10800, 10390, 10);
 * @endcode
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 70
 * @see setFM()
 * @see setFrequencyStep()
 *
 * @param fromFreq minimum frequency for the band
 * @param toFreq maximum frequency for the band
 * @param initialFreq initial frequency (default frequency)
 * @param step step used to go to the next channel
 */
void setFM(SI4735_t *cntrl_data, uint16_t fromFreq, uint16_t toFreq, uint16_t initialFreq, uint16_t step)
{
    ESP_LOGD(TAG, "*** setFM ***");
    if (initialFreq < fromFreq || initialFreq > toFreq)
        initialFreq = fromFreq;
    // From function SetupFM()
    powerDown();
    setPowerUp(cntrl_data, ctsIntEnable, gpo2Enable, 0, currentClockType, FM_CURRENT_MODE, currentAudioMode);
    radioPowerUp(cntrl_data);
    currentSsbStatus = 0;
    disableFmDebug(cntrl_data);
    lastMode = FM_CURRENT_MODE;
    //**************
    setFrequency(cntrl_data, initialFreq);
}

/**
 * @ingroup group13 Digital Audio setup
 *
 * @brief Configures the digital audio output format.
 *
 * @details Options: DCLK edge, data format, force mono, and sample precision.
 *
 * ATTENTION: The document AN383; "Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES"; rev 0.8; page 6; there is the following note:
 *            Crystal and digital audio mode cannot be used at the same time. Populate R1 and remove C10, C11, and X1 when using digital audio.
 *
 * @see Si47XX PROGRAMINGGUIDE; AN332 (REV 1.0); page 195.
 * @see Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES"; AN383; rev 0.8; page 6;

 * @param uint8_t OSIZE Dgital Output Audio Sample Precision (0=16 bits, 1=20 bits, 2=24 bits, 3=8bits).
 * @param uint8_t OMONO Digital Output Mono Mode (0=Use mono/stereo blend ).
 * @param uint8_t OMODE Digital Output Mode (0=I2S, 6 = Left-justified, 8 = MSB at second DCLK after DFS pulse, 12 = MSB at first DCLK after DFS pulse).
 * @param uint8_t OFALL Digital Output DCLK Edge (0 = use DCLK rising edge, 1 = use DCLK falling edge)
 */
void digitalOutputFormat(SI4735_t *cntrl_data, uint8_t OSIZE, uint8_t OMONO, uint8_t OMODE, uint8_t OFALL)
{
    cntrl_data->digital_output_format.refined.OSIZE = OSIZE;
    cntrl_data->digital_output_format.refined.OMONO = OMONO;
    cntrl_data->digital_output_format.refined.OMODE = OMODE;
    cntrl_data->digital_output_format.refined.OFALL = OFALL;
    cntrl_data->digital_output_format.refined.dummy = 0;

    ESP_LOGD(TAG, " digitalOutputFormat DIGITAL_OUTPUT_FORMAT");
    sendProperty(cntrl_data, DIGITAL_OUTPUT_FORMAT, cntrl_data->digital_output_format.raw);
}

/**
 * @ingroup group13 Digital Audio setup
 *
 * @brief Enables digital audio output and configures digital audio output sample rate in samples per second (sps).
 * @details When DOSR[15:0] is 0, digital audio output is disabled. The over-sampling rate must be set in order to
 * @details satisfy a minimum DCLK of 1 MHz. To enable digital audio output, program DOSR[15:0] with the sample rate
 * @details in samples per second. The system controller must establish DCLK and DFS prior to enabling the digital
 * @details audio output else the device will not respond and will require reset. The sample rate must be set to 0
 * @details before the DCLK/DFS is removed. FM_TUNE_FREQ command must be sent after the POWER_UP command to start
 * @details the internal clocking before setting this property.
 *
 * ATTENTION: The document AN383; "Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES"; rev 0.8; page 6; there is the following note:
 *            Crystal and digital audio mode cannot be used at the same time. Populate R1 and remove C10, C11, and X1 when using digital audio.
 *
 * @see Si47XX PROGRAMINGGUIDE; AN332 (REV 1.0); page 196.
 * @see Si47XX ANTENNA, SCHEMATIC, LAYOUT, AND DESIGN GUIDELINES; AN383; rev 0.8; page 6
 *
 * @param uint16_t DOSR Digital Output Sample Rate(32–48 ksps .0 to disable digital audio output).
 */
void digitalOutputSampleRate(SI4735_t *cntrl_data, uint16_t DOSR)
{
    ESP_LOGD(TAG, " digitalOutputSampleRate DIGITAL_OUTPUT_SAMPLE_RATE");
    sendProperty(cntrl_data, DIGITAL_OUTPUT_SAMPLE_RATE, DOSR);
}

/**
 * @ingroup group07 Device Power Down
 *
 * @brief Moves the device from powerup to powerdown mode.
 *
 * @details After Power Down command, only the Power Up command is accepted.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 67, 132
 * @see radioPowerUp()
 */
void powerDown(void)
{
    // Turns the external mute circuit on
    if (audioMuteMcuPin >= 0)
        setHardwareAudioMute(true);

    ESP_LOGD(TAG, " powerDown POWER_DOWN");
    ESP_ERROR_CHECK(register_write_byte(POWER_DOWN, 0));
    delay_ms(3);
}

/**
 * @ingroup group08 AGC
 *
 * @brief Automatic Gain Control setup
 *
 * @details If FM, overrides AGC setting by disabling the AGC and forcing the LNA to have a certain gain that ranges between 0
 * (minimum attenuation) and 26 (maximum attenuation).
 * @details If AM/SSB, Overrides the AGC setting by disabling the AGC and forcing the gain index that ranges between 0
 * (minimum attenuation) and 37+ATTN_BACKUP (maximum attenuation).
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); For FM page 81; for AM page 143
 *
 * @param uint8_t AGCDIS This param selects whether the AGC is enabled or disabled (0 = AGC enabled; 1 = AGC disabled);
 * @param uint8_t AGCIDX AGC Index (0 = Minimum attenuation (max gain); 1 – 36 = Intermediate attenuation);
 *                if >greater than 36 - Maximum attenuation (min gain) ).
 */
void setAutomaticGainControl(SI4735_t *cntrl_data, uint8_t AGCDIS, uint8_t AGCIDX)
{
    uint8_t cmd;
    uint8_t write_buf[3];

    // cmd = (currentTune == FM_TUNE_FREQ) ? FM_AGC_OVERRIDE : AM_AGC_OVERRIDE; // AM_AGC_OVERRIDE = SSB_AGC_OVERRIDE = 0x48
    if (cntrl_data->currentTune == FM_TUNE_FREQ)
        write_buf[0] = FM_AGC_OVERRIDE;
    else if (cntrl_data->currentTune == NBFM_TUNE_FREQ)
        write_buf[0] = NBFM_AGC_OVERRIDE;
    else
        write_buf[0] = AM_AGC_OVERRIDE;

    cntrl_data->agc_overrride.arg.DUMMY = 0; // ARG1: bits 7:1 Always write to 0;
    cntrl_data->agc_overrride.arg.AGCDIS = AGCDIS;
    cntrl_data->agc_overrride.arg.AGCIDX = AGCIDX;

    write_buf[1] = cntrl_data->agc_overrride.raw[0];
    write_buf[2] = cntrl_data->agc_overrride.raw[1];
    
    if (cntrl_data->currentTune == FM_TUNE_FREQ)
    ESP_LOGD(TAG, " setAutomaticGainControl FM_AGC_OVERRIDE AM_AGC_OVERRIDE");
    ESP_ERROR_CHECK( register_write_block(write_buf, sizeof(write_buf)));
}

/**
 * @ingroup group08 Frequency
 *
 * @brief Gets the current status  of the Si4735 (AM or FM)
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 73 (FM) and 139 (AM)
 *
 * @param uint8_t INTACK Seek/Tune Interrupt Clear. If set, clears the seek/tune complete interrupt status indicator;
 * @param uint8_t CANCEL Cancel seek. If set, aborts a seek currently in progress;
 */
void getStatus(SI4735_t *cntrl_data, uint8_t INTACK, uint8_t CANCEL)
{
    uint8_t cmd = FM_TUNE_STATUS;
    int limitResp = 8;
    uint8_t write_buf[2];

    if (cntrl_data->currentTune == FM_TUNE_FREQ)
        cmd = FM_TUNE_STATUS;
    else if (cntrl_data->currentTune == AM_TUNE_FREQ)
        cmd = AM_TUNE_STATUS;
    else if (cntrl_data->currentTune == NBFM_TUNE_FREQ)
    {
        cmd = NBFM_TUNE_STATUS;
        limitResp = 6;
    }

    cntrl_data->tune_status.arg.INTACK = INTACK;
    cntrl_data->tune_status.arg.CANCEL = CANCEL;
    cntrl_data->tune_status.arg.RESERVED2 = 0;

    write_buf[0] = cmd;
    write_buf[1] = cntrl_data->tune_status.raw;

    ESP_LOGD(TAG, " *****************  getStatus *************************");
    ESP_LOGD(TAG, " cmd = 0x%x", cmd);
    ESP_LOGD(TAG, " status.raw = 0x%x", cntrl_data->tune_status.raw);

    register_read_block(write_buf, 2, cntrl_data->response_status.raw, 8);

    for(uint8_t i=0; i<limitResp; ++i){
        ESP_LOGD(TAG, " currentStatus.raw[%d] = 0x%x", i, cntrl_data->response_status.raw[i]);
    }
    
}

 /**
 * @ingroup group08 Frequency
 *
 * @brief Gets the current frequency of the Si4735 (AM or FM)
 *
 * @details The method status do it an more. See getStatus below.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 73 (FM) and 139 (AM)
 */
uint16_t getFrequency(SI4735_t *cntrl_data)
{
    getStatus(cntrl_data, 0, 1);

    cntrl_data->frequency.raw.FREQL = cntrl_data->response_status.resp.READFREQL;
    cntrl_data->frequency.raw.FREQH = cntrl_data->response_status.resp.READFREQH;

    return cntrl_data->frequency.value;
}

/**
 * @ingroup group13 Aud volume
 *
 * @brief Gets the current volume level.
 *
 * @see setVolume()
 *
 * @return volume (domain: 0 - 63)
 */
uint8_t getVolume(SI4735_t *cntrl_data)
{
    return cntrl_data->volume;
}

/**
 * @ingroup group08
 * @brief Gets the SNR metric when tune is complete (dB)
 *
 * @details Returns the value  of the SNR metric when tune is complete (dB).
 *
 * @return uint8_t
 */
uint8_t getStatusSNR(SI4735_t *cntrl_data)
{
    return cntrl_data->response_status.resp.SNR; 
}

/**
 * @ingroup group08
 * @brief Gets the SNR metric when tune is complete (dB)
 *
 * @details Returns the value  of the SNR metric when tune is complete (dB).
 *
 * @return uint8_t
 */
uint8_t getStatusRSSI(SI4735_t *cntrl_data)
{
    return cntrl_data->response_status.resp.RSSI; 
}

/**
 * @ingroup group08
 * @brief Gets the current SNR metric (0–127 dB).
 *
 * @return uint8_t SNR value in dB (0-127)
 */
 uint8_t getCurrentSNR(SI4735_t *cntrl_data)
{
    return cntrl_data->rqs_status.resp.SNR;
}

/**
 * @ingroup group08
 * @brief Get the current receive signal strength (0–127 dBμV)
 *
 * @return uint8_t a value between 0 to 127
 */
 uint8_t getCurrentRSSI(SI4735_t *cntrl_data)
{
    return cntrl_data->rqs_status.resp.RSSI;
}

/**
 * @ingroup group08 AGC
 *
 * @brief Queries Automatic Gain Control STATUS
 *
 * @details After call this method, you can call isAgcEnabled to know the AGC status and getAgcGainIndex to know the gain index value.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); For FM page 80; for AM page 142.
 * @see AN332 REV 0.8 Universal Programming Guide Amendment for SI4735-D60 SSB and NBFM patches; page 18.
 *
 */
void getAutomaticGainControl(SI4735_t *cntrl_data)
{
    uint8_t cmd;
    uint8_t read_buf[3];

    if (cntrl_data->currentTune == FM_TUNE_FREQ)
    { // FM TUNE
        cmd = FM_AGC_STATUS;
    }
    else if (cntrl_data->currentTune == NBFM_TUNE_FREQ)
    {
        cmd = NBFM_AGC_STATUS;
    }
    else
    { // AM TUNE - SAME COMMAND used on SSB mode
        cmd = AM_AGC_STATUS;
    }

    ESP_LOGD(TAG, " getAutomaticGainControl");
    ESP_ERROR_CHECK(register_read(cmd, cntrl_data->agc_status.raw, 3));    
}

/**
 * @ingroup group08 Received Signal Quality
 *
 * @brief Queries the status of the Received Signal Quality (RSQ) of the current channel.
 *
 * @details This method sould be called berore call getCurrentRSSI(), getCurrentSNR() etc.
 * Command FM_RSQ_STATUS
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 75 and 141
 *
 * @param INTACK Interrupt Acknowledge.
 *        0 = Interrupt status preserved;
 *        1 = Clears RSQINT, BLENDINT, SNRHINT, SNRLINT, RSSIHINT, RSSILINT, MULTHINT, MULTLINT.
 */
void getCurrentReceivedSignalQuality(SI4735_t *cntrl_data, uint8_t INTACK)
{
    uint8_t arg;
    uint8_t cmd;
    int sizeResponse = 6;
    uint8_t write_buf[2];
    //uint8_t read_buf[sizeResponse];

    if (cntrl_data->currentTune == FM_TUNE_FREQ)
    { // FM TUNE
        cmd = FM_RSQ_STATUS;
        sizeResponse = 8;
    }
    else if (cntrl_data->currentTune == NBFM_TUNE_FREQ)
    {
        cmd = NBFM_RSQ_STATUS;
        sizeResponse = 8; // Check it
    }
    else
    { // AM TUNE
        cmd = AM_RSQ_STATUS;
        sizeResponse = 6; // Check it
    }
    arg = INTACK;
 
    write_buf[0] = cmd;
    write_buf[1] = arg;

    //i2c_master_write_read_device(I2C_MASTER_NUM, deviceAddress, write_buf, sizeof(write_buf), read_buf, sizeof(read_buf), I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    register_read_block(write_buf, sizeof(write_buf), cntrl_data->rqs_status.raw, sizeof(cntrl_data->rqs_status.raw));

}

/**
 * @ingroup group12 FM Mono Stereo audio setup
 *
 * @brief There is a debug feature that remains active in Si4704/05/3x-D60 firmware which can create periodic noise in audio.
 *
 * @details Silicon Labs recommends you disable this feature by sending the following bytes (shown here in hexadecimal form):
 * 0x12 0x00 0xFF 0x00 0x00 0x00.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 299.
 */
void disableFmDebug(SI4735_t *cntrl_data)
{

    ESP_LOGD(TAG, " disableFmDebug");
    sendProperty(cntrl_data, FM_DISABLE_DEBUG, 0x0000);

    delay_ms(3);
}

/**
* @ingroup group08
* @brief Checks the current pilot
*
* @details Indicates stereo pilot presence.
*
* @return true if stereo pilot presence has detected
*/
bool getCurrentPilot(SI4735_t *cntrl_data)
{
    return cntrl_data->rqs_status.resp.PILOT;
}

/* @ingroup group08 Check FM mode status
* @brief Returns true if the current function is FM (FM_TUNE_FREQ).
*
* @return true if the current function is FM (FM_TUNE_FREQ).
*/
bool isCurrentTuneFM(SI4735_t *cntrl_data)
{
    return (cntrl_data->currentTune == FM_TUNE_FREQ);
}

/*******************************************************************************
 * SSB implementation 
 ******************************************************************************/

/**
 * @defgroup group17 Si4735-D60 Single Side Band (SSB) support
 *
 * @brief Single Side Band (SSB) implementation.<br>
 * First of all, the SSB patch content **is not part of this library**.
 * The patches used here were made available by Mr. Vadim Afonkin on his [Dropbox repository](https://www.dropbox.com/sh/xzofrl8rfaaqh59/AAA5au2_CVdi50NBtt0IivyIa?dl=0).
 * Please note that the author of this library does not encourage anyone to use the SSB patches content for commercial purposes.
 * In other words, while this library supports SSB patches, the patches themselves are not a part of this library.
 *
 * @details This implementation was tested only on Si4735-D60  and SI4732-A10 devices.
 * @details SSB modulation is a refinement of amplitude modulation that one of the side band and the carrier are suppressed.
 *
 * @details What does SSB patch means?
 * In this context, a patch is a piece of software used to change the behavior of the SI4735-D60/SI4732-A10 device.
 * There is little information available about patching the SI4735-D60/SI4732-A10.
 *
 * The following information is the understanding of the author of this project and
 * is not necessarily correct.
 *
 * A patch is executed internally (run by internal MCU) of the device. Usually,
 * patches are used to fix bugs or add improvements and new features over what the firmware
 * installed in the internal ROM of the device offers. Patches for the SI4735 are distributed
 * in binary form and are transferred to the internal RAM of the device by the host MCU
 * (in this case, Arduino boards).
 *
 * Since the RAM is volatile memory, the patch stored into the device gets lost when
 * you turn off the system. Consequently, the content of the patch has to be transferred
 * to the device every time the device is powered up.
 *
 * I would like to thank Mr Vadim Afonkin for making the SSBRX patches available for
 * SI4735-D60/SI4732-A10 on his Dropbox repository. On this repository you have two files,
 * amrx_6_0_1_ssbrx_patch_full_0x9D29.csg and amrx_6_0_1_ssbrx_patch_init_0xA902.csg.
 * The patch content of the original files is in hexadecimal format, stored in an
 * ASCII text file.
 * If you are not using C/C++ or if you want to load the files directly to the SI4735,
 * you must convert the hexadecimal values to numeric values from 0 to 255.
 * For example: 0x15 = 21 (00010101); 0x16 = 22 (00010110); 0x01 = 1 (00000001);
 * 0xFF = 255 (11111111);
 *
 * @details ATTENTION: The author of this project cannot guarantee that procedures shown
 * here will work in your development environment. Proceed at your own risk.
 * This library works with the I²C communication protocol to send an SSB extension PATCH to
 * SI4735-D60 and SI4732-A10 devices. Once again, the author disclaims any and all liability for any
 * damage or effects this procedure may have on your devices. Procced at your own risk.
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 3 and 5
 */

/**
 * @ingroup group17 Patch and SSB support
 *
 * @details Tunes the SSB (LSB or USB) receiver to a frequency between 150 and 30 MHz.
 * @details Via VFO you have 1kHz steps.
 * @details Via BFO you have 8Hz steps.
 *
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 13 and 14
 * @see setAM()
 * @see setFrequencyStep()
 * @see void setFrequency(uint16_t freq)
 *
 * @param fromFreq minimum frequency for the band
 * @param toFreq maximum frequency for the band
 * @param initialFreq initial frequency
 * @param step step used to go to the next channel
 * @param usblsb SSB Upper Side Band (USB) and Lower Side Band (LSB) Selection;
 *               value 2 (banary 10) = USB;
 *               value 1 (banary 01) = LSB.
 */
void setSSB(SI4735_t *cntrl_data, uint16_t fromFreq, uint16_t toFreq, uint16_t initialFreq, uint16_t step, uint8_t usblsb)
{
    ESP_LOGD(TAG, " setSSB");

    if (initialFreq < fromFreq || initialFreq > toFreq)
        initialFreq = fromFreq;

    //setSSB(usblsb);
    // Is it needed to load patch when switch to SSB?
    powerDown();
    // It starts with the same AM parameters.
    // setPowerUp(1, 1, 0, 1, 1, currentAudioMode);
    setPowerUp(cntrl_data, ctsIntEnable, 0, 0, currentClockType, 1, currentAudioMode);
    radioPowerUp(cntrl_data);
    // ssbPowerUp(); // Not used for regular operation
    //setVolume(cntrl_data, volume); // Set to previus configured volume
    currentSsbStatus = usblsb;
    lastMode = SSB_CURRENT_MODE;

    setFrequency(cntrl_data, initialFreq);
    delay_ms(1);//// delayMicroseconds(550);
    
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Sets the SSB Beat Frequency Offset (BFO).
 *
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 5 and 23
 *
 * @param offset 16-bit signed value (unit in Hz). The valid range is -16383 to +16383 Hz.
 */
void setSSBBfo(SI4735_t *cntrl_data, int offset)
{
    uint8_t write_buf[5] = {0};

    if (cntrl_data->currentTune == FM_TUNE_FREQ) // Only for AM/SSB mode
        return;

    cntrl_data->property.value = SSB_BFO;
    cntrl_data->bfo_offset.value = offset;

/*  waitToSend();
    Wire.beginTransmission(deviceAddress);
    Wire.write(SET_PROPERTY);
    Wire.write(0x00);                  // Always 0x00
    Wire.write(property.raw.byteHigh); // High byte first
    Wire.write(property.raw.byteLow);  // Low byte after
    Wire.write(bfo_offset.raw.FREQH);  // Offset freq. high byte first
    Wire.write(bfo_offset.raw.FREQL);  // Offset freq. low byte first
    Wire.endTransmission();
*/
    write_buf[0] = 0x00;
    write_buf[1] = cntrl_data->property.raw.byteHigh;
    write_buf[2] = cntrl_data->property.raw.byteLow;
    write_buf[3] = cntrl_data->bfo_offset.raw.FREQH;
    write_buf[4] = cntrl_data->bfo_offset.raw.FREQL;

    ESP_LOGD(TAG, " setSSBBfo");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));

    delay_ms(1);//delayMicroseconds(550);
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Sets the SSB receiver mode.
 *
 * @details You can use this method for:
 * @details 1) Enable or disable AFC track to carrier function for receiving normal AM signals;
 * @details 2) Set the audio bandwidth;
 * @details 3) Set the side band cutoff filter;
 * @details 4) Set soft-mute based on RSSI or SNR;
 * @details 5) Enable or disbable automatic volume control (AVC) function.
 *
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24
 *
 * @param AUDIOBW SSB Audio bandwidth; 0 = 1.2kHz (default); 1=2.2kHz; 2=3kHz; 3=4kHz; 4=500Hz; 5=1kHz.
 * @param SBCUTFLT SSB side band cutoff filter for band passand low pass filter
 *                 if 0, the band pass filter to cutoff both the unwanted side band and high frequency
 *                  component > 2kHz of the wanted side band (default).
 * @param AVC_DIVIDER set 0 for SSB mode; set 3 for SYNC mode.
 * @param AVCEN SSB Automatic Volume Control (AVC) enable; 0=disable; 1=enable (default).
 * @param SMUTESEL SSB Soft-mute Based on RSSI or SNR.
 * @param DSP_AFCDIS DSP AFC Disable or enable; 0=SYNC MODE, AFC enable; 1=SSB MODE, AFC disable.
 */
void setSSBConfig(SI4735_t *cntrl_data,uint8_t AUDIOBW, uint8_t SBCUTFLT, uint8_t AVC_DIVIDER, uint8_t AVCEN, uint8_t SMUTESEL, uint8_t DSP_AFCDIS)
{
    if (cntrl_data->currentTune == FM_TUNE_FREQ) // Only AM/SSB mode
        return;

    cntrl_data->ssb_mode.param.AUDIOBW = AUDIOBW;
    cntrl_data->ssb_mode.param.SBCUTFLT = SBCUTFLT;
    cntrl_data->ssb_mode.param.AVC_DIVIDER = AVC_DIVIDER;
    cntrl_data->ssb_mode.param.AVCEN = AVCEN;
    cntrl_data->ssb_mode.param.SMUTESEL = SMUTESEL;
    cntrl_data->ssb_mode.param.DUMMY1 = 0;
    cntrl_data->ssb_mode.param.DSP_AFCDIS = DSP_AFCDIS;

    sendSSBModeProperty(cntrl_data);
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief SSB Audio Bandwidth for SSB mode
 *
 * @details 0 = 1.2 kHz low-pass filter  (default).
 * @details 1 = 2.2 kHz low-pass filter.
 * @details 2 = 3.0 kHz low-pass filter.
 * @details 3 = 4.0 kHz low-pass filter.
 * @details 4 = 500 Hz band-pass filter for receiving CW signal, i.e. [250 Hz, 750 Hz] with center
 * frequency at 500 Hz when USB is selected or [-250 Hz, -750 1Hz] with center frequency at -500Hz
 * when LSB is selected* .
 * @details 5 = 1 kHz band-pass filter for receiving CW signal, i.e. [500 Hz, 1500 Hz] with center
 * frequency at 1 kHz when USB is selected or [-500 Hz, -1500 1 Hz] with center frequency
 *     at -1kHz when LSB is selected.
 * @details Other values = reserved.
 *
 * @details If audio bandwidth selected is about 2 kHz or below, it is recommended to set SBCUTFLT[3:0] to 0
 * to enable the band pass filter for better high- cut performance on the wanted side band. Otherwise, set it to 1.
 *
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24
 *
 * @param AUDIOBW the valid values are 0, 1, 2, 3, 4 or 5; see description above
 */
void setSSBAudioBandwidth(SI4735_t *cntrl_data, uint8_t AUDIOBW)
{
    // Sets the audio filter property parameter
    cntrl_data->ssb_mode.param.AUDIOBW = AUDIOBW;
    sendSSBModeProperty(cntrl_data);
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Sets SSB Automatic Volume Control (AVC) for SSB mode
 *
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24
 *
 * @param AVCEN 0 = Disable AVC; 1 = Enable AVC (default).
 */
void setSSBAutomaticVolumeControl(SI4735_t *cntrl_data, uint8_t AVCEN)
{
    cntrl_data->ssb_mode.param.AVCEN = AVCEN;
    sendSSBModeProperty(cntrl_data);
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Sets SBB Sideband Cutoff Filter for band pass and low pass filters.
 *
 * @details 0 = Band pass filter to cutoff both the unwanted side band and high frequency components > 2.0 kHz of the wanted side band. (default)
 * @details 1 = Low pass filter to cutoff the unwanted side band.
 * Other values = not allowed.
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24
 *
 * @param SBCUTFLT 0 or 1; see above
 */
void setSSBSidebandCutoffFilter(SI4735_t *cntrl_data, uint8_t SBCUTFLT)
{
    cntrl_data->ssb_mode.param.SBCUTFLT = SBCUTFLT;
    sendSSBModeProperty(cntrl_data);
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Sets AVC Divider
 *
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24
 *
 * @param AVC_DIVIDER  SSB mode, set divider = 0; SYNC mode, set divider = 3; Other values = not allowed.
 */
void setSSBAvcDivider(SI4735_t *cntrl_data, uint8_t AVC_DIVIDER)
{
    cntrl_data->ssb_mode.param.AVC_DIVIDER = AVC_DIVIDER;
    sendSSBModeProperty(cntrl_data);
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Sets DSP AFC disable or enable
 *
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24
 *
 * @param DSP_AFCDIS 0 = SYNC mode, AFC enable; 1 = SSB mode, AFC disable
 */
void setSSBDspAfc(SI4735_t *cntrl_data, uint8_t DSP_AFCDIS)
{
    cntrl_data->ssb_mode.param.DSP_AFCDIS = DSP_AFCDIS;
    sendSSBModeProperty(cntrl_data);
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Sets SSB Soft-mute Based on RSSI or SNR Selection:
 *
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 24
 *
 * @param SMUTESEL  0 = Soft-mute based on RSSI (default); 1 = Soft-mute based on SNR.
 */
void setSSBSoftMute(SI4735_t *cntrl_data, uint8_t SMUTESEL)
{
    cntrl_data->ssb_mode.param.SMUTESEL = SMUTESEL;
    sendSSBModeProperty(cntrl_data);
}

/**
 * @ingroup group17 ???????????????????????????
 *
 * @brief Automatic Gain Control setup
 * @details Overrides the SSB AGC setting by disabling the AGC and forcing the gain index that ranges between 0 (minimum attenuation) and 37+ATTN_BACKUP (maximum attenuation).
 *
 * @param uint8_t SSBAGCDIS This param selects whether the AGC is enabled or disabled (0 = AGC enabled; 1 = AGC disabled);
 * @param uint8_t SSBAGCNDX If 1, this byte forces the AGC gain index. if 0,  Minimum attenuation (max gain)
 *
 */
void setSsbAgcOverrite(SI4735_t *cntrl_data, uint8_t SSBAGCDIS, uint8_t SSBAGCNDX, uint8_t reserved)
{
    uint8_t write_buf[3];

    cntrl_data->agc_overrride.arg.DUMMY = reserved; // ARG1: bits 7:1 - The manual says: Always write to 0;
    cntrl_data->agc_overrride.arg.AGCDIS = SSBAGCDIS;
    cntrl_data->agc_overrride.arg.AGCIDX = SSBAGCNDX;
/*
    waitToSend();
    Wire.beginTransmission(deviceAddress);
    Wire.write(SSB_AGC_OVERRIDE);
    Wire.write(agc.raw[0]);
    Wire.write(agc.raw[1]);
    Wire.endTransmission();
    waitToSend();
*/
    write_buf[0] = SSB_AGC_OVERRIDE;
    write_buf[1] = cntrl_data->agc_overrride.raw[0];
    write_buf[2] = cntrl_data->agc_overrride.raw[1];

    ESP_LOGD(TAG, " setSsbAgcOverrite");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));
        
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Just send the property SSB_MOD to the device.  Internal use (privete method).
 */
void sendSSBModeProperty(SI4735_t *cntrl_data)
{
    uint8_t write_buf[6] = {0};

    cntrl_data->property.value = SSB_MODE;

    write_buf[0] = 0x00;
    write_buf[1] = SSB_MODE;
    write_buf[2] = cntrl_data->property.raw.byteHigh;
    write_buf[3] = cntrl_data->property.raw.byteLow;
    write_buf[4] = cntrl_data->ssb_mode.raw[1];
    write_buf[5] = cntrl_data->ssb_mode.raw[0];

    ESP_LOGD(TAG, " sendSSBModeProperty");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));
    
    delay_ms(1);//delayMicroseconds(550);
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief This function can be useful for debug and test.
 */
void ssbPowerUp(SI4735_t *cntrl_data)
{
    uint8_t write_buf[3];
 
    write_buf[0] = POWER_UP;
    write_buf[1] = 0b00010001;
    write_buf[2] = 0b00000101;
    
    ESP_LOGD(TAG, " ssbPowerUp");
    ESP_ERROR_CHECK(register_write_block( write_buf,sizeof(write_buf)));

    cntrl_data->powerUp.arg.CTSIEN = ctsIntEnable;     // 1 -> Interrupt anabled;
    cntrl_data->powerUp.arg.GPO2OEN = 0;                     // 1 -> GPO2 Output Enable;
    cntrl_data->powerUp.arg.PATCH = 0;                       // 0 -> Boot normally;
    cntrl_data->powerUp.arg.XOSCEN = currentClockType; // 1 -> Use external crystal oscillator;
    cntrl_data->powerUp.arg.FUNC = 1;                        // 0 = FM Receive; 1 = AM/SSB (LW/MW/SW) Receiver.
    cntrl_data->powerUp.arg.OPMODE = 0b00000101;             // 0x5 = 00000101 = Analog audio outputs (LOUT/ROUT).
    delay_ms(2);//
}

 /**
 * @ingroup group17
 * @brief Sets the SSB Soft Mute Max Attenuation object
 * 
 * @details Sets maximum attenuation during soft mute (dB). Set to 0 to disable soft mute. 
 * @details Specified in units of dB. Default maximum attenuation is 8 dB.
 * @details You can use setAmSoftMuteMaxAttenuation instead. Same AM property values.  
 * @param smattn Maximum attenuation to apply when in soft mute.
 */
void setSsbSoftMuteMaxAttenuation(SI4735_t *cntrl_data, uint8_t smattn )
{
    // smattn = 0; - default value
    sendProperty(cntrl_data, SSB_SOFT_MUTE_MAX_ATTENUATION, smattn);
}

/**
* @ingroup group17
* @brief Sets the number of milliseconds the low IF peak detector
* 
* @details Sets the number of milliseconds the low IF peak detector must not be exceeded before increasing the gain. Default value is 140 (approximately 40 dB / s).
* @param param number of milliseconds ( from 4 to 248; step 4); default value 0x008C (140).
*/
void setSsbIfAgcReleaseRate(SI4735_t *cntrl_data, uint8_t param)
{
    // param = 140;  - default value
    sendProperty(cntrl_data, SSB_IF_AGC_RELEASE_RATE, param);
}

/**
* @ingroup group17
* @brief Sets the IF AGC attack rate
* 
* @details Large values provide slower attack, and smaller values provide faster attack
* @param param number of milliseconds ( from 4 to 248; step 4); default value 4.
*/
void setSsbIfAgcAttackRate(SI4735_t *cntrl_data, uint8_t param)
{
    // param = 4;  - default value
    sendProperty(cntrl_data, SSB_IF_AGC_ATTACK_RATE, param);
}

/**
* @ingroup group17
* @brief Sets the AGC attack rate on SSB mode.
* @details Large values provide slower attack, and smaller values provide faster attack.. 
* @see setSsbAgcAttackRate
* @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 29 
* @param parameter Range: 4–248 (The default is 0x04) 
*/
void setSsbAgcAttackRate(SI4735_t *cntrl_data, uint16_t parameter)
{
    sendProperty(cntrl_data, SSB_RF_AGC_ATTACK_RATE, parameter);
}
/**
* @ingroup group17
* @brief Sets the AGC Release rate on SSB mode.
* @details Larger values provide slower release, and smaller values provide faster release. 
* @see setSsbAgcAttackRate
* @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 29 
* @param parameter Range: 4–248 (The default is 0x18)
*/
void setSsbAgcReleaseRate(SI4735_t *cntrl_data, uint16_t parameter)
{
    sendProperty(cntrl_data ,SSB_RF_AGC_RELEASE_RATE, parameter);
}
/**
* @ingroup group17 Patch and SSB support
* @deprecated Use setSSBSidebandCutoffFilter instead.
* @see setSSBSidebandCutoffFilter
* @param SBCUTFLT 
*/
void setSBBSidebandCutoffFilter(SI4735_t *cntrl_data, uint8_t SBCUTFLT) 
{
     setSSBSidebandCutoffFilter(cntrl_data, SBCUTFLT); 
} // Wrong name! will be removed later.

/**
 * @ingroup group17 Patch and SSB support
 * 
 * @brief Set the radio to AM function. 
 * @todo Adjust the power up parameters
 * @details Set the radio to SSB (LW/MW/SW) function. 
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; pages 13 and 14 
 * @see setAM()
 * @see setFrequencyStep()
 * @see void SI4735::setFrequency(uint16_t freq)
 * 
 * @param usblsb upper or lower side band;  1 = LSB; 2 = USB
 */
void setSSBband(SI4735_t *cntrl_data, uint8_t usblsb)
{
    // Is it needed to load patch when switch to SSB?
    // powerDown(cntrl_data);
    // It starts with the same AM parameters.
    // setPowerUp(cntrl_data, 1, 1, 0, 1, 1, currentAudioMode);
    setPowerUp(cntrl_data, 1, 1, 0, 1, 1, currentAudioMode);
    radioPowerUp(cntrl_data);
    // ssbPowerUp(cntrl_data); // Not used for regular operation
    // setVolume(volume); // Set to previus configured volume
    cntrl_data->currentSsbStatus = usblsb;
    lastMode = SSB_CURRENT_MODE;
}


/*******************************************************************************
 * NBFM implementation 
 ******************************************************************************/
/**
 * @ingroup group20 Patch and NBFM support
 *
 * @details Tunes the SSB (LSB or USB) receiver to a frequency between 64 and 108 MHz.
 *
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE;
 * @see setAM(), setFM(), setSSB()
 * @see setFrequencyStep()
 * @see void setFrequency(uint16_t freq)
 *
 * @param fromFreq minimum frequency for the band
 * @param toFreq maximum frequency for the band
 * @param initialFreq initial frequency
 * @param step step used to go to the next channel

 */
void setNBFM(SI4735_t *cntrl_data, uint16_t fromFreq, uint16_t toFreq, uint16_t initialFreq, uint16_t step)
{
    if (initialFreq < fromFreq || initialFreq > toFreq)
        initialFreq = fromFreq;

    //setNBFM();
    // Is it needed to load patch when switch to SSB?
    // powerDown(cntrl_data);
    // It starts with the same AM parameters.
    // setPowerUp(cntrl_data, 1, 1, 0, 1, 1, currentAudioMode);
    setPowerUp(cntrl_data, ctsIntEnable, gpo2Enable, 0, currentClockType, 0, currentAudioMode);
    radioPowerUp(cntrl_data);
    cntrl_data->currentTune = NBFM_TUNE_FREQ; // Force current tune to NBFM commands
    // ssbPowerUp(cntrl_data); // Not used for regular operation
    //setVolume(cntrl_data, volume); // Set to previus configured volume
    currentSsbStatus = 0;
    lastMode = NBFM_CURRENT_MODE;

    setFrequency(cntrl_data, initialFreq);
}

/**
 * @ingroup   group20 Tune Frequency
 *
 * @brief Set the frequency to the corrent function of the Si4735 on NBFM mode
 * @details You have to call setup or setPowerUp before call setFrequency.
 *
 * @see maxDelaySetFrequency()
 * @see MAX_DELAY_AFTER_SET_FREQUENCY
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 70, 135
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE; page 39
 *
 * @param uint16_t  freq is the frequency to change. For example, FM => 10390 = 103.9 MHz; AM => 810 = 810 kHz.
 */
void setFrequencyNBFM(SI4735_t *cntrl_data, uint16_t freq)
{
    uint8_t write_buf[4];

    cntrl_data->frequency.value = freq;
    cntrl_data->set_frequency.arg.FREQH = cntrl_data->frequency.raw.FREQH;
    cntrl_data->set_frequency.arg.FREQL = cntrl_data->frequency.raw.FREQL;
   
    write_buf[0] = 0x00;
    write_buf[1] = NBFM_TUNE_FREQ;
    write_buf[2] = cntrl_data->frequency.raw.FREQH;
    write_buf[3] = cntrl_data->frequency.raw.FREQL;

    ESP_LOGD(TAG, " setFrequencyNBFM");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));

    delay_ms(250);                  // For some reason I need to delay here.
}

/***************************************************************************************
 * SI47XX PATCH RESOURCES
 **************************************************************************************/

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Query the library information of the Si47XX device
 *
 * @details Used to confirm if the patch is compatible with the internal device library revision.
 *
 * @details You have to call this function if you are applying a patch on SI47XX (SI4735-D60/SI4732-A10).
 * @details The first command that is sent to the device is the POWER_UP command to confirm
 * that the patch is compatible with the internal device library revision.
 * @details The device moves into the powerup mode, returns the reply, and moves into the
 * powerdown mode.
 * @details The POWER_UP command is sent to the device again to configure
 * the mode of the device and additionally is used to start the patching process.
 * @details When applying the patch, the PATCH bit in ARG1 of the POWER_UP command must be
 * set to 1 to begin the patching process. [AN332 (REV 1.0) page 219].
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 64 and 215-220.
 * @see struct si47x_firmware_query_library
 *
 * @return a struct si47x_firmware_query_library (see it in SI4735.h)
 */
si47x_firmware_query_library queryLibraryId(SI4735_t *cntrl_data)
{
    uint8_t write_buf[3];

    powerDown(); // Is it necessary

    write_buf[0] = POWER_UP;
    write_buf[1] = 0b00011111;
    write_buf[2] = SI473X_DIGITAL_AUDIO2; //SI473X_ANALOG_AUDIO

    ESP_LOGD(TAG, " queryLibraryId");
    ESP_ERROR_CHECK(register_read_block(write_buf, 3, cntrl_data->firmware_query_library.raw, 8));

    delay_ms(2);//delayMicroseconds(2500);

    return cntrl_data->firmware_query_library;
}

/**
 * @ingroup group20 Patch and NBFM support
 *
 * @brief This method can be used to prepare the device to apply NBFM patch
 *
 * @details Call queryLibraryId before call this method. Powerup the device by issuing the POWER_UP
 * command with FUNC = 0 (FM Receiver).
 *
 * @see setMaxDelaySetFrequency()
 * @see MAX_DELAY_AFTER_POWERUP
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 64 and 215-220 and
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE AMENDMENT FOR SI4735-D60 SSB AND NBFM PATCHES; page 32.
 */
void patchPowerUpNBFM(SI4735_t *cntrl_data)
{
    uint8_t write_buf[3];

    write_buf[0] = POWER_UP;
    write_buf[1] = 0b00110000;
    write_buf[2] = SI473X_DIGITAL_AUDIO2; //  SI473X_ANALOG_AUDIO

    ESP_LOGD(TAG, " patchPowerUpNBFM");
    ESP_ERROR_CHECK( register_write_block( write_buf, sizeof(write_buf)));
    
    delay_ms(maxDelayAfterPouwerUp);
}

/**
 * @ingroup group20 Patch and NBFM support
 * @brief Loads a given NBFM patch content
 * @details Configures the Si4735-D60/SI4732-A10 device to work with NBFM.
 *
 * @param patch_content        point to patch content array
 * @param patch_content_size   size of patch content
 */
void loadPatchNBFM(SI4735_t *cntrl_data, const uint8_t *patch_content, const uint16_t patch_content_size)
{
    queryLibraryId(cntrl_data);
    patchPowerUpNBFM(cntrl_data);
    delay_ms(50);
    downloadPatch(patch_content, patch_content_size);
    // TODO
    delay_ms(25);
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief Transfers the content of a patch stored in a array of bytes to the SI4735 device.
 *
 * @details You must mount an array as shown below and know the size of that array as well.
 *
 *  @details Patches for the SI4735 are distributed in binary
 *   form and are transferred to the internal RAM of the device by the host MCU (in this case, Arduino boards).
 *   Since the RAM is volatile memory, the patch stored on the device gets lost when you turn off
 *   the system. Consequently, the content of the patch has to be transferred to the device every
 *   time the device is powered up.
 *
 *  @details The disadvantage of this approach is the amount of memory used by the patch content.
 *  This may limit the use of other radio functions you want implemented in Arduino.
 *
 *  @details Example of content:
 *  @code
 *  const PROGMEM uint8_t ssb_patch_content_full[] =
 *   { // SSB patch for whole SSBRX full download
 *       0x15, 0x00, 0x0F, 0xE0, 0xF2, 0x73, 0x76, 0x2F,
 *       0x16, 0x6F, 0x26, 0x1E, 0x00, 0x4B, 0x2C, 0x58,
 *       0x16, 0xA3, 0x74, 0x0F, 0xE0, 0x4C, 0x36, 0xE4,
 *          .
 *          .
 *          .
 *       0x16, 0x3B, 0x1D, 0x4A, 0xEC, 0x36, 0x28, 0xB7,
 *       0x16, 0x00, 0x3A, 0x47, 0x37, 0x00, 0x00, 0x00,
 *       0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D, 0x29}
 *
 *  const int size_content_full = sizeof ssb_patch_content_full;
 *  @endcode
 *
 * @see Si47XX PROGRAMMING GUIDE; ;AN332 (REV 1.0) pages 64 and 215-220.
 *
 *  @param ssb_patch_content point to array of bytes content patch.
 *  @param ssb_patch_content_size array size (number of bytes). The maximum size allowed for a patch is 15856 bytes
 *
 *  @return false if an error is found.
 */
bool downloadPatch(const uint8_t *ssb_patch_content, const uint16_t ssb_patch_content_size)
{
    uint8_t content;

    // Send patch to the SI4735 device
    for (uint16_t offset = 0; offset < ssb_patch_content_size; offset += 8)
    {
        //Wire.beginTransmission(deviceAddress);
        for (uint16_t i = 0; i < 8; i++)
        {
            //content = pgm_read_byte_near(ssb_patch_content + (i + offset));
            content = ssb_patch_content[i + offset];//content = (const unsigned char *)(ssb_patch_content + (i + offset));
            ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &content, 1, I2C_MASTER_TIMEOUT_MS));
            esp_rom_delay_us(400);//ets_delay_us(400);// не удалять без этого не правильно грузится патч
            //Wire.write(content);
        }
        //Wire.endTransmission();

        // Testing download performance
        // approach 1 - Faster - less secure (it might crash in some architectures)
        //delay_ms(3);//delayMicroseconds(MIN_DELAY_WAIT_SEND_LOOP); // Need check the minimum value

        // approach 2 - More control. A little more secure than approach 1
        /*
        do
        {
            delayMicroseconds(150); // Minimum delay founded (Need check the minimum value)
            Wire.requestFrom(deviceAddress, 1);
        } while (!(Wire.read() & B10000000));
        */

        // approach 3 - same approach 2
        // waitToSend();

        // approach 4 - safer
        /*
        waitToSend();
        uint8_t cmd_status;
        // Uncomment the lines below if you want to check erro.
        Wire.requestFrom(deviceAddress, 1);
        cmd_status = Wire.read();
        // The SI4735 issues a status after each 8 byte transfered.
        // Just the bit 7 (CTS) should be seted. if bit 6 (ERR) is seted, the system halts.
        if (cmd_status != 0x80)
           return false;
        */
    }
    delay_ms(1);//delayMicroseconds(250);
    return true;
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief   Deal with compressed SSB patch
 * @details It works like the downloadPatch method but requires less memory to store the patch.
 * @details Transfers the content of a patch stored in a compressed array of bytes to the SI4735 device.
 * @details In the patch_init.h and patch_full.h files, the first byte of each line begins with either a 0x15 or 0x16 value
 * @details To shrink the original patch size stored on the master MCU (Arduino), the first byte
 * @details is omitted and a new array is added to indicate which lines begin with the value 0x15.
 * @details For the other lines, the downloadCompressedPatch method will insert the value 0x16.
 * @details The value 0x16 occurs on most lines in the patch. This approach will save about 1K of memory.
 * @details The example code below shows how to use compressed SSB patch.
 * @code
 *   #include <patch_ssb_compressed.h> // SSB patch for whole SSBRX initialization string
 *   const uint16_t size_content = sizeof ssb_patch_content; // See ssb_patch_content.h
 *   const uint16_t cmd_0x15_size = sizeof cmd_0x15;         // Array of lines where the 0x15 command occurs in the patch content.
 *
 *   void loadSSB()
 *   {
 *     .
 *     .
 *     rx.setI2CFastModeCustom(500000);
 *     rx.queryLibraryId();
 *     rx.patchPowerUp();
 *    delay_ms(50);
 *     rx.downloadCompressedPatch(ssb_patch_content, size_content, cmd_0x15, cmd_0x15_size);
 *     rx.setSSBConfig(bandwidthSSB[bwIdxSSB].idx, 1, 0, 1, 0, 1);
 *     rx.setI2CStandardMode();
 *     .
 *     .
 *   }
 * @endcode
 * @see  downloadPatch
 * @see  patch_ssb_compressed.h, patch_init.h, patch_full.h
 * @see  SI47XX_03_ALL_IN_ONE_NEW_INTERFACE_V15.ino
 * @see  SI47XX_09_NOKIA_5110/ALL_IN_ONE_7_BUTTONS/ALL_IN_ONE_7_BUTTONS.ino
 * @param ssb_patch_content         point to array of bytes content patch.
 * @param ssb_patch_content_size    array size (number of bytes). The maximum size allowed for a patch is 15856 bytes
 * @param cmd_0x15                  Array of lines where the first byte of each patch content line is 0x15
 * @param cmd_0x15_size             Array size
 */
bool downloadCompressedPatch(const uint8_t *ssb_patch_content, const uint16_t ssb_patch_content_size, const uint16_t *cmd_0x15, const int16_t cmd_0x15_size)
{
    uint8_t cmd, content;
    uint16_t command_line = 0;
    // Send patch to the SI4735 device
    for (uint16_t offset = 0; offset < ssb_patch_content_size; offset += 7)
    {
        // Checks if the current line starts with 0x15
        cmd = 0x16;
        for (uint16_t i = 0; i < cmd_0x15_size / sizeof(uint16_t); i++)
        {
            //if (pgm_read_word_near(cmd_0x15 + i) == command_line)
            if (cmd_0x15[i] == command_line)  // Исправлено
            { // it needs performance improvement: save the last "i" value to be used next time
                cmd = 0x15;
                break;
            }
        }
        //Wire.beginTransmission(deviceAddress);
        //Wire.write(cmd);
        ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &content, 1, I2C_MASTER_TIMEOUT_MS));
        for (uint16_t i = 0; i < 7; i++)
        {
            content = ssb_patch_content[i + offset];  // Исправлено
            //Wire.write(content);
            ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &cmd, 1, I2C_MASTER_TIMEOUT_MS));
        }
        //Wire.endTransmission();
       delay_ms(3);//delayMicroseconds(MIN_DELAY_WAIT_SEND_LOOP); // Need check the minimum value
        command_line++;
    }
    delay_ms(1);//delayMicroseconds(250);
    return true;
}

/**
 * @ingroup group17 Patch and SSB support
 *
 * @brief This method can be used to prepare the device to apply SSBRX patch
 *
 * @details Call queryLibraryId before call this method. Powerup the device by issuing the POWER_UP
 * command with FUNC = 1 (AM/SW/LW Receive).
 *
 * @see setMaxDelaySetFrequency()
 * @see MAX_DELAY_AFTER_POWERUP
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 64 and 215-220 and
 * @see AN332 REV 0.8 UNIVERSAL PROGRAMMING GUIDE AMENDMENT FOR SI4735-D60 SSB AND NBFM PATCHES; page 7.
 */
void patchPowerUp()
{
    uint8_t write_buf[3];

    write_buf[0] = POWER_UP;
    write_buf[1] = 0b00110001;
    write_buf[2] = SI473X_DIGITAL_AUDIO2; //SI473X_ANALOG_AUDIO

    ESP_LOGD(TAG, " patchPowerUp");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));

    delay_ms(maxDelayAfterPouwerUp);
}

/*******************************************************************************
 * RDS implementation 
 ******************************************************************************/

/**
 * @ingroup group18 Covert numbers to char array
 * @brief Converts a number to a char array
 * @details It is useful to mitigate memory space used by functions like sprintf or other generic similar functions
 * @details You can use it to format frequency using decimal or thousand separator and also to convert small numbers.
 *
 * @param value  value to be converted
 * @param strValue char array that will be receive the converted value
 * @param len final string size (in bytes)
 * @param dot the decimal or thousand separator position
 * @param separator symbol "." or ","
 * @param remove_leading_zeros if true removes up to two leading zeros (default is true)
 */
void convertToChar(uint16_t value, char *strValue, uint8_t len, uint8_t dot, uint8_t separator, bool remove_leading_zeros)
{
    char d;
    for (int i = (len - 1); i >= 0; i--)
    {
        d = value % 10;
        value = value / 10;
        strValue[i] = d + 48;
    }
    strValue[len] = '\0';
    if (dot > 0)
    {
        for (int i = len; i >= dot; i--)
        {
            strValue[i + 1] = strValue[i];
        }
        strValue[dot] = separator;
    }

    if (remove_leading_zeros)
    {
        if (strValue[0] == '0')
        {
            strValue[0] = ' ';
            if (strValue[1] == '0')
                strValue[1] = ' ';
        }
    }
}

/**
 * @ingroup group18 
 * @brief  Removes unwanted character from char array 
 * @details replaces non-printable characters to spaces
 * @param *str - string char array
 * @param size - char array size
 */
void removeUnwantedChar( char *str, int size){
    for (int i = 0; str[i] != '\0' && i < size; i++){
      if ( str[i] != 0 && str[i] < 32 ){
        str[i] = ' ';
      }       
    }
    str[size-1] = '\0'; 
}
  
/**
 * @ingroup group16 RDS setup 
 *  
 * @brief  Starts the control member variables for RDS.
 * 
 * @details This method is called by setRdsConfig()
 * 
 * @see setRdsConfig()
 */
void RdsInit()
{
    ESP_LOGD(TAG, " RdsInit");
    clearRdsBuffer2A();
    clearRdsBuffer2B();
    clearRdsBuffer0A();
    rdsTextAdress2A = rdsTextAdress2B = lastTextFlagAB = rdsTextAdress0A = 0;
}

/**
 * @ingroup group16 RDS setup 
 * 
 * @brief Sets RDS property
 * 
 * @details Configures RDS settings to enable RDS processing (RDSEN) and set RDS block error thresholds. 
 * @details When a RDS Group is received, all block errors must be less than or equal the associated block 
 * error threshold for the group to be stored in the RDS FIFO. 
 * @details IMPORTANT: 
 * All block errors must be less than or equal the associated block error threshold 
 * for the group to be stored in the RDS FIFO. 
 * |Value | Description |
 * |------| ----------- | 
 * | 0    | No errors |
 * | 1    | 1–2 bit errors detected and corrected |
 * | 2    | 3–5 bit errors detected and corrected |
 * | 3    | Uncorrectable |
 * 
 * @details Recommended Block Error Threshold options:
 * | Exemples | Description |
 * | -------- | ----------- |
 * | 2,2,2,2  | No group stored if any errors are uncorrected |
 * | 3,3,3,3  | Group stored regardless of errors |
 * | 0,0,0,0  | No group stored containing corrected or uncorrected errors |
 * | 3,2,3,3  | Group stored with corrected errors on B, regardless of errors on A, C, or D | 
 *  
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 104
 * 
 * @param uint8_t RDSEN RDS Processing Enable; 1 = RDS processing enabled.
 * @param uint8_t BLETHA Block Error Threshold BLOCKA.   
 * @param uint8_t BLETHB Block Error Threshold BLOCKB.  
 * @param uint8_t BLETHC Block Error Threshold BLOCKC.  
 * @param uint8_t BLETHD Block Error Threshold BLOCKD. 
 */
void setRdsConfig(SI4735_t *cntrl_data, uint8_t RDSEN, uint8_t BLETHA, uint8_t BLETHB, uint8_t BLETHC, uint8_t BLETHD)
{
    ESP_LOGD(TAG, " setRdsConfig");
    uint8_t write_buf[5] = {0};

    //waitToSend();

    // Set property value
    cntrl_data->property.value = FM_RDS_CONFIG;

    // Arguments
    cntrl_data->rds_config.arg.RDSEN = RDSEN;
    cntrl_data->rds_config.arg.BLETHA = BLETHA;
    cntrl_data->rds_config.arg.BLETHB = BLETHB;
    cntrl_data->rds_config.arg.BLETHC = BLETHC;
    cntrl_data->rds_config.arg.BLETHD = BLETHD;
    cntrl_data->rds_config.arg.DUMMY1 = 0;

    write_buf[0] = 0x00;
    write_buf[1] = cntrl_data->property.raw.byteHigh;
    write_buf[2] = cntrl_data->property.raw.byteLow;
    write_buf[3] = cntrl_data->rds_config.raw[1];
    write_buf[4] = cntrl_data->rds_config.raw[0];

    ESP_LOGD(TAG, " setRdsConfig");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));
    delay_ms(1);
    
    RdsInit();
}

/**
 * @ingroup group16 RDS
 * @brief Sets the minimum number of RDS groups stored in the RDS FIFO before RDSRECV is set.
 * @details Return the number of RDS FIFO used
 * @param value from 0 to 25. Default value is 0.
 */
 void setFifoCount(SI4735_t *cntrl_data, uint16_t value)
{
    ESP_LOGD(TAG, " setFifoCount FM_RDS_INT_FIFO_COUNT");
    sendProperty(cntrl_data, FM_RDS_INT_FIFO_COUNT, value);
}

/** 
 * @ingroup group16 RDS setup 
 * 
 * @brief Configures interrupt related to RDS
 * 
 * @details Use this method if want to use interrupt
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 103
 * 
 * @param RDSRECV If set, generate RDSINT when RDS FIFO has at least FM_RDS_INT_FIFO_COUNT entries.
 * @param RDSSYNCLOST If set, generate RDSINT when RDS loses synchronization.
 * @param RDSSYNCFOUND set, generate RDSINT when RDS gains synchronization.
 * @param RDSNEWBLOCKA If set, generate an interrupt when Block A data is found or subsequently changed
 * @param RDSNEWBLOCKB If set, generate an interrupt when Block B data is found or subsequently changed
 */
void setRdsIntSource(SI4735_t *cntrl_data, uint8_t RDSRECV, uint8_t RDSSYNCLOST, uint8_t RDSSYNCFOUND, uint8_t RDSNEWBLOCKA, uint8_t RDSNEWBLOCKB)
{

    si47x_property property;
    si47x_rds_int_source rds_int_source;

    uint8_t write_buf[5] = {0};

    if (cntrl_data->currentTune != FM_TUNE_FREQ)
        return;

    cntrl_data->rds_int_source.refined.RDSNEWBLOCKB = RDSNEWBLOCKB;
    cntrl_data->rds_int_source.refined.RDSNEWBLOCKA = RDSNEWBLOCKA;
    cntrl_data->rds_int_source.refined.RDSSYNCFOUND = RDSSYNCFOUND;
    cntrl_data->rds_int_source.refined.RDSSYNCLOST = RDSSYNCLOST;
    cntrl_data->rds_int_source.refined.RDSRECV = RDSRECV;
    cntrl_data->rds_int_source.refined.DUMMY1 = 0;
    cntrl_data->rds_int_source.refined.DUMMY2 = 0;

    cntrl_data->property.value = FM_RDS_INT_SOURCE;

    write_buf[0] = 0x00;
    write_buf[1] = cntrl_data->property.raw.byteHigh;
    write_buf[2] = cntrl_data->property.raw.byteLow;
    write_buf[3] = cntrl_data->rds_int_source.raw[1];
    write_buf[4] = cntrl_data->rds_int_source.raw[0];

    ESP_LOGD(TAG, " setRdsIntSource");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));

}

/**  
 * @ingroup group16 RDS status 
 * 
 * @brief Returns the programa type. 
 * 
 * @details Read the Block A content
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 77 and 78
 * 
 * @return BLOCKAL
 */
uint16_t getRdsPI(SI4735_t *cntrl_data)
{
    if (getRdsReceived(cntrl_data) && getRdsNewBlockA(cntrl_data))
    {
        return cntrl_data->rds_status.resp.BLOCKAL;
    }
    return 0;
}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Gets the RDS status. Store the status in currentRdsStatus member. RDS COMMAND FM_RDS_STATUS
 * 
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 55 and 77
 * 
 * @param INTACK Interrupt Acknowledge; 0 = RDSINT status preserved. 1 = Clears RDSINT.
 * @param MTFIFO 0 = If FIFO not empty, read and remove oldest FIFO entry; 1 = Clear RDS Receive FIFO.
 * @param STATUSONLY Determines if data should be removed from the RDS FIFO. 
 *                   0 = Data in BLOCKA, BLOCKB, BLOCKC, BLOCKD, and BLE contain the oldest data in the RDS FIFO.
 *                   1 = Data in BLOCKA will contain the last valid block A data received for the cur- rent station. Data in BLOCKB will contain the last valid block B data received for the current station. Data in BLE will describe the bit errors for the data in BLOCKA and BLOCKB.
 */
void getRdsStatus(SI4735_t *cntrl_data, uint8_t INTACK, uint8_t MTFIFO, uint8_t STATUSONLY)
{
    si47x_rds_command rds_cmd;
    static uint16_t lastFreq;
    uint8_t write_buf[3];

    // checking current FUNC (Am or FM)
    if (cntrl_data->currentTune != FM_TUNE_FREQ)
        return;

    if (lastFreq != cntrl_data->frequency.value)
    {
        lastFreq = cntrl_data->frequency.value;
        clearRdsBuffer2A();
        clearRdsBuffer2B();
        clearRdsBuffer0A();
    }

    cntrl_data->rds_command.arg.INTACK = INTACK;
    cntrl_data->rds_command.arg.MTFIFO = MTFIFO;
    cntrl_data->rds_command.arg.STATUSONLY = STATUSONLY;

    write_buf[0] = FM_RDS_STATUS;
    write_buf[1] = cntrl_data->rds_command.raw;

    ESP_LOGD(TAG, " getRdsStatus");
    ESP_ERROR_CHECK(register_read_block(write_buf, 2, cntrl_data->rds_status.raw, 13));

    delay_ms(1);//delayMicroseconds(550);
}

/**
 * @ingroup group16 RDS status
 *
 * @brief Retrieves the current RDS data to be utilized by other RDS functions.
 * @details Just another way to call getRdsStatus. Both do the same thing.
 * @details This function must be called before calling any RDS function.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 55 and 77
 * @see getRdsStatus
 */
void rdsBeginQuery(SI4735_t *cntrl_data) { getRdsStatus(cntrl_data, 0, 0, 0); }

/**
 * @ingroup group16 RDS
 * @brief Get the Rds Received FIFO
 * @details if FIFO is 1, it means the minimum number of groups was filled
 * @return true if minimum number of groups was filled.
 */
bool getRdsReceived(SI4735_t *cntrl_data)
{
    return (bool) cntrl_data->rds_status.resp.RDSRECV;
}

/**
 * @ingroup group16 RDS
 * @brief Get the Rds Sync Lost object
 * @details returns true (1) if Lost RDS synchronization is detected.
 * @return true if Lost RDS synchronization detected.
 */
bool getRdsSyncLost(SI4735_t *cntrl_data)
{
    return cntrl_data->rds_status.resp.RDSSYNCLOST;
}

/**
 * @ingroup group16 RDS
 * @brief Get the Rds Sync Found
 * @details return true if found RDS synchronization
 * @return true if found RDS synchronization
 */
bool getRdsSyncFound(SI4735_t *cntrl_data)
{
    return cntrl_data->rds_status.resp.RDSSYNCFOUND;
}

/**
 * @ingroup group16 RDS
 * @brief Get the Rds New Block A
 *
 * @details Returns true if valid Block A data has been received.
 * @return true or false
 */
bool getRdsNewBlockA(SI4735_t *cntrl_data)
{
    return (bool)cntrl_data->rds_status.resp.RDSNEWBLOCKA;
}

/**
 * @ingroup group16 RDS
 * @brief Get the Rds New Block B
 * @details Returns true if valid Block B data has been received.
 * @return true or false
 */
bool getRdsNewBlockB(SI4735_t *cntrl_data)
{
    return cntrl_data->rds_status.resp.RDSNEWBLOCKB;
}

/**
 * @ingroup group16 RDS
 * @brief Get the Rds Sync
 * @details Returns true if RDS currently synchronized.
 * @return true or false
 */
bool getRdsSync(SI4735_t *cntrl_data)
{
    return (bool)cntrl_data->rds_status.resp.RDSSYNC;
}

/**
 * @ingroup group16 RDS
 * @brief Get the Group Lost
 * @details Returns true if one or more RDS groups discarded due to FIFO overrun.
 * @return true or false
 */
bool getGroupLost(SI4735_t *cntrl_data)
{
    return cntrl_data->rds_status.resp.GRPLOST;
}

/**
 * @ingroup group16 RDS
 * @brief Get the Num Rds Fifo Used
 * @details Return the number of RDS FIFO used
 * @return uint8_t Total RDS FIFO used
 */
uint8_t getNumRdsFifoUsed(SI4735_t *cntrl_data)
{
    return (uint8_t)cntrl_data->rds_status.resp.RDSFIFOUSED;
}

/**
 * @ingroup group16 RDS
 * @brief Check if 0xD or 0xA special characters were received for group A
 * @see resetEndIndicatorGroupA
 * @return true or false
 */
bool getEndIndicatorGroupA()
{
    return rdsEndGroupA;
}

/**
 * @ingroup group16 RDS
 * @brief Resets 0xD or 0xA special characters condition (makes it false)
 * @see getEndIndicatorGroupA
 */
void resetEndIndicatorGroupA()
{
    rdsEndGroupA = false;
}

/**
 * @ingroup group16 RDS
 * @brief Check if 0xD or 0xA special characters were received for group B
 * @see resetEndIndicatorGroupB
 * @return true or false
 */
bool getEndIndicatorGroupB()
{
    return rdsEndGroupB;
}

/**
 * @ingroup group16 RDS
 * @brief Resets 0xD or 0xA special characters condition (makes it false)
 * @see getEndIndicatorGroupB
 */
void resetEndIndicatorGroupB()
{
    rdsEndGroupB = false;
}

/**
 * @ingroup group16 RDS status
 * @brief Empty FIFO
 * @details  Clear RDS Receive FIFO.
 * @see getRdsStatus
 */
void rdsClearFifo(SI4735_t *cntrl_data)
{
    getRdsStatus(cntrl_data, 1, 0, 0);// getRdsStatus(cntrl_data, 1, 0);
}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Returns the Group Type (extracted from the Block B)
 * 
 * @return BLOCKBL 
 */
uint8_t getRdsGroupType(SI4735_t *cntrl_data)
{
    cntrl_data->rds_blockb.raw.lowValue = cntrl_data->rds_status.resp.BLOCKBL;
    cntrl_data->rds_blockb.raw.highValue = cntrl_data->rds_status.resp.BLOCKBH;

    return cntrl_data->rds_blockb.refined.groupType;
}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Returns the current Text Flag A/B  
 * 
 * @return uint8_t current Text Flag A/B  
 */
uint8_t getRdsFlagAB(SI4735_t *cntrl_data)
{
    cntrl_data->rds_blockb.raw.lowValue = cntrl_data->rds_status.resp.BLOCKBL;
    cntrl_data->rds_blockb.raw.highValue = cntrl_data->rds_status.resp.BLOCKBH;

    return cntrl_data->rds_blockb.refined.textABFlag;
}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Returns the address of the text segment.
 * 
 * @details 2A - Each text segment in version 2A groups consists of four characters. A messages of this group can be 
 * have up to 64 characters. 
 * @details 2B - In version 2B groups, each text segment consists of only two characters. When the current RDS status is
 *      using this version, the maximum message length will be 32 characters.
 * 
 * @return uint8_t the address of the text segment.
 */
uint8_t getRdsTextSegmentAddress(SI4735_t *cntrl_data)
{
    cntrl_data->rds_blockb.raw.lowValue = cntrl_data->rds_status.resp.BLOCKBL;
    cntrl_data->rds_blockb.raw.highValue = cntrl_data->rds_status.resp.BLOCKBH;

    return cntrl_data->rds_blockb.refined.content;
}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Gets the version code (extracted from the Block B)
 * 
 * @returns  0=A or 1=B
 */
uint8_t getRdsVersionCode(SI4735_t *cntrl_data)
{
    cntrl_data->rds_blockb.raw.lowValue = cntrl_data->rds_status.resp.BLOCKBL;
    cntrl_data->rds_blockb.raw.highValue = cntrl_data->rds_status.resp.BLOCKBH;

    return cntrl_data->rds_blockb.refined.versionCode;
}

/**  
 * @ingroup group16 RDS status 
 * 
 * @brief Returns the Program Type (extracted from the Block B)
 * 
 * @see https://en.wikipedia.org/wiki/Radio_Data_System
 * 
 * @return program type (an integer betwenn 0 and 31)
 */
uint8_t getRdsProgramType(SI4735_t *cntrl_data)
{
    cntrl_data->rds_blockb.raw.lowValue = cntrl_data->rds_status.resp.BLOCKBL;
    cntrl_data->rds_blockb.raw.highValue = cntrl_data->rds_status.resp.BLOCKBH;

    return cntrl_data->rds_blockb.refined.programType;
}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Process data received from group 2B
 * 
 * @param c  char array reference to the "group 2B" text 
 */
void getNext2Block(SI4735_t *cntrl_data, char *c)
{
    c[1] = cntrl_data->rds_status.resp.BLOCKDL;
    c[0] = cntrl_data->rds_status.resp.BLOCKDH;
}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Process data received from group 2A
 * 
 * @param c  char array reference to the "group  2A" text 
 */
void getNext4Block(SI4735_t *cntrl_data, char *c)
{
    c[0] = cntrl_data->rds_status.resp.BLOCKCH;
    c[1] = cntrl_data->rds_status.resp.BLOCKCL;
    c[2] = cntrl_data->rds_status.resp.BLOCKDH;
    c[3] = cntrl_data->rds_status.resp.BLOCKDL;

}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Gets the RDS Text when the message is of the Group Type 2 version A
 * 
 * @return char*  The string (char array) with the content (Text) received from group 2A 
 */
char *getRdsText(SI4735_t *cntrl_data)
{
    // Needs to get the "Text segment address code".
    // Each message should be ended by the code 0D (Hex)
    if (rdsTextAdress2A >= 16)
        rdsTextAdress2A = 0;

    getNext4Block(cntrl_data, &rds_buffer2A[rdsTextAdress2A * 4]);

    rdsTextAdress2A += 4;

    return rds_buffer2A;
}

/**
 * @ingroup group16 RDS status 
 * @todo RDS Dynamic PS or Scrolling PS 
 * @brief Gets the station name and other messages. 
 * 
 * @return char* should return a string with the station name. 
 *         However, some stations send other kind of messages
 */
char *getRdsText0A(SI4735_t *cntrl_data)
{
    if (getRdsReceived(cntrl_data))
    {
        if (getRdsGroupType(cntrl_data) == 0)
        {
            if ( lastTextFlagAB != getRdsFlagAB(cntrl_data) )  {
                 lastTextFlagAB = getRdsFlagAB(cntrl_data); 
                 clearRdsBuffer0A(cntrl_data);
            } 
            // Process group type 0
            cntrl_data->rds_blockb.raw.highValue = cntrl_data->rds_status.resp.BLOCKBH;
            cntrl_data->rds_blockb.raw.lowValue = cntrl_data->rds_status.resp.BLOCKBL;

            rdsTextAdress0A = cntrl_data->rds_blockb.group0.address;
            if (rdsTextAdress0A >= 0 && rdsTextAdress0A < 4)
            {
                getNext2Block(cntrl_data, &rds_buffer0A[rdsTextAdress0A * 2]);
                rds_buffer0A[8] = '\0';
                return rds_buffer0A;
            }
        }
    }
    return NULL;
}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Gets the Text processed for the 2A group
 * 
 * @return char* string with the Text of the group A2  
 */
char *getRdsText2A(SI4735_t *cntrl_data)
{
    // getRdsStatus();
    if (getRdsReceived(cntrl_data))
    {
        if (getRdsGroupType(cntrl_data) == 2 /* && getRdsVersionCode() == 0 */)
        {
            // Process group 2A
            // Decode B block information
            cntrl_data->rds_blockb.raw.highValue = cntrl_data->rds_status.resp.BLOCKBH;
            cntrl_data->rds_blockb.raw.lowValue = cntrl_data->rds_status.resp.BLOCKBL;
            rdsTextAdress2A = cntrl_data->rds_blockb.group2.address;

            if (rdsTextAdress2A >= 0 && rdsTextAdress2A < 16)
            {
                getNext4Block(cntrl_data, &rds_buffer2A[rdsTextAdress2A * 4]);
                rds_buffer2A[63] = '\0';
                return rds_buffer2A;
            }
        }
    }
    return NULL;
}

/**
 * @ingroup group16 RDS status 
 * 
 * @brief Gets the Text processed for the 2B group
 * 
 * @return char* string with the Text of the group AB  
 */
char *getRdsText2B(SI4735_t *cntrl_data)
{

    getRdsStatus(cntrl_data, 0, 0, 0);
    if (getRdsReceived(cntrl_data))
    {
        if (getRdsNewBlockB(cntrl_data))
        {
            if (getRdsGroupType(cntrl_data) == 2 /* && getRdsVersionCode() == 1 */)
            {
                // Process group 2B
                cntrl_data->rds_blockb.raw.highValue = cntrl_data->rds_status.resp.BLOCKBH;
                cntrl_data->rds_blockb.raw.lowValue = cntrl_data->rds_status.resp.BLOCKBL;
                rdsTextAdress2B = cntrl_data->rds_blockb.group2.address;
                if (rdsTextAdress2B >= 0 && rdsTextAdress2B < 16)
                {
                    getNext2Block(cntrl_data, &rds_buffer2B[rdsTextAdress2B * 2]);
                    rds_buffer2B[32] = '\0'; 
                    return rds_buffer2B;
                }
            }
       }
     }
    return NULL;
}

/**
 * @ingroup group16 RDS 
 * @brief Gets Station Name, Station Information, Program Information and utcTime
 * @details This function populates four char pointer variable parameters with Station Name, Station Information, Programa Information and UTC time.
 * @details You must call  setRDS(true), setRdsFifo(true) before calling getRdsAllData(...)
 * @details ATTENTION: You don't need to call any additional function to obtain the RDS information; simply follow the steps outlined below. 
 * @details ATTENTION: If no data is found for the given parameter, it is assigned a NULL value. Prior to using the pointers variable, make sure to check if it is null.
 * @details the right way to call this function is shown below.
 * @code {.cpp}
 *
 * void setup() {
 *   rx.setup(RESET_PIN, FM_FUNCTION);
 *   rx.setFM(8400, 10800, currentFrequency, 10);
 *  delay_ms(500);
 *   rx.setRdsConfig(3, 3, 3, 3, 3);
 *   rx.setFifoCount(1);
 * }
 *
 * char *utcTime;
 * char *stationName;
 * char *programInfo;
 * char *stationInfo;
 * 
 * void showStationName() {
 *   if (stationName != NULL) {
 *     // do something
 *    }
 *  }
 * 
 * void showStationInfo() {
 *   if (stationInfo != NULL) {
 *     // do something
*     }
 *  }
 * 
 * void showProgramaInfo() {
 *  if (programInfo != NULL) {
 *    // do something
 *  }
 * }
 * 
 * void showUtcTime() {
 *   if (rdsTime != NULL) {
 *     // do something
 *   }
 * }
 * 
 * void loop() {
 *   .
 *   .
 *   .
 *   if (rx.isCurrentTuneFM()) {
 *     // The char pointers above will be populate by the call below. So, the char pointers need to be passed by reference (pointer to pointer).
 *     if (rx.getRdsAllData(&stationName, &stationInfo , &programInfo, &rdsTime) ) {
 *         showProgramaInfo(programInfo); // you need check if programInfo is null in showProgramaInfo
 *         showStationName(stationName); // you need check if stationName is null in showStationName
 *         showStationInfo(stationInfo); // you need check if stationInfo is null in showStationInfo
 *         showUtcTime(rdsTime); // // you need check if rdsTime is null in showUtcTime
 *     }
 *   }
 *   .
 *   .
 *   .
 *  delay_ms(5);
 * }
 * @endcode
 * @details ATTENTION: the parameters below are point to point to array of char.
 * @param stationName (reference)  - if NOT NULL,  point to Name of the Station (char array -  9 bytes)
 * @param stationInformation (reference)  - if NOT NULL, point to Station information (char array - 33 bytes)
 * @param programInformation (reference)  - if NOT NULL, point to program information (char array - 65 nytes)
 * @param utcTime  (reference)  - if NOT NULL, point to char array containing the current UTC time (format HH:MM:SS +HH:MM)
 * @return True if found at least one valid data
 * @see setRDS, setRdsFifo, getRdsAllData
 */
bool getRdsAllData(SI4735_t *cntrl_data, char **stationName, char **stationInformation, char **programInformation, char **utcTime)
{
    rdsBeginQuery(cntrl_data);
    if (!getRdsReceived(cntrl_data))  return false;
    if (!getRdsSync(cntrl_data) || getNumRdsFifoUsed(cntrl_data) == 0) return false;
    *stationName = getRdsText0A(cntrl_data);        // returns NULL if no information
    *stationInformation = getRdsText2B(cntrl_data); // returns NULL if no information
    *programInformation = getRdsText2A(cntrl_data); // returns NULL if no information
    *utcTime = getRdsTime(cntrl_data);              // returns NULL if no information

    return (bool)stationName | (bool)stationInformation | (bool)programInformation | (bool)utcTime;
}

/**
     * @ingroup group16
     * @brief Gets the Program Information (RT - Radio Text)
     * @details Process the program information data. Same getRdsText2A(). It is a alias for getRdsText2A.
     * @details ATTENTION: You must call getRdsReady before calling this function.
     * @return char array with the program information (63 bytes)
     * @see getRdsText2A
     */
char *getRdsProgramInformation(SI4735_t *cntrl_data) { return getRdsText2A(cntrl_data); };

   /**
     * @ingroup group16
     * @brief Gets the Station Name
     * @details Alias for getRdsText0A
     * @details ATTENTION: You must call getRdsReady before calling this function.
     * @return char* should return a string with the station name. However, some stations send other kind of messages
     * @see getRdsText0A
     */
char *getRdsStationName(SI4735_t *cntrl_data) { return getRdsText0A(cntrl_data); };
/**
 * @ingroup group16 RDS Time and Date 
 * 
 * @brief Gets the RDS time and date when the Group type is 4 
 * @details Returns theUTC Time and offset (to convert it to local time)
 * @details return examples: 
 * @details                 12:31 +03:00 
 * @details                 21:59 -02:30
 * 
 * @return  point to char array. Format:  +/-hh:mm (offset)
 */
char *getRdsTime(SI4735_t *cntrl_data)
{
    // Under Test and construction
    // Need to check the Group Type before.
    si47x_rds_date_time dt;

    uint16_t minute;
    uint16_t hour;

    if (getRdsGroupType(cntrl_data) == 4)
    {
        char offset_sign;
        int offset_h;
        int offset_m;
        // uint16_t y, m, d;
        cntrl_data->rds_date_time.raw[4] = cntrl_data->rds_status.resp.BLOCKBL;
        cntrl_data->rds_date_time.raw[5] = cntrl_data->rds_status.resp.BLOCKBH;
        cntrl_data->rds_date_time.raw[2] = cntrl_data->rds_status.resp.BLOCKCL;
        cntrl_data->rds_date_time.raw[3] = cntrl_data->rds_status.resp.BLOCKCH;
        cntrl_data->rds_date_time.raw[0] = cntrl_data->rds_status.resp.BLOCKDL;
        cntrl_data->rds_date_time.raw[1] = cntrl_data->rds_status.resp.BLOCKDH;

        // Unfortunately it was necessary dues to  the GCC compiler on 32-bit platform.
        // See si47x_rds_date_time (typedef union) and CGG “Crosses boundary” issue/features.
        // Now it is working on Atmega328, STM32, Arduino DUE, ESP32 and more.
        minute =  cntrl_data->rds_date_time.refined.minute;
        hour =   cntrl_data->rds_date_time.refined.hour;

        offset_sign = (cntrl_data->rds_date_time.refined.offset_sense == 1) ? '+' : '-';
        offset_h = (cntrl_data->rds_date_time.refined.offset * 30) / 60;
        offset_m = (cntrl_data->rds_date_time.refined.offset * 30) - (offset_h * 60);
        // sprintf(rds_time, "%02u:%02u %c%02u:%02u", dt.refined.hour, dt.refined.minute, offset_sign, offset_h, offset_m);
        // sprintf(rds_time, "%02u:%02u %c%02u:%02u", hour, minute, offset_sign, offset_h, offset_m);
        // Using convertToChar instead sprintf to save space (about 1.2K on ATmega328 compiler tools).
    
        if (offset_h > 12 || offset_m > 60 || hour > 24 || minute > 60)
            return NULL;

        convertToChar(hour, rds_time, 2, 0, ' ', false);
        rds_time[2] = ':';
        convertToChar(minute, &rds_time[3], 2, 0, ' ', false);
        rds_time[5] = ' ';
        rds_time[6] = offset_sign;
        convertToChar(offset_h, &rds_time[7], 2, 0, ' ', false);
        rds_time[9] = ':';
        convertToChar(offset_m, &rds_time[10], 2, 0, ' ', false);
        rds_time[12] = '\0';
        
        return rds_time;
    }

    return NULL;
}

/**
 * @ingroup group16 RDS Modified Julian Day Converter (MJD) 
 * @brief Converts the MJD number to integers Year, month and day
 * @details Calculates day, month and year based on MJD
 * @details This MJD algorithm is an adaptation of the javascript code found at http://www.csgnetwork.com/julianmodifdateconv.html
 * @param mjd   mjd number 
 * @param year  year variable reference 
 * @param month month variable reference 
 * @param day day variable reference 
 */
void mjdConverter(uint32_t mjd, uint32_t *year, uint32_t *month, uint32_t *day)
{
    uint32_t jd, ljd, njd;
    jd = mjd + 2400001;
    ljd = jd + 68569;
    njd = (uint32_t)(4 * ljd / 146097);
    ljd = ljd - (uint32_t)((146097 * njd + 3) / 4);
    *year = (uint32_t)(4000 * (ljd + 1) / 1461001);
    ljd = ljd - (uint32_t)((1461 * (*year) / 4)) + 31;
    *month = (uint32_t)(80 * ljd / 2447);
    *day = ljd - (uint32_t)(2447 * (*month) / 80);
    ljd = (uint32_t)(*month / 11);
    *month = (uint32_t)(*month + 2 - 12 * ljd);
    *year = (uint32_t)(100 * (njd - 49) + (*year) + ljd);
}

/**
 * @ingroup group16 RDS Time and Date
 * @brief   Decodes the RDS time to LOCAL Julian Day and time
 * @details This method gets the RDS date time massage and converts it from MJD to JD and UTC time to local time
 * @details The Date and Time service may not work correctly depending on the FM station that provides the service. 
 * @details I have noticed that some FM stations do not use the service properly in my location.  
 * @details Example:
 * @code
 *      uint16_t year, month, day, hour, minute;
 *      .
 *      .
 *      si4735.getRdsStatus();
 *      si4735.getRdsDateTime(&year, &month, &day, &hour, &minute);
 *      .
 *      .        
 * @endcode
 * @param rYear  year variable reference 
 * @param rMonth month variable reference 
 * @param rDay day variable reference 
 * @param rHour local hour variable reference 
 * @param rMinute local minute variable reference 
 * @return true, it the RDS Date and time were processed 
 */
bool getRdsDateTime(SI4735_t *cntrl_data, uint16_t *rYear, uint16_t *rMonth, uint16_t *rDay, uint16_t *rHour, uint16_t *rMinute)
{
    int16_t local_minute;
    uint16_t minute;
    uint16_t hour;
    uint32_t mjd, day, month, year;

    if (getRdsGroupType(cntrl_data) == 4)
    {

        cntrl_data->rds_date_time.raw[4] = cntrl_data->rds_status.resp.BLOCKBL;
        cntrl_data->rds_date_time.raw[5] = cntrl_data->rds_status.resp.BLOCKBH;
        cntrl_data->rds_date_time.raw[2] = cntrl_data->rds_status.resp.BLOCKCL;
        cntrl_data->rds_date_time.raw[3] = cntrl_data->rds_status.resp.BLOCKCH;
        cntrl_data->rds_date_time.raw[0] = cntrl_data->rds_status.resp.BLOCKDL;
        cntrl_data->rds_date_time.raw[1] = cntrl_data->rds_status.resp.BLOCKDH;

        // Unfortunately the resource below was necessary dues to  the GCC compiler on 32-bit platform.
        // See si47x_rds_date_time (typedef union) and CGG “Crosses boundary” issue/features.
        // Now it is working on Atmega328, STM32, Arduino DUE, ESP32 and more.

        mjd = cntrl_data->rds_date_time.refined.mjd;

        minute = cntrl_data->rds_date_time.refined.minute;
        hour =  cntrl_data->rds_date_time.refined.hour;

        // calculates the jd Year, Month and Day base on mjd number
        // mjdConverter(mjd, &year, &month, &day);

        // Converting UTC to local time
        local_minute = ((hour * 60) + minute) + ((cntrl_data->rds_date_time.refined.offset * 30) * ((cntrl_data->rds_date_time.refined.offset_sense == 1) ? -1 : 1));
        if (local_minute < 0) {
            local_minute += 1440;
            mjd--;  // drecreases one day 
        }
        else if (local_minute > 1440)
        {
            local_minute -= 1440;
            mjd++; // increases one day
        }

        // calculates the jd Year, Month and Day base on mjd number
        mjdConverter(mjd, &year, &month, &day);

        hour = (uint16_t)local_minute / 60;
        minute = local_minute - ( hour * 60);

        if (hour > 24 || minute > 60 || day > 31 || month > 12 )
            return false;

        *rYear = (uint16_t)year;
        *rMonth = (uint16_t) month;
        *rDay = (uint16_t) day;
        *rHour = hour;
        *rMinute = minute;

        return true;

    }
    return false;
}

/**
 * @ingroup group16 RDS Time and Date
 * @brief Gets the RDS the Time and Date when the Group type is 4 
 * @details Returns the Date, UTC Time and offset (to convert it to local time)
 * @details return examples: 
 * @details                 2021-07-29 12:31 +03:00 
 * @details                 1964-05-09 21:59 -02:30
 * 
 * @return array of char yy-mm-dd hh:mm +/-hh:mm offset
 */
char *getRdsDateTimeStr(SI4735_t *cntrl_data)
{
    uint16_t minute;
    uint16_t hour;
    uint32_t mjd, day, month, year;    

    if (getRdsGroupType(cntrl_data) == 4)
    {
        char offset_sign;
        int offset_h;
        int offset_m;

        cntrl_data->rds_date_time.raw[4] = cntrl_data->rds_status.resp.BLOCKBL;
        cntrl_data->rds_date_time.raw[5] = cntrl_data->rds_status.resp.BLOCKBH;
        cntrl_data->rds_date_time.raw[2] = cntrl_data->rds_status.resp.BLOCKCL;
        cntrl_data->rds_date_time.raw[3] = cntrl_data->rds_status.resp.BLOCKCH;
        cntrl_data->rds_date_time.raw[0] = cntrl_data->rds_status.resp.BLOCKDL;
        cntrl_data->rds_date_time.raw[1] = cntrl_data->rds_status.resp.BLOCKDH;

        // Unfortunately the resource below was necessary dues to  the GCC compiler on 32-bit platform.
        // See si47x_rds_date_time (typedef union) and CGG “Crosses boundary” issue/features.
        // Now it is working on Atmega328, STM32, Arduino DUE, ESP32 and more.

        mjd = cntrl_data->rds_date_time.refined.mjd;

        minute = cntrl_data->rds_date_time.refined.minute;
        hour =  cntrl_data->rds_date_time.refined.hour;

        // calculates the jd (Year, Month and Day) base on mjd number
        mjdConverter(mjd, &year, &month, &day);

        // Calculating hour, minute and offset
        offset_sign = (cntrl_data->rds_date_time.refined.offset_sense == 1) ? '+' : '-';
        offset_h = (cntrl_data->rds_date_time.refined.offset * 30) / 60;
        offset_m = (cntrl_data->rds_date_time.refined.offset * 30) - (offset_h * 60);

        // Converting the result to array char - 
        // Using convertToChar instead sprintf to save space (about 1.2K on ATmega328 compiler tools).

        if (offset_h > 12 || offset_m > 60 || hour > 24 || minute > 60 || day > 31 || month > 12 )
            return NULL;

        convertToChar(year, rds_time, 4, 0, ' ', false);
        rds_time[4] = '-';
        convertToChar(month, &rds_time[5], 2, 0, ' ', false);
        rds_time[7] = '-';
        convertToChar(day, &rds_time[8], 2, 0, ' ', false);
        rds_time[10] = ' ';
        convertToChar(hour, &rds_time[11], 2, 0, ' ', false);
        rds_time[13] = ':';
        convertToChar(minute, &rds_time[14], 2, 0, ' ', false);
        rds_time[16] = ' ';
        rds_time[17] = offset_sign;
        convertToChar(offset_h, &rds_time[18], 2, 0, ' ', false);
        rds_time[20] = ':';
        convertToChar(offset_m, &rds_time[21], 2, 0, ' ', false);
        rds_time[23] = '\0';

        return rds_time;
    }

    return NULL;
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/**
 * @ingroup group16 RDS setup
 * @brief Clear RDS buffer 2A (Radio Text / Program Information)
 * @details same clearRdsProgramInformation
 */
 void clearRdsBuffer2A() { memset(rds_buffer2A, 0, sizeof(rds_buffer2A)); }

/**
 * @ingroup group16 RDS setup
 * @brief Clear RDS buffer 2A (Radio Text / Program Information)
 * @details same clearRdsBuffer2A
 */
 void clearRdsProgramInformation() { memset(rds_buffer2A, 0, sizeof(rds_buffer2A)); }

/**
 * @ingroup group16 RDS setup
 * @brief Clear RDS buffer 2B (text / Station INformation 32 bytes)
 * @details Same clearRdsStationInformation
 */
 void clearRdsBuffer2B() { memset(rds_buffer2B, 0, sizeof(rds_buffer2B)); }

/**
 * @ingroup group16 RDS setup
 * @brief Clear RDS buffer 2B (text / Station INformation 32 bytes)
 * @details Same clearRdsBuffer2B
 */
 void clearRdsStationInformation() { memset(rds_buffer2B, 0, sizeof(rds_buffer2B)); }

/**
 * @ingroup group16 RDS setup
 * @brief Clear RDS buffer 0A (text / Station Name)
 * @details clearRdsStationName
 */
 void clearRdsBuffer0A() { memset(rds_buffer0A, 0, sizeof(rds_buffer0A)); }

/**
 * @ingroup group16 RDS setup
 * @brief Clear RDS buffer 0A (text / Station Name)
 * @details clearRdsBuffer0A
 */
 void clearRdsStationName() { memset(rds_buffer0A, 0, sizeof(rds_buffer0A)); }

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/************************************
 *        Seek station
 * @ingroup group08 Received Signal Quality
 *
 * @brief Queries the status of the Received Signal Quality (RSQ) of the current channel (FM_RSQ_STATUS)
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 75 and 141
 *
 * @param INTACK Interrupt Acknowledge.
 *        0 = Interrupt status preserved;
 *        1 = Clears RSQINT, BLENDINT, SNRHINT, SNRLINT, RSSIHINT, RSSILINT, MULTHINT, MULTLINT.
 */
/**
 * @ingroup group08 Seek
 *
 * @brief Look for a station (Automatic tune)
 * @details Starts a seek process for a channel that meets the RSSI and SNR criteria for AM.
 * @details __This function does not work on SSB mode__.
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 55, 72, 125 and 137
 *
 * @param SEEKUP Seek Up/Down. Determines the direction of the search, either UP = 1, or DOWN = 0.
 * @param Wrap/Halt. Determines whether the seek should Wrap = 1, or Halt = 0 when it hits the band limit.
 */
void seekStation(SI4735_t *cntrl_data, uint8_t SEEKUP, uint8_t WRAP)
{
    //si47x_seek seek;
    //si47x_seek_am_complement seek_am_complement;
    uint8_t write_buf[6];

    // Check which FUNCTION (AM or FM) is working now
    uint8_t seek_start_cmd = (cntrl_data->currentTune == FM_TUNE_FREQ) ? FM_SEEK_START : AM_SEEK_START;

    cntrl_data->seek.arg.SEEKUP = SEEKUP;
    cntrl_data->seek.arg.WRAP = WRAP;
    cntrl_data->seek.arg.RESERVED1 = 0;
    cntrl_data->seek.arg.RESERVED2 = 0;
    
    write_buf[0] = seek_start_cmd;
    write_buf[1] = cntrl_data->seek.raw;
    if (seek_start_cmd == AM_SEEK_START) // Sets additional configuration for AM mode
    {
        cntrl_data->seek_am_complement.ARG2 = cntrl_data->seek_am_complement.ARG3 = 0;
        cntrl_data->seek_am_complement.ANTCAPH = 0;
        cntrl_data->seek_am_complement.ANTCAPL = (cntrl_data->frequency.value > 1800) ? 1 : 0; // if SW = 1

        write_buf[2] = cntrl_data->seek_am_complement.ARG2;         // ARG2 - Always 0
        write_buf[3] = cntrl_data->seek_am_complement.ARG3;         // ARG3 - Always 0
        write_buf[4] = cntrl_data->seek_am_complement.ANTCAPH;      // ARG4 - Tuning Capacitor: The tuning capacitor value
        write_buf[5] = cntrl_data->seek_am_complement.ANTCAPL;      // ARG5 - will be selected automatically.
    }
    ESP_LOGD(TAG, " seekStation");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));

   delay_ms(MAX_DELAY_AFTER_SET_FREQUENCY << 2);
}

/**
 * @ingroup group08 Seek
 *
 * @brief Search for the next station.
 * @details Like seekStationUp this function goes to a next station.
 * @details The main difference is the method used to look for a station.
 *
 * @see seekStation, seekStationUp, seekStationDown, seekPreviousStation, seekStationProgress
 */
uint16_t seekNextStation(SI4735_t *cntrl_data)
{
    seekStation(cntrl_data, 1, 1);
    delay_ms(maxDelaySetFrequency);
    return getFrequency(cntrl_data);
}

/**
 * @ingroup group08 Seek
 *
 * @brief Search the previous station
 * @details Like seekStationDown this function goes to a previous station.
 * @details The main difference is the method used to look for a station.
 * @see seekStation, seekStationUp, seekStationDown, seekPreviousStation, seekStationProgress
 */
uint16_t seekPreviousStation(SI4735_t *cntrl_data)
{
    seekStation(cntrl_data, 0, 1);
    delay_ms(maxDelaySetFrequency);
    return getFrequency(cntrl_data);
}

/**
 * @ingroup group08 Seek
 * @brief Seeks a station up or down.
 * @details Seek up or down a station and call a function defined by the developer to show the frequency and stop seeking process by the user.
 * @details The first parameter of this function is a name of your function that you have to implement to show the current frequency.
 * @details The second parameter is the name function that will check stop seeking action. Thus function should return true or false and should read a button, encoder or some status to make decision to stop or keep seeking.
 * @details If you do not want to show the seeking progress,  you can set NULL instead the name of the function.
 * @details If you do not want stop seeking checking, you can set NULL instead the name of a function.
 * @details The code below shows an example using ta function the shows the current frequency on he Serial Monitor. You might want to implement a function that shows the frequency on your display device.
 * @details Also, you have to declare the frequency parameter that will be used by the function to show the frequency value.
 * @details __This function does not work on SSB mode__.
 * @code
 * void showFrequency( uint16_t freq ) {
 *    Serial.print(freq);
 *    Serial.println("MHz ");
 * }
 *
 * void loop() {
 *
 *  receiver.seekStationProgress(showFrequency, checkStopSeeking, 1); // Seek Up
 *  .
 *  .
 *  .
 *  receiver.seekStationProgress(showFrequency, checkStopSeeking, 0); // Seek Down
 *
 * }
 * @endcode
 *
 * @see seekStation, seekStationUp, seekStationDown, getStatus, setMaxSeekTime
 * @param showFunc  function that you have to implement to show the frequency during the seeking process. Set NULL if you do not want to show the progress.
 * @param stopSeeking functionthat you have to implement if you want to control the stop seeking action.
 * @param up_down   set up_down = 1 for seeking station up; set up_down = 0 for seeking station down
 */
void seekStationProgress(SI4735_t *cntrl_data, void (*showFunc)(uint16_t f), bool (*stopSeking)(), uint8_t up_down)
{
    portTickType LastTime;

    //long elapsed_seek = 100;// millis();
    LastTime = xTaskGetTickCount () + ( portTickType ) 10;
    // seek command does not work for SSB
    if (lastMode == SSB_CURRENT_MODE)
        return;

    do
    {
        seekStation(cntrl_data, up_down, 0);
        delay_ms(maxDelaySetFrequency);
        getStatus(cntrl_data,0, 0);
        delay_ms(maxDelaySetFrequency);
        cntrl_data->frequency.raw.FREQH = cntrl_data->response_status.resp.READFREQH;
        cntrl_data->frequency.raw.FREQL = cntrl_data->response_status.resp.READFREQL;
        if (showFunc != NULL)
            showFunc(cntrl_data->frequency.value);
        if (stopSeking != NULL)
            if (stopSeking())
                return;
    // } while (!currentStatus.resp.VALID && !currentStatus.resp.BLTF && (100 - elapsed_seek) < maxSeekTime);
    } while (xTaskGetTickCount() - LastTime < maxSeekTime);
}

/**
 * @ingroup group05 Interrupt
 *
 * @brief Enables output for GPO1, 2, and 3.
 *
 * @details GPO1, 2, and 3 can be configured for output (Hi-Z or active drive) by setting the GPO1OEN, GPO2OEN, and GPO3OEN bit.
 * @details The state (high or low) of GPO1, 2, and 3 is set with the GPIO_SET command.
 * @details To avoid excessive current consumption due to oscillation, GPO pins should not be left in a high impedance state.
 *
 * | GPIO Output Enable  | value 0 | value 1 |
 * | ---- ---------------| ------- | ------- |
 * | GPO1OEN             | Output Disabled (Hi-Z) (default) | Output Enabled |
 * | GPO2OEN             | Output Disabled (Hi-Z) (default) | Output Enabled |
 * | GPO3OEN             | Output Disabled (Hi-Z) (default) | Output Enabled |
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 82 and 144
 *
 * @param GPO1OEN
 * @param GPO2OEN
 * @param GPO3OEN
 */
void setGpioCtl(SI4735_t *cntrl_data, uint8_t GPO1OEN, uint8_t GPO2OEN, uint8_t GPO3OEN)
{
    esp_err_t ret;
    uint8_t write_buf[2];

    cntrl_data->gpio.arg.GPO1OEN = GPO1OEN;
    cntrl_data->gpio.arg.GPO2OEN = GPO2OEN;
    cntrl_data->gpio.arg.GPO3OEN = GPO3OEN;
    cntrl_data->gpio.arg.DUMMY1 = 0;
    cntrl_data->gpio.arg.DUMMY2 = 0;

    write_buf[0] = GPIO_CTL ;
    write_buf[1] = cntrl_data->gpio.raw;
    ESP_LOGD(TAG, " setGpioCtl");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));

}

/**
 * @ingroup group05 Interrupt
 *
 * @brief Sets the output level (high or low) for GPO1, 2, and 3.
 *
 * @details GPO1, 2, and 3 can be configured for output by setting the GPO1OEN, GPO2OEN, and GPO3OEN bit in the GPIO_CTL command.
 * @details To avoid excessive current consumption due to oscillation, GPO pins should not be left in a high impedance state.
 * @details To avoid excessive current consumption due to oscillation, GPO pins should not be left in a high impedance state.
 *
 * | GPIO Output Enable  | value 0 | value 1 |
 * | ---- ---------------| ------- | ------- |
 * | GPO1LEVEL            |  Output low (default) | Output high |
 * | GPO2LEVEL            |  Output low (default) | Output high |
 * | GPO3LEVEL            |  Output low (default) | Output high |
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); pages 83 and 145
 *
 * @param GPO1LEVEL
 * @param GPO2LEVEL
 * @param GPO3LEVEL
 */
void setGpio(SI4735_t *cntrl_data, uint8_t GPO1LEVEL, uint8_t GPO2LEVEL, uint8_t GPO3LEVEL)
{
    esp_err_t ret;
    uint8_t write_buf[2];

    cntrl_data->gpio.arg.GPO1OEN = GPO1LEVEL;
    cntrl_data->gpio.arg.GPO2OEN = GPO2LEVEL;
    cntrl_data->gpio.arg.GPO3OEN = GPO3LEVEL;
    cntrl_data->gpio.arg.DUMMY1 = 0;
    cntrl_data->gpio.arg.DUMMY2 = 0;

    write_buf[0] = GPIO_CTL ;
    write_buf[1] = cntrl_data->gpio.raw;

    ESP_LOGD(TAG, " setGpio");
    ESP_ERROR_CHECK(register_write_block( write_buf, sizeof(write_buf)));
    //ESP_ERROR_CHECK(register_write_byte(GPIO_SET, gpio.raw));
}

/**
 * @ingroup group05 Interrupt
 *
 * @brief Configures the sources for the GPO2/INT interrupt pin.
 *
 * @details Valid sources are the lower 8 bits of the STATUS byte, including CTS, ERR, RSQINT, and STCINT bits.
 * @details The corresponding bit is set before the interrupt occurs. The CTS bit (and optional interrupt) is set when it is safe to send the next command.
 * @details The CTS interrupt enable (CTSIEN) can be set with this property and the POWER_UP command.
 * @details The state of the CTSIEN bit set during the POWER_UP command can be read by reading this property and modified by writing this property.
 *
 * @see Si47XX PROGRAMMING GUIDE; AN332 (REV 1.0); page 146
 *
 * @param STCIEN Seek/Tune Complete Interrupt Enable (0 or 1).
 * @param RSQIEN RSQ Interrupt Enable (0 or 1).
 * @param ERRIEN ERR Interrupt Enable (0 or 1).
 * @param CTSIEN CTS Interrupt Enable (0 or 1).
 * @param STCREP STC Interrupt Repeat (0 or 1).
 * @param RSQREP RSQ Interrupt Repeat(0 or 1).
 */
void setGpioIen(SI4735_t *cntrl_data, uint8_t STCIEN, uint8_t RSQIEN, uint8_t ERRIEN, uint8_t CTSIEN, uint8_t STCREP, uint8_t RSQREP)
{
    cntrl_data->gpio_ien.arg.DUMMY1 = cntrl_data->gpio_ien.arg.DUMMY2 = cntrl_data->gpio_ien.arg.DUMMY3 = cntrl_data->gpio_ien.arg.DUMMY4 = 0;
    cntrl_data->gpio_ien.arg.STCIEN = STCIEN;
    cntrl_data->gpio_ien.arg.RSQIEN = RSQIEN;
    cntrl_data->gpio_ien.arg.ERRIEN = ERRIEN;
    cntrl_data->gpio_ien.arg.CTSIEN = CTSIEN;
    cntrl_data->gpio_ien.arg.STCREP = STCREP;
    cntrl_data->gpio_ien.arg.RSQREP = RSQREP;

    sendProperty(cntrl_data, GPO_IEN, cntrl_data->gpio_ien.raw);
}
