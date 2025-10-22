#pragma once
#include "board.h"


void http_player();
void http_player_task(void *pvParameters);

void http_player_start( );
void http_player_run( );
void http_player_stop();