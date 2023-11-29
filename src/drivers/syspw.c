/*
 *
 *  syspw, created by Rodmatronics
 *
 *
*/

#include "types.h"
#include "io_ports.h"
#include "panic.h"
#include "syspw.h"
#include "panic.h"

void syspw(int type) {

    if (type == 0)
    {

        //add_data_to_file(&rootfs, "kernel.logs", "kernel: Sending REBOOT (0) to kernel\n");
        //printf_dark("syspw Created by Rodmatronics\n");
        printf("Sending REBOOT (0) to kernel\n");

        //for Vbox
        outports(0x4004, 0x3400);
        __asm__ __volatile__("int $0x19");
    }

    if (type == 1)
    {

        //add_data_to_file(&rootfs, "kernel.logs", "kernel: Sending POWEROFF (1) to kernel\n");
        //printf_dark("syspw Created by Rodmatronics\n");
        printf("Sending POWEROFF (1) to kernel\n");

        //for Vbox
        outports(0x4004, 0x3400);
        outports(0x604, 0x2000);
        outports(0x600, 0x34);
        __asm__ __volatile__("int $0x80");
    }

    if (type == 2)
    {
        //add_data_to_file(&rootfs, "kernel.logs", "kernel: Sending HALT (2) to kernel\n");
        //printf_dark("syspw Created by Rodmatronics\n");
        printf("Sending HALT (2) to kernel\n\n");

        printf("The operating system has halted.\n");
        printf("Please switch off your machine now, or press the reset switch.\n");

        while(1){
            for (;;);
        }

    }

}
