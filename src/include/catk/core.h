#ifndef __CORE_H
#define __CORE_H

#include <catk/types.h>
#include <catk/compiler.h>
#include <stdint.h>

/* GDT */

#define GDT_NUM_DESCRIPTORS 8

struct segm_descriptor
{
  uint16_t limit;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t access;
  uint8_t flags;
  uint8_t base_high;
}_packed_;

struct gdtr
{
  uint16_t size;
  uint32_t offset;
}_packed_;

/* IDT */

#define X86_TRAP_DE		    0	  /* Divide-by-zero */
#define X86_TRAP_DB		    1	  /* Debug */
#define X86_TRAP_NMI	    2	  /* Non-maskable Interrupt */
#define X86_TRAP_BP		    3	  /* Breakpoint */
#define X86_TRAP_OF		    4	  /* Overflow */
#define X86_TRAP_BR		    5	  /* Bound Range Exceeded */
#define X86_TRAP_UD		    6	  /* Invalid Opcode */
#define X86_TRAP_NM		    7	  /* Device Not Available */
#define X86_TRAP_DF		    8	  /* Double Fault */
#define X86_TRAP_TS		    10	/* Invalid TSS */
#define X86_TRAP_NP		    11	/* Segment Not Present */
#define X86_TRAP_SS		    12	/* Stack Segment Fault */
#define X86_TRAP_GP		    13	/* General Protection Fault */
#define X86_TRAP_PF		    14	/* Page Fault */
#define X86_TRAP_SPURIOUS	15	/* Spurious Interrupt */
#define X86_TRAP_MF		    16	/* x87 Floating-Point Exception */
#define X86_TRAP_AC		    17	/* Alignment Check */
#define X86_TRAP_MC		    18	/* Machine Check */
#define X86_TRAP_XF		    19	/* SIMD Floating-Point Exception */
#define X86_TRAP_VE		    20	/* Virtualization Exception */
#define X86_TRAP_CP		    21	/* Control Protection Exception */
#define X86_TRAP_VC		    29	/* VMM Communication Exception */

#define IRQ(irq) (32 + (irq))

struct idt_reg
{
  uint16_t size;
  uint32_t offset;
}_packed_;

struct idt_vector
{
  uint16_t base_lo;
  uint16_t segm_selector;
  uint8_t reserved;
  uint8_t flags;
  uint16_t base_hi;
}_packed_;

#define IDT_NUM_ENTRIES 256

struct intr_stack_frame
{
  uint32_t ds;
  uint32_t edi; 
  uint32_t esi; 
  uint32_t ebp; 
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t intr;
  uint32_t err_code;
  uint32_t eip; 
  uint32_t cs;
  uint32_t eflags;
  uint32_t useresp;
  uint32_t ss;
}_packed_;

typedef void (*intr_handler)(struct intr_stack_frame *);
void interrupt_install(intr_handler handler, uint8_t intr);

extern void interrupt_0();
extern void interrupt_1();
extern void interrupt_2();
extern void interrupt_3();
extern void interrupt_4();
extern void interrupt_5();
extern void interrupt_6();
extern void interrupt_7();
extern void interrupt_8();
extern void interrupt_9();
extern void interrupt_10();
extern void interrupt_11();
extern void interrupt_12();
extern void interrupt_13();
extern void interrupt_14();
extern void interrupt_15();
extern void interrupt_16();
extern void interrupt_17();
extern void interrupt_18();
extern void interrupt_19();
extern void interrupt_20();
extern void interrupt_21();
extern void interrupt_22();
extern void interrupt_23();
extern void interrupt_24();
extern void interrupt_25();
extern void interrupt_26();
extern void interrupt_27();
extern void interrupt_28();
extern void interrupt_29();
extern void interrupt_30();
extern void interrupt_31();
extern void interrupt_32();
extern void interrupt_33();
extern void interrupt_34();
extern void interrupt_35();
extern void interrupt_36();
extern void interrupt_37();
extern void interrupt_38();
extern void interrupt_39();
extern void interrupt_40();
extern void interrupt_41();
extern void interrupt_42();
extern void interrupt_43();
extern void interrupt_44();
extern void interrupt_45();
extern void interrupt_46();
extern void interrupt_47();
extern void interrupt_128();

/* PIC */

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20
#define ICW1 0x11
#define ICW4_8086 0x01

void pic_eoi(uint8_t irq);

/* PIT */

#define PIT_FREQ_DIVISOR 1193180

void timer_init(void);
void msleep(uint32_t ms);
void beep(int ms);

/* TSS */

struct tss
{
  uint32_t previous;
  uint32_t esp0;
  uint32_t ss0;
  uint32_t esp1;
  uint32_t ss1;
  uint32_t esp2;
  uint32_t ss2;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint32_t es;
  uint32_t cs;
  uint32_t ss;
  uint32_t ds;
  uint32_t fs;
  uint32_t gs;
  uint32_t ldt;
  uint16_t trap;
  uint16_t iomap_base;
}_packed_;

void tss_init(void);
void set_tss_stack(uint32_t esp0);

/* Common CPU related functions */

void segment_dump(uint16_t segm);
int gen_random(void);

static inline void critical_enter(void)
{
  asm volatile("cli");
}

static inline void critical_exit(void)
{
  asm volatile("sti");
}

static inline void halt(void)
{
  asm volatile("hlt");
}

static inline void die(void)
{
  critical_enter();
  halt();
}

static inline void cpuid(uint32_t code, uint32_t * a, uint32_t * b, uint32_t * c, uint32_t * d) 
{
  asm volatile("cpuid" : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d) : "a"(code));
}

static inline void load_page_directory(uint32_t pd)
{
  asm volatile("mov %0, %%cr3" :: "r"(pd));
}

static inline void * platform_memcpy(void * dest, const void * src, size_t count) {
  asm volatile("cld");
  asm volatile("rep movsb" :: "D" (dest), "S" (src), "c" (count) :);
  return dest;
}

static inline void * platform_memset(void * dest, const uint8_t val, size_t count) {
  asm volatile("cld");
  asm volatile("rep stosb" : "+D"(dest), "+c"(count) : "a"(val) : "memory");
  return dest;
}

extern int cpuidcheck(void);
void cpu_dump_all_info(void);
uint32_t get_eip(void);

void cpu_init(void);
void setup_paging(void);

#endif
