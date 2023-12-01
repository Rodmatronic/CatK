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
#include "libc.h"

void panic(const char* panictext, ...)
{
    //add_data_to_file(&rootfs, "kernel.logs", "kernel: Kernel has panicked! Has been logged at /sbin/kernel.logs\n");
    //console_init(COLOR_WHITE, COLOR_BLACK);
    printf("\n%C--------------------------------------------------------------------------------", 0xF, 0x0);
    printf("%C\npanic: %s\n", 0x0, 0xF, panictext);
    cpusimple(1);
    printf("\n");
    GetCurrentTime();
    printf("%CCatK has been halted.\n", 0xF, 0x0);
    printf("%CPlease switch off the system now\n", 0xF, 0x0);
    vga_disable_cursor();
    console_gotoxy(0, 24);
    //printf_dark("The system will reboot in 5 seconds");
    //sleep(5);
    while(1)
    {
        for(;;);
    }
}