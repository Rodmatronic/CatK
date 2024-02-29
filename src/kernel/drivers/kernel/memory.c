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
 * Like, I really hate it. I have been working on making memory in CatK a real working thing for MONTHS!
 * Screw it! Memory marathon. Going to get this shit to work with an FS. If not, what's the point of CatK?
 * The memory_init used to be a glorified way to get the BIOS memory. Who needs that? It's nearly useless!
 */

// Define a structure for memory blocks
typedef struct Block {
    size_t size;
    struct Block* next;
} Block;

// Pointer to the beginning of the free memory
static Block* freeList = NULL;

unsigned short total;
unsigned char lowmem, highmem;

void memory_init() {
    freeList = (Block*)&mem;
    freeList->size = mem - sizeof(Block);
    freeList->next = NULL;
}

void *memcpy(void *dest, const void *src, size_t len)
{
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

// Function to allocate memory
void* malloc(size_t size) {
    // Check if the size is 0 or there is no free memory
    if (size == 0 || freeList == NULL) {
        return NULL;
    }

    // Align size to a multiple of sizeof(Block)
    size = (size + sizeof(Block) - 1) / sizeof(Block) * sizeof(Block);

    // Search for a block that fits the requested size
    Block* prev = NULL;
    Block* curr = freeList;
    while (curr != NULL) {
        if (curr->size >= size) {
            // Allocate from this block
            if (curr->size - size > sizeof(Block)) {
                // Split the block if there is enough space for another block
                Block* newBlock = (Block*)((char*)curr + size);
                newBlock->size = curr->size - size - sizeof(Block);
                newBlock->next = curr->next;
                curr->size = size;
                curr->next = newBlock;
            }
            // Remove the allocated block from the free list
            if (prev == NULL) {
                freeList = curr->next;
            } else {
                prev->next = curr->next;
            }
            return (void*)(curr + 1); // Return a pointer to the allocated memory
        }
        prev = curr;
        curr = curr->next;
    }

    if (size > mem)
    {
        panic("Failed to allocate memory");
    }

    // No suitable block found
    return NULL;
}

// Function to free allocated memory
void free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    // Get the block header
    Block* block = (Block*)ptr - 1;
    // Add the block back to the free list
    block->next = freeList;
    freeList = block;
}