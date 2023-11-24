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
        add_data_to_file(&rootfs, "kernel.logs", "kernel: Sending REBOOT (0) to kernel\n");
        printf_dark("syspw Created by Rodmatronics\n");
        printf("Sending REBOOT (0) to kernel\n");
        // This inline assembly code sends the restart command.

        //for Vbox
        outports(0x4004, 0x3400);
        __asm__ __volatile__("int $0x19");
    }

    if (type == 1)
    {
        add_data_to_file(&rootfs, "kernel.logs", "kernel: Sending POWEROFF (1) to kernel\n");
        printf_dark("syspw Created by Rodmatronics\n");
        printf("Sending POWEROFF (1) to kernel\n");

        //for Vbox
        outports(0x4004, 0x3400);
        __asm__ __volatile__("int $0x80");
    }

    if (type == 2)
    {
        add_data_to_file(&rootfs, "kernel.logs", "kernel: Sending HALT (2) to kernel\n");
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
