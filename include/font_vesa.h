#ifndef FONT_VESA_H
#define FONT_VESA_H

#include "types.h"

#define BITMAP_SIZE 20

void bitmap_draw_char(char ch, int x, int y, int color);

void bitmap_draw_string(const char *str, int x, int y, int color);
void bitmap_draw_string_upper(char *str, int x, int y, int color);

#endif
