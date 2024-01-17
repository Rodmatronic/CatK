/*
 * Get the 16-bit system memory
 * Written by Rodmatronics
*/

#include "string.h"
#include "io_ports.h"
#include "memory.h"
#include "libc.h"
#include "console.h"

int Memoryamount;

void GetMemory() {
    // Read the extended memory size from the BIOS Data Area (BDA)
    unsigned short* bda_memory_size = (unsigned short*)0x413;
    unsigned int conventional_memory_kb = *bda_memory_size;

    // Convert conventional memory size to bytes
    unsigned int conventional_memory_bytes = conventional_memory_kb * 1024;

    // Read extended memory size from another memory address (e.g., 0x15 or 0x88, depending on your system)
    unsigned short* extended_memory_size = (unsigned short*)0x88; // You may need to adjust this address
    unsigned int extended_memory_kb = *extended_memory_size;

    // Convert extended memory size to bytes
    unsigned int extended_memory_bytes = extended_memory_kb * 1024;

    printf("Memory: Conventional memory: %u bytes (%u KB)\n", conventional_memory_bytes, conventional_memory_kb);
    printf("Memory: Extended memory: %u bytes (%u KB)\n", extended_memory_bytes, extended_memory_kb);
    Memoryamount = extended_memory_bytes;
}

// A simple structure to represent a block of memory
typedef struct {
    size size;
    int free; // 1 if the block is free, 0 if it's allocated
} Block;

// Assuming you have a large memory buffer for your kernel
#define MEMORY_SIZE 1024 * 1024 * 15 // 15 MB
char memory[MEMORY_SIZE];

// A function to initialize your memory allocator
void initialize_memory() {
    // Create an initial free block that spans the entire memory
    Block* initial_block = (Block*)memory;
    initial_block->size = MEMORY_SIZE - sizeof(Block);
    initial_block->free = 1;
}

// A function to allocate memory
void* allocate_memory(size size) {
    // Find the first free block that is large enough
    Block* current_block = (Block*)memory;
    while (current_block->size < size || !current_block->free) {
        current_block = (Block*)((char*)current_block + current_block->size + sizeof(Block));
    }

    // Split the block if it's larger than needed
    if (current_block->size > size + sizeof(Block)) {
        Block* new_block = (Block*)((char*)current_block + size + sizeof(Block));
        new_block->size = current_block->size - size - sizeof(Block);
        new_block->free = 1;
        current_block->size = size;
    }

    current_block->free = 0; // Mark the block as allocated
    return (char*)current_block + sizeof(Block);
}

// A function to free memory
void free_memory(void* ptr) {
    // Mark the corresponding block as free
    Block* block_to_free = (Block*)((char*)ptr - sizeof(Block));
    block_to_free->free = 1;
}