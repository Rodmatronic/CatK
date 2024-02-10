/* keyboard interface IO port: data and control
   READ:   status port
   WRITE:  control register */
#include <term.h>
#define KBRD_INTRFC 0x64

/* keyboard interface bits */
#define KBRD_BIT_KDATA 0 /* keyboard data is in buffer (output buffer is empty) (bit 0) */
#define KBRD_BIT_UDATA 1 /* user data is in buffer (command buffer is empty) (bit 1) */

#define KBRD_IO 0x60 /* keyboard IO port */
#define KBRD_RESET 0xFE /* reset CPU command */

#define bit(n) (1<<(n)) /* Set bit n to 1 */

/* Check if bit n in flags is set */
#define check_flag(flags, n) ((flags) & bit(n))


#include "pc.h"
#include "panic.h"
#include "term.h"
#include "printk.h"
#include "power.h"

void poweroff(int type) {

    if (type == 0)
    {
        terminal_setcolor(VGA_COLOR_WHITE);
        printk("Rebooting...\n");
        uint8_t temp;
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
        terminal_setcolor(VGA_COLOR_WHITE);
        printk("Powering off...\n");

        //for Vbox
        outports(0x4004, 0x3400);
        outports(0x604, 0x2000);
        outports(0x600, 0x34);
        __asm__ __volatile__("int $0x80");
    }

    if (type == 2)
    {
        terminal_setcolor(VGA_COLOR_WHITE);
        printk("Halting...\n");

        printk("CatK has been halted.\n");
        printk("Please switch off your machine now, or press the reset switch.");

        vga_disable_cursor();

        while(1){
            for (;;);
        }

    }

}