#include <panic.h>
#include <printk.h>
#include <cpu.h>

void panic(char* message)
{
    printk("\npanic: %s\n", message);
    cpuid_info();
    printk("CatK has panicked due to an unrecoverable error, please reboot");
    for(;;);
}