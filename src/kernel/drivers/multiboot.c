#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "term.h"
#include "printk.h"
#include "panic.h"
#include "kernel.h"
#include "multiboot.h"

void bootloader_info(unsigned long magic, unsigned long addr) {

    MULTIBOOT_INFO *mboot_info;

    mboot_info = (MULTIBOOT_INFO *)addr;

    printk("Bootloader info -------\n");
    printk("cmdline: %s\n", (char *)mboot_info->cmdline);
    printk("bootloader: %s\n", (char *)mboot_info->boot_loader_name);
    printk("magic: 0x%x\n", magic);
    printk("flags: 0x%x\n", mboot_info->flags);
    printk("mem_low: 0x%x KB\n", mboot_info->mem_low);
    printk("mem_high: 0x%x KB\n", mboot_info->mem_high);
    printk("boot_device: 0x%x\n", mboot_info->boot_device);
    printk("----------------------\n");
}