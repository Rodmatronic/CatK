/*
 * This is used when the system has encountered a major error.
 * Written by Rodmatronics
*/

#include "string.h"
#include "kernel.h"
#include "time.h"
#include "io_ports.h"
#include "vga.h"
#include "cpu.h"
#include "bitmap.h"
#include "syspw.h"
#include "libc.h"
#include "console.h"
#include "panic.h"
#include "interface.h"

void panic(const char* panictext, ...)
{
    //add_data_to_file(&rootfs, "kernel.logs", "kernel: Kernel has panicked! Has been logged at /sbin/kernel.logs\n");
    //console_init(COLOR_WHITE, COLOR_BLACK);
    
    printf("\n%C--------------------------------------------------------------------------------\n", 0xF, 0x0);
    write_serial("--------------------------------------------------------------------------------");
    write_serial("\n");
    printf("%Cpanic: %s\n", 0x0, 0xF, panictext);

    //this is for VGA graphical mode

    vga_graphics_clear_color(back_color);
    app = "fatal error";
    constUI(0);
    drawpopupwindow(0);
    draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 4, COLOR_DARK_GREY, "panic");
    draw_string(VGA_MAX_WIDTH/4 + 2, VGA_MAX_HEIGHT/4 + 20, COLOR_DARK_GREY, panictext);

    //more kernel stuff

    write_serial("panic: ");
    write_serial(panictext);
    write_serial("\n");
    cpusimple(1);
    printf("\n");
    GetCurrentTime();

    printf("%CCatK has been halted.\n", 0xF, 0x0);
    printf("%CPlease switch off the system now\n", 0xF, 0x0);

    vga_disable_cursor();

    for (;;);

}