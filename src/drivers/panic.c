/*
 * This is used when the system has encountered a major error.
 * Written by Rodmatronics
*/

#include "string.h"
#include "time.h"
#include "io_ports.h"
#include "vga.h"
#include "cpu.h"
#include "syspw.h"

void panic(const char* panictext, ...)
{
    //add_data_to_file(&rootfs, "kernel.logs", "kernel: Kernel has panicked! Has been logged at /sbin/kernel.logs\n");
    //console_init(COLOR_WHITE, COLOR_BLACK);
    printf("%Cpanic: %s\n", 0x0, 0x7, panictext);
    cpuinfo();
    GetCurrentTime();
    printf("CatK has been halted.\n");
    printf("Please switch off the system now\n");
    vga_disable_cursor();
    console_gotoxy(0, 24);
    //printf_dark("The system will reboot in 5 seconds");
    //sleep(5);
    while(1)
    {
        for(;;);
    }
}