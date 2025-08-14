
idf_component_register(SRCS                           
                            "touch_driver.c"
                            "tp_spi.c" 
                            "xpt2046.c"
                            
                         INCLUDE_DIRS .
                         REQUIRES lvgl driver)
