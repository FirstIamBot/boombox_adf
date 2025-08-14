
idf_component_register(SRCS 
                            "disp_driver.c"
                            "disp_spi.c"
                            "ili9341.c"
                            
                       INCLUDE_DIRS . ../
                       REQUIRES lvgl driver)
