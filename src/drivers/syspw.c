/*
 *
 *  syspw, created by Rodmatronics
 *
 *
*/

#include "types.h"
#include "io_ports.h"
#define ACPI_RESET 0x06

void syspw();

void syspw(int type) {

    if (type == 0)
    {
        printf_dark("syspw Created by Rodmatronics\n");
        printf("Sending REBOOT (0) to kernel\n");
        // This inline assembly code sends the restart command.
        __asm__ __volatile__("int $0x19");
    }

    if (type == 1)
    {
        printf_dark("syspw Created by Rodmatronics\n");
        printf("Sending POWEROFF (1) to kernel\n");

        __asm__ __volatile__("int $0x80");
    }

    if (type == 2)
    {
        printf_dark("syspw Created by Rodmatronics\n");
        printf("Sending HALT (2) to kernel\n\n");

        printf("The operating system has halted.\n");
        printf("Please switch off your machine now, or press the reset switch.\n");

        while(1){
            for (;;);
        }

    }

    panic("syspw Failed to read the command! Possibly invalid number.", type);

}
