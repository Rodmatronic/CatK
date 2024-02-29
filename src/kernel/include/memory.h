#pragma once

#include <stddef.h>
#include <stdint.h>

void memory_init();

void *memcpy(void *dest, const void *src, size_t n);
void *malloc(size_t size);
void free(void *ptr);