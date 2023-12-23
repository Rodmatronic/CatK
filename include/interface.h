#ifndef INTERFACE_H
#define INTERFACE_H

#include "types.h"
#include "config.h"
#include "vga.h"

void constUI();
void restartvgadriv();
void vga_graphics_interface();
extern enum vga_color back_color;
extern char* app;

#endif

