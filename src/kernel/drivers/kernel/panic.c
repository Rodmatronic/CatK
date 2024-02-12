#include <panic.h>
#include <printk.h>
#include <cpu.h>
#include <term.h>
#include <time.h>
#include <read.h>
#include <power.h>
#include <config.h>
#include <string.h>

/*
 *  Written by Rod.
 */

void panic(char* message)
{
    if (strcmp(cmdline, "debugboot") == 0)
    {    
        terminal_setcolor(VGA_COLOR_LIGHT_BROWN);
        printk("!!!!!!!!!!!!!!!!!!\n");
        printk("panic: %s \n'debugboot' is active. Skipped.\n", message);
        terminal_setcolor(VGA_COLOR_WHITE);
        return;
    }
    terminal_setcolor(VGA_COLOR_LIGHT_BROWN);

    // This is here to make PANIC more noticeable during boot
    printk("!!!!!!!!!!!!!!!!!!\n");

    // On panic, set the color to the color used for kernel logs. (white)
    // This should be standard across CatK. Debugging/kernel text is White, user mode is normal light gray.
    terminal_setcolor(VGA_COLOR_WHITE);

    // Spit out system info. CPU, TIME, CMDLINE, etc.
    cpuid_info();
    printk("time: ");
    current_time();
    printk("\n");
    printk("cmdline: %s\n", cmdline);
    printk("loader: %s\n", loader);
    uint32_t seconds = counter / 10000;
    uint32_t milliseconds = counter % 10000;
    printk("uptime: %d.%u seconds\n", seconds, milliseconds);

    // Print the panic reason
    printk("%Cpanic: ", VGA_COLOR_WHITE);
    printk("%s\n", message);
    vga_disable_cursor();

    // This was added to make debugging easier, Reboot on read break.
    printk("Press any key to reboot...");
    read(1);
    poweroff(0);
    for(;;);
}