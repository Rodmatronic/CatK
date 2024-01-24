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

void drawwindow(int xw, int yw, char* name)
{

    // Window grabber
    for (int x = 5; x < 330; x++) {
        for (int y = 5; y < 30; y++) {
            vbe_putpixel(x, y, VBE_RGB(2,190,249));
        }
    }

    // Window close button shadow
    for (int x = 10; x < 25; x++) {
        for (int y = 10; y < 25; y++) {
            vbe_putpixel(x, y, VBE_RGB(0,90,180));
        }
    }

    // Window close button
    for (int x = 11; x < 23; x++) {
        for (int y = 11; y < 23; y++) {
            vbe_putpixel(x, y, VBE_RGB(0,120,230));
        }
    }

    // Window Body
    for (int x = 8; x < 328; x++) {
        for (int y = 33; y < 278; y++) { 
            vbe_putpixel(x, y, VBE_RGB(255,255,255));
        }
    }

    // Window Border UP
    for (int x = 5; x < 327; x++) {
        for (int y = 30; y < 33; y++) { 
            vbe_putpixel(x, y, VBE_RGB(203,203,203));
        }
    }

    // Window Border LEFT
    for (int x = 5; x < 8; x++) {
        for (int y = 33; y < 277; y++) { 
            vbe_putpixel(x, y, VBE_RGB(203,203,203));
        }
    }

    // Window Border RIGHT
    for (int x = 327; x < 330; x++) {
        for (int y = 30; y < 277; y++) { 
            vbe_putpixel(x, y, VBE_RGB(203,203,203));
        }
    }

    // Window Border DOWN
    for (int x = 5; x < 330; x++) {
        for (int y = 277; y < 280; y++) { 
            vbe_putpixel(x, y, VBE_RGB(203,203,203));
        }
    }

    bitmap_draw_string(name, 25, 9, VBE_RGB(0, 0, 5));

}

void desktop()
{
    int ret = vesa_init(800, 600, 32);
    if (ret < 0) {
        printf("failed to init vesa graphics\n");
    }
    if (ret == 1) {
    } else {
        // Fill the desktop
        for (int x = 0; x < 800; x++) {
            for (int y = 0; y < 600; y++) {
                vbe_putpixel(x, y, VBE_RGB(37, 150, 190));
            }
        }

        bitmap_draw_string("CatK", 2, 570, VBE_RGB(2, 25, 50));
        bitmap_draw_string(versionnumber, 76, 570, VBE_RGB(2, 25, 50));

        drawwindow(80, 80, "Hello");

    }
}