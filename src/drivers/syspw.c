/*
 *
 *  syspw, created by Rodmatronics
 *
 *
*/

/* keyboard interface IO port: data and control
   READ:   status port
   WRITE:  control register */
#define KBRD_INTRFC 0x64

/* keyboard interface bits */
#define KBRD_BIT_KDATA 0 /* keyboard data is in buffer (output buffer is empty) (bit 0) */
#define KBRD_BIT_UDATA 1 /* user data is in buffer (command buffer is empty) (bit 1) */

#define KBRD_IO 0x60 /* keyboard IO port */
#define KBRD_RESET 0xFE /* reset CPU command */

#define bit(n) (1<<(n)) /* Set bit n to 1 */

/* Check if bit n in flags is set */
#define check_flag(flags, n) ((flags) & bit(n))


#include "types.h"
#include "io_ports.h"
#include "panic.h"
#include "syspw.h"
#include "panic.h"
#include "libc.h"
#include "console.h"

void syspw(int type) {

    vga_disable_cursor();

    if (type == 0)
    {
        printf("\n%C--------------------------------------------------------------------------------\n", 0xF, 0x0);
        printf("%CSending REBOOT (0) to the kernel\n", 0xF, 0x0);
        uint8 temp;
        asm volatile ("cli"); /* disable all interrupts */
        /* Clear all keyboard buffers (output and command buffers) */
        do
        {
            temp = inportb(KBRD_INTRFC); /* empty user data */
            if (check_flag(temp, KBRD_BIT_KDATA) != 0)
                inportb(KBRD_IO); /* empty keyboard data */
        } while (check_flag(temp, KBRD_BIT_UDATA) != 0);
        outportb(KBRD_INTRFC, KBRD_RESET); /* pulse CPU reset line */
        loop:
        asm volatile ("hlt"); /* if that didn't work, halt the CPU */
        goto loop; /* if a NMI is received, halt again */

        outports(0x4004, 0x3400);
        __asm__ __volatile__("int $0x19");
    }

    if (type == 1)
    {

        //add_data_to_file(&rootfs, "kernel.logs", "kernel: Sending POWEROFF (1) to kernel\n");
        //printf_dark("syspw Created by Rodmatronics\n");
        printf("\n%C--------------------------------------------------------------------------------\n", 0xF, 0x0);
        printf("%CSending POWEROFF (1) to the kernel\n", 0xF, 0x0);

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
        printf("\n%C--------------------------------------------------------------------------------\n", 0xF, 0x0);
        printf("%CSending HALT (2) to the kernel\n\n", 0xF, 0x0);

        printf("The operating system has halted.\n");
        printf("Please switch off your machine now, or press the reset switch.\n");

        while(1){
            for (;;);
        }

    }

}
