#ifndef __MEMORY_H
#define __MEMORY_H

/* 
 * The Jamix Operating System portable heap allocator (JPALLOC)
 * Made by Jamix from OSDev.org
*/

#include <stdint.h>
#include <catk/types.h>

void heap_init(uintptr_t * start);
uintptr_t heap_get_used(void);
void * malloc(size_t n);
void * calloc(size_t num, size_t size);
void free(void * ptr);

#endif
