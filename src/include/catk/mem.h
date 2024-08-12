#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <catk/types.h>

/* Physical memory */

int physmem_init(uint32_t mbi);
void physmem_block_free(uint32_t addr);
uint32_t physmem_block_alloc(void);

/* Virtual memory */


/*
 * The Jamix Portable Allocator => (JPalloc)
 * Made by Jamix from OSDev.org / deyzi-the-youtuber on GitHub :P
*/

void heap_init(void);
uintptr_t get_heap_start(void);
uintptr_t get_heap_end(void);
uintptr_t heap_get_used(void);
void * malloc(size_t n);
void * calloc(size_t num, size_t size);
void free(void * ptr);

#endif
