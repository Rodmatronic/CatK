/*
 * This is used when the system has encountered a major error.
 * Written by Rodmatronics
*/

#include "string.h"
#include "io_ports.h"
#include "vga.h"

void panic(const char* panictext);

void panic(const char* panictext)
{
    console_init(COLOR_WHITE, COLOR_BLACK);
    printf("panic: %s\n", panictext);
    cpuinfo();
    GetCurrentTime();
    printf("CatK has been halted.\n");
    printf("Please switch off the system now\n");
    vga_disable_cursor();

        console_gotoxy(0, 24);
        printf_dark("Rebooting in 5...");
        sleep(1);
        console_gotoxy(0, 24);
        printf_dark("Rebooting in 4...");
        sleep(1);
        console_gotoxy(0, 24);
        printf_dark("Rebooting in 3...");
        sleep(1);
        console_gotoxy(0, 24);
        printf_dark("Rebooting in 2...");
        sleep(1);
        console_gotoxy(0, 24);
        sleep("Rebooting in 1...");
        sleep(1);

    while(1)
    {
        reboot();
        for(;;);
    }
}
