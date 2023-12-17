#include "fs.h"
#include "console.h"
#include "libc.h"
#include "read.h"
#include "config.h"
#include "kernel.h"
#include "vga.h"
#include "launchp.h"
#include "bitmap.h"
#include "panic.h"
#include "keyboard.h"
#include "syspw.h"

char* app = "CatK vga interface";

void drawpopupwindow()
{
    vga_graphics_draw_rect(VGA_MAX_WIDTH/4, VGA_MAX_HEIGHT/4, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/2, BLACK);
    vga_graphics_draw_rect(VGA_MAX_WIDTH/4 + 1, VGA_MAX_HEIGHT/4 + 1, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/2, BLACK);
    vga_graphics_fill_rect(VGA_MAX_WIDTH/4 + 1, VGA_MAX_HEIGHT/4 + 1, VGA_MAX_WIDTH/2 - 2, VGA_MAX_HEIGHT/2 - 2, GREY);
}

void constUI()
{
    // Menu bar
    vga_graphics_draw_rect(0, 0, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 2, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 4, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 6, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 8, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 10, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 11, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 13, VGA_MAX_WIDTH-1, 0, BLACK);
    draw_string(1, 3, COLOR_DARK_GREY, app);

    // Taskbar
    vga_graphics_draw_rect(0, 14, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 16, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 18, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 20, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 22, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 24, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 26, VGA_MAX_WIDTH-1, 1, GREY);
    vga_graphics_draw_rect(0, 28, VGA_MAX_WIDTH-1, 0, BLACK);
    draw_string(1, 17, COLOR_DARK_GREY, "About");
    draw_string(50, 17, COLOR_DARK_GREY, "panic");
    draw_string(100, 17, COLOR_DARK_GREY, "shutdown");
    vga_graphics_draw_rect(0, 26, VGA_MAX_WIDTH-1, 1, GREY);
}

void about()
{
    app = "about";
    char* workingdir = current_directory;
    current_directory = "/etc";
    read_from_file(&rootfs, "hostname", buffer, sizeof(buffer), 0);
    read_from_file(&rootfs, "session", buffer2, sizeof(buffer2), 0);
    write_serial("uname: ");
    write_serial(buffer);
    write_serial("\n");
    current_directory = workingdir;
    while (1)
    {
        constUI();
        // Read a key scancode
        unsigned char scancode = read_key();

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        if (scancode == 0x2D) {
            vga_graphics_interface();
            break;
        }

        drawpopupwindow();
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 4, COLOR_DARK_GREY, "About catk ");
        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 20, COLOR_DARK_GREY, "hostname ");
        draw_string(VGA_MAX_WIDTH/4 + 75, VGA_MAX_HEIGHT/4 + 20, COLOR_DARK_GREY, buffer);

        draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 32, COLOR_DARK_GREY, "session ");
        draw_string(VGA_MAX_WIDTH/4 + 75, VGA_MAX_HEIGHT/4 + 32, COLOR_DARK_GREY, buffer2);
    }
}

void vga_graphics_interface()
{   
    app = "desktop";

    while (1)
    {
        constUI();
        // Read a key scancode
        unsigned char scancode = read_key();

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        if (scancode == 0x02)
        {
            about();
        }

        if (scancode == 0x03)
        {
            panic("manually triggered");
            break;
        }

        if (scancode == 0x04)
        {
            vga_graphics_clear_color(RED);
            vga_graphics_exit();
            syspw(1);
        }

        draw_string(1, 17, COLOR_DARK_GREY, key);

        vga_graphics_clear_color(BLUE);

    }
}
