#include "fs.h"
#include "console.h"
#include "libc.h"
#include "read.h"
#include "config.h"
#include "kernel.h"
#include "vga.h"
#include "bitmap.h"

void drawpopupwindow()
{
    vga_graphics_draw_rect(VGA_MAX_WIDTH/4, VGA_MAX_HEIGHT/4, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/2, BLACK);
    vga_graphics_draw_rect(VGA_MAX_WIDTH/4 + 1, VGA_MAX_HEIGHT/4 + 1, VGA_MAX_WIDTH/2, VGA_MAX_HEIGHT/2, BLACK);
    vga_graphics_fill_rect(VGA_MAX_WIDTH/4 + 1, VGA_MAX_HEIGHT/4 + 1, VGA_MAX_WIDTH/2 - 2, VGA_MAX_HEIGHT/2 - 2, GREY);
}

void about()
{
    char* workingdir = current_directory;
    current_directory = "/etc";
    read_from_file(&rootfs, "hostname", buffer, sizeof(buffer), 0);
    write_serial("uname: ");
    write_serial(buffer);
    write_serial("\n");
    drawpopupwindow();
    draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 4, COLOR_DARK_GREY, buffer);
    read_from_file(&rootfs, "session", buffer, sizeof(buffer), 0);
    draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 13, COLOR_DARK_GREY, buffer);
    current_directory = workingdir;
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
    draw_string(1, 3, COLOR_DARK_GREY, "Catk VGA interface");

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
    vga_graphics_draw_rect(0, 26, VGA_MAX_WIDTH-1, 1, GREY);
}

void vga_graphics_interface()
{   

    while (1)
    {
        constUI();
        read(0);
        about();
        read(0);
        vga_graphics_clear_color(BLUE);

    }
}
