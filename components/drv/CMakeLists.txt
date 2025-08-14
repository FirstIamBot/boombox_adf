# CMakeLists.txt for the drv component
# This component handles the display and touch driver for the LVGL GUI
# It includes the TFT display driver (ILI9341) and the touch driver (XPT2046)
# The display is connected via SPI, and the touch screen is also connected via SPI   
idf_component_register(SRCS 
                         "disp_driver.c"
                         "tft/disp_spi.c"
                         "tft/ili9341.c"
                         "touch/touch_driver.c"
                         "touch/tp_spi.c"
                         "touch/xpt2046.c"

                         INCLUDE_DIRS . ./tft ./touch
                         REQUIRES lvgl driver)