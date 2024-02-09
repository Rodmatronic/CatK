#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "term.h"
#include "printk.h"
#include "panic.h"
#include "kernel.h"
#include "multiboot.h"
#include "config.h"
#include "string.h"

void bootloader_info(unsigned long magic, unsigned long addr) {

    MULTIBOOT_INFO *mboot_info;

    mboot_info = (MULTIBOOT_INFO *)addr;

    strcpy(cmdline, (const char *)mboot_info->cmdline);
    strcpy(loader, (const char *)mboot_info->boot_loader_name);

    printk("cmdline: %s\n", (char *)mboot_info->cmdline);
    printk("booted from: %s\n", (char *)mboot_info->boot_loader_name);
    printk("loader magic: 0x%x\n", magic);
    printk("loader flags: 0x%x\n", mboot_info->flags);
    printk("bios low mem: 0x%x KB\n", mboot_info->mem_low);
    printk("bios high mem: 0x%x KB\n", mboot_info->mem_high);
}