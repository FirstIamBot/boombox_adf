#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

ifdef CONFIG_AUDIO_BOARD_CUSTOM
COMPONENT_ADD_INCLUDEDIRS += ./boombox_codec_driver
COMPONENT_SRCDIRS += ./boombox_codec_driver

COMPONENT_ADD_INCLUDEDIRS += ./boombox_board_v1_0
COMPONENT_SRCDIRS += ./boombox_board_v1_0
endif
