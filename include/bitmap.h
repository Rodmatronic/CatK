#ifndef BITMAP_H
#define BITMAP_H

#include "types.h"
#include "creu.kpattern"

#define BITMAP_SIZE 8

void draw_char(uint16 x, uint16 y, uint8 color, char ch);
void draw_string(uint16 x, uint16 y, uint8 color, char *str);
//void creu(int x, int y);

#endif
