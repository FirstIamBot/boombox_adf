#pragma once

#include "board.h"
#include "commons.h"



void init_http_player( );
void http_player_run(Data_GUI_Boombox_t *set_data,  Data_Boombox_GUI_t *get_data);
void deinit_http_player();
