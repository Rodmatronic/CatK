#include "printk.h"

void panic(char* message)
{
    printk("\n--------------------------------------------------------------------------------");
    printk("panic: %s", message);
    for(;;);
}