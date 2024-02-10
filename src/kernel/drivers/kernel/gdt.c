#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <pc.h>
#include <gdt.h>
#include <printk.h>

GDT g_gdt[NO_GDT_DESCRIPTORS];
GDT_PTR g_gdt_ptr;

void pic8259_init() {
    uint8_t a1, a2;

    // save mask registers
    a1 = inportb(PIC1_DATA);
    a2 = inportb(PIC2_DATA);

    // send commands to pic to initialize both master & slave
    outportb(PIC1_COMMAND, ICW1);
    outportb(PIC2_COMMAND, ICW1);

    // map vector offset of all default IRQ's from 0x20 to 0x27 in master(ICW2)
    outportb(PIC1_DATA, 0x20);
    // map vector offset of all default IRQ's from 0x28 to 0x2F in slave(ICW2)
    outportb(PIC2_DATA, 0x28);

    // ICW3: tell master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outportb(PIC1_DATA, 4);
    // ICW3: tell slave PIC its cascade identity (0000 0010)
    outportb(PIC2_DATA, 2);

    // ICW4, set x86 mode
    outportb(PIC1_DATA, ICW4_8086);
    outportb(PIC2_DATA, ICW4_8086);

    // restore the mask registers
    outportb(PIC1_DATA, a1);
    outportb(PIC2_DATA, a2);
}

void pic8259_eoi(uint8_t irq) {
    if(irq >= 0x28)
        outportb(PIC2, PIC_EOI);
    outportb(PIC1, PIC_EOI);
}

void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    pic8259_init();
    GDT *this = &g_gdt[index];

    this->segment_limit = limit & 0xFFFF;
    this->base_low = base & 0xFFFF;
    this->base_middle = (base >> 16) & 0xFF;
    this->access = access;

    this->granularity = (limit >> 16) & 0x0F;
    this->granularity = this->granularity | (gran & 0xF0);

    this->base_high = (base >> 24 & 0xFF);
}

// initialize GDT
void gdt_init() {

    printk("gdt_init: init\n");
    g_gdt_ptr.limit = sizeof(g_gdt) - 1;
    g_gdt_ptr.base_address = (uint32_t)g_gdt;

    // NULL segment
    gdt_set_entry(0, 0, 0, 0, 0);
    // code segment
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    // data segment
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    // user code segment
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    // user data segment
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    load_gdt((uint32_t)&g_gdt_ptr);

    printk("gdt_init: loaded!\n");

}