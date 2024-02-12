#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "term.h"
#include "printk.h"
#include "panic.h"
#include "memory.h"
#include "config.h"
#include "pc.h"

/*
 * I really hate memory.
 */

unsigned short total;
unsigned char lowmem, highmem;

void memory_init() {
    printk("memory_init: init\n");
    outportb(0x70, 0x30);
    lowmem = inportb(0x71);
    outportb(0x70, 0x31);
    highmem = inportb(0x71);
    total = lowmem | highmem << 8;
    printk("bios mode memory = %d KB\n", total / 1000);
    bmem = total / 1000;
}

void *memcpy(void *dest, const void *src, size_t n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    
    for (size_t i = 0; i < n; ++i) {
        d[i] = s[i];
    }
    
    return dest;
}

void *malloc(size_t size) {
    size_t total_allocated = total_blocks * size;
    
    if (total_allocated + size <= memreal && total_blocks < MAX_BLOCKS) {
        void *ptr = (void *)((char *)memory + total_blocks * size);
        memory[total_blocks].size = size;
        memory[total_blocks].ptr = ptr;
        total_blocks++;
        return ptr;
    } else {
        panic("Memory violation. Out of memory!");
        return NULL; // Memory allocation failed
    }
}

void free(void *ptr) {
    for (size_t i = 0; i < total_blocks; ++i) {
        if (memory[i].ptr == ptr) {
            memory[i].ptr = NULL;
            memory[i].size = 0;
            break;
        }
    }
}
