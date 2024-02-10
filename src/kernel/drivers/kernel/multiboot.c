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

    printk("bootloader_info: cmdline: %s\n", (char *)mboot_info->cmdline);
    printk("bootloader_info: booted from: %s\n", (char *)mboot_info->boot_loader_name);
    printk("bootloader_info: loader magic: 0x%x\n", magic);
    printk("bootloader_info: loader flags: 0x%x\n", mboot_info->flags);
    printk("bootloader_info: bios low mem: 0x%x KB\n", mboot_info->mem_low);
    printk("bootloader_info: bios high mem: 0x%x KB\n", mboot_info->mem_high);
    printk("bootloader_info: drives length: 0x%x KB\n", mboot_info->drives_length);
    printk("bootloader_info: drives address: 0x%x KB\n", mboot_info->drives_addr);
    printk("It is reccomended to use GRUB, if you arent, be warned that certain features \nmay not function properly in the future\n");
}