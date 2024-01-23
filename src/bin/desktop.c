#include "vesa.h"
#include "console.h"
#include "libc.h"
#include "font_vesa.h"
#include "config.h"

// Function to draw a filled rectangle
void vbe_draw_rectangle(uint32 x, uint32 y, uint32 width, uint32 height, uint32 color) {
    for (uint32 i = y; i < y + height; i++) {
        for (uint32 j = x; j < x + width; j++) {
            vbe_putpixel(j, i, color);
        }
    }
}

// Function to draw a simple non-filled rectangle (outline)
void vbe_draw_outline(uint32 x, uint32 y, uint32 width, uint32 height, uint32 color) {
    // Draw top and bottom edges
    for (uint32 i = x; i < x + width; i++) {
        vbe_putpixel(i, y, color);             // Top edge
        vbe_putpixel(i, y + height - 1, color); // Bottom edge
    }

    // Draw left and right edges
    for (uint32 i = y; i < y + height; i++) {
        vbe_putpixel(x, i, color);             // Left edge
        vbe_putpixel(x + width - 1, i, color); // Right edge
    }
}

void drawmenubar()
{
    vbe_draw_outline(0, 0, 800, 35, VBE_RGB(255, 255, 255));
    vbe_draw_outline(1, 1, 799, 34, VBE_RGB(255, 255, 255));
    vbe_draw_outline(0, 36, 800, 2, VBE_RGB(80, 80, 80));
    vbe_draw_rectangle(2, 2, 799, 33, VBE_RGB(170, 170, 170));
}

void windowdraw(int x, int y, char* name)
{
    vbe_draw_outline(x, y, 500, 350, VBE_RGB(155, 155, 155));
    vbe_draw_outline(x+1, y+1, 498, 348, VBE_RGB(220, 220, 220));
    vbe_draw_outline(x+2, y+2, 496, 346, VBE_RGB(155, 155, 155));

    vbe_draw_rectangle(x+3, y+3, 494, 344, VBE_RGB(255, 255, 255));

    bitmap_draw_string(name, x+3, y+6, VBE_RGB(0, 0, 5));
    bitmap_draw_string("The kernel is Operating System", x+3, y+30, VBE_RGB(0, 0, 5));
    bitmap_draw_string("proper", x+3, y+44, VBE_RGB(0, 0, 5));
    bitmap_draw_string("--Dennis Ritchie", x+3, y+70, VBE_RGB(0, 0, 5));
}

void desktop()
{
    int ret = vesa_init(800, 600, 32);
    if (ret < 0) {
        printf("failed to init vesa graphics\n");
    }
    if (ret == 1) {
    } else {
        // fill with a solid color
        uint32 x = 0;
        for (uint32 i = 0; i < 600; i++) {
            for (uint32 j = 0; j < 800; j++) {
                vbe_putpixel(j, i, VBE_RGB(0, 100, 190));
            }
        }
        drawmenubar();

        bitmap_draw_string("CatK", 2, 10, VBE_RGB(2, 25, 50));
        bitmap_draw_string(versionnumber, 76, 10, VBE_RGB(2, 25, 50));

        windowdraw(110, 120, "Welcome");

    }
}