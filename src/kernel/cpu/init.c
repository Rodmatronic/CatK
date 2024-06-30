#include <catk/core.h>
#include <catk/io.h>
#include <lib/common.h>
#include <stdint.h>

/* GDT */

static struct segm_descriptor gdt[GDT_NUM_DESCRIPTORS];

static struct gdtr gdtr = {
  .offset = (uint32_t)gdt,
  .size = sizeof(gdt),
};

extern void gdt_flush(uint32_t gdtr);

static void segm_descriptors_init(void)
{
  // null descriptor
  memset((void *)&gdt[0], 0, sizeof(struct segm_descriptor));
  // kernel code segment
  gdt[1].base_low = 0;
  gdt[1].base_mid = 0;
  gdt[1].base_high = 0;
  gdt[1].limit = 0xffff;
  gdt[1].access = 0x9a;
  gdt[1].flags = 0xcf;
  // kernel data segment
  gdt[2].base_low = 0;
  gdt[2].base_mid = 0;
  gdt[2].base_high = 0;
  gdt[2].limit = 0xffff;
  gdt[2].access = 0x92;
  gdt[2].flags = 0xcf;
  // user code segment
  gdt[3].base_low = 0;
  gdt[3].base_mid = 0;
  gdt[3].base_high = 0;
  gdt[3].limit = 0xffff;
  gdt[3].access = 0xfa;
  gdt[3].flags = 0xcf;
  // user data segment 
  gdt[4].base_low = 0;
  gdt[4].base_mid = 0;
  gdt[4].base_high = 0;
  gdt[4].limit = 0xffff;
  gdt[4].access = 0xf2;
  gdt[4].flags = 0xcf;

  gdt_flush((uint32_t)&gdtr);
}

/* IDT */

static void pic_init(void);
extern void idt_flush(uint32_t idtr);

static struct idt_vector idt[IDT_NUM_ENTRIES];

static struct idt_reg idtr = {
  .size = sizeof(idt),
  .offset = (uint32_t)idt,
};

static void idt_set_vector(int idx, uint32_t base, uint16_t segm_sel, uint8_t flags)
{
  idt[idx].base_lo = base & 0xFFFF;
  idt[idx].segm_selector = segm_sel;
  idt[idx].reserved = 0;
  idt[idx].flags = flags | 0x60;
  idt[idx].base_hi = (base >> 16) & 0xFFFF;
}

static void idt_setup(void)
{
  pic_init();
  idt_set_vector(0, (uint32_t)interrupt_0, 0x08, 0x8e);
  idt_set_vector(1, (uint32_t)interrupt_1, 0x08, 0x8e);
  idt_set_vector(2, (uint32_t)interrupt_2, 0x08, 0x8e);
  idt_set_vector(3, (uint32_t)interrupt_3, 0x08, 0x8e);
  idt_set_vector(4, (uint32_t)interrupt_4, 0x08, 0x8e);
  idt_set_vector(5, (uint32_t)interrupt_5, 0x08, 0x8e);
  idt_set_vector(6, (uint32_t)interrupt_6, 0x08, 0x8e);
  idt_set_vector(7, (uint32_t)interrupt_7, 0x08, 0x8e);
  idt_set_vector(8, (uint32_t)interrupt_8, 0x08, 0x8e);
  idt_set_vector(9, (uint32_t)interrupt_9, 0x08, 0x8e);
  idt_set_vector(10, (uint32_t)interrupt_10, 0x08, 0x8e);
  idt_set_vector(11, (uint32_t)interrupt_11, 0x08, 0x8e);
  idt_set_vector(12, (uint32_t)interrupt_12, 0x08, 0x8e);
  idt_set_vector(13, (uint32_t)interrupt_13, 0x08, 0x8e);
  idt_set_vector(14, (uint32_t)interrupt_14, 0x08, 0x8e);
  idt_set_vector(15, (uint32_t)interrupt_15, 0x08, 0x8e);
  idt_set_vector(16, (uint32_t)interrupt_16, 0x08, 0x8e);
  idt_set_vector(17, (uint32_t)interrupt_17, 0x08, 0x8e);
  idt_set_vector(18, (uint32_t)interrupt_18, 0x08, 0x8e);
  idt_set_vector(19, (uint32_t)interrupt_19, 0x08, 0x8e);
  idt_set_vector(20, (uint32_t)interrupt_20, 0x08, 0x8e);
  idt_set_vector(21, (uint32_t)interrupt_21, 0x08, 0x8e);
  idt_set_vector(22, (uint32_t)interrupt_22, 0x08, 0x8e);
  idt_set_vector(23, (uint32_t)interrupt_23, 0x08, 0x8e);
  idt_set_vector(24, (uint32_t)interrupt_24, 0x08, 0x8e);
  idt_set_vector(25, (uint32_t)interrupt_25, 0x08, 0x8e);
  idt_set_vector(26, (uint32_t)interrupt_26, 0x08, 0x8e);
  idt_set_vector(27, (uint32_t)interrupt_27, 0x08, 0x8e);
  idt_set_vector(28, (uint32_t)interrupt_28, 0x08, 0x8e);
  idt_set_vector(29, (uint32_t)interrupt_29, 0x08, 0x8e);
  idt_set_vector(30, (uint32_t)interrupt_30, 0x08, 0x8e);
  idt_set_vector(31, (uint32_t)interrupt_31, 0x08, 0x8e);
  idt_set_vector(32, (uint32_t)interrupt_32, 0x08, 0x8e);
  idt_set_vector(33, (uint32_t)interrupt_33, 0x08, 0x8e);
  idt_set_vector(34, (uint32_t)interrupt_34, 0x08, 0x8e);
  idt_set_vector(35, (uint32_t)interrupt_35, 0x08, 0x8e);
  idt_set_vector(36, (uint32_t)interrupt_36, 0x08, 0x8e);
  idt_set_vector(37, (uint32_t)interrupt_37, 0x08, 0x8e);
  idt_set_vector(38, (uint32_t)interrupt_38, 0x08, 0x8e);
  idt_set_vector(39, (uint32_t)interrupt_39, 0x08, 0x8e);
  idt_set_vector(40, (uint32_t)interrupt_40, 0x08, 0x8e);
  idt_set_vector(41, (uint32_t)interrupt_41, 0x08, 0x8e);
  idt_set_vector(42, (uint32_t)interrupt_42, 0x08, 0x8e);
  idt_set_vector(43, (uint32_t)interrupt_43, 0x08, 0x8e);
  idt_set_vector(44, (uint32_t)interrupt_44, 0x08, 0x8e);
  idt_set_vector(45, (uint32_t)interrupt_45, 0x08, 0x8e);
  idt_set_vector(46, (uint32_t)interrupt_46, 0x08, 0x8e);
  idt_set_vector(47, (uint32_t)interrupt_47, 0x08, 0x8e);
  idt_set_vector(128, (uint32_t)syscall_dispatcher, 0x08, 0x8e); /* syscall interrupt vector */
  idt_flush((uint32_t)&idtr);
}

static void pic_init(void)
{
  uint8_t a1, a2;
  a1 = inb(PIC1_DATA);
  a2 = inb(PIC2_DATA);

  outb(PIC1_COMMAND, ICW1);
  outb(PIC2_COMMAND, ICW1);

  outb(PIC1_DATA, 0x20);
  outb(PIC2_DATA, 0x28);

  outb(PIC1_DATA, 4);

  outb(PIC2_DATA, 2);

  outb(PIC1_DATA, ICW4_8086);
  outb(PIC2_DATA, ICW4_8086);

  outb(PIC1_DATA, a1);
  outb(PIC2_DATA, a2);
}

void pic_eoi(uint8_t irq)
{
  if(irq >= 0x28)
    outb(PIC2, PIC_EOI);
  outb(PIC1, PIC_EOI);
}

void cpu_init(void)
{
  segm_descriptors_init();
  idt_setup();
  timer_init();
}
