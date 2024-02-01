#include <panic.h>
#include <printk.h>
#include <cpu.h>
#include <term.h>

void panic(char* message)
{
    printk("\n%Cpanic: ", VGA_COLOR_WHITE);
    printk("%s\n", message);
    cpuid_info();
    printk("CatK has panicked due to an unrecoverable error, please reboot");
    for(;;);
}