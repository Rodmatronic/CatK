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
#include "syspw.h"
#include "libc.h"
#include "console.h"
#include "panic.h"

void panic(const char* panictext, ...)
{
    //add_data_to_file(&rootfs, "kernel.logs", "kernel: Kernel has panicked! Has been logged at /sbin/kernel.logs\n");
    //console_init(COLOR_WHITE, COLOR_BLACK);
    printf("\n%C--------------------------------------------------------------------------------\n", 0xF, 0x0);
    write_serial("--------------------------------------------------------------------------------");
    write_serial("\n");
    printf("%Cpanic: %s\n", 0x0, 0xF, panictext);
    write_serial("panic: ");
    write_serial(panictext);
    write_serial("\n");
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