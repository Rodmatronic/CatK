#include <catk/core.h>
#include <catk/io.h>
#include <catk/syscall.h>
#include <catk/printk.h>
#include <catk/virt.h>
#include <catk/ipc.h>
#include <lib/common.h>
#include <stdint.h>

/* GDT */

extern void gdt_flush(uint32_t gdtr);

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
  { DEFINE_SEGM_DESC(0, 0x0000, 0x00, 0x00) },
  // kernel code segment descriptor (32-bit)
  { DEFINE_SEGM_DESC(0, 0xffff, GDT_KERNEL_CODE, GDT_IA32_FLAGS) },
  // kernel data segment descriptor (32-bit)
  { DEFINE_SEGM_DESC(0, 0xffff, GDT_KERNEL_DATA, GDT_IA32_FLAGS) },
  // user code segment descriptor (32-bit) 
  { DEFINE_SEGM_DESC(0, 0xffff, GDT_USER_CODE, GDT_IA32_FLAGS) },
  // user data segment descriptor (32-bit)
  { DEFINE_SEGM_DESC(0, 0xffff, GDT_USER_DATA, GDT_IA32_FLAGS) }
};

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
  printk("DESCRIPTOR NUMBER  -  SEGMENT LIMIT  -  SEGMENT ACCESS  -  SEGMENT FLAGS\n");
  printk("%d                     0x%04x            0x%02x               0x%02x\n", descriptor_num, descriptor->limit, descriptor->access, descriptor->flags);
}

void gdt_install(void) {
  gdt_flush((uint32_t)&gdtr);
}

/* IDT */

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

static void pic_remap_vectors(void);

static void idt_setup(void)
{
  pic_remap_vectors();
  idt_set_vector(0, (uint32_t)interrupt_0, 0x08, 0x8f);
  idt_set_vector(1, (uint32_t)interrupt_1, 0x08, 0x8f);
  idt_set_vector(2, (uint32_t)interrupt_2, 0x08, 0x8f);
  idt_set_vector(3, (uint32_t)interrupt_3, 0x08, 0x8f);
  idt_set_vector(4, (uint32_t)interrupt_4, 0x08, 0x8f);
  idt_set_vector(5, (uint32_t)interrupt_5, 0x08, 0x8f);
  idt_set_vector(6, (uint32_t)interrupt_6, 0x08, 0x8f);
  idt_set_vector(7, (uint32_t)interrupt_7, 0x08, 0x8f);
  idt_set_vector(8, (uint32_t)interrupt_8, 0x08, 0x8f);
  idt_set_vector(9, (uint32_t)interrupt_9, 0x08, 0x8f);
  idt_set_vector(10, (uint32_t)interrupt_10, 0x08, 0x8f);
  idt_set_vector(11, (uint32_t)interrupt_11, 0x08, 0x8f);
  idt_set_vector(12, (uint32_t)interrupt_12, 0x08, 0x8f);
  idt_set_vector(13, (uint32_t)interrupt_13, 0x08, 0x8f);
  idt_set_vector(14, (uint32_t)interrupt_14, 0x08, 0x8f);
  idt_set_vector(15, (uint32_t)interrupt_15, 0x08, 0x8f);
  idt_set_vector(16, (uint32_t)interrupt_16, 0x08, 0x8f);
  idt_set_vector(17, (uint32_t)interrupt_17, 0x08, 0x8f);
  idt_set_vector(18, (uint32_t)interrupt_18, 0x08, 0x8f);
  idt_set_vector(19, (uint32_t)interrupt_19, 0x08, 0x8f);
  idt_set_vector(20, (uint32_t)interrupt_20, 0x08, 0x8f);
  idt_set_vector(21, (uint32_t)interrupt_21, 0x08, 0x8f);
  idt_set_vector(22, (uint32_t)interrupt_22, 0x08, 0x8f);
  idt_set_vector(23, (uint32_t)interrupt_23, 0x08, 0x8f);
  idt_set_vector(24, (uint32_t)interrupt_24, 0x08, 0x8f);
  idt_set_vector(25, (uint32_t)interrupt_25, 0x08, 0x8f);
  idt_set_vector(26, (uint32_t)interrupt_26, 0x08, 0x8f);
  idt_set_vector(27, (uint32_t)interrupt_27, 0x08, 0x8f);
  idt_set_vector(28, (uint32_t)interrupt_28, 0x08, 0x8f);
  idt_set_vector(29, (uint32_t)interrupt_29, 0x08, 0x8f);
  idt_set_vector(30, (uint32_t)interrupt_30, 0x08, 0x8f);
  idt_set_vector(31, (uint32_t)interrupt_31, 0x08, 0x8f);
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
  idt_set_vector(128, (uint32_t)interrupt_128, 0x08, 0x8e); /* syscall interrupt vector */
  idt_flush((uint32_t)&idtr);
}

#define PIC_MASTER_COMMAND_PORT 0x0020
#define PIC_MASTER_DATA_PORT    0x0021
#define PIC_SLAVE_COMMAND_PORT  0x00a0
#define PIC_SLAVE_DATA_PORT     0x00a1

#define PIC_EOI	0x20
#define ICW1_ICW4	0x01
#define ICW1_SINGLE	0x02
#define ICW1_INTERVAL4	0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT	0x10
 
#define ICW4_8086	0x01
#define ICW4_AUTO	0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER	0x0C
#define ICW4_SFNM	0x10

static void pic_master_send_cmd(uint8_t cmd)
{
  outb(PIC_MASTER_COMMAND_PORT, cmd);
}

static void pic_master_send_dat(uint8_t data)
{
  outb(PIC_MASTER_DATA_PORT, data);
}

static void pic_slave_send_cmd(uint8_t cmd)
{
  outb(PIC_SLAVE_COMMAND_PORT, cmd);
}

static void pic_slave_send_dat(uint8_t data)
{
  outb(PIC_SLAVE_DATA_PORT, data);
}

uint8_t read_pic_master_data(void)
{
  return inb(PIC_MASTER_DATA_PORT);
}

uint8_t read_pic_slave_data(void)
{
  return inb(PIC_SLAVE_DATA_PORT);
}

static void io_wait(void)
{
  outb(0x80, 0);
}

void pic_eoi(uint8_t intr)
{
	if(intr >= 0x28)
		pic_slave_send_cmd(PIC_EOI);
	pic_master_send_cmd(PIC_EOI);
}

void pic_mask(uint8_t intr)
{
  uint16_t port;
  uint8_t value;
 
  if(intr < 8)
  {
    port = PIC_MASTER_DATA_PORT;
  } 
  else 
  {
    port = PIC_SLAVE_DATA_PORT;
    intr -= 8;
  }
  value = inb(port) | (1 << intr);
  outb(port, value);        
}
 
void pic_unmask(uint8_t intr)
{
  uint16_t port;
  uint8_t value;
 
  if(intr < 8)
  {
    port = PIC_MASTER_DATA_PORT;
  } 
  else 
  {
    port = PIC_SLAVE_DATA_PORT;
    intr -= 8;
  }
  value = inb(port) & ~(1 << intr);
  outb(port, value);        
}

static void pic_remap_vectors(void)
{
	uint8_t a1, a2;
 
	a1 = read_pic_slave_data();
	a2 = read_pic_master_data();
 
	pic_master_send_cmd(ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	pic_slave_send_cmd(ICW1_INIT | ICW1_ICW4);
	io_wait();
	pic_master_send_dat(0x20);                 // ICW2: Master PIC vector offset
	io_wait();
	pic_slave_send_dat(0x28);                 // ICW2: Slave PIC vector offset
	io_wait();
	pic_master_send_dat(4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	pic_slave_send_dat(2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	pic_master_send_dat(ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	pic_slave_send_dat(ICW4_8086);
	io_wait();
 
	pic_master_send_dat(a1);   // restore saved masks.
	pic_slave_send_dat(a2);
  /* UEFI masks these interrupts. I don't know why. */
  for(int i = 0; i < 42; i++)
    pic_unmask(i);
}

/* TSS */

struct tss tss;

extern void tss_install(void);

static void tss_setup(int segment, uint16_t ss0, uint32_t esp0)
{
	uint32_t base = (uint32_t)&tss;
	uint32_t limit = base + sizeof(tss);

  gdt[segment].base_low = (base & 0xffff);
  gdt[segment].base_mid = (base >> 16) & 0xff;
  gdt[segment].base_high = (base >> 24) & 0xff;
  gdt[segment].limit = limit;
  gdt[segment].access = 0xe9;
  gdt[segment].flags = 0x00;

	memset(&tss, 0, sizeof(struct tss));

	tss.ss0		= ss0;		// kernel stack segment
	tss.esp0	= esp0;		// kernel stack pointer

	tss.cs		= 0x0b;
	tss.ss		= tss.ds = tss.es = tss.fs = tss.gs = 0x13;
}

void set_tss_stack(uint32_t esp0)
{
  tss.esp0 = esp0;
}

void tss_init(void)
{
  tss_setup(5, 0x10, 0);
  tss_install();
}


static inline char * gpf_tbltostr(const uint8_t tbl) {
  switch(tbl) {
    case 0b00: {
      return "GDT";
    }
    case 0b01: {
      return "IDT";
    }
    case 0b10: {
      return "LDT";
    }
    case 0b11: {
      return "IDT";
    }
  }
  return NULL;
}

static void gpf_handler(struct intr_stack_frame * regs) {
  printk("x86 Trap: General Protection Fault:\n");
  /* bit 0 is set if it was caused by software */
  printk("\tExternal: %s\n", (regs->err_code & BIT(0)) ? "true" : "false");
  /* bits 1-2 are set to tell us what table it originated from */
  uint8_t table = ((regs->err_code >> 1) & 0xf0) & 0b1100;
  printk("\tTable: %s (0x%02x)\n", gpf_tbltostr(table), table);
  /* bit 3-15 is the selector index in the table */
  uint8_t idx = (regs->err_code >> 3) ;
  printk("\tSelector index of origin: %d\n", idx);
  /* bit 16-31 are zero-padded to form a uint32_t (we ignore those bits) */
  if(regs->cs == 0x1b)
    dispatch_signal(SIGILL);
  panic("General protection fault in kernel mode\n");
}

void exceptions_install(void) {
  interrupt_install(gpf_handler, X86_TRAP_GP);
}

void cpu_init(void)
{
  gdt_install();
  idt_setup();
  exceptions_install();
  timer_init();
  tss_init();
  syscall_install();
}
