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

uint64_t get_real_memory_size(MULTIBOOT_INFO *mboot_info) {
    uint64_t real_memory_size = 0;

    // Access memory map
    MULTIBOOT_MEMORY_MAP *mmap = (MULTIBOOT_MEMORY_MAP *)mboot_info->mmap_addr;
    for (uint32_t i = 0; i < mboot_info->mmap_length; i += sizeof(MULTIBOOT_MEMORY_MAP)) {
        // Get the current memory map entry
        MULTIBOOT_MEMORY_MAP *entry = (MULTIBOOT_MEMORY_MAP *)((uint32_t)mmap + i);

        // Check if the memory region is available
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
            // Add the length of the memory region to the total real memory size
            real_memory_size += ((uint64_t)entry->len_high << 32) | entry->len_low;
        }
    }

    return real_memory_size;
}

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
    printk("bootloader_info: memory length: 0x%x KB\n", mboot_info->mmap_length);
    printk("bootloader_info: memory address: 0x%x KB\n", mboot_info->mmap_addr);
    printk("It is recommended to use GRUB, if you arent, be warned that certain features may not function properly in the future\n");
    uint32_t i;
    for (i = 0; i < mboot_info->mmap_length; i += sizeof(MULTIBOOT_MEMORY_MAP)) {
        MULTIBOOT_MEMORY_MAP *mmap = (MULTIBOOT_MEMORY_MAP *)(mboot_info->mmap_addr + i);
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {}
    }
    // Calculate the total real memory size in megabytes
    uint64_t total_real_memory_bytes = get_real_memory_size(mboot_info);
    uint64_t total_real_memory_mb = total_real_memory_bytes / (1024 * 1024);
    printk("real memory = %u MB\n", total_real_memory_mb + 1);
    mem = total_real_memory_mb+1;
}