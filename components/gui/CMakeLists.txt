
FILE(GLOB_RECURSE SOURCES ./*.c 
                          ./custom/*.c
                          ./generated/*.c 
                          ./import/image/*.c
)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS . generated custom import import/image 
                    REQUIRES lvgl nvs_flash encoder drv)
