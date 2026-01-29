
FILE(GLOB_RECURSE SOURCES  "app_main.c"                             
                           "boombox.c" 
                           "playlist_parser.c" 
                           "task/bt_task.c"
                           "task/http_task.c" 
                           "task/air_task.c" 
                           "task/server_task.c")

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "." 
                                 "task" 
                                 "/home/gena/.espressif/esp-adf"
                    EMBED_FILES  "../spiffs/www/index.html"  # Добавьте эту строку
                                 "../spiffs/playlist.pls"  # Добавьте эту строку
                    #REQUIRES spiffs esp_wifi nvs_flash bt
)
# Явно указываем зависимость от esp_vfs_spiffs

# Путь к директории с данными SPIFFS (относительно корня проекта)
set(SPIFFS_IMAGE_DIR "${CMAKE_SOURCE_DIR}/spiffs")

# Проверяем существование директории и наличие файлов в ней
if(EXISTS ${SPIFFS_IMAGE_DIR})
    file(GLOB SPIFFS_FILES "${SPIFFS_IMAGE_DIR}/*")
    list(LENGTH SPIFFS_FILES SPIFFS_FILES_COUNT)
    
    if(SPIFFS_FILES_COUNT GREATER 0)
        message(STATUS "SPIFFS data directory found: ${SPIFFS_IMAGE_DIR}")
        message(STATUS "Files count: ${SPIFFS_FILES_COUNT}")
        
        # Создание образа SPIFFS
        spiffs_create_partition_image(storage ${SPIFFS_IMAGE_DIR} FLASH_IN_PROJECT)
    else()
        message(WARNING "SPIFFS data directory is empty: ${SPIFFS_IMAGE_DIR}")
        message(WARNING "Skipping SPIFFS image creation. Add files to the directory and rebuild.")
    endif()
else()
    message(WARNING "SPIFFS data directory not found: ${SPIFFS_IMAGE_DIR}")
    message(WARNING "Create the directory and add files, then rebuild.")
endif()