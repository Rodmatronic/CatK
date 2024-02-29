#include "printk.h"

void start_init()
{
    printk("start_init: attempting to load /bin/init\n");
    printk("start_init: attempting to load /sbin/init\n");
    printk("start_init: attempting to load /init\n");
}