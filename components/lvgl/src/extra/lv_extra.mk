CSRCS += $(shell find -L $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/extra -name "*.c")

VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/extra/widgets/textprogress
