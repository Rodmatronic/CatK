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
        printf("syspw Created by Rodmatronics\n");
        printf("Sending REBOOT (0) to kernel\n");
        // This inline assembly code sends the restart command.
        __asm__ __volatile__("int $0x19");
    }

    if (type == 1)
    {
        printf("syspw Created by Rodmatronics\n");
        printf("Sending REBOOT (0) to kernel\n");

        printf("Powering off is currently not supported, sorry!");
    }

    panic("syspw Failed to exectute the command! Possibly invalid number.", type);

}
