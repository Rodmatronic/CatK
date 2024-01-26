#ifndef LIBC_H
#define LIBC_H

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char* str);
uint8_t inportb(uint16_t port);
void outportb(uint16_t port, uint8_t val);

#endif