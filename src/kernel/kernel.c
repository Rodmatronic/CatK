#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "include/config.h"
#include "include/fsnode.h"
#include "include/gdt.h"
#include "term.h"
#include "printk.h"
#include "panic.h"
#include "cpu.h"
#include "keyboard.h"
#include "k_sh.h"
#include "vfs.h"
#include "config.h"
#include "fsnode.h"
#include "pc.h"
#include "ramfs.h"
#include "kernel.h"
#include "multiboot.h"
#include "memory.h"
#include "time.h"
#include "gdt.h"
#include "idt.h"
#include "read.h"
#include "serial.h"
#include "isr.h"

void bootart();

void kmain(unsigned long magic, unsigned long addr) 
{
	terminal_init();                                /* This starts the terminal, prints the "Cat... kernel!"" message, and */
	printk("%CCat... ", VGA_COLOR_CYAN);			/* shows the CatK boot logo.*/
	printk("%Ckernel!\n", VGA_COLOR_LIGHT_CYAN);
	bootart();
    printk("booting in 3...");
    sleep(1);
    printk(" 2..."); sleep(1); printk(" 1..."); sleep(1);

    /* This is where the actual loading starts. */

    terminal_init();   
    printk("%C-<CATKERNEL START!>-\n", VGA_COLOR_WHITE);

    asm volatile("cli");
    bootloader_info(magic, addr);

    gdt_init();
    idt_init();
    init_pit();

	cpuid_info();
    memory_init();
    init_serial();
    time_init();
    vfs_init();
    //createramfs();
    printk("%Chit ESC to continue boot to k_sh\n", VGA_COLOR_DARK_GREY);
    read();
    k_sh();

    /*
     * We dont normally get here. If we do, either something has crashed, 
     * or the kernel has literally nothing left to do.
    */

    panic("Kernel has nothing left to do!");
}

void bootart()
{
        printk("\n%C           __           __             \n",VGA_COLOR_WHITE);
        printk("%C          /  \\         /  \\        \n", VGA_COLOR_WHITE);
        printk("%C         / /\\ \\       / /\\ \\       \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C        / /  \\ \\     / /  \\ \\      \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C       / /      \\___/      \\ \\         _______   _____   _______  ___   _\n", VGA_COLOR_LIGHT_CYAN);
        printk("%C      /                       \\       |   ____| /  _  \\ |       ||   | | |\n", VGA_COLOR_LIGHT_CYAN);
        printk("%C     |        |      |         |      |  |     |  | |  ||_     _||   |_| |\n", VGA_COLOR_LIGHT_CYAN);
        printk("%C   ---        |      |         ---    |  |     |  |_|  |  |   |  |      _|\n", VGA_COLOR_LIGHT_CYAN);
        printk("%C     |                         |      |  |     |       |  |   |  |     |_ \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C   ---  ", VGA_COLOR_LIGHT_CYAN);
        printk("%C//", VGA_COLOR_CYAN);
        printk("%C       ^       ", VGA_COLOR_LIGHT_CYAN);
        printk("%C//", VGA_COLOR_CYAN);
        printk("%C    ---    |  |____ |   _   |  |   |  |    _  |\n", VGA_COLOR_LIGHT_CYAN);
        printk("%C      \\         \\/\\/          /       |_______||__| |__|  |___|  |___| |_|\n", VGA_COLOR_LIGHT_CYAN);
        printk(" %C      \\                     /        Written from scratch by the team! :3 \n", VGA_COLOR_CYAN);
        printk("%C        \\___________________/      \n", VGA_COLOR_CYAN);
        printk("%C         ===================       \n", VGA_COLOR_LIGHT_RED);
        printk("%C        =========", VGA_COLOR_RED);
        printk("%C\\/", VGA_COLOR_LIGHT_BROWN);
        printk("%C==========      \n", VGA_COLOR_RED);
        printk("%C                /  \\               \n", VGA_COLOR_LIGHT_BROWN);
        printk("%C               |CatK|              \n", VGA_COLOR_LIGHT_BROWN);
        printk("%C                \\__/               \n", VGA_COLOR_LIGHT_BROWN);
		printk("%C", VGA_COLOR_LIGHT_GREY);
        printk("\n\n\nCatK(mascot) was created by Rodmatronics\n", VGA_COLOR_LIGHT_GREY);
}