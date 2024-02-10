#include <panic.h>
#include <printk.h>
#include <cpu.h>
#include <term.h>
#include <time.h>
#include <read.h>
#include <power.h>
#include <config.h>

void panic(char* message)
{
    terminal_setcolor(VGA_COLOR_WHITE);
    cpuid_info();
    printk("time: ");
    current_time();
    printk("\n");
    printk("cmdline: %s\n", cmdline);
    printk("loader: %s\n", loader);
    printk("uptime: %d\n", counter);
    printk("%Cpanic: ", VGA_COLOR_WHITE);
    printk("%s\n", message);
    vga_disable_cursor();

    printk("Press any key to reboot...");
    read(1);
    poweroff(0);
    for(;;);
}