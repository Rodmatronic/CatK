#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <catk/types.h>

/*
 * We use liballoc now. :)
*/

void physmem_init(void);

void * physmem_alloc_block(void);
void * physmem_alloc_blocks(size_t sz);

void physmem_free_block(void * addr);
void physmem_free_blocks(void * addr, size_t sz);

void * malloc(size_t);
void * realloc(void *, size_t);
void * calloc(size_t, size_t);

void free(void *);

#endif
