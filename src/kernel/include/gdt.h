#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NO_GDT_DESCRIPTORS     8

#define PIC1            0x20  /* IO base address for master PIC */
#define PIC2            0xA0  /* IO base address for slave PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)    /* master data */
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)    /* slave data */

#define PIC_EOI         0x20   /* end of interrupt */

#define ICW1            0x11    /* interrupt control command word PIC for initialization */
#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */

typedef struct {
    uint16_t segment_limit;  // segment limit first 0-15 bits
    uint16_t base_low;       // base first 0-15 bits
    uint8_t base_middle;     // base 16-23 bits
    uint8_t access;          // access byte
    uint8_t granularity;     // high 4 bits (flags) low 4 bits (limit 4 last bits)(limit is 20 bit wide)
    uint8_t base_high;       // base 24-31 bits
} __attribute__((packed)) GDT;

typedef struct {
    uint16_t limit;       // limit size of all GDT segments
    uint32_t base_address;  // base address of the first GDT segment
} __attribute__((packed)) GDT_PTR;

// asm gdt functions
extern void load_gdt(uint32_t gdt_ptr);
void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void gdt_init();
void pic8259_init();
void pic8259_eoi(uint8_t irq);
