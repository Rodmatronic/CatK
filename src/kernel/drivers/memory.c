#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "term.h"
#include "printk.h"
#include "panic.h"
#include "memory.h"
#include "pc.h"

unsigned short total;
unsigned char lowmem, highmem;

void memory_init() {
    printk("Memory info -------\n");

    outportb(0x70, 0x30);
    lowmem = inportb(0x71);
    outportb(0x70, 0x31);
    highmem = inportb(0x71);
    total = lowmem | highmem << 8;
    printk("BIOS memory: %d", total);
    printk("\n----------------------\n");
}