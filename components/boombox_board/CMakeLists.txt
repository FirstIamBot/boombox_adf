idf_component_register(
    SRCS 
        "boombox_board_v1_0/board.c"
        "boombox_board_v1_0/board_pins_config.c"
        "boombox_codec_driver/new_codec.c"
    INCLUDE_DIRS 
        "boombox_board_v1_0" 
        "boombox_codec_driver"
    PRIV_INCLUDE_DIRS
        "boombox_board_v1_0"
    REQUIRES 
        audio_hal 
        audio_board
        audio_sal
        esp_peripherals
    PRIV_REQUIRES 
        audio_stream 
        esp_dispatcher 
        display_service
)

# Make board.h available globally to all ADF components
idf_build_set_property(COMPILE_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}/boombox_board_v1_0" APPEND)

# Link with audio_board component
idf_component_get_property(audio_board_lib audio_board COMPONENT_LIB)
target_link_libraries(${COMPONENT_LIB} INTERFACE ${audio_board_lib})
