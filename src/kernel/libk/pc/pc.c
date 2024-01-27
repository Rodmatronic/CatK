#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

uint8_t inportb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outportb(uint16_t port, uint8_t val) {
    asm volatile("outb %1, %0" :: "dN"(port), "a"(val));
}

void outports(uint16_t port, uint16_t data) {
    asm volatile ("outw %1, %0" : : "dN" (port), "a" (data));
}