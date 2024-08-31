#include <stdint.h>
#include <catk/platform.h>
#include <catk/printk.h>
#include <lib/common.h>

#include "irq.h"
#include "i8259.h"
#include "paging.h"

#define GDT_ACCESS_ACCESSED   BIT(0)
#define GDT_ACCESS_READWRITE  BIT(1)
#define GDT_ACCESS_DIRECTION  BIT(2)
#define GDT_ACCESS_EXEC       BIT(3)
#define GDT_ACCESS_SYSTEM     BIT(4)
#define GDT_ACCESS_RING0      (0 << 5 | 0 << 6)
#define GDT_ACCESS_RING3      (1 << 5 | 1 << 6)
#define GDT_ACCESS_PRESENT    BIT(7)

/* size flag should always be cleared if this is set */
#define GDT_FLAGS_64BIT       BIT(1)
/* set if we're using 32-bit bases */
#define GDT_FLAGS_SIZE32      BIT(2)
/* if set, the segment limit is in 4KiB blocks, otherwise, its in 1 byte blocks */
#define GDT_FLAGS_GRAN        BIT(3)

/* code segments are executable */
#define GDT_KERNEL_CODE (GDT_ACCESS_READWRITE | GDT_ACCESS_EXEC | GDT_ACCESS_SYSTEM | GDT_ACCESS_RING0 | GDT_ACCESS_PRESENT)
/* data segments are not executable, since they contain data */
#define GDT_KERNEL_DATA (GDT_ACCESS_READWRITE | GDT_ACCESS_SYSTEM | GDT_ACCESS_RING0 | GDT_ACCESS_PRESENT)
/* these are the same, except that the DPL values are 3 (for user-mode) */
#define GDT_USER_CODE   (GDT_ACCESS_READWRITE | GDT_ACCESS_EXEC | GDT_ACCESS_SYSTEM | GDT_ACCESS_RING3 | GDT_ACCESS_PRESENT)
#define GDT_USER_DATA   (GDT_ACCESS_READWRITE | GDT_ACCESS_SYSTEM | GDT_ACCESS_RING3 | GDT_ACCESS_PRESENT)
/* segment descriptor flags used for 32-bit cpus */
#define GDT_IA32_FLAGS  0xcf


#define DEFINE_SEGM_DESC(base, limit, access, flags) \
  (limit & 0xffff), \
  (base & 0xffff), \
  ((base >> 16) & 0xff), \
  (access), \
  flags, \
  ((base >> 24) & 0xff)

#define X86_KRNL_CODE_SEGM (1 << 3)
#define X86_KRNL_DATA_SEGM (2 << 3)
#define X86_USER_CODE_SEGM (3 << 3)
#define X86_USER_DATA_SEGM (4 << 3)

/* Linux does this, but better */
struct segm_descriptor gdt[8] = {
  // null segment descriptor
  DEFINE_SEGM_DESC(0, 0, 0, 0),
  // kernel code segment descriptor (32-bit)
  DEFINE_SEGM_DESC(0, 0xffff, GDT_KERNEL_CODE, GDT_IA32_FLAGS),
  // kernel data segment descriptor (32-bit)
  DEFINE_SEGM_DESC(0, 0xffff, GDT_KERNEL_DATA, GDT_IA32_FLAGS),
  // user code segment descriptor (32-bit) 
  DEFINE_SEGM_DESC(0, 0xffff, GDT_USER_CODE, GDT_IA32_FLAGS),
  // user data segment descriptor (32-bit)
  DEFINE_SEGM_DESC(0, 0xffff, GDT_USER_DATA, GDT_IA32_FLAGS)
};

extern void native_gdt_load(uint32_t gdtr);

struct gdtr gdtr = {
  /* size of all of the segment descriptors */
  sizeof(gdt) - 1,
  /* address of all of the segment descriptors */
  (uint32_t)&gdt
};

void segment_dump(uint16_t segm) {
  uint16_t descriptor_num = segm >> 3;
  struct segm_descriptor * descriptor = &gdt[descriptor_num];
  if(!descriptor) {
    printk("Segment descriptor does not exist for 0x%04x\n", segm);
    return;
  }
  printk("DESCRIPTOR NUMBER  -  SEGMENT LIMIT  -  SEGMENT ACCESS  -  SEGMENT_FLAGS\n");
  printk("%d                     0x%04x             0x%02x              0x%02x\n", descriptor_num, descriptor->limit, descriptor->access, descriptor->flags);
}

static void global_descriptors_init(void) {
  native_gdt_load((uint32_t)&gdtr);
}

#define IDT_FLAGS_TASK_GATE   0b0101
#define IDT_FLAGS_INTR16      0b0110
#define IDT_FLAGS_TRAP16      0b0111
#define IDT_FLAGS_INTR32      0b1110
#define IDT_FLAGS_TRAP32      0b1111

#define IDT_FLAGS_RING3       (BIT(5) | BIT(6))
#define IDT_FLAGS_RING0       (ZEROBIT(5) | ZEROBIT(6))

#define IDT_FLAGS_PRESENT     BIT(7)

struct interrupt_vector idt[256];

struct idtr idtr = {
  sizeof(idt) - 1,
  (uint32_t)idt
};

extern void native_idt_load(uint32_t idtr);

static inline void idt_setup_descriptor(int gate, uint32_t addr, uint8_t flags) {
  idt[gate].base_low = addr & 0xffff;
  idt[gate].base_high = (addr >> 16) & 0xffff;
  idt[gate].segm_selector = 0x08;
  idt[gate].reserved = 0;
  idt[gate].flags = flags;
}

static const void * intr_stub_list[33] = {
  intr_stub_0,
  intr_stub_1,
  intr_stub_2,
  intr_stub_3,
  intr_stub_4,
  intr_stub_5,
  intr_stub_6,
  intr_stub_7,
  intr_stub_8,
  intr_stub_9,
  intr_stub_10,
  intr_stub_11,
  intr_stub_12,
  intr_stub_13,
  intr_stub_14,
  intr_stub_15,
  intr_stub_16,
  intr_stub_17,
  intr_stub_18,
  intr_stub_19,
  intr_stub_20,
  intr_stub_21,
  intr_stub_22,
  intr_stub_23,
  intr_stub_24,
  intr_stub_25,
  intr_stub_26,
  intr_stub_27,
  intr_stub_28,
  intr_stub_29,
  intr_stub_30,
  intr_stub_31,
  intr_stub_32
};

static void set_intr_gate(int gate, const void * addr) {
  uint8_t flags = (gate < 33) ? (IDT_FLAGS_TRAP32 | IDT_FLAGS_RING3 | IDT_FLAGS_PRESENT) : (IDT_FLAGS_INTR32 | IDT_FLAGS_RING3 | IDT_FLAGS_PRESENT);
  idt_setup_descriptor(gate, (uint32_t)addr, flags);
}

static inline void invalidate_intr_gate(int trapnr) {
  idt[trapnr].flags &= ~IDT_FLAGS_PRESENT;
}

static void interrupt_descriptors_init(void) {
  int i = 0;
  i8259_remap_vectors(0x20, 0x28);
  for(; i < 33; i++) {
    set_intr_gate(i, intr_stub_list[i]);
  }
  for(; i < 256; i++) {
    set_intr_gate(i, intr_stub_ignore);
  }
  native_idt_load((uint32_t)&idtr);
}

void early_platform_init(void) {
  global_descriptors_init();
  interrupt_descriptors_init();
  exceptions_install();
  setup_paging();
}
