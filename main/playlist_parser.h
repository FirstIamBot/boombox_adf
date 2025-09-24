#pragma once

int parse_playlist(const char *filename);
const char* playlist_get_title(int index);
const char* playlist_get_url(int index);
int playlist_get_count();
