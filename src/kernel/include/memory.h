#pragma once

#include <stddef.h>
#include <stdint.h>

#define MAX_BLOCKS 1000

void memory_init();

typedef struct {
    size_t size;
    void *ptr;
} Block;


static Block memory[MAX_BLOCKS];
static size_t total_blocks = 0;

void *memcpy(void *dest, const void *src, size_t n);
void *malloc(size_t size);
void free(void *ptr);