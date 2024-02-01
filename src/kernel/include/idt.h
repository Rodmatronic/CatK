#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NO_IDT_DESCRIPTORS     256

typedef struct {
    uint16_t base_low;          // lower 16 bits 0-15 of the address to jump to when this interrupt fires
    uint16_t segment_selector;  // code segment selector in GDT
    uint8_t zero;               // unused, always be zero
    uint8_t type;               // types trap, interrupt gates
    uint16_t base_high;         // upper 16 bits 16-31 of the address to jump to
} __attribute__((packed)) IDT;

typedef struct {
    uint16_t limit;         // limit size of all IDT segments
    uint32_t base_address;  // base address of the first IDT segment
} __attribute__((packed)) IDT_PTR;

// asm gdt functions, define in load_idt.asm
extern void load_idt(uint32_t idt_ptr);
void idt_set_entry(int index, uint32_t base, uint16_t seg_sel, uint8_t flags);
void idt_init();