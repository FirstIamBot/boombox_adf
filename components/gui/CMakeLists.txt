
FILE(GLOB_RECURSE SOURCES ./*.c 
                          ./custom/*.c
                          ./generated/*.c 
                          ./import/image/*.c
                          ./gui_touch_calibration/*.c
                          ./gui_touch_calibration/img/*.c)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS . generated custom import import/image gui_touch_calibration
                    REQUIRES lvgl nvs_flash drv commons)