#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <catk/types.h>

<<<<<<< HEAD
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

=======
/* Physical memory */

void physmem_init(void);
/* allocation */
void * physmem_alloc_block(void);
void * physmem_alloc_blocks(size_t sz);
/* freeing */
void physmem_free_block(void * addr);
void physmem_free_blocks(void * addr, size_t sz);

/* liballoc */
void * malloc(size_t);
void * realloc(void *, size_t);
void * calloc(size_t, size_t);
>>>>>>> 6be3bbb619dded66460b24ba64e5925e2bee774a
void free(void *);

#endif
