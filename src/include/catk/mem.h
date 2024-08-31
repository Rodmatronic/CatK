#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <catk/types.h>

/* Physical memory */

void physmem_init(uintptr_t mbi);
void * physmem_alloc_block(void);

#endif
