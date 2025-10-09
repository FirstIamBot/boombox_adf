//#pragma once

#include "board.h"
#include "commons.h"
#include "si4735.h"



void clearRDSbuffer();
char *checkRDS(SI4735_t *rx);

void radio_init(SI4735_t *rx, air_config_t *cnfg);
void set_radio(SI4735_t *rx, Data_GUI_Boombox_t *set_data, air_config_t *set);
void get_radio(SI4735_t *rx, Data_Boombox_GUI_t *get_data, air_config_t *get);

void init_air_player(BoomBox_config_t  *init_air_config);
void deinit_air_player();
void air_player( Data_GUI_Boombox_t *xDataGUI,  Data_Boombox_GUI_t *xDataBoomBox);
