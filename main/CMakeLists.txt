set(COMPONENT_SRCS "playlist_parser.c" "boombox.c" "app_main.c" "task/bt_task.c" "task/http_task.c" "task/air_task.c" "task/audio_equalizer.c" ) 
set(COMPONENT_ADD_INCLUDEDIRS . "task" "/home/gena/.espressif/esp-adf" )

register_component()