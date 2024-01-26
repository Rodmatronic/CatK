#ifndef TERM_H
#define TERM_H

#include <stddef.h>
#include <stdint.h>
#include "libc.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;


/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

void terminal_init(void);
void terminal_write(const char* data, size_t size) ;
void terminal_setcolor(uint8_t color);
void vga_enable_cursor();
void vga_set_cursor_pos(uint8_t x, uint8_t y);

#endif
